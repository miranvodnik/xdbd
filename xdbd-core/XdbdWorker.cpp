/*
 * XdbdWorker.cpp
 *
 *  Created on: 30. okt. 2015
 *      Author: miran
 */

#define	_GNU_SOURCE
#include <sched.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <uuid/uuid.h>
#include "XdbdWorker.h"
#include "XdbdXmlDataBase.h"
#include <iostream>
using namespace std;
using namespace xml_db_daemon;

namespace xml_db_daemon
{

//	file descriptors 3, 4, 5, 6 are inherited (!!!) by controlling entity
const	int	XdbdWorker::g_localListener = ::g_localListener;	// worker's unix domain listener
const	int	XdbdWorker::g_networkListener = ::g_networkListener;	// worker's network listener
const	int	XdbdWorker::g_mqListener = ::g_mqListener;	// worker's posix queue listener
const	int	XdbdWorker::g_localClientReader = ::g_localWrkReader;	// worker's unix domain master client
const	int	XdbdWorker::g_localClientWriter = ::g_localWrkWriter;	// worker's unix domain master client

XdbdRequest	XdbdWorker::g_stopRequest;
XdbdWorker*	XdbdWorker::g_genWorker = new XdbdWorker (0);

void sigint (int signo)
{
	// signo /= 0;
}

/*! @brief private constructor
 *
 *  this constructor cannot be used. Its only purpose is
 *  to create g_stopRequest by c++ running environment
 *
 */
XdbdWorker::XdbdWorker(int n) : XdbdRunningContext (0, 0, 0, 0, 0)
{
	g_stopRequest.m_requestType = AdminRequestCode;

	XdbdAdminRequest*	p_XdbdAdminRequest = &g_stopRequest.XdbdRequest_u.m_adminRequest;
	XdbdMessageHeader*	header = (XdbdMessageHeader*) p_XdbdAdminRequest;
	header->m_version.m_major = 1;
	header->m_version.m_minor = 0;
	header->m_index = -1;
	uuid_generate (header->m_hash);
	header->m_error = 0;

	p_XdbdAdminRequest->m_command = (u_int) XdbdAdminStopWord;
	p_XdbdAdminRequest->m_parameters.m_parameters_len = 0;
	p_XdbdAdminRequest->m_parameters.m_parameters_val = 0;
}

/*! @brief working entity constructor
 *
 *  initializes internal data structures and provides its superclass
 *  multiplexer with start, stop and time-hook routines
 *
 *  @param n number of program parameters
 *  @param p program parameters
 *
 */
XdbdWorker::XdbdWorker(int n, char* p[]) : XdbdRunningContext (StartWorker, StopWorker, TimeHookWorker, this, "xdbd-worker")
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdWorker %ld", this);
#endif

	m_localClientReaderHandler = 0;
	m_localClientWriterHandler = 0;
	m_localListenerHandler = 0;
	m_networkListenerHandler = 0;
	m_mqListenerHandler = 0;

	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	m_hangupSignalHandler = 0;
	m_abortSignalHandler = 0;

	int	dummy = 0;
	m_intSize = xdr_sizeof ((xdrproc_t)xdr_int, &dummy);

	signal (SIGINT, sigint);
#if defined (XDBD_HAM)
	m_hamThread = 0;
#endif
}

/*! @brief working entity destructor
 *
 *  releases all allocated resources
 *
 */
XdbdWorker::~XdbdWorker()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdWorker %ld", this);
#endif
	Release ();
}

/*! release allocated resources
 *
 *  removes all I/O handlers for listening sockets and controlling pipes.
 *  Terminates working and client driver threads and unloads database.
 *  Finally it terminates internal message queue.
 *
 */
void	XdbdWorker::Release ()
{
	if (m_localClientReaderHandler != 0)
		RemoveDescriptor(m_localClientReaderHandler);
	m_localClientReaderHandler = 0;

	if (m_localClientWriterHandler != 0)
		RemoveDescriptor(m_localClientWriterHandler);
	m_localClientWriterHandler = 0;

	if (m_localListenerHandler != 0)
		RemoveDescriptor(m_localListenerHandler);
	m_localListenerHandler = 0;

	if (m_networkListenerHandler != 0)
		RemoveDescriptor(m_networkListenerHandler);
	m_networkListenerHandler = 0;

	if (m_mqListenerHandler != 0)
		RemoveDescriptor(m_mqListenerHandler);
	m_mqListenerHandler = 0;

	if (m_hangupSignalHandler != 0)
		RemoveSignal(m_hangupSignalHandler);
	m_hangupSignalHandler = 0;

	if (m_abortSignalHandler != 0)
		RemoveSignal(m_abortSignalHandler);
	m_abortSignalHandler = 0;

	if (m_outputBuffer != 0)
		free (m_outputBuffer);
	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	XdbdWorkingThread::StopWorkingThreads ();
	XdbdClientDriver::StopClientDrivers ();
	XdbdXmlDataBase::UnloadXmlDataBase();

	#if defined (XDBD_HAM)
	if (m_hamThread != 0)
	{
		m_hamThread->Terminate ();
		delete m_hamThread;
		m_hamThread = 0;
	}
#endif
	Stop ();
}

/*! @brief SIGHUP signal handler
 *
 *  when SIGHUP is detected by working environment (one of I/O multiplexing
 *  functionalities) this handler is called: it logs message and posts stop
 *  administrative message to controlling entity through output pipe controller
 *  connection
 *
 */
void	XdbdWorker::HangupSignalHandler (XdbdRunningContext *ctx, ctx_fddes_t handler, siginfo_t* info)
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info("HANGUP detected: xdbd worker, PID = %d"), getpid());
	xdbdErrReport (SC_XDBD, SC_ERR, err_info("post stop request to xdbd controller"));
	PostAdminRequest (&g_stopRequest);
}

/*! @brief ABORT signal handler
 *
 *  abort signal is reported
 *
 */
void	XdbdWorker::AbortSignalHandler (XdbdRunningContext *ctx, ctx_fddes_t handler, siginfo_t* info)
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info("ABORT detected: xdbd worker, PID = %d"), getpid());
}

/*! @brief start routine
 *
 *  this routine is invoked by I/O multiplexing working environment. It makes all necessary steps
 *  to start working entity:
 *
 *  - first it checks controller connections making simple fcntl() calls
 *
 *  - signal handlers for SIGHUP and SIGABRT are installed next
 *
 *  - next step loads database files
 *
 *  - internal message queue is created and initialized
 *
 *  - client driver threads are started
 *
 *  - working threads are started
 *
 *  - handlers to I/O redirections are installed
 *
 *  - FD 3 redirection: UNIX domain listening socket
 *  - FD 4 redirection: network TCP/IP listening socket
 *  - FD 5 redirection: local message queue listening socket
 *  - FD 6 redirection: input pipe controller connection
 *  - FD 7 redirection: output pipe controller connection
 *
 *  If all these steps succeed than working entity will be started otherwise it will throw
 *  exception causing it to cease.
 *
 *  @param ctx reference to superclass part of this object (I/O multiplexer core)
 *
 */
void	XdbdWorker::StartWorker (XdbdRunningContext *ctx)
{
	if ((fcntl (g_localClientReader, F_GETFD, 0) < 0) || (fcntl (g_localClientReader, F_GETFD, 0) < 0))
		throw	2;

	XdbdCommon::entity (XdbdWorkerEntity);
	m_hangupSignalHandler = RegisterSignal(SIGHUP, HangupSignalHandler, this);
	m_abortSignalHandler = RegisterSignal(SIGABRT, AbortSignalHandler, this);
//	signal (SIGABRT, SIG_IGN);

#if defined (XDBD_HAM)
	m_hamThread = new XdbdHamThread ();
	m_hamThread->Start ();

	char* env = getenv ("XDBD_WORKER_RELOADED");
	if (env == 0)
	while (m_hamThread->useHam())
	{
		switch (m_hamThread->Registration())
		{
		case	EV_START:
			sleep (1);
			continue;
		case	EV_REG_OK:
			break;
		default:
			throw	3;
		}
		break;
	}
#endif

	if (XdbdXmlDataBase::LoadXmlDataBase(ctx) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot load xml data base"));
#if defined (XDBD_HAM)
		if (m_hamThread->useHam())
			m_hamThread->WorkerReady (false);
#endif
		throw	1;
	}

	XdbdMessageQueue::Initialize ();
	XdbdClientDriver::StartClientDrivers ();
	XdbdWorkingThread::StartWorkingThreads ();
	XdbdXmlDataBase::InitStatAdapter ();

	m_localListenerHandler = ctx->RegisterDescriptor (EPOLLIN, g_localListener, HandleLocalListener, this, ctx_info);
	m_networkListenerHandler = ctx->RegisterDescriptor (EPOLLIN, g_networkListener, HandleNetworkListener, this, ctx_info);
	m_mqListenerHandler = ctx->RegisterDescriptor (EPOLLIN, g_mqListener, HandleMqListener, this, ctx_info);
	m_localClientReaderHandler = ctx->RegisterDescriptor (EPOLLIN, g_localClientReader, HandleMainLocalClientReader, this, ctx_info);
	m_localClientWriterHandler = ctx->RegisterDescriptor (EPOLLIN, g_localClientWriter, HandleMainLocalClientWriter, this, ctx_info);

#if defined (XDBD_HAM)
	if (m_hamThread->useHam())
		m_hamThread->WorkerReady (true);
#endif
}

/*! @brief stop routine
 *
 *  this routine is invoked by working environment of I/O multiplexer
 *  it reports termination message
 *
 */
void	XdbdWorker::StopWorker (XdbdRunningContext *ctx)
{
#if defined (XDBD_HAM)
	if (m_hamThread != 0)
		m_hamThread->Terminate ();
#endif
	XdbdXmlDataBase::ReleaseStatAdapter();
	XdbdXmlDataBase::UnloadXmlDataBase();
	xdbdErrReport (SC_XDBD, SC_ERR, "xdbd worker terminated, PID = %d", getpid());
}

/*! @brief time-hook routine
 *
 *  this routine is invoked by working environment of I/O multiplexer
 *  if system time changes; useless routine
 *
 */
void	XdbdWorker::TimeHookWorker (XdbdRunningContext *ctx, struct timespec oldTime, struct timespec newTime, long long timeDiff)
{

}

/*! @brief controller pipes handler
 *
 *  this function is registered with I/O multiplexing mechanism to handle
 *  I/O activity between working and controlling entities through pipes
 *  connecting them:
 *
 *  - input pipe: when this pipe is closed, working entity will report
 *  appropriate message and release all allocated resources causing it
 *  to cease
 *
 *  - output pipe: output pipe activity is needed to communicate messages
 *  to controlling entity. These messages can be created by working entity
 *  itself: detecting hangup causes stop request to be sent to controller.
 *  Administrative requests created by ordinary clients willing to restart
 *  or stop server, will also be transfered to controlling entity.
 *
 *  - exceptions: all I/O exceptions will cause working entity to release
 *  its resources causing it to cease
 *
 */
void	XdbdWorker::HandleMainLocalClientReader (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	if (flags & EPOLLIN)
	{
		int	count;
		char	buff[1024];
		if ((count = read (fd, buff, 1024)) <= 0)
		{
			if (count == 0)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd controller connection closed"));
			else
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("recv() failed, errno = %d"), errno);
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd worker terminating, PID = %d"), getpid());
			Release ();
			return;
		}
	}
	else	if (flags & EPOLLOUT)
	{
		size_t	size = m_outputPtr - m_outputBuffer;
		ssize_t	count = write (fd, m_outputBuffer, size);
		if (count <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("send() failed, errno = %d"), errno);
			Release ();
			return;
		}
		if ((size_t)count < size)
			memcpy (m_outputBuffer, m_outputBuffer + count, size - count);
		else
			DisableDescriptor(handler, EPOLLOUT);
		m_outputPtr -= count;
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd worker terminating, PID = %d"), getpid());
		Release ();
	}
}

/*! @brief controller pipes handler
 *
 *  this function is registered with I/O multiplexing mechanism to handle
 *  I/O activity between working and controlling entities through pipes
 *  connecting them:
 *
 *  - input pipe: when this pipe is closed, working entity will report
 *  appropriate message and release all allocated resources causing it
 *  to cease
 *
 *  - output pipe: output pipe activity is needed to communicate messages
 *  to controlling entity. These messages can be created by working entity
 *  itself: detecting hangup causes stop request to be sent to controller.
 *  Administrative requests created by ordinary clients willing to restart
 *  or stop server, will also be transfered to controlling entity.
 *
 *  - exceptions: all I/O exceptions will cause working entity to release
 *  its resources causing it to cease
 *
 */
void	XdbdWorker::HandleMainLocalClientWriter (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	if (flags & EPOLLIN)
	{
		int	count;
		char	buff[1024];
		if ((count = read (fd, buff, 1024)) <= 0)
		{
			if (count == 0)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd controller connection closed"));
			else
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("recv() failed, errno = %d"), errno);
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd worker terminating, PID = %d"), getpid());
			Release ();
			return;
		}
	}
	else	if (flags & EPOLLOUT)
	{
		size_t	size = m_outputPtr - m_outputBuffer;
		ssize_t	count = write (fd, m_outputBuffer, size);
		if (count <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("send() failed, errno = %d"), errno);
			Release ();
			return;
		}
		if ((size_t)count < size)
			memcpy (m_outputBuffer, m_outputBuffer + count, size - count);
		else
			DisableDescriptor(handler, EPOLLOUT);
		m_outputPtr -= count;
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdbd worker terminating, PID = %d"), getpid());
		Release ();
	}
}

/*! @brief UNIX domain listening socket handler
 *
 *  this function accepts local client connections (administrative and SQL requests)
 *  Every client is set in nonblocking I/O mode. After that its FD is sent to
 *  selected client driver thread (current client driver). If any of these steps fail,
 *  client FD is closed and appropriate error message is reported.
 *
 *  @param ctx I/O multiplexer reference (this object's superclass part)
 *  @param flags I/O flags (EPOLLIN, EPOLLOUT, ...) only EPOLLIN is valid for listening socket
 *  @param handler I/O multiplexing handler associated with this function
 *  @param fd UNIX domain listening socket
 *
 */
void	XdbdWorker::HandleLocalListener (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	int	localClientSocket;
	int	gflags;
	struct sockaddr_un addr;
	socklen_t slen = sizeof(struct sockaddr_un);

	memset (&addr, 0, sizeof(struct sockaddr_un));
	if ((localClientSocket = accept (fd, (struct sockaddr*) &addr, &slen)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("accept() failed, errno = %d"), errno);
		return;
	}
	if ((gflags = fcntl (localClientSocket, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_GETFL) failed, errno = %d"), errno);
		close (localClientSocket);
		return;
	}
	if (fcntl (localClientSocket, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_SETFL) failed, errno = %d"), errno);
		close (localClientSocket);
		return;
	}
	if (XdbdClientDriver::currentClientDriver()->SendLocalClientHandlerActivation(localClientSocket) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot activate local client"));
		close (localClientSocket);
		return;
	}
}

/*! @brief TCP/IP listening socket handler
 *
 *  this function accepts network client connections (administrative and SQL requests)
 *  Every client is set in nonblocking I/O mode. After that its FD is sent to
 *  selected client driver thread (current client driver). If any of these steps fail,
 *  client FD is closed and appropriate error message is reported.
 *
 *  @param ctx I/O multiplexer reference (this object's superclass part)
 *  @param flags I/O flags (EPOLLIN, EPOLLOUT, ...) only EPOLLIN is valid for listening socket
 *  @param handler I/O multiplexing handler associated with this function
 *  @param fd TCP/IP listening socket
 *
 */
void	XdbdWorker::HandleNetworkListener (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	int	networkClientSocket;
	int	gflags;
	struct sockaddr_in addr;
	socklen_t slen = sizeof(struct sockaddr_in);

	memset (&addr, 0, sizeof(struct sockaddr_in));
	if ((networkClientSocket = accept (fd, (struct sockaddr*) &addr, &slen)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("accept() failed, errno = %d"), errno);
		return;
	}
	if ((gflags = fcntl (networkClientSocket, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_GETFL) failed, errno = %d"), errno);
		close (networkClientSocket);
		return;
	}
	if (fcntl (networkClientSocket, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_SETFL) failed, errno = %d"), errno);
		close (networkClientSocket);
		return;
	}
}

/*! @brief message queue listening socket handler
 *
 *  this function accepts message queue client connections (administrative and SQL requests)
 *  Every client is set in nonblocking I/O mode. After that its FD is sent to
 *  selected client driver thread (current client driver). If any of these steps fail,
 *  client FD is closed and appropriate error message is reported.
 *
 *  @param ctx I/O multiplexer reference (this object's superclass part)
 *  @param flags I/O flags (EPOLLIN, EPOLLOUT, ...) only EPOLLIN is valid for listening socket
 *  @param handler I/O multiplexing handler associated with this function
 *  @param fd message queue listening socket
 *
 */
void	XdbdWorker::HandleMqListener (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{

}

/*! @brief post administrative request
 *
 *  when HANGUP signal is detected, stop message (g_stopRequest) need to
 *  be sent to controlling entity. And that is exactly what this function
 *  do: it allocates sufficient space, serializes message and enables
 *  output processing on output pipe connection with controlling entity
 *
 *  @param cln unused reference to local client connection, since client
 *  is actually known: controlling entity is nothing but special client
 *  obeying same communication rules as any other client
 *  @param req request reference
 *
 */
void	XdbdWorker::HandleAdminRequest (XdbdLocalClient* cln, XdbdRequest* req)
{
	u_int	msgSize = xdr_sizeof ((xdrproc_t)xdr_XdbdRequest, req);
	u_int	needSpace = msgSize + m_intSize;
	u_int	usedSpace = m_outputPtr - m_outputBuffer;
	u_int	freeSpace = m_outputEnd - m_outputPtr;

	if (freeSpace < needSpace)
	{
		needSpace += usedSpace;
		needSpace >>= 10;
		needSpace++;
		needSpace <<= 10;

		u_char*	buffer = (u_char*) malloc (needSpace);
		if (buffer == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc() failed, errno = %d"), errno);
			return;
		}

		if (m_outputBuffer != 0)
		{
			memcpy (buffer, m_outputBuffer, usedSpace);
			free (m_outputBuffer);
		}
		m_outputBuffer = buffer;
		m_outputPtr = buffer + usedSpace;
		m_outputEnd = buffer + needSpace;
		freeSpace = needSpace - usedSpace;
	}

	XDR	xdr;

	xdrmem_create(&xdr, (char*) m_outputPtr, freeSpace, XDR_ENCODE);
	if (xdr_int(&xdr, (int*) &msgSize) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_int() failed"));
		xdr_destroy (&xdr);
		return;
	}
	xdr_destroy (&xdr);

	xdrmem_create(&xdr, (char*) (m_outputPtr + m_intSize), freeSpace - m_intSize, XDR_ENCODE);
	if (xdr_XdbdRequest (&xdr, req) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_XdbdRequest() failed"));
		xdr_destroy (&xdr);
		return;
	}
	xdr_destroy (&xdr);

	u_int	pos = xdr_getpos (&xdr);
	m_outputPtr += pos + m_intSize;

	EnableDescriptor (m_localClientWriterHandler, EPOLLOUT);
}

} /* namespace xml_db_daemon */

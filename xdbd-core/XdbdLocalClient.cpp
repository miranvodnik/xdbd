/*
 * XdbdLocalClient.cpp
 *
 *  Created on: 30. okt. 2015
 *      Author: miran
 */

#include <fcntl.h>
#include <sys/ioctl.h>
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
#include "XdbdLocalClient.h"
#include "XdbdClientDriver.h"
#include "XdbdWorker.h"
#include "XdbdMessages.h"
#include "XdbdSqlConnectJob.h"
#include "XdbdSqlDisconnectJob.h"
#include "XdbdSqlCreateStatementJob.h"
#include "XdbdSqlDeleteStatementJob.h"
#include "XdbdSqlPrepareJob.h"
#include "XdbdSqlExecuteJob.h"
#include "XdbdSqlExecuteDirectJob.h"

#include <sstream>
using namespace std;

namespace xml_db_daemon
{

const char*	XdbdLocalClient::g_errorString[] =
{
		/*	0	*/	"no error",
		/*	1	*/	"not connected",
		/*	2	*/	"already connected",
		/*	3	*/	"unknown sql request",
};
int XdbdLocalClient::g_reqIndex = 0;

/*! @brief create an instance class representing UNIX domain client connection with XML DB server
 *
 *  initialize internal I/O buffers (initialy empty) and creates I/O handler for XML DB
 *  server connection FD by registering HandleLocalClient() as I/O handler for that FD.
 *  It also initializes its superclass interface XdbdWorkingClient
 *
 *  @param ctx reference to I/O multiplexer which will handle its I/O activity
 *  @param fd UNIX domain socket FD connecting this client with XML DB server
 *  @param mainThread local client driver thread reference (it should imply I/O multiplexer)
 *
 */
XdbdLocalClient::XdbdLocalClient(XdbdRunningContext* ctx, int fd, XdbdRunnable* mainThread) : XdbdWorkingClient (mainThread)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "CREATE XdbdLocalClient %ld", this);
#endif
	m_ctx = ctx;
	m_fd = fd;

	m_connected = false;

	m_localHandler = ctx->RegisterDescriptor (EPOLLIN, fd, HandleLocalClient, this, ctx_info);

	int	dummy = 0;
	m_intSize = xdr_sizeof ((xdrproc_t)xdr_int, &dummy);

	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;

	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	m_errorList = 0;
}

/*! @brief destroy all allocated resources
 *
 */
XdbdLocalClient::~XdbdLocalClient()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "DELETE XdbdLocalClient %ld", this);
#endif
	Release ();
}

/*! @brief destroy all allocated resources
 *
 *  release all allocated resources: I/O handler, I/O internal buffers,
 *  allocated shared memory objects and list of error reporting objects
 *
 */
void XdbdLocalClient::Release ()
{
	if (m_localHandler != 0)
		m_ctx->RemoveDescriptor(m_localHandler);
	m_localHandler = 0;

	if (m_fd > 0)
		close (m_fd);
	m_fd = 0;

	if (m_inputBuffer != 0)
		free (m_inputBuffer);
	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;

	if (m_outputBuffer != 0)
		free (m_outputBuffer);
	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	for (shmset::iterator it = m_shmset.begin(); it != m_shmset.end(); ++it)
	{
		delete	*it;
	}
	m_shmset.clear();

	if (m_errorList != 0)
		xdr_free ((xdrproc_t) xdr_XdbdError, (char*) m_errorList);

	((XdbdClientDriver*)mainThread())->ReleaseLocalClient(this);
}

/*! @brief delete specific SHM segment
 *
 *  function frees specific XdbdShmSegment object indicated with ticket number. This
 *  ticket is actually a reference to this object communicated fort and back between
 *  different entities competing in communication between them
 *
 *  @param ticket long integer representing XdbdShmSegment object to be erased
 *
 *  @return 0 ticket probably does not represent existing object
 *  @return ticket associated object erased
 *
 */
XdbdShmSegment*	XdbdLocalClient::deleteShmSegment (u_long ticket)
{
	shmset::iterator	it = m_shmset.find((XdbdShmSegment*) ticket);
	if (it == m_shmset.end())
		return	0;
	delete	*it;
	m_shmset.erase(it);
	return	(XdbdShmSegment*) ticket;
}

/*! @brief I/O handler for UNIX domain client
 *
 *  function processes input, output and exception processing for
 *  UNIX domain client I/O activity
 *
 *  Input processing: allocates sufficient space in input buffer for all
 *  incoming messages, deserializes them and processes them one by one
 *  in incoming order
 *
 *  Output processing: send as many posted requests as possible and
 *  rearrange output buffer appropriately
 *
 *  Exception processing: release I/O resources causing client to cease
 *
 *  All errors in steps implicitly or explicitly appearing in above
 *  statements have fatal consequences: I/O resources are released
 *  causing client to cease
 *
 *  @param ctx I/O multiplexer reference (provided by constructor)
 *  @param flags I/O processing type (input - EPOLLIN, output - EPOLLOUT, exception - other)
 *  @param handler handler reference associated with this function
 *  @param fd UNIX domain socket FD for whom this handler was registered
 *
 */
void	XdbdLocalClient::HandleLocalClient (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	if (flags & EPOLLIN)
	{
		int	needSpace = 0;
		int	usedSpace = m_inputPtr - m_inputBuffer;
		int	freeSpace = m_inputEnd - m_inputPtr;

		if (ioctl (fd, FIONREAD, &needSpace) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client request, ioctl() failed, errno = %d"), errno);
			delete this;
			return;
		}

		if (needSpace == 0)
		{
			xdbdErrReport (SC_XDBD, SC_APL, err_info("cannot receive client request, no more data"));
			delete this;
			return;
		}

		if (freeSpace < needSpace)
		{
			needSpace += usedSpace;
			needSpace >>= 10;
			needSpace++;
			needSpace <<= 10;
			u_char*	buffer = (u_char*) malloc (needSpace);
			if (buffer == 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client request, malloc() failed, errno = %d"), errno);
				delete this;
				return;
			}
			if (m_inputBuffer != 0)
			{
				memcpy (buffer, m_inputBuffer, usedSpace);
				free (m_inputBuffer);
			}
			m_inputBuffer = buffer;
			m_inputPtr = buffer + usedSpace;
			m_inputEnd = buffer + needSpace;
			freeSpace = needSpace - usedSpace;
		}

		int	recvSize;
		if ((recvSize = recv (fd, m_inputPtr, freeSpace, 0)) <= 0)
		{
			if (errno == EWOULDBLOCK)
				return;
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client request, recv() failed, errno = %d"), errno);
			delete this;
			return;
		}

		usedSpace += recvSize;

		u_char*	ptr = m_inputBuffer;
		while (true)
		{
			if (usedSpace < (int) m_intSize)
				break;

			XDR	xdr;

			int	msgSize;
			xdrmem_create(&xdr, (char*) ptr, m_intSize, XDR_DECODE);
			if (xdr_int (&xdr, &msgSize) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot decode client message size"));
				xdr_destroy (&xdr);
				return;
			}
			xdr_destroy (&xdr);

			if (usedSpace < msgSize + (int) m_intSize)
				break;

			XdbdRequest*	req = new XdbdRequest;
			memset (req, 0, sizeof (XdbdRequest));
			xdrmem_create(&xdr, (char*) (ptr + m_intSize), msgSize, XDR_DECODE);
			if (xdr_XdbdRequest(&xdr, req) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot decode client message"));
				xdr_destroy (&xdr);
				return;
			}
			xdr_destroy (&xdr);

			ptr += msgSize + m_intSize;
			usedSpace -= msgSize + m_intSize;
			if (usedSpace > 0)
				memcpy (m_inputBuffer, ptr, usedSpace);
			m_inputPtr = m_inputBuffer + usedSpace;

			DisplayXdbdRequest(req);

			switch (req->m_requestType)
			{
			case	AdminRequestCode:
				{
					((XdbdClientDriver*)mainThread())->HandleAdminRequest (this, req);
					((XdbdClientDriver*)mainThread())->ReleaseLocalClient (this);
					xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
					delete	req;
					delete	this;
				}
				break;
			case	SqlRequestCode:
				switch (req->XdbdRequest_u.m_sqlRequest.m_command)
				{
				case	XdbdSqlConnectWord:
					HandleSqlConnectRequest (req);
					break;
				case	XdbdSqlDisconnectWord:
					HandleSqlDisconnectRequest (req);
					break;
				case	XdbdSqlCreateStatementWord:
					HandleSqlCreateStatementRequest (req);
					break;
				case	XdbdSqlDeleteStatementWord:
					HandleSqlDeleteStatementRequest (req);
					break;
				case	XdbdSqlPrepareWord:
					HandleSqlPrepareRequest (req);
					break;
				case	XdbdSqlExecuteWord:
					HandleSqlExecuteRequest (req);
					break;
				case	XdbdSqlExecuteDirectWord:
					HandleSqlExecuteDirectRequest (req);
					break;
				default:
					xdbdErrReport (SC_XDBD, SC_ERR, err_info("unknown client message code = %d"), req->XdbdRequest_u.m_sqlRequest.m_command);
					HandleSqlUnknownRequest (req);
					break;
				}
				break;
			default:
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("unknown client message type = %d"), req->m_requestType);
				xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
				delete	req;
				delete this;
				break;
			}
		}
	}
	else	if (flags & EPOLLOUT)
	{
		size_t	size = m_outputPtr - m_outputBuffer;
		ssize_t	count = send (fd, m_outputBuffer, size, 0);
		if (count <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot send client reply, send() failed, errno = %d"), errno);
			delete this;
			return;
		}
		if ((size_t)count < size)
			memcpy (m_outputBuffer, m_outputBuffer + count, size - count);
		else
			ctx->DisableDescriptor(handler, EPOLLOUT);
		m_outputPtr -= count;
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot determine client I/O type = %d"), flags);
		Release ();
//		delete this;
	}
}

/*! @brief first half call-back
 *
 *  every SQL job provided by UNIX domain client are processed by working threads
 *  represented with XdbdWorkingThread class instances, which does not take account for
 *  multiplexing since this is not possible to do for SQL jobs. They are processed
 *  sequentially by specific thread, though as many in parallel as there are working threads.
 *  As soon as they are processed, they need to be communicated back to actual XML DB
 *  clients. Since client communication is processed by I/O multiplexers, which run in special client
 *  driver threads, they need to be first communicated back to these multiplexers, which is
 *  done by first_half_callback() function. This function takes address of SQL job,
 *  deduces client driver thread from it and sends a message to it specifying SQL request
 *  reference.
 *
 *  Shortly: main purpose of first_half_callback() function is to communicate SQL job
 *  reference from working thread instance XdbdWorkingThread to correct instance of
 *  XdbdClientDriver instance client driver thread
 *
 *  @param data SQL job reference
 *
 */
void	XdbdLocalClient::first_half_callback (void* data)
{
	XdbdClientDriver*	clientDriver = (XdbdClientDriver*) mainThread();
	if (clientDriver->SendLocalClientHandlerResponse (this, data) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot send local client response, errno = %d"), errno);
		((XdbdSqlJob*) data)->Cleanup();
	}
}

/*! @brief second half call-back
 *
 *  after SQL job reference is deserialized from client driver internal message
 *  queue, it should be communicated back to associated XML DB client. This is what
 *  this function do: it takes address of SQL job, checks its return status (provided
 *  by an instance of XdbdWorkingThread working thread), forms appropriate SQL reply
 *  and posts it using I/O multiplexer of client driver thread associated with this
 *  UNIX domain client. After that is done (successfully or not) SQL job is deleted.
 *
 *  @param data SQL job reference
 *
 */
void	XdbdLocalClient::second_half_callback (void* data)
{
	m_ctx->EnableDescriptor (m_localHandler, EPOLLIN);
	XdbdSqlJob*	job = (XdbdSqlJob*) data;
	switch (job->request()->XdbdRequest_u.m_sqlRequest.m_command)
	{
	case	XdbdSqlConnectWord:
		{
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
			{
				m_connected = true;
			}
			PostSqlConnectReply (job->request());
		}
		break;
	case	XdbdSqlDisconnectWord:
		{
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			PostSqlDisconnectReply (job->request());
		}
		break;
	case	XdbdSqlCreateStatementWord:
		{
			XdbdShmSegment*	shmSegment = 0;
			XdbdSqlCreateStatementJob*	createStatementJob = (XdbdSqlCreateStatementJob*) job;
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
			{
				shmSegment = new XdbdShmSegment(createStatementJob->shmName());
				m_shmset.insert(shmSegment);
			}
			PostSqlCreateStatementReply (job->request(), shmSegment);
		}
		break;
	case	XdbdSqlDeleteStatementWord:
		{
			XdbdShmSegment*	shmSegment = 0;
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
				shmSegment = ((XdbdSqlDeleteStatementJob*)job)->shmSegment();
			PostSqlDeleteStatementReply (job->request(), shmSegment);
		}
		break;
	case	XdbdSqlPrepareWord:
		{
			XdbdShmSegment*	shmSegment = 0;
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
				(shmSegment = ((XdbdSqlPrepareJob*)job)->shmSegment())->sqlParser(((XdbdSqlPrepareJob*)job)->extractStatement());
			PostSqlPrepareReply (job->request(), shmSegment);
		}
		break;
	case	XdbdSqlExecuteWord:
		{
			XdbdShmSegment*	shmSegment = 0;
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
				shmSegment = ((XdbdSqlExecuteJob*)job)->shmSegment();
			PostSqlExecuteReply (job->request(), shmSegment);
		}
		break;
	case	XdbdSqlExecuteDirectWord:
		{
			XdbdShmSegment*	shmSegment = 0;
			if (job->returnStatus() != 0)
			{
				PostSqlErrorResponse (XdbdSysErrorType, (XdbdErrorCode) job->errorCode(), job->errorMessage().c_str());
			}
			else
				(shmSegment = ((XdbdSqlExecuteDirectJob*)job)->shmSegment())->sqlParser(((XdbdSqlExecuteDirectJob*)job)->extractStatement());
			PostSqlExecuteDirectReply (job->request(), shmSegment);
		}
		break;
	default:
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("illegal reply code = %d"), job->request()->XdbdRequest_u.m_sqlRequest.m_command);
		delete	this;
		break;
	}
	delete	job;
}

/*! @param create message handler
 *
 *  create common message header: version number, message index, UUID,
 *  message type and PID of sending process
 *
 *  @param header reference of message header
 *
 */
static	void	CreateMessageHeader (XdbdMessageHeader* header)
{
	static	int	g_messageIndex = 0;

	header->m_version.m_major = 1;
	header->m_version.m_minor = 0;
	header->m_index = ++g_messageIndex;
	uuid_generate (header->m_hash);
	header->m_error = 0;
	header->m_entity = XdbdCommon::entity();
	header->m_pid = XdbdCommon::pid();
}

/*! @brief post SQL message reply
 *
 *  provide sufficient space in output buffer, serializes SQL message
 *  reply in it and enables output processing which will be done as
 *  soon as possible bey I/O multiplexer
 *
 *  @param rpl reference to posted SQL reply message
 *
 */
void XdbdLocalClient::PostSqlReply (XdbdReply* rpl)
{
	CreateMessageHeader(&rpl->XdbdReply_u.m_header);
	DisplayXdbdReply (rpl);
	if (m_localHandler == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post reply, message handler null"));
		delete	this;
		return;
	}

	rpl->XdbdReply_u.m_sqlReply.m_header.m_error = m_errorList;
	m_errorList = 0;

	u_int	msgSize = xdr_sizeof ((xdrproc_t)xdr_XdbdReply, rpl);
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
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post reply, malloc() failed, errno = %d"), errno);
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
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post reply, failed to encode message size"));
		xdr_destroy (&xdr);
		return;
	}
	xdr_destroy (&xdr);

	xdrmem_create(&xdr, (char*) (m_outputPtr + m_intSize), freeSpace - m_intSize, XDR_ENCODE);
	if (xdr_XdbdReply (&xdr, rpl) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post reply, failed to encode message"));
		xdr_destroy (&xdr);
		return;
	}
	xdr_destroy (&xdr);

	u_int	pos = xdr_getpos (&xdr);
	m_outputPtr += pos + m_intSize;

	m_ctx->EnableDescriptor (m_localHandler, EPOLLOUT);
}

/*! @brief form error response message
 *
 *  prepare error message which will be appended to SQL reply
 *
 *  @param errorType error type
 *  @param errorCode error code
 *  @param errorMessage detailed error message text
 *
 */
void XdbdLocalClient::PostSqlErrorResponse (XdbdErrorType errorType, XdbdErrorCode errorCode, const char* errorMessage)
{
	if (g_debug == true)
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("ERROR: errorType = %d, errorCode = %d, errorMessage = %s"), errorType, errorCode, ((errorMessage == 0) ? "*** NULL ***" : errorMessage));
	XdbdError*	error = new XdbdError;
	error->m_type = errorType;
	error->m_errorCode = errorCode;
	error->m_errorMessage = strdup ((char*) errorMessage);
	error->m_next = m_errorList;
	m_errorList = error;
}

/*! @brief prepare SQL connect request job
 *
 *  function creates an instance of XdbdSqlConnectJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL connect job
 *
 */
void XdbdLocalClient::HandleSqlConnectRequest (XdbdRequest* req)
{
	if (m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlAlreadyConnected, g_errorString[2]);
		PostSqlConnectReply (req);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlConnectJob (this, 0, req));
}

/*! @brief prepare SQL disconnect request job
 *
 *  function creates an instance of XdbdSqlDisconnectJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL disconnect job
 *
 */
void XdbdLocalClient::HandleSqlDisconnectRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlDisconnectReply (req);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlDisconnectJob (this, 0, req));
}

/*! @brief prepare SQL create statement request job
 *
 *  function creates an instance of XdbdSqlCreateStatementJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL create statement job
 *
 */
void XdbdLocalClient::HandleSqlCreateStatementRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlCreateStatementReply (req, 0);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlCreateStatementJob (this, 0, req));
}

/*! @brief prepare SQL delete statement request job
 *
 *  function creates an instance of XdbdSqlDeleteStatementJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL delete statement job
 *
 */
void XdbdLocalClient::HandleSqlDeleteStatementRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlDeleteStatementReply (req, 0);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlDeleteStatementJob (this, 0, req));
}

/*! @brief prepare SQL prepare request job
 *
 *  function creates an instance of XdbdSqlPrepareJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL prepare job
 *
 */
void XdbdLocalClient::HandleSqlPrepareRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlPrepareReply (req, 0);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlPrepareJob (this, 0, req));
}

/*! @brief prepare SQL execute request job
 *
 *  function creates an instance of XdbdSqlExecuteJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL execute job
 *
 */
void XdbdLocalClient::HandleSqlExecuteRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlExecuteReply (req, 0);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlExecuteJob (this, 0, req));
}

/*! @brief prepare SQL execute direct request job
 *
 *  function creates an instance of XdbdSqlExecuteDirectJob and put its reference into message
 *  queue which is read by working threads. Request contains enough information to be
 *  communicated back to I/O multiplexer in which it was created. Input processing for
 *  this client is disabled for the time of SQL job execution, eventually until it will
 *  be handled by second_half_callback()
 *
 *  @param req reference to protocol message representing SQL execute direct job
 *
 */
void XdbdLocalClient::HandleSqlExecuteDirectRequest (XdbdRequest* req)
{
	if (!m_connected)
	{
		PostSqlErrorResponse (XdbdAppErrorType, SqlNotConnected, g_errorString[1]);
		PostSqlExecuteDirectReply (req, 0);
		xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
		delete	req;
		return;
	}
	m_ctx->DisableDescriptor (m_localHandler, EPOLLIN);
	XdbdMessageQueue::Put(new XdbdSqlExecuteDirectJob (this, 0, req));
}

/*! @brief unknown SQL request
 *
 *  post error message for unknown SQL request message
 *
 *  @param req reference to protocol message representing unknown SQL request
 *
 */
void XdbdLocalClient::HandleSqlUnknownRequest (XdbdRequest* req)
{
	PostSqlErrorResponse (XdbdAppErrorType, SqlUnknownRequest, g_errorString[3]);
	PostSqlUnknownReply (req);
	xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) req);
	delete	req;
}

/*! @brief display protocol request
 *
 *  if debugging is enabled (g_debug == true) display all
 *  components of XML DB protocol request message
 *
 *  @param req request reference
 *
 */
void XdbdLocalClient::DisplayXdbdRequest (XdbdRequest* req)
{
	if (g_debug == false)
		return;

	stringstream	str;
	str << "XDBD REQUEST" << endl;
	str << "{" << endl;
	switch (req->m_requestType)
	{
	case	AdminRequestCode:
		{
			str << "\tindex = " << req->XdbdRequest_u.m_adminRequest.m_header.m_index << endl;
			str << "\ttype = admin" << endl;
			str << "\trequest = ";
			switch (req->XdbdRequest_u.m_adminRequest.m_command)
			{
			case	XdbdAdminStartWord:
				str << "start";
				break;
			case	XdbdAdminRestartWord:
				str << "restart";
				break;
			case	XdbdAdminStopWord:
				str << "stop";
				break;
			case	XdbdAdminStatusWord:
				str << "status";
				break;
			default:
				str << "unknown";
				break;
			}
			str << endl;
		}
		break;
	case	SqlRequestCode:
		{
			str << "\tindex = " << req->XdbdRequest_u.m_sqlRequest.m_header.m_index << endl;
			str << "\ttype = sql" << endl;
			str << "\trequest = ";
			switch (req->XdbdRequest_u.m_sqlRequest.m_command)
			{
			case	XdbdSqlConnectWord:
				str << "Connect";
				break;
			case	XdbdSqlDisconnectWord:
				str << "Disconnect";
				break;
			case	XdbdSqlCreateStatementWord:
				str << "CreateStatement";
				break;
			case	XdbdSqlDeleteStatementWord:
				str << "DeleteStatement";
				break;
			case	XdbdSqlPrepareWord:
				str << "Prepare";
				break;
			case	XdbdSqlExecuteWord:
				str << "Execute";
				break;
			case	XdbdSqlExecuteDirectWord:
				str << "ExecuteDirect";
				break;
			default:
				str << "Unknown";
				break;
			}
			str << endl;
			u_int	parlen = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_len;
			XdbdParameter*	pars = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_val;
			for (u_int i = 0; i < parlen; ++i)
			{
				str << "\tpar " << i << ": ";
				if (pars[i].m_flags & INTVAL)
					str << pars[i].m_longVal << " ";
				if (pars[i].m_flags & STRVAL)
					str << pars[i].m_stringVal;
				str << endl;
			}
		}
		break;
	default:
		str << "\ttype = unknown" << endl;
		break;
	}
	str << "}" << endl;
	str << ends;

	const char*	msg = str.str().c_str();
	xdbdErrReport(SC_XDBD, SC_ERR, msg);
}

/*! @brief display protocol reply
 *
 *  if debugging is enabled (g_debug == true) display all
 *  components of XML DB protocol reply message
 *
 *  @param req request reference
 *
 */
void XdbdLocalClient::DisplayXdbdReply (XdbdReply* rpl)
{
	if (g_debug == false)
		return;

	stringstream	str;
	str << "XDBD REPLY" << endl;
	str << "{" << endl;

	str << "\tindex = " << rpl->XdbdReply_u.m_sqlReply.m_header.m_index << endl;
	for (XdbdError* error = rpl->XdbdReply_u.m_sqlReply.m_header.m_error; error != 0; error = error->m_next)
	{
		str << "\terror" << endl;
		str << "{" << endl;
		str << "\t\terrorType = " << error->m_type << endl;
		str << "\t\terrorCode = " << error->m_errorCode << endl;
		str << "\t\terrorMsg  = " << error->m_errorMessage << endl;
		str << "}" << endl;
	}
	str << "\ttype = ";
	switch (rpl->m_replyType)
	{
	case	AdminReplyCode:
		str << "admin";
		break;
	case	SqlReplyCode:
		str << "sql";
		break;
	default:
		str << "unknown";
		break;
	}
	str << endl;
	str << "\trequest = ";
	switch (rpl->XdbdReply_u.m_sqlReply.m_command)
	{
		case	XdbdAdminStartWord:
			str << "start";
			break;
		case	XdbdAdminRestartWord:
			str << "restart";
			break;
		case	XdbdAdminStopWord:
			str << "stop";
			break;
		case	XdbdAdminStatusWord:
			str << "status";
			break;
		case	XdbdSqlConnectWord:
			str << "Connect";
			break;
		case	XdbdSqlDisconnectWord:
			str << "Disconnect";
			break;
		case	XdbdSqlCreateStatementWord:
			str << "CreateStatement";
			break;
		case	XdbdSqlDeleteStatementWord:
			str << "DeleteStatement";
			break;
		case	XdbdSqlPrepareWord:
			str << "Prepare";
			break;
		case	XdbdSqlExecuteWord:
			str << "Execute";
			break;
		case	XdbdSqlExecuteDirectWord:
			str << "ExecuteDirect";
			break;
		default:
			str << "Unknown";
			break;
	}
	str << endl;
	for (u_int i = 0; i < rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len; ++i)
	{
		XdbdParameter*	par = rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val + i;
		str << "\tparameter " << i << ": ";
		if (par->m_flags & INTVAL)
			str << par->m_longVal << " ";
		if (par->m_flags & STRVAL)
			str << par->m_stringVal;
		str << endl;
	}
	str << "}" << endl;
	str << ends;

	const char*	msg = str.str().c_str();
	xdbdErrReport(SC_XDBD, SC_ERR, msg);
}

/*! @brief post connect reply message
 *
 *  create connect reply message and post it into internal output buffer
 *  of local client connection object
 *
 *  @param req associated connect request message
 *
 */
void XdbdLocalClient::PostSqlConnectReply (XdbdRequest* req)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 0;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = 0;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post disconnect reply message
 *
 *  create disconnect reply message and post it into internal output buffer
 *  of local client connection object
 *
 *  @param req associated disconnect request message
 *
 */
void XdbdLocalClient::PostSqlDisconnectReply (XdbdRequest* req)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 0;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = 0;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post create-statement reply message
 *
 *  create create-statement reply message and post it into internal output buffer
 *  of local client connection object. Message has two additional parameters:
 *  first is address of shared memory segment associated with SQL statement, the
 *  second one is its name. This segment will be used to exchange data between
 *  XML DB server and its client to exchanged data retrieved by executing SQL
 *  statements
 *
 *  @param req associated create-statement request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlCreateStatementReply (XdbdRequest* req, XdbdShmSegment* shmSegment)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	XdbdParameter*	parameters = new XdbdParameter[1];
	parameters[0].m_flags = INTVAL|STRVAL;
	parameters[0].m_longVal = (u_long) shmSegment;
	parameters[0].m_stringVal = strdup ((shmSegment != 0) ? shmSegment->shmName().c_str() : "");
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 1;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = parameters;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post delete-statement reply message
 *
 *  create delete-statement reply message and post it into internal output buffer
 *  of local client connection object.
 *
 *  @param req associated delete-statement request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlDeleteStatementReply (XdbdRequest* req, XdbdShmSegment* shmSegment)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 0;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = 0;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post prepare reply message
 *
 *  create prepare reply message and post it into internal output buffer
 *  of local client connection object. Message has two additional parameters:
 *  first is size of shared memory segment associated with SQL statement, the
 *  second one is size of its header.
 *
 *  @param req associated prepare request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlPrepareReply (XdbdRequest* req, XdbdShmSegment* shmSegment)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	XdbdParameter*	parameters = new XdbdParameter[2];
	parameters[0].m_flags = INTVAL;
	parameters[0].m_longVal = (shmSegment != 0) ? shmSegment->shmSize() : 0;
	parameters[0].m_stringVal = strdup ("");
	parameters[1].m_flags = INTVAL;
	parameters[1].m_longVal = (shmSegment != 0) ? shmSegment->hdrSize() : 0;
	parameters[1].m_stringVal = strdup ("");
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 2;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = parameters;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post execute reply message
 *
 *  create execute reply message and post it into internal output buffer
 *  of local client connection object. Message has two additional parameters:
 *  first is size of shared memory segment associated with SQL statement, the
 *  second one is size of its header. Header contains description of fields
 *  following it. Every field is described by its type actual and reserved size.
 *  Fields are nothing but actual values of data retrieved from XML DB
 *
 *  @param req associated execute request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlExecuteReply (XdbdRequest* req, XdbdShmSegment* shmSegment)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	XdbdParameter*	parameters = new XdbdParameter[2];
	parameters[0].m_flags = INTVAL;
	parameters[0].m_longVal = (shmSegment != 0) ? shmSegment->shmSize() : 0;
	parameters[0].m_stringVal = strdup ("");
	parameters[1].m_flags = INTVAL;
	parameters[1].m_longVal = (shmSegment != 0) ? shmSegment->hdrSize() : 0;
	parameters[1].m_stringVal = strdup ("");
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 2;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = parameters;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post execute-direct reply message
 *
 *  create execute-direct reply message and post it into internal output buffer
 *  of local client connection object. Message has two additional parameters:
 *  first is size of shared memory segment associated with SQL statement, the
 *  second one is size of its header. Header contains description of fields
 *  following it. Every field is described by its type actual and reserved size.
 *  Fields are nothing but actual values of data retrieved from XML DB
 *
 *  @param req associated execute request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlExecuteDirectReply (XdbdRequest* req, XdbdShmSegment* shmSegment)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	XdbdParameter*	parameters = new XdbdParameter[2];
	parameters[0].m_flags = INTVAL;
	parameters[0].m_longVal = (shmSegment != 0) ? shmSegment->shmSize() : 0;
	parameters[0].m_stringVal = strdup ("");
	parameters[1].m_flags = INTVAL;
	parameters[1].m_longVal = (shmSegment != 0) ? shmSegment->hdrSize() : 0;
	parameters[1].m_stringVal = strdup ("");
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 2;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = parameters;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief post reply to unknown message
 *
 *  create SQL reply message with the same message type as SQL messsage request
 *  and post it into internal output buffer
 *
 *  @param req associated execute request message
 *  @param shmSegment shared memory object associated with SQL statement
 *
 */
void XdbdLocalClient::PostSqlUnknownReply (XdbdRequest* req)
{
	XdbdReply*	rpl = new XdbdReply;
	rpl->m_replyType = SqlReplyCode;
	CreateMessageHeader (&rpl->XdbdReply_u.m_header);
	rpl->XdbdReply_u.m_sqlReply.m_command = req->XdbdRequest_u.m_sqlRequest.m_command;

	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_len = 0;
	rpl->XdbdReply_u.m_sqlReply.m_parameters.m_parameters_val = 0;

	PostSqlReply (rpl);
	xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) rpl);
	delete	rpl;
}

/*! @brief XML DB client activation
 *
 *  main thread of working entity accepts client connections, selects
 *  one of client driver threads and sends it 'client activation' message
 *  consisting mainly of client connection FD. After this message has been
 *  processed by actual client driver thread, connection is declared
 *  'activated'. After activation all client server communication is
 *  processed by I/O multiplexer residing in client driver thread.
 *
 *  @param ctx I/O multiplexer reference
 *
 */
int	XdbdLocalClientActivationMessage::Invoke (XdbdRunningContext* ctx)
{
	((XdbdClientDriver*)m_clientDriver)->ActivateLocalClientHandler(m_localClientFd);
	return	0;
}

/*! @brief post client response messages
 *
 *  all client responses are actually generated as a result of SQL jobs
 *  processed by working threads. Since client communication is governed
 *  by client driver threads these responses must also be communicated by
 *  them and not by working threads where they are actually created. This
 *  function does this job. It posts reply messages generated by working
 *  threads into internal output buffers of I/O multiplexer processing
 *  client communication
 *
 *  @param ctx I/O multiplexer reference
 *
 */
int	XdbdLocalClientResponseMessage::Invoke (XdbdRunningContext* ctx)
{
	m_localClient->second_half_callback(m_data);
	return	0;
}

} /* namespace xml_db_daemon */

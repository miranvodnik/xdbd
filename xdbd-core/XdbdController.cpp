/*
 * XdbdController.cpp
 *
 *  Created on: 28. okt. 2015
 *      Author: miran
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
#include "XdbdController.h"
#include "XdbdMessages.h"
using namespace xml_db_daemon;

namespace xml_db_daemon
{

const	string	XdbdController::g_localSocketPath = ::g_localSocketPath;	//!< UNIX domain socket name
const	string	XdbdController::g_localMqPath = ::g_localMqPath;	//!< message queue name
const	int	XdbdController::g_tcpSocketNumber = ::g_tcpSocketNumber;	//!< network socket number
const	int	XdbdController::g_localListener = ::g_localListener;	//!< working process's UNIX domain socket redirection
const	int	XdbdController::g_networkListener = ::g_networkListener;	//!< working process's network socket redirection
const	int	XdbdController::g_mqListener = ::g_mqListener;	//!< working process's message queue redirection
int	XdbdController::g_messageIndex = 0;

/*! @brief controlling entity object constructor
 *
 *  It provides start, stop and time hook routines to its superclass;
 *  It fetches command parameters
 *  It initializes internal data structures
 *
 */
XdbdController::XdbdController (int n, char**p) : XdbdRunningContext (StartController, StopController, TimeHookController, this, "xdbd-ctl")
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdController %ld", this);
#endif
	if (n < 2)
	{
		m_commandType = XdbdAdminWord;
		m_commandCode = XdbdAdminStatusWord;
	}
	else
	{
		m_commandType = XdbdCommon::findWord (p[1]);
		if (n < 3)
			m_commandCode = XdbdAdminStatusWord;
		else
			m_commandCode = XdbdCommon::findWord (p[2]);
	}

	m_argc = n;
	m_argv = p;

	m_sync = -1;

	m_localListenFd = -1;
	m_localClientFd = -1;
	m_masterClientReaderFd = -1;
	m_masterClientWriterFd = -1;

	m_networkListenFd = -1;
	m_networkClientFd = -1;

	m_mqListenFd = -1;
	m_mqClientFd = -1;

	m_worker = (pid_t) -1;

	m_localClientHandler = 0;
	m_masterClientReaderHandler = 0;
	m_masterClientWriterHandler = 0;

	m_localClientIOTimer = 0;
	m_masterClientIOTimer = 0;
	m_sigcldSignalHandler = 0;
	m_hangupSignalHandler = 0;

	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;

	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	int	dummy = 0;
	m_intSize = xdr_sizeof ((xdrproc_t)xdr_int, &dummy);
	m_isController = false;
	m_deamonized = false;
}

/*! brief controlling entity object destructor
 *
 *  it releases all internal data structures
 *
 */
XdbdController::~XdbdController()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdController %ld", this);
#endif
	Release ();
}

/*! brief start routine
 *
 *  Implementation of start routine required by XdbdRunningContext interface.
 *  If program has been given meaningful command parameters then
 *  this routine makes first step of quite simple procedure which
 *  will enable correct invocation of program regardless of how many
 *  instances of program are started simultaneously. Main purpose of
 *  this procedure is, that mostly one of these instances execute
 *  successfully as process controller. All other instances will simply
 *  cease or execute as client
 *
 *  First step of program invocation is simple: if program parameters
 *  are meaningful then the second step is prepared with registering
 *  timer handler for AccessSyncPath(), which will be invoked as soon
 *  as possible
 *
 *  @param ctx reference to superclass part of this object
 *
 *  @see XdbdRunningContext
 *
 */
void	XdbdController::StartController (XdbdRunningContext *ctx)
{
	if (m_commandCode != XdbdFirstWord)
		ctx->RegisterTimer (ctx->realTime(), AccessSyncPath, this, ctx_info);
}

/*! brief stop routine
 *
 *  Implementation of stop routine required by XdbdRunningContext interface.
 *  It reports message
 *
 *  @param ctx reference to superclass part of this object
 *
 *  @see XdbdRunningContext
 *
 */
void	XdbdController::StopController (XdbdRunningContext *ctx)
{
	if (m_isController)
		xdbdErrReport (SC_XDBD, SC_ERR, "XDBD controller terminated, PID = %d", getpid());
}

/*! brief time hook routine
 *
 *  Implementation of time hook routine required by XdbdRunningContext interface.
 *  It reports time change message
 *
 *  @param ctx reference to superclass part of this object
 *  @param oldTime time stamp before time change
 *  @param new Time time stamp after time change
 *  @param timeDiff difference between time stamps expressed in nanoseconds
 *
 *  @see XdbdRunningContext
 *
 */
void	XdbdController::TimeHookController (XdbdRunningContext *ctx, struct timespec oldTime, struct timespec newTime, long long timeDiff)
{
	xdbdErrReport (SC_XDBD, SC_ERR, "XDBD controller time correction: old time = %d,%06d, new time = %d,%06d",
		oldTime.tv_sec, oldTime.tv_nsec, newTime.tv_sec, newTime.tv_nsec);
}

/*! @brief release procedure
 *
 *  releases all internal data structures
 *
 */
void	XdbdController::Release ()
{
	m_sync = -1;

	if (m_localListenFd > 0)
		close (m_localListenFd);
	m_localListenFd = -1;

	if (m_localClientFd > 0)
		close (m_localClientFd);
	m_localClientFd = -1;

	if (m_masterClientReaderFd > 0)
		close (m_masterClientReaderFd);
	m_masterClientReaderFd = -1;

	if (m_masterClientWriterFd > 0)
		close (m_masterClientWriterFd);
	m_masterClientWriterFd = -1;

	if (m_networkListenFd > 0)
		close (m_networkListenFd);
	m_networkListenFd = -1;

	if (m_networkClientFd > 0)
		close (m_networkClientFd);
	m_networkClientFd = -1;

	if (m_mqListenFd > 0)
		mq_close(m_mqListenFd);
	m_mqListenFd = -1;
	mq_unlink (g_localMqPath.c_str());

	if (m_mqClientFd > 0)
		mq_close(m_mqClientFd);
	m_mqClientFd = -1;

	m_worker = (pid_t) -1;

	if (m_localClientHandler != 0)
		RemoveDescriptor(m_localClientHandler);
	m_localClientHandler = 0;

	if (m_masterClientReaderHandler != 0)
		RemoveDescriptor(m_masterClientReaderHandler);
	m_masterClientReaderHandler = 0;

	if (m_masterClientWriterHandler != 0)
		RemoveDescriptor(m_masterClientWriterHandler);
	m_masterClientWriterHandler = 0;

	if (m_localClientIOTimer != 0)
		DisableTimer(m_localClientIOTimer);
	m_localClientIOTimer = 0;

	if (m_masterClientIOTimer != 0)
		DisableTimer(m_masterClientIOTimer);
	m_masterClientIOTimer = 0;

	if (m_sigcldSignalHandler != 0)
		RemoveSignal(m_sigcldSignalHandler);
	m_sigcldSignalHandler = 0;

	if (m_hangupSignalHandler != 0)
		RemoveSignal(m_hangupSignalHandler);
	m_hangupSignalHandler = 0;

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

	Stop ();
}

/*! @brief lock controlling path
 *
 *  This function locks synchronization semaphore owned by this object.
 *
 *  @return 0 success - controlling path has been secured
 *  @return -1 failure - controlling path has not been secured
 */
int	XdbdController::LockControllingPath (void)
{
	struct	sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	if (semop (m_sync, &sb, 1) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot lock controlling path, semop() failed, errno = %d"), errno);
		return	-1;
	}
	return	0;
}

/*! @brief unlock controlling path
 *
 *  This function unlocks synchronization semaphore owned by this object.
 *
 *  @return 0 success - controlling path has been unsecured
 *  @return -1 failure - controlling path has not been unsecured
 */
int	XdbdController::UnlockControllingPath (void)
{
	struct	sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	if (semop (m_sync, &sb, 1) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot unlock controlling path, semop() failed, errno = %d"), errno);
		return	-1;
	}
	return	0;
}

/*! @brief access synchronization path
 *
 *  Second step of program invocation: This
 *  function provides access to semaphore with key XDBD_SEM_KEY
 *  either creating or simply accessing it. If that can be done,
 *  function prepares next step by registering timer handler
 *  implemented by DetermineXdbdEntityRole(), which will be
 *  invoked as soon as possible.
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler reference to registration handler of this routine
 *  @param t time stamp of invocation
 *
 */
void	XdbdController::AccessSyncPath (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	if ((m_sync = semget (XDBD_SEM_KEY, 0, 0)) < 0)
	{
		if (errno == EINTR)
		{
			xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot access synchronization path, semget() failed, errno = %d"), errno);
			ctx->RegisterTimer (t, AccessSyncPath, this, ctx_info);
			return;
		}
		if ((m_sync = semget (XDBD_SEM_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)
		{
			if (errno == EINTR)
			{
				xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot access synchronization path, semget() failed, errno = %d"), errno);
				ctx->RegisterTimer (t, AccessSyncPath, this, ctx_info);
				return;
			}
			t.tv_sec += 1;
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot access synchronization path, semget() failed, errno = %d"), errno);
			ctx->RegisterTimer (t, AccessSyncPath, this, ctx_info);
			return;
		}
		semctl (m_sync, 0, SETVAL, 1);
	}
	ctx->RegisterTimer(t, DetermineXdbdEntityRole, this, ctx_info);
}

/*! brief determine process role
 *
 *  Third step of program invocation: this step is secured by semaphore
 *  which ensures that almost one instance of program executes this code
 *  at once.
 *
 *  Program instance, which successfully enter secured section will first
 *  try to create obvious client entity. If that client can be created it is
 *  due to the fact that controller and working entity already exist. In this
 *  case client will be prepared to make next step by registering
 *  DriveXdbdClientEntity() timer, which will be executed as soon as possible
 *
 *  If client cannot be created (probably because controller an working entity
 *  are not running) command codes are checked to see if it is meaningful
 *  to start controlling entity. Only 'start' and 'restart' commands are
 *  allowed to start controller. If that is not the case, program will simply cease.
 *
 *  In case of appropriate command codes ('start' or 'restart') controlling
 *  entity is created. If successful controller will be prepared to make next step
 *  by registering DriveXdbdControllerEntity() timer, which will be executed
 *  as soon as possible.
 *
 *  If even controlling entity cannot be created the whole step is repeated
 *  as soon as possible.
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler reference to registration handler of this routine
 *  @param t time stamp of invocation
 *
 */
void	XdbdController::DetermineXdbdEntityRole (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	if (LockControllingPath () < 0)
	{
		t.tv_sec += 1;
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot determine xdbd entity role"));
		ctx->RegisterTimer(t, DetermineXdbdEntityRole, this, ctx_info);
		return;
	}

	if (CreateClientEntity () == 0)
	{
		UnlockControllingPath ();
		ctx->RegisterTimer (t, DriveXdbdClientEntity, this, ctx_info);
		return;
	}

	if ((m_commandCode != XdbdAdminStartWord) && (m_commandCode != XdbdAdminRestartWord))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("illegal command code for non-client role, code = %d"), m_commandCode);
		UnlockControllingPath ();
		Release ();
		return;
	}

	if (CreateControllerEntity () == 0)
	{
		UnlockControllingPath ();
		ctx->RegisterTimer (t, DriveXdbdControllerEntity, this, ctx_info);
		return;
	}

	xdbdErrReport (SC_XDBD, SC_WRN, err_info("undetermined xdbd entity role, trying again"));
	UnlockControllingPath ();
	t.tv_sec += 1;
	ctx->RegisterTimer (t, DetermineXdbdEntityRole, this, ctx_info);
}

/*! @brief create client entity
 *
 *  Creates client entity: only local UNIX domain client can be created
 *
 *  @return 0 client entity has been created
 *  @return -1 client entity has not been created, error has been logged
 */
int	XdbdController::CreateClientEntity (void)
{
	if ((CreateLocalClient() == 0)) // && (CreateNetworkClient() == 0) && (CreateMqClient() == 0))
		return	0;
	xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create client entity role"));
	return	-1;
}

/*! @brief create local UNIX domain client
 *
 *  Function returns immediately if local client already exists. Otherwise
 *  it will create UNIX domain socket, connect to UNIX domain server and
 *  sets nonblocking I/O behavior.
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mention above failed; error is logged in that case
 */
int	XdbdController::CreateLocalClient()
{
	if (m_localClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("xdbd entity canot become local client because it is already client"));
		return	0;
	}

	struct	sockaddr_un	clnaddr;

	if ((m_localClientFd = socket (AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local client socket, socket() failed, errno = %d"), errno);
		return	-1;
	}

	bzero (&clnaddr, sizeof (clnaddr));
	clnaddr.sun_family = AF_LOCAL;
	strncpy (clnaddr.sun_path, g_localSocketPath.c_str(), sizeof (clnaddr.sun_path) - 1);
	if (connect (m_localClientFd, (sockaddr*) &clnaddr, sizeof (clnaddr)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot connect local client socket, connect() failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_localClientFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot retrieve local client socket flags, fcntl() failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	if (fcntl (m_localClientFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot update local client socket flags, fcntl() failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief create remote BSD socket client
 *
 *  Function returns immediately if remote client already exists. Otherwise
 *  it will create BSD TCP/IP socket, connect it to TCP/IP server and
 *  sets nonblocking I/O behavior.
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mention above failed; error is logged in that case
 */
int	XdbdController::CreateNetworkClient()
{
	if (m_networkClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("xdbd entity canot become network client because it is already client"));
		return	0;
	}

	struct	sockaddr_in	clnaddr;

	if ((m_networkClientFd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network client socket, socket() failed, errno = %d"), errno);
		return	-1;
	}

	bzero (&clnaddr, sizeof (clnaddr));
	clnaddr.sin_family = AF_INET;
	clnaddr.sin_port = htons (g_tcpSocketNumber);
	clnaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (connect (m_networkClientFd, (sockaddr*) &clnaddr, sizeof (clnaddr)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot connect network client socket, connect() failed, errno = %d"), errno);
		close (m_networkClientFd);
		m_networkClientFd = -1;
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_networkClientFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot retrieve network client socket flags, fcntl() failed, errno = %d"), errno);
		close (m_networkClientFd);
		m_networkClientFd = -1;
		return	-1;
	}

	if (fcntl (m_networkClientFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot update network client socket flags, fcntl() failed, errno = %d"), errno);
		close (m_networkClientFd);
		m_networkClientFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief create local message queue client
 *
 *  Function returns immediately if local message queue client already exists. Otherwise
 *  it will open local message queue and sets nonblocking I/O behavior.
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mention above failed; error is logged in that case
 */
int	XdbdController::CreateMqClient()
{
	if (m_mqClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("xdbd entity canot become MQ client because it is already MQ client"));
		return	0;
	}

	if ((m_mqClientFd = mq_open (g_localMqPath.c_str(), O_RDWR)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ client socket, socket() failed, errno = %d"), errno);
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_mqClientFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ client socket, fcntl() failed, errno = %d"), errno);
		mq_close (m_mqClientFd);
		m_mqClientFd = -1;
		return	-1;
	}

	if (fcntl (m_mqClientFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ client socket, fcntl() failed, errno = %d"), errno);
		mq_close (m_mqClientFd);
		m_mqClientFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief create controlling entity
 *
 *  function will create all listening entities: local UNIX domain server,
 *  remote BSD TCP/IP server and message queue server
 *
 *  @return 0 all servers created
 *  @return -1 any server failed to create
 *
 */
int	XdbdController::CreateControllerEntity (void)
{
	if ((CreateLocalListener() != 0) || (CreateNetworkListener() != 0) || (CreateMqListener() != 0))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("controlling entity creation failed"));
		return	-1;
	}
	return	0;
}

/*! @brief create local UNIX domain server
 *
 *  Function will fail if process already represent existing
 *  client or controller entity
 *
 *  If that is not the case, function will create socket, bind
 *  it to UNIX domain path, make it listening socket and sets
 *  nonblocking I/O behavior
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mentioned above fail
 *
 */
int	XdbdController::CreateLocalListener()
{
	if (m_localClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create local listener: local listener already exists"));
		return	-1;
	}
	if (m_localListenFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create local listener: local listener already exists"));
		return	-1;
	}

	if ((m_localListenFd = socket (AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local listener: socket() failed, errno = %d"), errno);
		return	-1;
	}

	unlink (g_localSocketPath.c_str());

	struct	sockaddr_un	srvaddr;

	bzero (&srvaddr, sizeof (srvaddr));
	srvaddr.sun_family = AF_LOCAL;
	strncpy (srvaddr.sun_path, g_localSocketPath.c_str(), sizeof (srvaddr.sun_path) - 1);
	if (bind (m_localListenFd, (sockaddr*) &srvaddr, sizeof (srvaddr)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local listener: bind(%s) failed, errno = %d"), g_localSocketPath.c_str(), errno);
		close (m_localListenFd);
		m_localListenFd = -1;
		return	-1;
	}

	if (listen (m_localListenFd, SOMAXCONN) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local listener: listen() failed, errno = %d"), errno);
		close (m_localListenFd);
		m_localListenFd = -1;
		return	-1;
	}
	m_isController = true;

	int	gflags;

	if ((gflags = fcntl (m_localListenFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local listener: fcntl() failed, errno = %d"), errno);
		close (m_localListenFd);
		m_localListenFd = -1;
		return	-1;
	}

	if (fcntl (m_localListenFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local listener: fcntl() failed, errno = %d"), errno);
		close (m_localListenFd);
		m_localListenFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief create remote BSD TCP/IP server
 *
 *  Function will fail if process already represent existing
 *  remote client or remote controller entity
 *
 *  If that is not the case, function will create socket, bind
 *  it to BSD TCP/IP port, make it listening socket and sets
 *  nonblocking I/O behavior
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mentioned above fail
 *
 */
int	XdbdController::CreateNetworkListener()
{
	if (m_networkClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create network listener: network listener already exists"));
		return	-1;
	}
	if (m_networkListenFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create network listener: network listener already exists"));
		return	-1;
	}

	if ((m_networkListenFd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network listener: socket() failed, errno = %d"), errno);
		return	-1;
	}

	struct	sockaddr_in	srvaddr;

	bzero (&srvaddr, sizeof (srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons (g_tcpSocketNumber);
	srvaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (m_networkListenFd, (sockaddr*) &srvaddr, sizeof (srvaddr)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network listener: bind() failed, errno = %d"), errno);
		close (m_networkListenFd);
		m_networkListenFd = -1;
		return	-1;
	}

	if (listen (m_networkListenFd, SOMAXCONN) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network listener: listen() failed, errno = %d"), errno);
		close (m_networkListenFd);
		m_networkListenFd = -1;
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_networkListenFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network listener: fcntl() failed, errno = %d"), errno);
		close (m_networkListenFd);
		m_networkListenFd = -1;
		return	-1;
	}

	if (fcntl (m_networkListenFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create network listener: fcntl() failed, errno = %d"), errno);
		close (m_networkListenFd);
		m_networkListenFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief create local message queue server
 *
 *  Function will fail if process already represent existing
 *  local message queue client or controller entity
 *
 *  If that is not the case, function will open message queue
 *  and sets nonblocking I/O behavior
 *
 *  @return 0 all steps mentioned above succeed
 *  @return -1 any step mentioned above fail
 *
 */
int	XdbdController::CreateMqListener()
{
	struct	mq_attr	attr;
	attr.mq_flags = O_NONBLOCK;
	attr.mq_maxmsg = 32;
	attr.mq_msgsize = sizeof (void*);
	attr.mq_curmsgs = 0;

	if (m_mqClientFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create MQ listener: MQ listener already exists"));
		return	-1;
	}
	if (m_mqListenFd > 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info("cannot create MQ listener: MQ listener already exists"));
		return	-1;
	}
	if ((mq_unlink (g_localMqPath.c_str()) != 0) && (errno != ENOENT))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ listener: mq_unlink(%s) failed, errno = %d"), g_localMqPath.c_str(), errno);
		return	-1;
	}

	if ((m_mqListenFd = mq_open (g_localMqPath.c_str(), O_RDWR|O_NONBLOCK|O_CREAT|O_EXCL, 0666, &attr)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ listener: mq_open(%s) failed, errno = %d"), g_localMqPath.c_str(), errno);
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_mqListenFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ listener: fcntl() failed, errno = %d"), errno);
		mq_close (m_mqListenFd);
		m_mqListenFd = -1;
		return	-1;
	}

	if (fcntl (m_mqListenFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create MQ listener: fcntl() failed, errno = %d"), errno);
		mq_close (m_mqListenFd);
		m_mqListenFd = -1;
		return	-1;
	}

	return	0;
}

/*! @brief drive client entity
 *
 *  function prepares message, which should be sent to
 *  controlling entity
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler reference to registration handler of this routine
 *  @param t time stamp of invocation
 *
 */
void	XdbdController::DriveXdbdClientEntity (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	XdbdCommon::entity (XdbdClientEntity);
	m_localClientHandler = RegisterDescriptor(EPOLLIN, m_localClientFd, HandleLocalClient, this, ctx_info);
	if (PrepareLocalClientMessage() < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("client entity creation failed: cannot prepare client message"));
		Release();
		return;
	}
	t.tv_sec += 10;
	m_localClientIOTimer = RegisterTimer(t, LocalClientIOTimer, this, ctx_info);
}

/*! @brief drive controlling entity
 *
 *  Function performs two tasks: it daemonizes process
 *  and starts working entity
 *
 */
void	XdbdController::DriveXdbdControllerEntity (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	BecomeDaemon();
	ForkWorker();
	m_deamonized = true;
}

/*! @brief daemonize process
 *
 *  function clones itself, making clone to run in background
 *  and then ceases. Controlling entity now runs as a
 *  background process
 */
void	XdbdController::BecomeDaemon ()
{
	if (m_deamonized)
		return;

	pid_t	pid;
	pid_t	spid;

	switch (pid = fork ())
	{
	case	-1:
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot become daemon, fork() failed, errno = %d"), errno);
		return;
	case	0:
		break;
	default:
		exit (0);
		break;
	}

	if ((spid = setsid ()) == (pid_t) -1)
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot become session leader, setsid() failed, errno = %d"), errno);
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller starting new session - PID = %d"), spid);

	umask (027);
}

/*! @brief fork working entity
 *
 *  Function follows these steps:
 *
 *  two pipes are created connecting controlling and working
 *  entity. This connections are called master client connections
 *  Controller entity is thus called master client. All other
 *  clients (client entites) never connect to controlling entity
 *  since this entity redirects all I/O activity to working
 *  entity.
 *
 *  next step creates working entity by cloning controlling entity
 *  and running the same process with argument 'worker'. Prior to
 *  running new process cloned controller must redirect its listening
 *  file descriptors to predefined values used by working entity.
 *  Working entity assumes that local listening socket number, network
 *  listening socket number, message queue server FD, pipe flowing from
 *  controller to worker and pipe flowing from worker to controller
 *  have the following numbers: 3, 4, 5, 6 and 7. To achieve this
 *  both cloned and original controller makes appropriate redirections
 *  using dup() and close() system calls. When redirections are established
 *  it starts new working entity and ceases, living alive original controller
 *  entity
 *
 *  Since controlling entity does not need listening sockets any more,
 *  it closes them. It then creates I/O mechanisms to handle communication
 *  with working entity: it registers I/O handlers on open ends of pipes
 *  created at the beginning of this function using function HandleMasterClient()
 *
 *  @return 0 all steps done without errors
 *  @return -1 any step failed
 */
int	XdbdController::ForkWorker ()
{
	int	p1[2];
	int	p2[2];

	if (pipe (p1) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot create working entity, pipe() failed, errno = %d"), errno);
		return	-1;
	}

	if (pipe (p2) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot create working entity, pipe() failed, errno = %d"), errno);
		return	-1;
	}

	switch (m_worker = fork())
	{
	case	-1:
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot create working entity, fork() failed, errno = %d"), errno);
		return	-1;
	case	0:
		//
		//	file descriptors created by controller and used by worker:
		//	0, 1, 2 are closed and not used
		//
		//	fd 3 - unix domain listening socket
		if (m_localListenFd != g_localListener)
		{
			close (g_localListener);
			dup2 (m_localListenFd, g_localListener);
			close (m_localListenFd);
		}
		//	fd 4 - tcp listening socket
		if (m_networkListenFd != g_networkListener)
		{
			close (g_networkListener);
			dup2 (m_networkListenFd, g_networkListener);
			close (m_networkListenFd);
		}
		//	fd 5 - mq listener
		if (m_mqListenFd != g_mqListener)
		{
			close (g_mqListener);
			dup2 (m_mqListenFd, g_mqListener);
			close (m_mqListenFd);
		}
		//	fd 6 - pipe: controller --> worker
		close (p1[1]);
		if (p1[0] != g_localWrkReader)
		{
			close (g_localWrkReader);
			dup2 (p1[0], g_localWrkReader);
			close (p1[0]);
		}
		//	fd 7 - pipe: controller <-- worker
		close (p2[0]);
		if (p2[1] != g_localWrkWriter)
		{
			close (g_localWrkWriter);
			dup2 (p2[1], g_localWrkWriter);
			close (p2[1]);
		}
		for (int i = g_localWrkWriter + 1; i < 1024; ++i)
			close (i);
		{
			const char* const	cmd[] = {"xdbd-worker", "worker", 0};
			if (m_deamonized)
				setenv ("XDBD_WORKER_RELOADED", "TRUE", 1);
			execvp (cmd[0], (char*const*) cmd);
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd-controller entity cannot run working entity, execvp () failed, errno = %d"), errno);
			exit (0);
		}
		return	-1;
	default:
		//	disambiguate controller <--> worker pipes
		close (p1[0]);
		close (p2[1]);
		break;
	}

	XdbdCommon::entity (XdbdControllerEntity);

	xdbdErrReport (SC_XDBD, SC_APL, err_info("xdbd-working entity created - PID = %d"), m_worker);
	m_sigcldSignalHandler = RegisterSignal(SIGCLD, SigcldSignalHandler, this);
	m_hangupSignalHandler = RegisterSignal(SIGHUP, HangupSignalHandler, this);

	close (m_localListenFd);
	close (m_networkListenFd);
	close (m_mqListenFd);
	m_localListenFd = -1;
	m_networkListenFd = -1;
	m_mqListenFd = -1;

	m_masterClientReaderFd = p2[0];
	m_masterClientWriterFd = p1[1];

	m_masterClientReaderHandler = RegisterDescriptor(EPOLLIN, m_masterClientReaderFd, HandleMasterClient, this, ctx_info);
	m_masterClientWriterHandler = RegisterDescriptor(EPOLLIN, m_masterClientWriterFd, HandleMasterClient, this, ctx_info);
	return	0;
}

/*! @brief SIGCLD signal handler
 *
 *  SIGCLD is generated by system when working entity dies. In that case
 *  timer is created by registering function WaitWorkingEntity(), which
 *  will handle died process
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler reference to registration handler of this routine
 *  @param info original signal information as caught by infrastructure
 */
void	XdbdController::SigcldSignalHandler (XdbdRunningContext *ctx, ctx_fddes_t handler, siginfo_t* info)
{
	m_commandCode = XdbdAdminStopWord;
	RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
}

/*! @brief SIGHUP signal handler
 *
 *  SIGHUP can be generated by system (shutdown, ...) or by working
 *  entity. In any case communication pipes with working entity  are
 *  close which will cause working entity to cease. That is why timer
 *  is created by registering WaitWorkingEntity(), which will handle
 *  died working process
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler reference to registration handler of this routine
 *  @param info original signal information as caught by infrastructure
 */
void	XdbdController::HangupSignalHandler (XdbdRunningContext *ctx, ctx_fddes_t handler, siginfo_t* info)
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info("HANGUP detected: xdbd controller, PID = %d"), getpid());
	xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd controller: close xdbd worker connection"));
	close (m_masterClientReaderFd);
	close (m_masterClientWriterFd);
	m_masterClientReaderFd = -1;
	m_masterClientWriterFd = -1;
	if (m_masterClientReaderHandler != 0)
		RemoveDescriptor(m_masterClientReaderHandler);
	m_masterClientReaderHandler = 0;
	if (m_masterClientWriterHandler != 0)
		RemoveDescriptor(m_masterClientWriterHandler);
	m_masterClientWriterHandler = 0;
	m_commandCode = XdbdAdminStopWord;
	RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
}

/*! @brief create header common to all protocol messages
 *
 *  function fills version, message number, UUID,
 *  entity type and PID of sending process
 *
 *  @param header message header reference
 *
 */
void	XdbdController::CreateMessageHeader (XdbdMessageHeader* header)
{
	header->m_version.m_major = 1;
	header->m_version.m_minor = 0;
	header->m_index = ++g_messageIndex;
	uuid_generate (header->m_hash);
	header->m_error = 0;
	header->m_entity = XdbdCommon::entity();
	header->m_pid = XdbdCommon::pid();
}

/*! @brief prepare local client message for sending
 *
 *  prepares message which should be sent to
 *  working entity in accordance with program
 *  parameters: message type (administrative or
 *  SQL request) command code (start, stop for
 *  admin requests, fetch, execute, etc. for
 *  SQL requests. Finally it calls function
 *  PostRequest() which ensures that it will
 *  be sent as soon as possible
 *
 *  @return 0 message successfully posted
 *  @return -1 message not posted, incorrect command
 */
int	XdbdController::PrepareLocalClientMessage ()
{
	XdbdRequest*	req = new XdbdRequest;
	memset (req, 0, sizeof (XdbdRequest));

	switch (m_commandType)
	{
	case	XdbdAdminWord:
		{
			req->m_requestType = AdminRequestCode;

			XdbdAdminRequest*	p_XdbdAdminRequest = &req->XdbdRequest_u.m_adminRequest;
			p_XdbdAdminRequest->m_command = (u_int) m_commandCode;
			p_XdbdAdminRequest->m_parameters.m_parameters_len = 0;
			p_XdbdAdminRequest->m_parameters.m_parameters_val = 0;
		}
		break;
	case	XdbdSqlWord:
		{
			req->m_requestType = SqlRequestCode;

			XdbdSqlRequest*	p_XdbdSqlRequest = &req->XdbdRequest_u.m_sqlRequest;
			p_XdbdSqlRequest->m_command = m_commandCode;
			p_XdbdSqlRequest->m_parameters.m_parameters_len = 0;
			p_XdbdSqlRequest->m_parameters.m_parameters_val = 0;
		}
		break;
	default:
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot create local client message, incorrect message type = %d"), m_commandType);
		return	-1;
	}

	int	status = PostRequest (req);
	xdr_free ((xdrproc_t)xdr_XdbdRequest, (char*) req);
	delete	req;
	return	status;
}

/*! @post request to server (working entity)
 *
 *  function tries to serializes message, allocates
 *  sufficient space in internal buffers and puts
 *  serialized message into this buffer. If allocation
 *  and serialization succeed it enables output
 *  processing of associated I/O channel (local UNIX
 *  domain socket, network socket or message queue)
 *  thus enabling message to be sent as soon as possible
 *
 *  @return 0 all steps (allocation, serialization and
 *  activation) succeed
 *  @return -1 some step failed, error logged
 *
 */
int	XdbdController::PostRequest (XdbdRequest* req)
{
	CreateMessageHeader (&req->XdbdRequest_u.m_header);
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
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post client request, malloc() failed, errno = %d"), errno);
			return	-1;
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
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post client request, cannot encode message size, xdr_int() failed"));
		return	-1;
	}

	xdrmem_create(&xdr, (char*) (m_outputPtr + m_intSize), freeSpace - m_intSize, XDR_ENCODE);
	if (xdr_XdbdRequest (&xdr, req) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot post client request, cannot encode message request, xdr_XdbdRequest() failed"));
		return	-1;
	}

	u_int	pos = xdr_getpos (&xdr);
	m_outputPtr += pos + m_intSize;
	EnableDescriptor (m_localClientHandler, EPOLLOUT);
	return	0;
}

/*! @brief I/O handler for XML DB client
 *
 *  function handles all I/O activities for associated I/O channel (UNIX domain,
 *  network or message queue) for ordinary (non-controller) XML DB client.
 *  It handles all incoming and outgoing requests produced in communication
 *  with XML DB server working entity.
 *
 *  Incoming messages: sufficient space is allocated in internal message buffer
 *  to accomodate incoming message(s) (one or more messages should arrive). Every
 *  incoming message is deserialized and not processed at all, since they are
 *  simple replies to previously sent start and stop requests
 *
 *  Outgoing messages: these are posted serialized messages previously handled
 *  by PostRequest(). Serialized form of these messages is sent to associated FD,
 *  allocated space is freed and it should be used to post new messages which is
 *  never the case since controllers send only one message in its life time
 *
 *  @param ctx reference to superclass part of this object (I/O multiplexer)
 *  @param flags epoll events (EPOLLIN, EPOLLOUT, ...)
 *  @param handler reference to I/O multiplexing handler (useful if it needs to be released)
 *  @param fd FD associated with this handler (UNIX domain client, network client, message queue client)
 *
 */
void	XdbdController::HandleLocalClient (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	DisableTimer(m_localClientIOTimer);
	m_localClientIOTimer = 0;

	if (flags & EPOLLIN)
	{
		int	needSpace = 0;
		int	usedSpace = m_inputPtr - m_inputBuffer;
		int	freeSpace = m_inputEnd - m_inputPtr;

		if (ioctl (fd, FIONREAD, &needSpace) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client reply, ioctl() failed, errno = %d"), errno);
			Release();
			return;
		}

		if (needSpace == 0)
		{
			xdbdErrReport (SC_XDBD, SC_APL, err_info("cannot receive client reply, empty message, ioctl() failed, errno = %d"), errno);
			Release();
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
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client reply, malloc(%d) failed, errno = %d"), needSpace, errno);
				Release();
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
			{
				return;
			}
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client reply, recv() failed, errno = %d"), errno);
			Release();
			return;
		}

		usedSpace += recvSize;

		u_char*	ptr = m_inputBuffer;
		while (true)
		{
			if (usedSpace < (int) m_intSize)
			{
				break;
			}

			XDR	xdr;

			int	msgSize;
			xdrmem_create(&xdr, (char*) ptr, m_intSize, XDR_DECODE);
			if (xdr_int (&xdr, &msgSize) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client reply, cannot decode message size, xdr_int() failed"));
				Release ();
				return;
			}

			if (usedSpace < msgSize + (int) m_intSize)
			{
				break;
			}

			XdbdReply*	req = new XdbdReply;
			memset (req, 0, sizeof (XdbdReply));
			xdrmem_create(&xdr, (char*) (ptr + m_intSize), msgSize, XDR_DECODE);
			if (xdr_XdbdReply(&xdr, req) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive client reply, cannot decode message body, xdr_XdbdReply() failed"));
				Release ();
				return;
			}

			ptr += msgSize + m_intSize;
			usedSpace -= msgSize + m_intSize;

			break;
		}
		if (usedSpace > 0)
		{
			timespec	t = ctx->realTime();
			t.tv_sec += 10;
			m_localClientIOTimer = RegisterTimer(t, LocalClientIOTimer, this, ctx_info);
		}
	}
	else	if (flags & EPOLLOUT)
	{
		size_t	size = m_outputPtr - m_outputBuffer;
		ssize_t	count = send (fd, m_outputBuffer, size, 0);
		if (count <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot send client request, send() failed, errno = %d"), errno);
			Release ();
			return;
		}
		if ((size_t) count < size)
		{
			timespec	t = ctx->realTime();
			t.tv_sec += 10;
			memcpy (m_outputBuffer, m_outputBuffer + count, size - count);
			m_localClientIOTimer = RegisterTimer(t, LocalClientIOTimer, this, ctx_info);
		}
		else
			DisableDescriptor(handler, EPOLLOUT);
		m_outputPtr -= count;
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot determine controller data type"));
		Release ();
	}
}

/*! @brief timer for ordinary client I/O activity
 *
 *  every time certain I/O activity is carried out, previous time out control is switched
 *  off and new one is switched on by registering this function as time out handler which
 *  will fire it. If it fires (lazy communication) it releases I/O connections causing
 *  process to cease possibly not carrying out its job
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler time out handler reference
 *  @param t time stamp of fire event
 *
 */
void	XdbdController::LocalClientIOTimer (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	m_localClientIOTimer = 0;
	Release ();
}

/*! @brief I/O handler for XML DB controller
 *
 *  function handles all I/O activities for both pipes connecting controlling and
 *  working entities
 *
 *  Input pipe: sufficient space is allocated in internal message buffer
 *  to accomodate incoming message(s) (one or more messages should arrive). Every
 *  incoming message is deserialized and processed appropriately. Only administrative
 *  requests are processed. Start, stop or restart requests release pipes connecting
 *  controlling and working entities causing both of them to cease.
 *
 *  Outgoing pipe: normal (EPOLLOUT) or exceptional output processing releases pipes
 *  connecting controlling and working entities causing both of them to cease.
 *
 *  @param ctx reference to superclass part of this object (I/O multiplexer)
 *  @param flags epoll events (EPOLLIN, EPOLLOUT, ...)
 *  @param handler reference to I/O multiplexing handler (useful if it needs to be released)
 *  @param fd FD associated with this handler (UNIX domain client, network client, message queue client)
 *
 */
void	XdbdController::HandleMasterClient (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	DisableTimer(m_masterClientIOTimer);
	m_masterClientIOTimer = 0;

	if (flags & EPOLLIN)
	{
		int	needSpace = 0;
		int	usedSpace = m_inputPtr - m_inputBuffer;
		int	freeSpace = m_inputEnd - m_inputPtr;

		if (ioctl (fd, FIONREAD, &needSpace) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, ioctl() failed, errno = %d"), errno);
			Release();
			RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
			return;
		}

		if (needSpace == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, empty message, ioctl() failed, errno = %d"), errno);
			Release();
			RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
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
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, malloc(%d) failed, errno = %d"), needSpace, errno);
				Release();
				RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
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
		if ((recvSize = read (fd, m_inputPtr, freeSpace)) <= 0)
		{
			if (errno == EWOULDBLOCK)
			{
				return;
			}
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, recv() failed, errno = %d"), errno);
			Release();
			RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
			return;
		}

		usedSpace += recvSize;

		u_char*	ptr = m_inputBuffer;
		while (true)
		{
			if (usedSpace < (int) m_intSize)
			{
				break;
			}

			XDR	xdr;

			int	msgSize;
			xdrmem_create(&xdr, (char*) ptr, m_intSize, XDR_DECODE);
			if (xdr_int (&xdr, &msgSize) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, cannot decode message size, xdr_int() failed"));
				return;
			}

			if (usedSpace < msgSize + (int) m_intSize)
			{
				break;
			}

			XdbdRequest*	req = new XdbdRequest;
			memset (req, 0, sizeof (XdbdRequest));
			xdrmem_create(&xdr, (char*) (ptr + m_intSize), msgSize, XDR_DECODE);
			if (xdr_XdbdRequest(&xdr, req) != TRUE)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("cannot receive master client reply, cannot decode message body, xdr_XdbdReply() failed"));
				return;
			}

			ptr += msgSize + m_intSize;
			usedSpace -= msgSize + m_intSize;

			if (req->m_requestType != AdminRequestCode)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info("incorrect master client reply code: %d"), req->m_requestType);
				return;
			}

			switch (m_commandCode = (XdbdWordCode) req->XdbdRequest_u.m_adminRequest.m_command)
			{
			case	XdbdAdminStartWord:
			case	XdbdAdminRestartWord:
			case	XdbdAdminStopWord:
				{
					Release();
					RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
					return;
				}
				break;
			case	XdbdAdminStatusWord:
				{
				}
				break;
			default:
				{
					xdbdErrReport (SC_XDBD, SC_ERR, err_info("incorrect master client reply command code: %d"), m_commandCode);
				}
				break;
			}
		}
		if (usedSpace > 0)
		{
			timespec	t = ctx->realTime();
			t.tv_sec += 10;
			m_masterClientIOTimer = RegisterTimer(t, MasterClientIOTimer, this, ctx_info);
		}
	}
	else	if (flags & EPOLLOUT)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("incorrect master client I/O type: %d"), flags);
		Release ();
		RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("unknown master client I/O type: %d"), flags);
		Release ();
		RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
	}
}

/*! @brief timer for controlling entity I/O activity
 *
 *  every time certain I/O activity is carried out, previous time out control is switched
 *  off and new one is switched on for all incompletely received requests. This is achieved
 *  by registering this function as time out handler which will fire it. If it fires (lazy
 *  communication) it releases I/O connections causing controller and working entities to
 *  cease
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler time out handler reference
 *  @param t time stamp of fire event
 *
 */
void	XdbdController::MasterClientIOTimer (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	m_masterClientIOTimer = 0;
	Release ();
	RegisterTimer (ctx->realTime(), WaitWorkingEntity, this, ctx_info);
}

/*! @brief wait working entity to cease
 *
 *  every time controlling entity releases pipes connecting it to working entity it
 *  switches on timer (by registering this function as time out handler) which will
 *  indefinitely wait for working entity to cease. It will repeatedly wait it as
 *  many times as needed. When awaited it will fetch working entity exit status and
 *  report it into log queue. After that it will examine administrative code obtained
 *  from input pipe. In case of stop request, controller will cease otherwise it will
 *  repeat starting processing again by registering AccessSyncPath() time out function.
 *
 *  @param ctx reference to superclass part of this object
 *  @param handler time out handler reference
 *  @param t time stamp of beginning of wait event
 *
 */
void	XdbdController::WaitWorkingEntity (XdbdRunningContext *ctx, void *handler, struct timespec t)
{
	int	status;
	pid_t	pid;

	errno = 0;
	switch (pid = waitpid ((pid_t) -1, &status, WNOHANG))
	{
	case	0:
	case	(pid_t) -1:
		if (errno == ECHILD)
			break;
		t.tv_sec += 1;
		RegisterTimer (t, WaitWorkingEntity, this, ctx_info);
		return;
	default:
		break;
	}

	if (pid == (pid_t) -1)
		;
	else	if (WIFEXITED (status))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd worker terminated, exit status = %d, PID = %d"), WEXITSTATUS (status), pid);
	}
	else	if (WIFSIGNALED (status))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd worker terminated, killed by signal = %d, PID = %d"), WTERMSIG (status), pid);
	}
	else
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info("xdbd worker terminated, status = %d, PID = %d"), status, pid);
	}

	if (m_commandCode == XdbdAdminStopWord)
	{
		Release();
		return;
	}
	RegisterTimer (t, AccessSyncPath, this, ctx_info);
}

} /* namespace xml_db_daemon */

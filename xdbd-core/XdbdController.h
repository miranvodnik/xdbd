/*
 * XdbdController.h
 *
 *  Created on: 28. okt. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdRunningContext.h"
#include "XdbdCommon.h"
#include "XdbdMessages.h"

namespace xml_db_daemon
{

/*! @brief XML DB entity class
 *
 *  this class represents XML DB entity being it ordinary
 *  XML DB client or XML DB controller. Of all concurrently started
 *  entities, at most one will become controlling entity, all
 *  others will become ordinary clients. Controller will become
 *  an entity which will successfully invoke start administrative
 *  request
 *
 */
class XdbdController: public XdbdRunningContext
{
public:
	XdbdController (int n, char**p);
	virtual ~XdbdController ();

private:
	ctx_starter (StartController, XdbdController)
	ctx_finisher (StopController, XdbdController)
	ctx_timehook (TimeHookController, XdbdController)

	timer_handler (AccessSyncPath, XdbdController)
	timer_handler (DetermineXdbdEntityRole, XdbdController)
	timer_handler (DriveXdbdClientEntity, XdbdController)
	timer_handler (DriveXdbdControllerEntity, XdbdController)
	timer_handler (WaitWorkingEntity, XdbdController)

	fd_handler (HandleMasterClient, XdbdController)
	fd_handler (HandleLocalClient, XdbdController)

	sig_handler (SigcldSignalHandler, XdbdController)
	sig_handler (HangupSignalHandler, XdbdController)

	timer_handler (LocalClientIOTimer, XdbdController)
	timer_handler (MasterClientIOTimer, XdbdController)

	int	LockControllingPath (void);
	int	UnlockControllingPath (void);
	int	CreateClientEntity (void);
	int	CreateControllerEntity (void);
	int	CreateLocalListener();
	int	CreateNetworkListener();
	int	CreateMqListener();
	int	CreateLocalClient();
	int	CreateNetworkClient();
	int	CreateMqClient();

	int	PrepareLocalClientMessage ();
	int	PostRequest (XdbdRequest* req);
	void	CreateMessageHeader (XdbdMessageHeader* header);

	void	BecomeDaemon ();
	int	ForkWorker ();

	void	Release ();
private:
	static	const	string	g_localSocketPath;	//!< UNIX domain listening socket path
	static	const	string	g_localMqPath;	//!< message queue listening path
	static	const	int	g_tcpSocketNumber;	//!< network TCP/IP listening number
	static	const	int	g_localListener;	//!< redirected UNIX domain listening FD (3)
	static	const	int	g_networkListener;	//!< redirected network listening FD (4)
	static	const	int	g_mqListener;	//!< redirected message queue listening FD (5)
	static	int	g_messageIndex;	//!< protocol message number
private:
	int	m_argc;	//!< number of arguments provided by main() function
	char**	m_argv;	//!< program arguments provided by main() function

	XdbdWordCode	m_commandType;	//!< administrative command type
	XdbdWordCode	m_commandCode;	//!< administrative command code: stop, start, restart
	int	m_sync;		//!< synchronization path semaphore

	int	m_localListenFd;	//!< local socket listening fd - working entity controller
	int	m_localClientFd;	//!< local socket client fd - controlling client
	int	m_masterClientReaderFd;	//!< local socket client fd - controlling entity master client reader pipe
	int	m_masterClientWriterFd;	//!< controlling entity master client writer pipe

	int	m_networkListenFd;	//!< network socket listening fd - working entity controller
	int	m_networkClientFd;	//!< network socket client fd - controlling client

	int	m_mqListenFd;	//!< message queue listening fd - working entity controller
	int	m_mqClientFd;	//!< message queue client fd - controlling client

	pid_t	m_worker;	//!< working entity PID

	ctx_fddes_t	m_localClientHandler;	//!< UNIX domain client FD I/O handler
	ctx_fddes_t	m_masterClientReaderHandler;	//!< input pipe I/O handler
	ctx_fddes_t	m_masterClientWriterHandler;	//!< output pipe I/O handler
	ctx_timer_t	m_localClientIOTimer;	//!< UNIX domain client FD I/O timer
	ctx_timer_t	m_masterClientIOTimer;	//!< master client pipe I/O timer

	ctx_sig_t	m_sigcldSignalHandler;	//!< SIGCLD signal handler
	ctx_sig_t	m_hangupSignalHandler;	//!< SIGHUP signal handler

	u_char*	m_inputBuffer;	//!< internal input buffer for either ordinary client or controlling entity
	u_char*	m_inputPtr;	//!< internal input buffer read head
	u_char*	m_inputEnd;	//!< internal input buffer far end point

	u_char*	m_outputBuffer;	//!< internal output buffer for either ordinary client or controlling entity
	u_char*	m_outputPtr;	//!< internal output buffer write head
	u_char*	m_outputEnd;	//!< internal output buffer far end point

	int	m_intSize;	//!< size of XDR integer representation
	bool	m_isController;	//!< indicates controlling entity: false - ordinary client, true - controller
	bool	m_deamonized;	//!< controller daemonized: true - does not need to be daemonized again when restarting
};

} /* namespace xml_db_daemon */

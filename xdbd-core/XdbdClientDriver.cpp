/*
 * XdbdClientDriver.cpp
 *
 *  Created on: 11. nov. 2016
 *      Author: miran
 */

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "XdbdClientDriver.h"
#include <sstream>
using namespace std;

namespace xml_db_daemon
{

XdbdClientDriver::cdlist	XdbdClientDriver::g_cdlist;	//!< collection of all client driver objects
bool	XdbdClientDriver::g_started = false;	//!< status of client driver threads
XdbdClientDriver::cdlist::iterator	XdbdClientDriver::g_currentClientDriver;	//!< current client driver thread

/*! @brief constructor of client driver instance
 *
 *  it does nothing but creates client driver multiplexer
 *  which is capable of simultaneously drive any number of
 *  client handlers
 */
XdbdClientDriver::XdbdClientDriver ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdClientDriver %d", this);
#endif

	m_clientHandler = new XdbdClientHandler ();
}

/*! @brief destructor of client driver instance
 *
 *  it deletes all client instances (local, network and message queue)
 *  collected so far together with client driver multiplexer
 */
XdbdClientDriver::~XdbdClientDriver ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdClientDriver %d", this);
#endif

	for (lcset::iterator it = m_lcset.begin(); it != m_lcset.end(); ++it)
	{
		delete	*it;
	}
	m_lcset.clear();

	for (ncset::iterator it = m_ncset.begin(); it != m_ncset.end(); ++it)
	{
		delete	*it;
	}
	m_ncset.clear();

	for (mcset::iterator it = m_mcset.begin(); it != m_mcset.end(); ++it)
	{
		delete	*it;
	}
	m_mcset.clear();

	delete	m_clientHandler;
	m_clientHandler = 0;
}

/*! @brief start client drivers
 *
 *  start small number of client driver threads. This number
 *  is obviously equal to the number of CPU cores or just
 *  slightly grater if CPU has small number of cores. Every thread
 *  is associated with particular core which will execute it. First thread
 *  is declared to become current driver thread
 *
 */
void	XdbdClientDriver::StartClientDrivers ()
{
	if (g_started)
		return;
	g_started = true;

	cpu_set_t	cpuset;
	int	nrcpus = XdbdCommon::cpuCount();
	int	nrthreads = XdbdCommon::threadCount();

	for (int i = 0; i < nrthreads; ++i)
	{
		CPU_SET (i % nrcpus, &cpuset);
		XdbdClientDriver*	p_XdbdClientDriver = new XdbdClientDriver ();
		g_cdlist.push_back (p_XdbdClientDriver);
		p_XdbdClientDriver->Start(0);
		sched_setaffinity (p_XdbdClientDriver->getTid(), sizeof cpuset, &cpuset);
		CPU_CLR (i % nrcpus, &cpuset);
	}
	g_currentClientDriver = g_cdlist.begin();
}

/*! @brief stop client drivers
 *
 *  stop all client driver threads one by one. Every thread is canceled
 *  and joined before proceeding to next one. Currently executed driver
 *  handler (if any) is canceled together with driver thread
 *
 */
void XdbdClientDriver::StopClientDrivers ()
{
	for (cdlist::iterator it = g_cdlist.begin(); it != g_cdlist.end(); ++it)
	{
		pthread_t	thread = (*it)->getHandle();
		void*	res = 0;
		pthread_cancel (thread);
		pthread_join (thread, &res);
		xdbdErrReport (SC_XDBD, SC_ERR, "Connection handling thread terminated, TID = %d", (*it)->getTid ());
		delete	*it;
	}
	g_cdlist.clear();
}

/*! @brief select current driver thread
 *
 *  Function returns current client driver thread and selects new one
 *  in round-robin fashion by iterating client driver thread list. After
 *  reaching end of list it starts from the beginning. Every new client
 *  driver handler is associated with current client driver thread returned
 *  by this function call. Following this principle we can assume that
 *  all client handlers are equally distributed between client driver threads
 *  although there are many scenarios that this is not always true, especially
 *  when there are clients with different life living lengths
 *
 *  @return current client driver thread
 */
XdbdClientDriver*	XdbdClientDriver::currentClientDriver ()
{
	XdbdClientDriver*	cln = *g_currentClientDriver;
	if (++g_currentClientDriver == g_cdlist.end())
		g_currentClientDriver = g_cdlist.begin();
	return	cln;
}

/*! @brief Run() method of client driver thread
 *
 *  This function is thread running function. It installs thread cleaning
 *  function ThreadCleanup() and runs client handler multiplexer main loop
 *
 *  @return status status of client handler multiplexer. This status is never
 *  examined because client driver threads never terminate normally. They are
 *  always canceled.
 */
int XdbdClientDriver::Run (void)
{
	int	status;
	pthread_cleanup_push (ThreadCleanup, this);
	xdbdErrReport (SC_XDBD, SC_ERR, "Connection handling thread started, TID = %d", getTid ());
	status = m_clientHandler->MainLoop();
	pthread_cleanup_pop(0);
	return	status;
}

/*! @brief client driver thread cancellation function
 *
 *  this function is invoked by cancellation mechanism when main working thread
 *  terminates execution of client driver threads. It does nothing but reports
 *  the cancellation fact
 */
void XdbdClientDriver::ThreadCleanup ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, "Connection handling thread canceled, TID = %d", getTid ());
}

/*! @brief client driver thread initialization function
 *
 *  function does nothing since there is nothing special to be initialized. Since PID of
 *  thread is not known at initialization time, starting message is reported in thread
 *  Run() function
 *
 *  @return 0 always
 *
 */
int XdbdClientDriver::InitInstance (void)
{
	return	0;
}

/*! @brief client driver thread termination function
 *
 *  function reports termination message and stops client driver multiplexer
 *
 *  @return 0 always
 *
 */
int XdbdClientDriver::ExitInstance (void)
{
	xdbdErrReport (SC_XDBD, SC_ERR, "Connection handling thread terminated, TID = %d", getTid ());
	m_clientHandler->Stop();
	return	0;
}

/*! @brief handle administrative request
 *
 *  this function handles stop request. In that case it delivers SIGHUP
 *  interrupt to self process. This interrupt is intercepted by one of
 *  XdbdRunningContext multiplexers active in this process (possibly to
 *  client driver multiplexer owned by this thread) and synchronized
 *  with all entities interested to handle it.
 *
 *  @see XdbdWorker
 *
 */
void	XdbdClientDriver::HandleAdminRequest (XdbdLocalClient* cln, XdbdRequest* req)
{
	if ((req->m_requestType != AdminRequestCode) || (req->XdbdRequest_u.m_adminRequest.m_command != XdbdAdminStopWord))
		return;
	kill (getpid(), SIGHUP);
}

/*! @brief release local client handler
 *
 *  exclude local client handler, connected locally through UNIX domain
 *  socket, from internal list
 *
 *  @param localClient local client driver reference
 *
 */
void	XdbdClientDriver::ReleaseLocalClient (XdbdLocalClient* localClient)
{
	lcset::iterator	it = m_lcset.find(localClient);
	if (it != m_lcset.end())
		m_lcset.erase(it);
}

/*! @brief release network client handler
 *
 *  exclude network client handler, connected remotely through network
 *  socket, from internal list
 *
 *  @param networkClient network client driver reference
 *
 */
void	XdbdClientDriver::ReleaseNetworkClient (XdbdNetworkClient* networkClient)
{
	ncset::iterator	it = m_ncset.find(networkClient);
	if (it != m_ncset.end())
		m_ncset.erase(it);
}

/*! @brief release message queue client handler
 *
 *  exclude message queue client handler, connected locally through message
 *  queue, from internal list
 *
 *  @param mqClient message queue client driver reference
 *
 */
void	XdbdClientDriver::ReleaseMqClient (XdbdMqClient* mqClient)
{
	mcset::iterator	it = m_mcset.find(mqClient);
	if (it != m_mcset.end())
		m_mcset.erase(it);
}

/*! @brief send local client handler activation request
 *
 *  this function is called immediately after local client has been successfully
 *  connected through UNIX domain socket. Since connection has been made in
 *  special thread which does nothing but establishes connections (either local
 *  using UNIX domain sockets, local using message queues or remote using network
 *  sockets), info about new connection must be transfered to current client handler
 *  thread. That is exactly what it does this function: it takes local client FD,
 *  makes message from it and sends this message into internal message queue of
 *  client driver multiplexer.
 *
 *  @return status success of message sending
 *
 *  @see MQSend
 *  @see XdbdLocalClientActivationMessage
 *
 */
int	XdbdClientDriver::SendLocalClientHandlerActivation (int localClientFd)
{
	int	status;
	XdbdLocalClientActivationMessage*	msg = new XdbdLocalClientActivationMessage (localClientFd, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

/*! @brief activate local client handler
 *
 *  Once activation request has been read by client multiplexer, new instance
 *  of XdbdLocalClient is created and added to internal list of this object. Newly
 *  created object registers I/O handler in client multiplexer
 *
 *  @return 0 always
 *
 *  @see XdbdLocalClient
 */
int	XdbdClientDriver::ActivateLocalClientHandler (int localClientFd)
{
	XdbdLocalClient*	p_XdbdLocalClient = new XdbdLocalClient (m_clientHandler, localClientFd, this);
	m_lcset.insert (p_XdbdLocalClient);
	return	0;
}

/*! @brief send network client handler activation request
 *
 *  this function is called immediately after remote client has been successfully
 *  connected through network socket. Since connection has been made in
 *  special thread which does nothing but establishes connections (either local
 *  using UNIX domain sockets, local using message queues or remote using network
 *  sockets), info about new connection must be transfered to current client handler
 *  thread. That is exactly what it does this function: it takes remote client FD,
 *  makes message from it and sends this message into internal message queue of
 *  client driver multiplexer.
 *
 *  @return status success of message sending
 *
 *  @see MQSend
 *  @see XdbdNetworkClientActivationMessage
 *
 */
int	XdbdClientDriver::SendNetworkClientHandlerActivation (int networkClientFd)
{
	int	status;
	XdbdNetworkClientActivationMessage*	msg = new XdbdNetworkClientActivationMessage (networkClientFd, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

/*! @brief activate network client handler
 *
 *  Once activation request has been read by client multiplexer, new instance
 *  of XdbdNetworkClient is created and added to internal list of this object. Newly
 *  created object registers I/O handler in client multiplexer
 *
 *  @return 0 always
 *
 *  @see XdbdNetworkClient
 */
int	XdbdClientDriver::ActivateNetworkClientHandler (int networkClientFd)
{
	XdbdNetworkClient*	p_XdbdNetworkClient = new XdbdNetworkClient (m_clientHandler, networkClientFd, this);
	m_ncset.insert (p_XdbdNetworkClient);
	return	0;
}

/*! @brief send nessage queue client handler activation request
 *
 *  this function is called immediately after local client has been successfully
 *  connected through message queue socket. Since connection has been made in
 *  special thread which does nothing but establishes connections (either local
 *  using UNIX domain sockets, local using message queues or remote using network
 *  sockets), info about new connection must be transfered to current client handler
 *  thread. That is exactly what it does this function: it takes message queue client FD,
 *  makes message from it and sends this message into internal message queue of
 *  client driver multiplexer.
 *
 *  @return status success of message sending
 *
 *  @see MQSend
 *  @see XdbdMqClientActivationMessage
 *
 */
int	XdbdClientDriver::SendMqClientHandlerActivation (int mqClientFd)
{
	int	status;
	XdbdMqClientActivationMessage*	msg = new XdbdMqClientActivationMessage (mqClientFd, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

/*! @brief activate message queue client handler
 *
 *  Once activation request has been read by client multiplexer, new instance
 *  of XdbdNetworkClient is created and added to internal list of this object. Newly
 *  created object registers I/O handler in client multiplexer
 *
 *  @return 0 always
 *
 *  @see XdbdMqClient
 */
int	XdbdClientDriver::ActivateMqClientHandler (int mqClientFd)
{
	XdbdMqClient*	p_XdbdMqClient = new XdbdMqClient (m_clientHandler, mqClientFd, this);
	m_mcset.insert (p_XdbdMqClient);
	return	0;
}

/*! @brief send local client response
 *
 *  This is actually request to send response. This request is generally generated by one of
 *  working threads. Since this thread is different from thread which handles local client
 *  communication it has to be sent into the message queue of that particular thread. And that is
 *  exactly what that function do: it generates message (request) to send client response and sends
 *  it into client driver thread associated with actual client
 *
 *  @param localClient object representing client communication channel (UNIX domain connection)
 *  @param data response reference
 *
 *  @see XdbdLocalClientResponseMessage
 */
int	XdbdClientDriver::SendLocalClientHandlerResponse (XdbdLocalClient* localClient, void* data)
{
	int	status;
	XdbdLocalClientResponseMessage*	msg = new XdbdLocalClientResponseMessage (localClient, data, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

/*! @brief send remote client response
 *
 *  This is actually request to send response. This request is generally generated by one of
 *  working threads. Since this thread is different from thread which handles remote client
 *  communication it has to be sent into the message queue of that particular thread. And that is
 *  exactly what that function do: it generates message (request) to send client response and sends
 *  it into client driver thread associated with actual client
 *
 *  @param localClient object representing client communication channel (network connection)
 *  @param data response reference
 *
 *  @see XdbdNetworkClientResponseMessage
 */
int	XdbdClientDriver::SendNetworkClientHandlerResponse (XdbdNetworkClient* networkClient, void* data)
{
	int	status;
	XdbdNetworkClientResponseMessage*	msg = new XdbdNetworkClientResponseMessage (networkClient, data, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

/*! @brief send message queue client response
 *
 *  This is actually request to send response. This request is generally generated by one of
 *  working threads. Since this thread is different from thread which handles message queue client
 *  communication it has to be sent into the message queue of that particular thread. And that is
 *  exactly what that function do: it generates message (request) to send client response and sends
 *  it into client driver thread associated with actual client
 *
 *  @param localClient object representing client communication channel (message queue connection)
 *  @param data response reference
 *
 *  @see XdbdMqClientResponseMessage
 */
int	XdbdClientDriver::SendMqClientHandlerResponse (XdbdMqClient* mqClient, void* data)
{
	int	status;
	XdbdMqClientResponseMessage*	msg = new XdbdMqClientResponseMessage (mqClient, data, this);
	struct { void* msg; }	m = { (void*) msg };
	if ((status = m_clientHandler->MQSend ((char*) &m, sizeof m)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot save internal message"));
		delete	msg;
	}
	return	status;
}

} /* namespace xml_db_daemon */

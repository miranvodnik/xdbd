/*
 * XdbdClientDriver.h
 *
 *  Created on: 11. nov. 2016
 *      Author: miran
 */

#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "XdbdRunnable.h"
#include "XdbdCommon.h"
#include "XdbdClientHandler.h"
#include "XdbdLocalClient.h"
#include "XdbdNetworkClient.h"
#include "XdbdMqClient.h"

#include <set>
#include <map>
#include <deque>
#include <list>

using namespace std;

namespace xml_db_daemon
{

/*! @brief client driver thread
 *
 *  this object implements XdbdRunnable interface and is thus capable
 *  of running in its own working thread. Its main purpose is to handle
 *  theoretically unlimited number of communication channels between
 *  XML database server and its clients regardless if they are local or
 *  remote and regardless of their connection mode (UNIX domain, network
 *  or message queue). All these channels are multiplexed and treated
 *  with equal priority by this object
 *
 */
class XdbdClientDriver : public XdbdRunnable
{
private:
	typedef set < XdbdLocalClient* >	lcset;	//!< type representing set of local clients connected through UNIX domain socket
	typedef set < XdbdNetworkClient* >	ncset;	//!< type representing set of network clients connected through BSD sockets
	typedef set < XdbdMqClient* >	mcset;	//!< type representing set of local clients connected through POSIX message queue
	typedef	list < XdbdClientDriver* >	cdlist;	//!< type representing list of client driver threads

public:
	XdbdClientDriver ();
	virtual ~XdbdClientDriver ();

	static void StartClientDrivers ();
	static void StopClientDrivers ();
	static XdbdClientDriver*	currentClientDriver ();

private:
	virtual int Run (void);
	virtual int InitInstance (void);
	virtual int ExitInstance (void);

public:
	int	SendLocalClientHandlerActivation (int localClientFd);
	int	SendNetworkClientHandlerActivation (int networkClientFd);
	int	SendMqClientHandlerActivation (int mqClientFd);

	int	SendLocalClientHandlerResponse (XdbdLocalClient* localClient, void*data);
	int	SendNetworkClientHandlerResponse (XdbdNetworkClient* networkClient, void*data);
	int	SendMqClientHandlerResponse (XdbdMqClient* mqClient, void*data);

	int	ActivateLocalClientHandler (int localClientFd);
	int	ActivateNetworkClientHandler (int networkClientFd);
	int	ActivateMqClientHandler (int mqClientFd);

	void	HandleAdminRequest (XdbdLocalClient* cln, XdbdRequest* req);
	inline void	PostAdminRequest (XdbdRequest* req) { HandleAdminRequest (0, req); }

	void	ReleaseLocalClient (XdbdLocalClient* localClient);
	void	ReleaseNetworkClient (XdbdNetworkClient* networkClient);
	void	ReleaseMqClient (XdbdMqClient* mqClient);

private:
	inline static void ThreadCleanup (void* data) { ((XdbdClientDriver*)data)->ThreadCleanup(); }
	void ThreadCleanup ();
private:
	static cdlist	g_cdlist;	//!< list of client driver threads
	static bool	g_started;	//!< indicator set to true when client driver set is populated
	static cdlist::iterator	g_currentClientDriver;	//!< current client driver
	XdbdClientHandler*	m_clientHandler;	//!< client driver multiplexer

	lcset	m_lcset;	//!< set of local clients connected through UNIX domain socket
	ncset	m_ncset;	//!< set of remote clients connected through BSD socket
	mcset	m_mcset;	//!< set of local clients connected through POSIX message queue
};

} /* namespace xml_db_daemon */

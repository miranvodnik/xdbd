/*
 * XdbdNetworkClient.h
 *
 *  Created on: 30. okt. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdRunningContext.h"
#include "XdbdRunnable.h"
#include "XdbdWorkingClient.h"
#include "XdbdClientHandler.h"

namespace xml_db_daemon
{

class XdbdNetworkClient : public XdbdWorkingClient
{
public:
	XdbdNetworkClient(XdbdRunningContext* ctx, int fd, XdbdRunnable* mainThread);
	virtual ~XdbdNetworkClient();
	virtual	void	first_half_callback (void* data) {};
	virtual	void	second_half_callback (void* data) {};

private:
	fd_handler(HandleNetworkClient, XdbdNetworkClient)

	void	Release ();
private:
	XdbdRunningContext*	m_ctx;
	int	m_fd;

	void*	m_networkHandler;
};

class XdbdNetworkClientActivationMessage : public XdbdRunningContextMsg
{
public:
	XdbdNetworkClientActivationMessage (int networkClientFd, void* clientDriver) : XdbdRunningContextMsg () { m_networkClientFd = networkClientFd; m_clientDriver = clientDriver; }
	virtual ~XdbdNetworkClientActivationMessage () {}
	virtual int	Invoke (XdbdRunningContext* ctx);
	virtual const char* Name () { return "XdbdNetworkClientActivationMessage"; }
private:
	int	m_networkClientFd;
	void*	m_clientDriver;
};

class XdbdNetworkClientResponseMessage : public XdbdRunningContextMsg
{
public:
	XdbdNetworkClientResponseMessage (XdbdNetworkClient* networkClient, void* data, void* clientDriver) : XdbdRunningContextMsg () { m_networkClient = networkClient; m_data = data; m_clientDriver = clientDriver; }
	virtual ~XdbdNetworkClientResponseMessage () {}
	virtual int	Invoke (XdbdRunningContext* ctx);
	virtual const char* Name () { return "XdbdNetworkClientResponseMessage"; }
private:
	XdbdNetworkClient*	m_networkClient;
	void*	m_data;
	void*	m_clientDriver;
};

} /* namespace xml_db_daemon */

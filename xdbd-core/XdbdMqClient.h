/*
 * XdbdMqClient.h
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

class XdbdMqClient : public XdbdWorkingClient
{
public:
	XdbdMqClient(XdbdRunningContext* ctx, int fd, XdbdRunnable* mainThread);
	virtual ~XdbdMqClient();
	virtual	void	first_half_callback (void* data) {};
	virtual	void	second_half_callback (void* data) {};

	void	Release ();
private:
	fd_handler(HandleMqClient, XdbdMqClient)

private:
	XdbdRunningContext*	m_ctx;
	int	m_fd;

	void*	m_mqHandler;
};

class XdbdMqClientActivationMessage : public XdbdRunningContextMsg
{
public:
	XdbdMqClientActivationMessage (int mqClientFd, void* clientDriver) : XdbdRunningContextMsg () { m_mqClientFd = mqClientFd; m_clientDriver = clientDriver; }
	virtual ~XdbdMqClientActivationMessage () {}
	virtual int	Invoke (XdbdRunningContext* ctx);
	virtual const char* Name () { return "XdbdMqClientActivationMessage"; }
private:
	int	m_mqClientFd;
	void*	m_clientDriver;
};

class XdbdMqClientResponseMessage : public XdbdRunningContextMsg
{
public:
	XdbdMqClientResponseMessage (XdbdMqClient* mqClient, void* data, void* clientDriver) : XdbdRunningContextMsg () { m_mqClient = mqClient; m_data = data; m_clientDriver = clientDriver; }
	virtual ~XdbdMqClientResponseMessage () {}
	virtual int	Invoke (XdbdRunningContext* ctx);
	virtual const char* Name () { return "XdbdMqClientResponseMessage"; }
private:
	XdbdMqClient*	m_mqClient;
	void*	m_data;
	void*	m_clientDriver;
};

} /* namespace xml_db_daemon */

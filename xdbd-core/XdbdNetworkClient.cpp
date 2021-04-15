/*
 * XdbdNetworkClient.cpp
 *
 *  Created on: 30. okt. 2015
 *      Author: miran
 */

#include <fcntl.h>
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
#include "XdbdNetworkClient.h"
#include "XdbdWorker.h"
#include "XdbdClientDriver.h"

namespace xml_db_daemon
{

XdbdNetworkClient::XdbdNetworkClient(XdbdRunningContext* ctx, int fd, XdbdRunnable* mainThread) : XdbdWorkingClient (mainThread)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "CREATE XdbdNetworkClient %ld", this);
#endif
	m_ctx = ctx;
	m_fd = fd;

	m_networkHandler = ctx->RegisterDescriptor(EPOLLIN, fd, HandleNetworkClient, this, ctx_info);
}

XdbdNetworkClient::~XdbdNetworkClient()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "DELETE XdbdNetworkClient %ld", this);
#endif
	Release ();
}

void XdbdNetworkClient::Release ()
{
	if (m_networkHandler != 0)
		m_ctx->RemoveDescriptor(m_networkHandler);
	m_networkHandler = 0;
	if (m_fd > 0)
		close (m_fd);
	m_fd = 0;
}

void	XdbdNetworkClient::HandleNetworkClient (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	if (flags & EPOLLIN)
	{
		int	count;
		char	buff[1024];
		if ((count = recv (fd, buff, 1024, 0)) <= 0)
		{
			((XdbdClientDriver*)mainThread())->ReleaseNetworkClient(this);
			delete	this;
			return;
		}
	}
	else	if (flags & EPOLLOUT)
	{
	}
	else
	{
	}
}

int	XdbdNetworkClientActivationMessage::Invoke (XdbdRunningContext* ctx)
{
	((XdbdClientDriver*)m_clientDriver)->ActivateNetworkClientHandler(m_networkClientFd);
	return	0;
}

int	XdbdNetworkClientResponseMessage::Invoke (XdbdRunningContext* ctx)
{
	return	0;
}

} /* namespace xml_db_daemon */

/*
 * XdbdMqClient.cpp
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
#include "XdbdMqClient.h"
#include "XdbdWorker.h"
#include "XdbdClientDriver.h"

namespace xml_db_daemon
{

XdbdMqClient::XdbdMqClient(XdbdRunningContext* ctx, int fd, XdbdRunnable* mainThread) : XdbdWorkingClient (mainThread)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "CREATE XdbdMqClient %ld", this);
#endif
	m_ctx = ctx;
	m_fd = fd;

	m_mqHandler = ctx->RegisterDescriptor(EPOLLIN, fd, HandleMqClient, this, ctx_info);
}

XdbdMqClient::~XdbdMqClient()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "DELETE XdbdMqClient %ld", this);
#endif
	Release ();
}

void XdbdMqClient::Release ()
{
	if (m_mqHandler != 0)
		m_ctx->RemoveDescriptor(m_mqHandler);
	m_mqHandler = 0;
	if (m_fd > 0)
		close (m_fd);
	m_fd = 0;
	((XdbdClientDriver*)mainThread())->ReleaseMqClient(this);
}

void	XdbdMqClient::HandleMqClient (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{

}

int	XdbdMqClientActivationMessage::Invoke (XdbdRunningContext* ctx)
{
	((XdbdClientDriver*)m_clientDriver)->ActivateMqClientHandler(m_mqClientFd);
	return	0;
}

int	XdbdMqClientResponseMessage::Invoke (XdbdRunningContext* ctx)
{
	return	0;
}

} /* namespace xml_db_daemon */

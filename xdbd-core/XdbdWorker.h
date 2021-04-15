/*
 * XdbdWorker.h
 *
 *  Created on: 30. okt. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdRunningContext.h"
#include "XdbdWorkingThread.h"
#include "XdbdClientDriver.h"
#include "XdbdLocalClient.h"
#include "XdbdNetworkClient.h"
#include "XdbdMqClient.h"
#include "XdbdCommon.h"
#include "XdbdMessages.h"
#include "XdbdMessageQueue.h"
#include "XdbdAnyJob.h"
#include "XdbdHamThread.h"

#include <set>
#include <map>
#include <deque>
#include <list>

using namespace std;

namespace xml_db_daemon
{

/*! @brief working entity
 *
 *  this class represents working entity I/O multiplexer and runs
 *  in main thread of working entity process. It is XML DB server
 *  which enables virtually unlimited number of client to connect
 *  to it simultaneously
 *
 */
class XdbdWorker: public XdbdRunningContext
{
private:
	XdbdWorker (int);
public:
	XdbdWorker (int n, char* p[]);
	virtual ~XdbdWorker();

	void	HandleAdminRequest (XdbdLocalClient* cln, XdbdRequest* req);
	inline void	PostAdminRequest (XdbdRequest* req) { HandleAdminRequest (0, req); }

private:
	// starting routine of cdcp worker proxy
	ctx_starter (StartWorker, XdbdWorker)
	ctx_finisher (StopWorker, XdbdWorker)
	ctx_timehook (TimeHookWorker, XdbdWorker)

	fd_handler (HandleMainLocalClient, XdbdWorker)
	fd_handler (HandleMainLocalClientReader, XdbdWorker)
	fd_handler (HandleMainLocalClientWriter, XdbdWorker)
	fd_handler (HandleLocalListener, XdbdWorker)
	fd_handler (HandleNetworkListener, XdbdWorker)
	fd_handler (HandleMqListener, XdbdWorker)

	sig_handler (HangupSignalHandler, XdbdWorker)
	sig_handler (AbortSignalHandler, XdbdWorker)

	void	Release ();
private:
	static const int	g_localListener;	//!< UNIX domain listener fd (3)
	static const int	g_networkListener;	//!< network listener fd (4)
	static const int	g_mqListener;	//!< mq listener fd (5)
	static const int	g_localClientReader;	//!< input controller pipe fd (6)
	static const int	g_localClientWriter;	//!< output controller pipe fd (7)

	static	XdbdWorker*	g_genWorker;	//!< helper instance to make runtime initialization
	static	XdbdRequest	g_stopRequest;	//!< stop request message reference made by runtime initialization

	int	m_intSize;	//!< size of XDR integer serialization
	u_char*	m_outputBuffer;	//!< output buffer for output controller pipe
	u_char*	m_outputPtr;	//!< write head of output buffer
	u_char*	m_outputEnd;	//!< par end point of output buffer

	ctx_fddes_t	m_localClientReaderHandler;	//!< input controller pipe handler
	ctx_fddes_t	m_localClientWriterHandler;	//!< output controller pipe handler

	ctx_fddes_t	m_localListenerHandler;	//!< UNIX domain listener handler
	ctx_fddes_t	m_networkListenerHandler;	//!< network listener handler
	ctx_fddes_t	m_mqListenerHandler;	//!< message queue listener handler

	ctx_sig_t	m_hangupSignalHandler;	//!< HANGUP handler
	ctx_sig_t	m_abortSignalHandler;	//!< ABORT handler

	#if defined (XDBD_HAM)
	XdbdHamThread*	m_hamThread;
#endif
};

} /* namespace xml_db_daemon */

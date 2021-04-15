/*
 * XdbdMessageQueue.cpp
 *
 *  Created on: 3. nov. 2015
 *      Author: miran
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "XdbdCommon.h"
#include "XdbdMessageQueue.h"
#include <iostream>
using namespace std;
using namespace xml_db_daemon;

namespace xml_db_daemon
{

bool	XdbdMessageQueue::g_initialized = false;
XdbdMessageQueue*	XdbdMessageQueue::g_msgQueue = 0;
XdbdMessageQueue*	XdbdMessageQueue::g_intQueue = 0;

/*! @brief initialize working message queue
 *
 *  function initializes message queue used by working threads. This
 *  queue is used to store references to job requests which should
 *  be executed by working threads
 *
 */
void	XdbdMessageQueue::_Initialize ()
{
	if (g_initialized)
		return;

	g_initialized = true;

#if defined(DEBUG)
	xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdMessageQueue %ld", this);
#endif

	const char*	wfname = ::g_workingQueueKeyPath.c_str();
	if (access (wfname, F_OK) < 0)
	{
		int	fd;
		if ((fd = open (wfname, O_CREAT|O_RDONLY, 0x444)) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xdbd message queue key file: open(%s) failed, errno = "), wfname, errno);
			throw	1;
		}
		close (fd);
	}
	key_t	key = ftok (wfname, 1);
	if (key == (key_t)-1)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xdbd message queue key: ftok(%s) failed, errno = "), wfname, errno);
		throw	2;
	}
	g_msgQueue = new XdbdMessageQueue(key);

	key = ftok (wfname, 2);
	if (key == (key_t)-1)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xdbd message queue key: ftok(%s) failed, errno = "), wfname, errno);
		throw	3;
	}
	g_intQueue = new XdbdMessageQueue(key);
}

/*! brief message queue object constructor
 *
 *  it doesn't do anything. Initialization is performed by _Initialize()
 *
 */
XdbdMessageQueue::XdbdMessageQueue(key_t key)
{
	m_mtype = (long) getpid ();
	if ((m_reqQue = msgget (key, IPC_CREAT | 0666)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xdbd message queue: msgget(%d) failed, errno = "), key, errno);
		throw	3;
	}

	_Clear (false);
}

/*! @brief destroy message queue
 *
 *  it deletes System V message queue initialize by _Initialize()
 *
 */
XdbdMessageQueue::~XdbdMessageQueue()
{
#if defined(DEBUG)
	xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdMessageQueue %ld", this);
#endif
	if (m_reqQue > 0)
	{
		msqid_ds	mqds;
		msgctl (m_reqQue, IPC_STAT, &mqds);
		msgctl (m_reqQue, IPC_RMID, &mqds);
	}
}

/*! @brief put job reference into message queue
 *
 *  function forms message composed of message type (m_mtype is
 *  in fact a process PID) and job reference and writes it into
 *  System V message queue governed by this object. If it does
 *  not succeed, it logs error message and deletes job
 *
 *  @param job job reference
 *
 *  @return true job reference was successfully put into message queue
 *  @return false job reference was not put into message queue
 *
 */
bool XdbdMessageQueue::_Put (XdbdBaseJob* job)
{
	int	res;
	struct
	{
		long	mtype;
		XdbdBaseJob*	job;
	}	msg;

	msg.mtype = m_mtype;
	msg.job = job;
	if ((res = msgsnd (m_reqQue, &msg, sizeof (XdbdBaseJob*), IPC_NOWAIT)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot put job: msgsnd(%d) failed, errno = %d"), m_reqQue, errno);
		delete	job;
	}

	return	(res == 0) ? true : false;
}

/*! @brief get job reference from message queue
 *
 *  function blocks until new message is received from System V
 *  message queue governed by this object. Received message is
 *  composed of two components: message type (which is in fact
 *  process PID) and job reference. In case of any error error
 *  message is logged and queue descriptor is recreated.
 *
 *  @return 0 job reference cannot be fetched from message queue
 *  @return other job reference fetched from message queue
 *
 */
XdbdBaseJob*	XdbdMessageQueue::_Get ()
{
	XdbdBaseJob*	job = 0;
	int	ret;
	struct
	{
		long	mtype;
		XdbdBaseJob*	job;
	}	msg;

	while (true)
	{
		ret = msgrcv (m_reqQue, &msg, sizeof (XdbdBaseJob*), m_mtype, 0);

		if (ret > 0)
		{
			job = msg.job;
			break;
		}
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot get job: msgrcv(%d) failed, errno = %d"), m_reqQue, errno);
		if ((errno != EIDRM) && (errno != EINVAL))
			break;
		if ((m_reqQue = msgget (IPC_PRIVATE, IPC_CREAT | 0666)) < 0)
			break;
	}

	return	job;
}

/*! @brief remove all job references from message queue
 *
 *  this function removes all job references from message queue
 *  governed by this object. Removal can be destructive (every
 *  referenced job is deleted) or non-destructive (useful at the
 *  start of process if there are references to jobs created by
 *  previous incarnation of process)
 *
 *  @param destructive flag to alternate between destructive and
 *  non-destructive removal of job references
 *
 */
void XdbdMessageQueue::_Clear (bool destructive)
{
	struct
	{
		long	mtype;
		XdbdBaseJob*	job;
	}	msg;

	while (msgrcv (m_reqQue, &msg, sizeof (XdbdBaseJob*), 0, IPC_NOWAIT | MSG_NOERROR) >= 0)
	{
		try
		{
			if (destructive)
			{
				delete	msg.job;
			}
		}
		catch (...)
		{
		}
	}
}

} /* namespace xml_db_daemon */

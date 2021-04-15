/*
 * XdbdSqlJob.h
 *
 *  Created on: 6. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdAnyJob.h"
#include "XdbdWorkingClient.h"
#include "XdbdWorkingThread.h"
#include "XdbdMessages.h"
#include "XdbdCommon.h"
#include "XdbdShmSegment.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! brief superclass representing common properties of SQL jobs
 *
 *  superclass for the following SQL job classes: XddbSqlConnectJob,
 *  XdbdSqlDisconnectJob, XdbdSqlCreateStatementJob, XdbdSqlDeleteStatementJob,
 *  XdbdSqlPrepareJob, XdbdSqlExecuteJob and XdbdSqlExecuteDirectJob. It
 *  defines common components of all jobs above: shared memory segment
 *  and protocol request associated with SQL job
 *
 */
class XdbdSqlJob: public XdbdAnyJob
{
public:
	XdbdSqlJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request, u_long execTimer);
	virtual ~XdbdSqlJob();

	inline XdbdRequest* request () { return m_request; }	//!< get protocol message reference
	inline XdbdShmSegment* shmSegment () { return m_shmSegment; }	//!< get shared memory object reference
protected:
	XdbdRequest* m_request;	//!< protocol message reference
	XdbdShmSegment*	m_shmSegment;	//!< shared memory object reference
};

} /* namespace xml_db_daemon */

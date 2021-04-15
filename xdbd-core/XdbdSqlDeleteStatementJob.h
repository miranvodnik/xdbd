/*
 * XdbdSqlDeleteStatementJob.h
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief delete-statement job class
 *
 *  this job is used to delete shared memory
 *  segment associated with SQL statement
 *
 */
class XdbdSqlDeleteStatementJob: public XdbdSqlJob
{
public:
	XdbdSqlDeleteStatementJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlDeleteStatementJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	XdbdShmSegment* shmSegment () { return m_shmSegment; }	//!< get shared memory name associated with SQL statement (always invalid reference)
	virtual void Report ();
private:
	XdbdShmSegment*	m_shmSegment;	//!< deleted shared memory segment reference
};

} /* namespace xml_db_daemon */

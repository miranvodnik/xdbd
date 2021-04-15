/*
 * XdbdSqlCreateStatementJob.h
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief create-statement job class
 *
 *  this job is used to create name of shared memory
 *  segment associated with SQL statement
 *
 */
class XdbdSqlCreateStatementJob: public XdbdSqlJob
{
public:
	XdbdSqlCreateStatementJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlCreateStatementJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	string shmName () { return m_shmName; }	//!< get shared memory name associated with SQL statement
	virtual void Report ();
private:
	static	unsigned long	g_shmIndex;	//!< every increasing index used to produce shared memory name
	string	m_shmName;	//!< shared memory name associated with SQL statement
};

} /* namespace xml_db_daemon */

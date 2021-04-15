/*
 * XdbdSqlExecuteJob.h
 *
 *  Created on: 18. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlParser.h"
#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief SQL execute job
 *
 *  executes prepared SQL statements
 *
 */
class XdbdSqlExecuteJob: public XdbdSqlJob
{
public:
	XdbdSqlExecuteJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlExecuteJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
};

} /* namespace xml_db_daemon */

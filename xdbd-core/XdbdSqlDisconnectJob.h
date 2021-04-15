/*
 * XdbdSqlDisconnectJob.h
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief disconnect job
 *
 *  this class is almost useless since it provides almost
 *  no functionality by its own. But since it is one of SQL
 *  jobs, it must implement all functionality like other jobs
 *  which are treated similarly
 *
 */
class XdbdSqlDisconnectJob: public XdbdSqlJob
{
public:
	XdbdSqlDisconnectJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlDisconnectJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
};

} /* namespace xml_db_daemon */

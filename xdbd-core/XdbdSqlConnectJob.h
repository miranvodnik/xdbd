/*
 * XdbdSqlConnectJob.h
 *
 *  Created on: 9. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief connect job
 *
 *  this class is almost useless since it provides almost
 *  no functionality by its own. But since it is one of SQL
 *  jobs, it must implement all functionality like other jobs
 *  which are treated similarly
 *
 */
class XdbdSqlConnectJob: public XdbdSqlJob
{
public:
	XdbdSqlConnectJob (XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlConnectJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
};

} /* namespace xml_db_daemon */

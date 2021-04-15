/*
 * XdbdSqlDeleteJob.h
 *
 *  Created on: 9. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlJob.h"
#include "XdbdSqlParser.h"

namespace xml_db_daemon
{

class XdbdSqlDeleteJob: public XdbdSqlJob
{
public:
	XdbdSqlDeleteJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlDeleteJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
private:
	XdbdSqlParser*	m_sqlParser;
};

} /* namespace xml_db_daemon */

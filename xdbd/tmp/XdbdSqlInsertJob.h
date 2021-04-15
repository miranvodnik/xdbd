/*
 * XdbdSqlInsertJob.h
 *
 *  Created on: 9. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlJob.h"
#include "XdbdSqlParser.h"

namespace xml_db_daemon
{

class XdbdSqlInsertJob: public XdbdSqlJob
{
public:
	XdbdSqlInsertJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlInsertJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
private:
	XdbdSqlParser*	m_sqlParser;
};

} /* namespace xml_db_daemon */

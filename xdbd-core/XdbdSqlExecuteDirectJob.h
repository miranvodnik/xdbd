/*
 * XdbdSqlExecuteDirectJob.h
 *
 *  Created on: 18. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlParser.h"
#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief execute-direct job
 *
 *  parses and executes SQL statements
 *
 */
class XdbdSqlExecuteDirectJob: public XdbdSqlJob
{
public:
	XdbdSqlExecuteDirectJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlExecuteDirectJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	inline XdbdSqlParser* extractStatement () { XdbdSqlParser* sqlStatement = m_sqlParser; m_sqlParser = 0; return sqlStatement; }
	virtual void Report ();
private:
	XdbdSqlParser*	m_sqlParser;	//!< SQL statement parser object
};

} /* namespace xml_db_daemon */

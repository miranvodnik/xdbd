/*
 * XdbdSqlPrepareJob.h
 *
 *  Created on: 18. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdSqlJob.h"
#include "XdbdSqlParser.h"

namespace xml_db_daemon
{

/*! @brief SQL prepare job class
 *
 *  prepares SQL statements for execution
 *
 */
class XdbdSqlPrepareJob: public XdbdSqlJob
{
public:
	XdbdSqlPrepareJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request);
	virtual ~XdbdSqlPrepareJob();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	inline XdbdSqlParser* extractStatement () { XdbdSqlParser* sqlStatement = m_sqlParser; m_sqlParser = 0; return sqlStatement; }
	virtual void Report ();
private:
	XdbdSqlParser*	m_sqlParser;	//!< parser for associated SQL statement
};

} /* namespace xml_db_daemon */

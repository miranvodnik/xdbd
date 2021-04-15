/*
 * XdbdSqlInsertJob.cpp
 *
 *  Created on: 9. nov. 2015
 *      Author: miran
 */

#include "XdbdLocalClient.h"
#include "XdbdSqlInsertJob.h"

namespace xml_db_daemon
{

XdbdSqlInsertJob::XdbdSqlInsertJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlInsertJob %ld", this);
#endif
	m_sqlParser = 0;
}

XdbdSqlInsertJob::~XdbdSqlInsertJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlInsertJob %ld", this);
#endif
	delete	m_sqlParser;
	m_sqlParser = 0;
}

int XdbdSqlInsertJob::Execute (XdbdWorkingThread* wt)
{
	XdbdRequest*	req = request();
	int	nrpar = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_len;
	XdbdParameter*	parameters = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_val;

	if (nrpar < 1)
	{
		m_errorCode = -1;
		m_errorMessage = "missing SQL statement";
		return	m_returnStatus = -1;
	}

	if ((parameters[0].m_flags & STRVAL) == 0)
	{
		m_errorCode = -1;
		m_errorMessage = "missing SQL statement";
		return	m_returnStatus = -1;
	}

	XdbdLocalClient*	cln = (XdbdLocalClient*) workingClient();
	XdbdShmSegment*	shmSegment = cln->shmSegment(parameters[0].m_longVal);
	if (shmSegment == 0)
	{
		m_errorCode = -1;
		m_errorMessage = "incorrect SHM segment";
		return	m_returnStatus = -1;
	}

	m_sqlParser = new XdbdSqlParser ();
	if (m_sqlParser->Parse (parameters[0].m_stringVal) != 0)
	{
		m_errorCode = -1;
		m_errorMessage = "syntax errors in SQL statement '";
		m_errorMessage += parameters[0].m_stringVal;
		m_errorMessage += "'";
		return	m_returnStatus = -1;
	}

	if (m_sqlParser->PrepareInsertStatement(shmSegment) != 0)
	{
		m_errorCode = -1;
		m_errorMessage = "cannot prepare INSERT statement '";
		m_errorMessage += parameters[0].m_stringVal;
		m_errorMessage += "'";
		return	m_returnStatus = -1;
	}

	return	m_returnStatus = 0;
}

int XdbdSqlInsertJob::Cleanup ()
{
	return	0;
}

} /* namespace xml_db_daemon */

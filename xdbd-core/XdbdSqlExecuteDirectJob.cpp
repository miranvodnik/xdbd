/*
 * XdbdSqlExecuteDirectJob.cpp
 *
 *  Created on: 18. dec. 2015
 *      Author: miran
 */

#include "XdbdSqlExecuteDirectJob.h"
#include "XdbdLocalClient.h"

namespace xml_db_daemon
{

/*! @brief SQL execute-direct job construction
 *
 *  initializes its superclass object with all input parameters
 *  and additional execution timer value of 10 seconds
 *
 *  @param p_workingClient client driver thread reference which will
 *  be used to communicate reply message
 *  @param p _shmSegment shared memory reference which is always 0
 *  since clients has not been connected and thus has not created
 *  no sql statements
 *  @param p_request original protocol request reference
 *
 */
XdbdSqlExecuteDirectJob::XdbdSqlExecuteDirectJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlExecuteDirectJob %ld", this);
#endif
	m_sqlParser = 0;
}

/*! brief SQL execute-direct job destruction
 *
 *  deletes SQL statement parser object if it has been
 *  created and not reallocated. If statement executes
 *  successfully, SQL parser object is reallocated to
 *  shared memory object associated with this object
 *
 */
XdbdSqlExecuteDirectJob::~XdbdSqlExecuteDirectJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlExecuteDirectJob %ld", this);
#endif
	delete	m_sqlParser;
	m_sqlParser = 0;
}

/*! brief execute-direct job execution
 *
 *  function invokes direct execution of SQL statement. It follows
 *  these steps:
 *
 *  - checks to see if there are all parameters present and that they
 *  have correct type
 *
 *  - integer valued parameter is shared memory ticket referencing
 *  shared memory description created by SQL create statement
 *
 *  - string valued parameter is SQL statement text
 *
 *  - in next step function parses SQL statement
 *
 *  - then prepares it for execution
 *
 *  - finally it executes it and stores retrieved data into
 *  shared memory object
 *
 *  Function terminates by reporting SQL reply message to client
 *  driver thread which created this job
 *
 *  @param wt working thread reference
 *
 *  @return 0 all steps executed successfully
 *  @return -1 one of steps failed
 *
 */
int XdbdSqlExecuteDirectJob::Execute (XdbdWorkingThread* wt)
{
	XdbdRequest*	req = request();
	int	nrpar = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_len;
	XdbdParameter*	parameters = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_val;
	XdbdLocalClient*	cln = 0;
	XdbdShmSegment*	shmSegment = 0;

	m_returnStatus = -1;
	do
	{
		if (nrpar < 1)
		{
			m_errorCode = -1;
			m_errorMessage = "missing SQL statement";
			break;
		}

		if ((parameters[0].m_flags & (INTVAL|STRVAL)) != (INTVAL|STRVAL))
		{
			m_errorCode = -1;
			m_errorMessage = "missing SQL statement";
			break;
		}

		cln = (XdbdLocalClient*) workingClient();
		shmSegment = cln->shmSegment(parameters[0].m_longVal);
		if (shmSegment == 0)
		{
			m_errorCode = -1;
			m_errorMessage = "incorrect SHM segment";
			break;
		}

		m_sqlParser = new XdbdSqlParser ();
		if (m_sqlParser->Parse (parameters[0].m_stringVal) != 0)
		{
			m_errorCode = -1;
			m_errorMessage = "syntax errors in SQL statement '";
			m_errorMessage += parameters[0].m_stringVal;
			m_errorMessage += "'";
			break;
		}

		if (m_sqlParser->PrepareStatement(shmSegment) != 0)
		{
			m_errorCode = -1;
			m_errorMessage = "cannot prepare SELECT statement '";
			m_errorMessage += parameters[0].m_stringVal;
			m_errorMessage += "'";
			break;
		}

		if (m_sqlParser->ExecuteStatement (shmSegment) != 0)
		{
			m_errorCode = -1;
			m_errorMessage = "cannot execute statement '";
			m_errorMessage += parameters[0].m_stringVal;
			m_errorMessage += "'";
			break;
		}

		m_shmSegment = shmSegment;
		m_returnStatus = 0;
	}
	while (false);
	workingClient()->first_half_callback(this);
	return	m_returnStatus;
}

/*! brief execute-direct job cleanup
 *
 *  long-term executing job should be canceled by working
 *  thread cancellation mechanism. In this case return status
 *  of job will remain negative (initialized in Execute()
 *  method). Reply message will be communicated back to client
 *  driver thread, which will take appropriate action: report
 *  error message to the client and destroy job
 *
 *  @param wt working thread reference
 *
 *  @return	0 always
 *
 */
int XdbdSqlExecuteDirectJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report execute-direct job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: sql execute direct
 *
 */
void XdbdSqlExecuteDirectJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - execute direct"));
}

} /* namespace xml_db_daemon */

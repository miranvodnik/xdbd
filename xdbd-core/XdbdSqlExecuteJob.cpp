/*
 * XdbdSqlExecuteJob.cpp
 *
 *  Created on: 18. dec. 2015
 *      Author: miran
 */

#include "XdbdSqlExecuteJob.h"
#include "XdbdLocalClient.h"

namespace xml_db_daemon
{

/*! @brief SQL execute job construction
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
XdbdSqlExecuteJob::XdbdSqlExecuteJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlExecuteJob %ld", this);
#endif
}

/*! brief SQL execute job destruction
 *
 *  nothing special
 *
 */
XdbdSqlExecuteJob::~XdbdSqlExecuteJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlExecuteJob %ld", this);
#endif
}

/*! brief SQL execute job execution
 *
 *  function invokes execution of prepared SQL statement. It follows
 *  these steps:
 *
 *  - checks to see if there is shared memory ticket present in
 *  request parameters collection and that this ticket references
 *  existing shared memory object of client associated with this
 *  request
 *
 *  - in next step function checks if statement has been successfully
 *  parsed
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
int XdbdSqlExecuteJob::Execute (XdbdWorkingThread* wt)
{
	XdbdRequest*	req = request();
	int	nrpar = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_len;
	XdbdParameter*	parameters = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_val;
	XdbdLocalClient*	cln = 0;
	XdbdShmSegment*	shmSegment = 0;
	XdbdSqlParser*	sqlParser = 0;

	m_returnStatus = -1;
	do
	{
		if (nrpar < 1)
		{
			m_errorCode = -1;
			m_errorMessage = "missing SQL statement";
			break;
		}

		if ((parameters[0].m_flags & INTVAL) != INTVAL)
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

		sqlParser = (XdbdSqlParser*) shmSegment->sqlParser();
		if (sqlParser == 0)
		{
			m_errorCode = -1;
			m_errorMessage = "SQL statement '";
			m_errorMessage += shmSegment->sqlStmt();
			m_errorMessage += "' was not prepared";
			break;
		}

#if defined(DEBUG)
		if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "Execute statement: %s", shmSegment->sqlStmt().c_str());
#endif

		if (sqlParser->ExecuteStatement (shmSegment) != 0)
		{
			m_errorCode = -1;
			m_errorMessage = "cannot execute statement '";
			m_errorMessage += shmSegment->sqlStmt();
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

/*! brief SQL execute job cleanup
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
int XdbdSqlExecuteJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report SQL execute job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: sql execute
 *
 */
void XdbdSqlExecuteJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - execute"));
}

} /* namespace xml_db_daemon */

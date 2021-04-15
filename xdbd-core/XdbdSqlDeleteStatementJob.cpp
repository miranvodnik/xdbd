/*
 * XdbdSqlDeleteStatementJob.cpp
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlDeleteStatementJob.h"
#include "XdbdLocalClient.h"

namespace xml_db_daemon {

/*! @brief delete-statement job construction
 *
 *  it initializes its superclass with its own parameters adding
 *  them execution time limit of 10 seconds
 *
 *  @param p_workingClient client driver thread reference which will
 *  be used to communicate reply message
 *  @param p _shmSegment shared memory reference which is always 0
 *  since clients has not been connected and thus has not created
 *  no sql statements
 *  @param p_request original protocol request reference
 *
 */
XdbdSqlDeleteStatementJob::XdbdSqlDeleteStatementJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlDeleteStatementJob %ld", this);
#endif
	m_shmSegment = 0;
}

/*! brief SQL delete-statement job destruction
 *
 *  nothing special
 *
 */
XdbdSqlDeleteStatementJob::~XdbdSqlDeleteStatementJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlDeleteStatementJob %ld", this);
#endif
}

/*! brief delete-statement job execution
 *
 *  purpose of this function is to delete shared segment whose
 *  ticket is deduced from protocol message request remembered
 *  by object constructor. If all goes well (ticket parameter
 *  is present, ticket is of integer type and actual segment
 *  is associated with that ticket) than shared memory segment
 *  is deleted and return status is set to 0. In this case delete
 *  statement will succeed. Otherwise error report message is
 *  generated and return status set to -1 in which case delete
 *  statement will fail. After that reply message is communicated
 *  back to client driver thread
 *
 *  After completion of this function shared memory reference of
 *  deleted shared memory object is set. This reference is thus
 *  always invalid and it cun be used just to check its value. If
 *  it is 0, then shared memory has not been retrieved (either
 *  because it does not exist or because of some erroneous reason),
 *  otherwise it has been retrieved and deleted
 *
 *  @param wt working thread reference
 *
 *  @return 0 delete statement succeeded
 *  @return -1 delete statement failed
 *
 */
int XdbdSqlDeleteStatementJob::Execute (XdbdWorkingThread* wt)
{
	XdbdRequest*	req = request();
	int	nrpar = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_len;
	XdbdParameter*	parameters = req->XdbdRequest_u.m_sqlRequest.m_parameters.m_parameters_val;
	XdbdLocalClient*	cln = 0;

	m_returnStatus = -1;
	do
	{
		if (nrpar < 1)
		{
			m_errorCode = -1;
			m_errorMessage = "missing SHM segment ticket";
			break;
		}

		if ((parameters[0].m_flags & (INTVAL)) != (INTVAL))
		{
			m_errorCode = -1;
			m_errorMessage = "missing SHM segment ticket";
			break;
		}

		cln = (XdbdLocalClient*) workingClient();
		if ((m_shmSegment = cln->deleteShmSegment(parameters[0].m_longVal)) == 0)
		{
			m_errorCode = -1;
			m_errorMessage = "incorrect SHM segment";
			break;
		}

		m_returnStatus = 0;
	}
	while (false);
	workingClient()->first_half_callback(this);
	return	m_returnStatus;
}

/*! brief delete-statement job cleanup
 *
 *  it is almost impossible that delete-statement job will be cleaned up but since
 *  this class's superclass requires implementation of this function it is
 *  implemented like this: it does nothing special but reports reply back
 *  to originating client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return	0 always
 *
 */
int XdbdSqlDeleteStatementJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report delete-statement job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: delete statement
 *
 */
void XdbdSqlDeleteStatementJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - delete statement"));
}

} /* namespace xml_db_daemon */

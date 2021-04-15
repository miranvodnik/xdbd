/*
 * XdbdSqlCreateStatementJob.cpp
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlCreateStatementJob.h"

namespace xml_db_daemon
{

unsigned long	XdbdSqlCreateStatementJob::g_shmIndex = 0;

/*! @brief create-statement job construction
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
XdbdSqlCreateStatementJob::XdbdSqlCreateStatementJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlCreateStatementJob %ld", this);
#endif
}

/*! brief SQL cretae-statement job destruction
 *
 *  nothing special
 *
 */
XdbdSqlCreateStatementJob::~XdbdSqlCreateStatementJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlCreateStatementJob %ld", this);
#endif
}

/*! brief create-statement job execution
 *
 *  creates shared memory name associated with this statement
 *  and reports reply back to originating client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return 0 status of create-statement job
 *
 */
int XdbdSqlCreateStatementJob::Execute (XdbdWorkingThread* wt)
{
	char	shmName[64];

	m_returnStatus = 0;
	sprintf (shmName, "/xdbd-shm.%09ld", g_shmIndex++);
	m_shmName = shmName;
	workingClient()->first_half_callback(this);
	return	m_returnStatus;
}

/*! brief create-statement job cleanup
 *
 *  it is almost impossible that create-statement job will be cleaned up but since
 *  this class's superclass requires implementation of this function it is
 *  implemented like this: it does nothing special but reports reply back
 *  to originating client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return	0 always
 *
 */
int XdbdSqlCreateStatementJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report create-statement job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: create statement
 *
 */
void XdbdSqlCreateStatementJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - create statement"));
}

} /* namespace xml_db_daemon */

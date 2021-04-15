/*
 * XdbdSqlConnectJob.cpp
 *
 *  Created on: 9. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlConnectJob.h"

namespace xml_db_daemon
{

/*! @brief SQL connect job construction
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
XdbdSqlConnectJob::XdbdSqlConnectJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob(p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlConnectJob %ld", this);
#endif
}

/*! brief SQL connect job destruction
 *
 *  nothing special
 *
 */
XdbdSqlConnectJob::~XdbdSqlConnectJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlConnectJob %ld", this);
#endif
}

/*! brief connect job execution
 *
 *  it does nothing special but reports reply back to originating
 *  client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return 0 status of connect job
 *
 */
int XdbdSqlConnectJob::Execute (XdbdWorkingThread* wt)
{
	m_returnStatus = 0;
	workingClient()->first_half_callback(this);
	return	m_returnStatus;
}

/*! brief connect job cleanup
 *
 *  it is almost impossible that connect job will be cleaned up but since
 *  this class's superclass require implementation of this function it is
 *  implemented like this: it does nothing special but reports reply back
 *  to originating client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return	0 always
 *
 */
int XdbdSqlConnectJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report connect job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: sql connect
 *
 */
void XdbdSqlConnectJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - sql connect"));
}

} /* namespace xml_db_daemon */

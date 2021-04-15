/*
 * XdbdSqlDisconnectJob.cpp
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlDisconnectJob.h"

namespace xml_db_daemon
{

/*! @brief SQL disconnect job construction
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
XdbdSqlDisconnectJob::XdbdSqlDisconnectJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request) :
	XdbdSqlJob (p_workingClient, p_shmSegment, p_request, 10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlDisconnectJob %ld", this);
#endif
}

/*! brief SQL disconnect job destruction
 *
 *  nothing special
 *
 */
XdbdSqlDisconnectJob::~XdbdSqlDisconnectJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlDisconnectJob %ld", this);
#endif
}

/*! brief disconnect job execution
 *
 *  it does nothing special but reports reply back to originating
 *  client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return 0 status of disconnect job
 *
 */
int XdbdSqlDisconnectJob::Execute (XdbdWorkingThread* wt)
{
	m_errorCode = 0;
	workingClient()->first_half_callback(this);
	return	m_errorCode;
}

/*! brief disconnect job cleanup
 *
 *  it is almost impossible that disconnect job will be cleaned up but since
 *  this class's superclass require implementation of this function it is
 *  implemented like this: it does nothing special but reports reply back
 *  to originating client driver thread
 *
 *  @param wt working thread reference
 *
 *  @return	0 always
 *
 */
int XdbdSqlDisconnectJob::Cleanup ()
{
	workingClient()->first_half_callback(this);
	return	0;
}

/*! @brief report disconnect job
 *
 *  another function required by this class's superclass. It reports
 *  kind of job: sql disconnect
 *
 */
void XdbdSqlDisconnectJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - sql disconnect"));
}

} /* namespace xml_db_daemon */

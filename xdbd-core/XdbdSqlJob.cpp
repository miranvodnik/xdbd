/*
 * XdbdSqlJob.cpp
 *
 *  Created on: 6. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlJob.h"

namespace xml_db_daemon
{

/*! @brief SQL job class initializer
 *
 *  initializes superclass components for all subclasses: XddbSqlConnectJob,
 *  XdbdSqlDisconnectJob, XdbdSqlCreateStatementJob, XdbdSqlDeleteStatementJob,
 *  XdbdSqlPrepareJob, XdbdSqlExecuteJob and XdbdSqlExecuteDirectJob. It
 *  initializes shared memory object and protocol request associated with
 *  this job
 *
 *  @param p_workingClient working thread which fetched this job.
 *  It is simply transfered to its superclass
 *  @param p_shmSegment shared memory segment associated with statement executing this job
 *  @param p_request protocol request message associated with this job
 *  @param execTimer maximally allowed time to execute request before it will
 *  be canceled by cancellation mechanism of working thread
 *
 */
XdbdSqlJob::XdbdSqlJob(XdbdWorkingClient* p_workingClient, XdbdShmSegment* p_shmSegment, XdbdRequest* p_request, u_long execTimer) : XdbdAnyJob(p_workingClient, execTimer)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdSqlJob %ld", this);
#endif
	m_shmSegment = p_shmSegment;
	m_request = p_request;
}

/*! @brief SQL job destructor
 *
 *  deletes protocol message associated with this job
 *
 */
XdbdSqlJob::~XdbdSqlJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdSqlJob %ld", this);
#endif
	xdr_free ((xdrproc_t) xdr_XdbdRequest, (char*) m_request);
	delete m_request;
}

} /* namespace xml_db_daemon */

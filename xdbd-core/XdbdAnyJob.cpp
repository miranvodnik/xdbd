/*
 * XdbdAnyJob.cpp
 *
 *  Created on: 6. nov. 2015
 *      Author: miran
 */

#include "XdbdAnyJob.h"

namespace xml_db_daemon
{

/*! @brief XdbdAnyJob constructor
 *
 *  Its purpose is to expose working client thread to any
 *  inherited class object
 *
 *  @param p_workingClient XdbdWorkingClient object instance which executed this job
 *  @param execTimer watch dog timer which governs job execution. Job executing too long
 *  is canceled together with working thread executing it
 */

XdbdAnyJob::XdbdAnyJob(XdbdWorkingClient* p_workingClient, u_long execTimer) : XdbdBaseJob (execTimer)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "CREATE XdbdAnyJob %ld", this);
#endif
	m_workingClient = p_workingClient;
}

/*! XdbdAnyJob destructor
 *
 *  virtual destructor enables destruction of inherited class objects
 */
XdbdAnyJob::~XdbdAnyJob()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_APL, "DELETE XdbdAnyJob %ld", this);
#endif
}

} /* namespace xml_db_daemon */

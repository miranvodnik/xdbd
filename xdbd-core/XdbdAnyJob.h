/*
 * XdbdAnyJob.h
 *
 *  Created on: 6. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdBaseJob.h"
#include "XdbdWorkingClient.h"

namespace xml_db_daemon
{

/*! @brief base class for all jobs
 *
 *  it defines interface which should be implemented by all inherited job classes
 */

class XdbdAnyJob : public XdbdBaseJob
{
public:
	XdbdAnyJob(XdbdWorkingClient* p_workingClient, u_long execTimer);	//!< constructor
	virtual ~XdbdAnyJob();	//!< destructor
	virtual int Execute (XdbdWorkingThread* wt) = 0;	//!< must implement job execution logic
	virtual int Cleanup () = 0;	//!< must implement job cancellation logic

	inline XdbdWorkingClient* workingClient () { return m_workingClient; }	//!< returns working thread object reference executing this job
private:
	XdbdWorkingClient*	m_workingClient;	//!< working thread object reference
};

} /* namespace xml_db_daemon */

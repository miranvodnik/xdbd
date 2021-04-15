/*
 * XdbdBaseJob.cpp
 *
 *  Created on: 19. feb. 2016
 *      Author: miran
 */

#include "XdbdBaseJob.h"

namespace xml_db_daemon
{

unsigned int	XdbdBaseJob::g_jobCounter = 0;	//!< application wide unique job number

/*! @brief construct base job
 *
 *  - increments unique job number
 *  - remembers job execution timer (in nanoseconds)
 *
 */
XdbdBaseJob::XdbdBaseJob (u_long execTimer)
{
	m_jobCounter = ++g_jobCounter;
	m_execTimer = execTimer;
}

XdbdBaseJob::~XdbdBaseJob ()
{
	// TODO Auto-generated destructor stub
}

} /* namespace xml_db_daemon */

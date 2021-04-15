/*
 * XdbdWorkingClient.h
 *
 *  Created on: 3. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdRunnable.h"

namespace xml_db_daemon
{

/*! @brief working client superclass
 *
 *  superclass for XdbdLocalClient, XdbdNetworkClient and XdbdMqClient
 *  classes. It requires implementation of first_half_callback () and
 *  second_half_callback () functions. It stores reference to client
 *  driver thread, in which clients run
 *
 */
class XdbdWorkingClient
{
public:
	XdbdWorkingClient(XdbdRunnable* mainThread);
	virtual ~XdbdWorkingClient();
	virtual	void	first_half_callback (void* data) = 0;
	virtual	void	second_half_callback (void* data) = 0;
	XdbdRunnable*	mainThread() { return m_mainThread; }
private:
	XdbdRunnable*	m_mainThread;	//!< thread reference
};

} /* namespace xml_db_daemon */

/*
 * XdbdWorkingClient.cpp
 *
 *  Created on: 3. nov. 2015
 *      Author: miran
 */

#include "XdbdWorkingClient.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief working client constructor
 *
 *  it remembers thread references in which it is running
 *
 *  @param mainThread thread reference
 *
 */
XdbdWorkingClient::XdbdWorkingClient(XdbdRunnable* mainThread)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdWorkingClient %ld", this);
#endif
	m_mainThread = mainThread;
}

/*! @brief working client destructor
 *
 *  it does nothin special
 *
 */
XdbdWorkingClient::~XdbdWorkingClient()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdWorkingClient %ld", this);
#endif
}

} /* namespace xml_db_daemon */

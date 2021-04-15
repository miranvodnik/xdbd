/*
 * XdbdTriggerParameter.cpp
 *
 *  Created on: 9. dec. 2015
 *      Author: miran
 */

#include <string.h>
#include <stdlib.h>
#include <XdbdTriggerParameter.h>

namespace xml_db_daemon
{

XdbdTriggerParameter::XdbdTriggerParameter(short id, short trigid, u_char* name, u_char* value)
{
	m_id = id;
	m_trigid = trigid;
	m_name = (u_char*) strdup ((char*)name);
	m_value = (u_char*) strdup ((char*)value);
}

XdbdTriggerParameter::~XdbdTriggerParameter()
{
	free (m_name);
	free (m_value);
}

} /* namespace xml_db_daemon */

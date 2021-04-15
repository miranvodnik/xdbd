/*
 * XdbdTriggerParameter.h
 *
 *  Created on: 9. dec. 2015
 *      Author: miran
 */

#pragma once

#include <sys/types.h>

namespace xml_db_daemon
{

class XdbdTriggerParameter
{
public:
	XdbdTriggerParameter(short id, short trigid, u_char* name, u_char* value);
	virtual ~XdbdTriggerParameter();
	inline short id () { return m_id; }
	inline short trigid () { return m_trigid; }
	inline u_char* name () { return m_name; }
	inline u_char* value () { return m_value; }
private:
	short	m_id;
	short	m_trigid;
	u_char*	m_name;
	u_char*	m_value;
};

} /* namespace xml_db_daemon */

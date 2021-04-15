/*
 * XdbdOdbcHandle.h
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#pragma once

#include <sqlxdbd.h>
#include "XdbdApi.h"

namespace xml_db_daemon
{

class XdbdOdbcHandle
{
public:
	XdbdOdbcHandle(XdbdOdbcHandle* parent);
	virtual ~XdbdOdbcHandle();
	inline XdbdOdbcHandle* parent () { return m_parent; }
private:
	XdbdOdbcHandle*	m_parent;
};

} /* namespace xml_db_daemon */

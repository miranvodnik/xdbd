/*
 * XdbdOdbcHandle.cpp
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#include "XdbdOdbcHandle.h"

namespace xml_db_daemon
{

XdbdOdbcHandle::XdbdOdbcHandle(XdbdOdbcHandle* parent)
{
	m_parent = parent;
}

XdbdOdbcHandle::~XdbdOdbcHandle()
{
}

} /* namespace xml_db_daemon */

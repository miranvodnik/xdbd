/*
 * XdbdOdbcEnvironment.cpp
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#include "XdbdOdbcEnvironment.h"

namespace xml_db_daemon
{

XdbdOdbcEnvironment::XdbdOdbcEnvironment(XdbdOdbcHandle* parent) : XdbdOdbcHandle (parent)
{
	m_attrOutputNts = SQL_TRUE;
}

XdbdOdbcEnvironment::~XdbdOdbcEnvironment()
{
	for (connlist::iterator it = m_connlist.begin(); it != m_connlist.end(); ++it)
	{
		delete	*it;
	}
	m_connlist.clear();
}

XdbdOdbcConnection*	XdbdOdbcEnvironment::createConnection ()
{
	XdbdOdbcConnection*	connection = new XdbdOdbcConnection (this);
	m_connlist.push_back (connection);
	return	connection;
}

void XdbdOdbcEnvironment::deleteConnection (XdbdOdbcConnection* connection)
{
	for (connlist::iterator it = m_connlist.begin(); it != m_connlist.end(); ++it)
	{
		if (*it != connection)
			continue;
		m_connlist.erase (it);
		break;
	}
	delete connection;
}

} /* namespace xml_db_daemon */

/*
 * XdbdOdbcConnection.cpp
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#include "XdbdOdbcConnection.h"

namespace xml_db_daemon
{

XdbdOdbcConnection::XdbdOdbcConnection(XdbdOdbcHandle* parent) : XdbdOdbcHandle (parent)
{
	Reset ();
}

XdbdOdbcConnection::~XdbdOdbcConnection()
{
	Disconnect ();
	for (stmtlist::iterator it = m_stmtlist.begin(); it != m_stmtlist.end(); ++it)
	{
		delete *it;
	}
	m_stmtlist.clear();
	for (desclist::iterator it = m_desclist.begin(); it != m_desclist.end(); ++it)
	{
		delete *it;
	}
	m_desclist.clear();
	delete [] m_serverName;
	delete [] m_userName;
	delete [] m_authentication;

	Reset ();
}

void XdbdOdbcConnection::Reset ()
{
	m_connection = 0;
	m_attrAutoIpd = SQL_FALSE;
	m_attrMetadataId = SQL_FALSE;
	m_serverName = 0;
	m_userName = 0;
	m_authentication = 0;
}

SQLINTEGER XdbdOdbcConnection::Connect (SQLCHAR* ServerName, SQLSMALLINT NameLength1, SQLCHAR* UserName, SQLSMALLINT NameLength2, SQLCHAR* Authentication, SQLSMALLINT NameLength3)
{
	SQLSMALLINT	serverNameLen = (NameLength1 == SQL_NTS) ? strlen ((char*) ServerName) : NameLength1;
	SQLSMALLINT	userNameLen = (NameLength2 == SQL_NTS) ? strlen ((char*) UserName) : NameLength2;
	SQLSMALLINT	authLen = (NameLength3 == SQL_NTS) ? strlen ((char*) Authentication) : NameLength3;

	m_serverName = new SQLCHAR[serverNameLen + 1];
	m_userName = new SQLCHAR[userNameLen + 1];
	m_authentication = new SQLCHAR[authLen + 1];

	memcpy (m_serverName, ServerName, serverNameLen);
	memcpy (m_userName, UserName, userNameLen);
	memcpy (m_authentication, Authentication, authLen);

	m_serverName [serverNameLen] = 0;
	m_userName [userNameLen] = 0;
	m_authentication [authLen] = 0;

	xdbd_attributes_t	attr;
	attr.wdTimer.tv_sec = 10;
	attr.wdTimer.tv_usec = 0;
	if ((m_connection = XdbdCreateConnection (&attr)) == 0)
		return	SQL_ERROR;
	if (XdbdPostConnect(m_connection, 0, 0) < 0)
		return	SQL_ERROR;
	return	SQL_SUCCESS;
}

SQLINTEGER XdbdOdbcConnection::Disconnect ()
{
	if (m_connection == 0)
		return	SQL_SUCCESS;
	XdbdPostDisconnect(m_connection, 0, 0);
	m_connection = 0;
	return	SQL_SUCCESS;
}

XdbdOdbcStatement*	XdbdOdbcConnection::createStatement ()
{
	if (m_connection == 0)
		return	0;
	xdbd_stmt_t	p_statement = XdbdPostCreateStatement (m_connection, 0, 0);
	if (p_statement == 0)
		return	0;
	XdbdOdbcStatement*	statement = new XdbdOdbcStatement (this, p_statement);
	m_stmtlist.push_back (statement);
	return	statement;
}

XdbdOdbcDescriptor*	XdbdOdbcConnection::createDescriptor ()
{
	XdbdOdbcDescriptor*	descriptor = new XdbdOdbcDescriptor (this, SQL_DESC_ALLOC_USER);
	m_desclist.push_back (descriptor);
	return	descriptor;
}

void	XdbdOdbcConnection::deleteStatement (XdbdOdbcStatement* statement)
{
	for (stmtlist::iterator it = m_stmtlist.begin(); it != m_stmtlist.end(); ++it)
	{
		if (*it != statement)
			continue;
		m_stmtlist.erase (it);
		break;
	}
	if (m_connection != 0)
		XdbdPostDeleteStatement(statement->statement(), 0, 0);
	delete statement;
}

void	XdbdOdbcConnection::deleteDescriptor (XdbdOdbcDescriptor* descriptor)
{
	for (desclist::iterator it = m_desclist.begin(); it != m_desclist.end(); ++it)
	{
		if (*it != descriptor)
			continue;
		m_desclist.erase (it);
		break;
	}
	delete descriptor;
}

} /* namespace xml_db_daemon */

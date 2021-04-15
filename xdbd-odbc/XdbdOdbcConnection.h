/*
 * XdbdOdbcConnection.h
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdOdbcStatement.h"
#include <vector>
using namespace std;

namespace xml_db_daemon
{

typedef vector < XdbdOdbcStatement* >	stmtlist;
typedef vector < XdbdOdbcDescriptor* >	desclist;

class XdbdOdbcConnection: public XdbdOdbcHandle
{
public:
	XdbdOdbcConnection(XdbdOdbcHandle* parent);
	virtual ~XdbdOdbcConnection();
	void Reset ();

	SQLINTEGER Connect (SQLCHAR* ServerName, SQLSMALLINT NameLength1, SQLCHAR* UserName, SQLSMALLINT NameLength2, SQLCHAR* Authentication, SQLSMALLINT NameLength3);
	SQLINTEGER Disconnect ();
	XdbdOdbcStatement*	createStatement ();
	XdbdOdbcDescriptor*	createDescriptor ();
	void deleteStatement (XdbdOdbcStatement* statement);
	void deleteDescriptor (XdbdOdbcDescriptor* descriptor);
	inline SQLINTEGER	attrAutoIpd () { return m_attrAutoIpd; }
	inline void attrAutoIpd (SQLINTEGER attrAutoIpd) { m_attrAutoIpd = attrAutoIpd; }
	inline SQLINTEGER	attrMetadataId () { return m_attrMetadataId; }
	inline void attrMetadataId (SQLINTEGER attrMetadataId) { m_attrMetadataId = attrMetadataId; }
private:
	xdbd_conn_t	m_connection;
	stmtlist	m_stmtlist;
	desclist	m_desclist;
	SQLINTEGER	m_attrAutoIpd;
	SQLINTEGER	m_attrMetadataId;
	SQLCHAR*	m_serverName;
	SQLCHAR*	m_userName;
	SQLCHAR*	m_authentication;
};

} /* namespace xml_db_daemon */

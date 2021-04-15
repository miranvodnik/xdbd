/*
 * XdbdOdbcEnvironment.h
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdOdbcConnection.h"
#include <vector>
using namespace std;

namespace xml_db_daemon
{

typedef vector < XdbdOdbcConnection* >	connlist;

class XdbdOdbcEnvironment: public XdbdOdbcHandle
{
public:
	XdbdOdbcEnvironment(XdbdOdbcHandle* parent);
	virtual ~XdbdOdbcEnvironment();
	XdbdOdbcConnection*	createConnection ();
	void deleteConnection (XdbdOdbcConnection* connection);
	inline SQLINTEGER attrOutputNts() { return m_attrOutputNts; }
	inline void attrOutputNts(SQLINTEGER attrOutputNts) { m_attrOutputNts = attrOutputNts; }
private:
	connlist	m_connlist;
	SQLINTEGER	m_attrOutputNts;
};

} /* namespace xml_db_daemon */

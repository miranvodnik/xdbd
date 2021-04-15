/*
 * XdbdSqlParser.h
 *
 *  Created on: 12. nov. 2015
 *      Author: miran
 */

#pragma once

#include "StdAfx.h"
#include "SqlCompilerCallbacks.h"
#include "sql_statement.h"
#include "XdbdShmSegment.h"
#include "XdbdSqlSelectFunction.h"
#include "XdbdSqlUpdateFunction.h"
#include "XdbdSqlInsertFunction.h"
#include "XdbdSqlDeleteFunction.h"
#include "XdbdSqlCatalogFunction.h"
#include <string>
using namespace std;

namespace xml_db_daemon
{

class XdbdSqlParser : public SqlCompilerCallbacks
{
public:
	XdbdSqlParser();
	virtual ~XdbdSqlParser();
	int Parse (const char* str);

	int	PrepareStatement (XdbdShmSegment* shmSegment);

	int	ExecuteStatement (XdbdShmSegment* shmSegment);
	inline int	ExecuteSelectStatement (XdbdShmSegment* shmSegment)
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlSelectFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	inline int	ExecuteUpdateStatement (XdbdShmSegment* shmSegment)
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlUpdateFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	inline int	ExecuteInsertStatement (XdbdShmSegment* shmSegment)
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlInsertFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	inline int	ExecuteDeleteStatement (XdbdShmSegment* shmSegment)
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlDeleteFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	inline int	ExecuteCatalogStatement (XdbdShmSegment* shmSegment)
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlCatalogFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }

	inline void SaveLastName (char* name) { m_lastName = name; }
	inline void SaveLastNumber (char* name) { m_lastNumber = name; }
	inline void SaveLastString (char* name) { int len = strlen (name); if (len > 0) name[len-1] = 0; m_lastString = name+1; }
	inline char* RetrieveLastName () { return (char*) m_lastName.c_str(); }
	inline char* RetrieveLastNumber () { return (char*) m_lastNumber.c_str(); }
	inline char* RetrieveLastString () { return (char*) m_lastString.c_str(); }
private:

#include "SqlCompilerCallbackDecls.h"

	sql_statement*	m_sql_statement;
	SqlCompilerCallbacks*	m_sqlPrepare;
	string	m_lastName;
	string	m_lastNumber;
	string	m_lastString;
	int	m_compCounter;
};

} /* namespace xml_db_daemon */

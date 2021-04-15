/*
 * XdbdSqlParser.cpp
 *
 *  Created on: 12. nov. 2015
 *      Author: miran
 */

extern "C"
{
#define	YY_NO_UNISTD_H
#include "yystype.h"
#include "sql-select.lex.h"
#include "sql-select.tab.h"
}

#include "XdbdSqlParser.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

XdbdSqlParser::XdbdSqlParser()
{
	m_sql_statement = 0;
	m_sqlPrepare = 0;
	m_compCounter = 0;
}

XdbdSqlParser::~XdbdSqlParser()
{
	delete	m_sql_statement;
	m_sql_statement = 0;
	delete	m_sqlPrepare;
	m_sqlPrepare = 0;
}

int	XdbdSqlParser::Parse (const char* str)
{
	int	res = 0;
	yyscan_t	scanner = 0;

	yylex_init_extra (this, &scanner);
	YY_BUFFER_STATE	bstate = yy_scan_string (str, scanner);
	res = yyparse (scanner, this);
	yy_delete_buffer (bstate, scanner);
	yylex_destroy (scanner);
	return	res;
}

int	XdbdSqlParser::PrepareStatement (XdbdShmSegment* shmSegment)
{
	delete	m_sqlPrepare;
	switch (m_sql_statement->kind())
	{
	case	sql_statement::g_sql_statement_1:
		m_sqlPrepare = (SqlCompilerCallbacks*) new XdbdSqlSelectFunction (m_sql_statement, m_compCounter);
		return	((XdbdSqlSelectFunction*)m_sqlPrepare)->Prepare(shmSegment, true);
	case	sql_statement::g_sql_statement_2:
		m_sqlPrepare = (SqlCompilerCallbacks*) new XdbdSqlUpdateFunction (m_sql_statement, m_compCounter);
		return	((XdbdSqlUpdateFunction*)m_sqlPrepare)->Prepare(shmSegment, true);
	case	sql_statement::g_sql_statement_3:
		m_sqlPrepare = (SqlCompilerCallbacks*) new XdbdSqlInsertFunction (m_sql_statement, m_compCounter);
		return	((XdbdSqlInsertFunction*)m_sqlPrepare)->Prepare(shmSegment, true);
	case	sql_statement::g_sql_statement_4:
		m_sqlPrepare = (SqlCompilerCallbacks*) new XdbdSqlDeleteFunction (m_sql_statement, m_compCounter);
		return	((XdbdSqlDeleteFunction*)m_sqlPrepare)->Prepare(shmSegment, true);
	case	sql_statement::g_sql_statement_5:
		m_sqlPrepare = (SqlCompilerCallbacks*) new XdbdSqlCatalogFunction (m_sql_statement, m_compCounter);
		return	((XdbdSqlCatalogFunction*)m_sqlPrepare)->Prepare(shmSegment, true);
	default:
		return	-1;
	}
}

int	XdbdSqlParser::ExecuteStatement (XdbdShmSegment* shmSegment)
{
	switch (m_sql_statement->kind())
	{
	case	sql_statement::g_sql_statement_1:
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlSelectFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	case	sql_statement::g_sql_statement_2:
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlUpdateFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	case	sql_statement::g_sql_statement_3:
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlInsertFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	case	sql_statement::g_sql_statement_4:
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlDeleteFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	case	sql_statement::g_sql_statement_5:
		{ return	(m_sqlPrepare != 0) ? ((XdbdSqlCatalogFunction*)m_sqlPrepare)->Execute(shmSegment) : -1; }
	default:
		return	-1;
	}
}

#define	CLASSNAME	XdbdSqlParser

#define	sql_statement_CallbackDefined
void	CLASSNAME::Invoke_sql_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_statement* p_sql_statement)
{
	m_sql_statement = p_sql_statement;
}

#define	sql_eq_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_eq_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_eq_expression* p_sql_eq_expression)
{
	++m_compCounter;
}

#define	sql_ne_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_ne_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_ne_expression* p_sql_ne_expression)
{
	++m_compCounter;
}

#define	sql_lt_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_lt_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_lt_expression* p_sql_lt_expression)
{
	++m_compCounter;
}

#define	sql_le_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_le_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_le_expression* p_sql_le_expression)
{
	++m_compCounter;
}

#define	sql_gt_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_gt_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_gt_expression* p_sql_gt_expression)
{
	++m_compCounter;
}

#define	sql_ge_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_ge_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_ge_expression* p_sql_ge_expression)
{
	++m_compCounter;
}

#include "SqlCompilerCallbackImpls.template"

} /* namespace xml_db_daemon */

extern	"C"
{

extern void SaveLastName (void* obj, char* name) { ((xml_db_daemon::XdbdSqlParser*)obj)->SaveLastName (name); }
extern void SaveLastNumber (void* obj, char* name) { ((xml_db_daemon::XdbdSqlParser*)obj)->SaveLastNumber (name); }
extern void SaveLastString (void* obj, char* name) { ((xml_db_daemon::XdbdSqlParser*)obj)->SaveLastString (name); }
extern char* RetrieveLastName (void* obj) { return ((xml_db_daemon::XdbdSqlParser*)obj)->RetrieveLastName (); }
extern char* RetrieveLastNumber (void* obj) { return ((xml_db_daemon::XdbdSqlParser*)obj)->RetrieveLastNumber (); }
extern char* RetrieveLastString (void* obj) { return ((xml_db_daemon::XdbdSqlParser*)obj)->RetrieveLastString (); }

}

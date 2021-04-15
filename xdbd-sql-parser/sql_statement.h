#pragma once

#include "SqlSyntax.h"
#include "sql_select_statement.h"
#include "sql_update_statement.h"
#include "sql_insert_statement.h"
#include "sql_delete_statement.h"
#include "sql_set_statement.h"

class	sql_statement : public SqlSyntax
{
public:
	sql_statement (sql_select_statement* c_sql_select_statement);
	sql_statement (sql_update_statement* c_sql_update_statement);
	sql_statement (sql_insert_statement* c_sql_insert_statement);
	sql_statement (sql_delete_statement* c_sql_delete_statement);
	sql_statement (sql_set_statement* c_sql_set_statement);
	virtual ~sql_statement();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_select_statement*	get_sql_select_statement () { return m_sql_select_statement; }
	inline	void	set_sql_select_statement (sql_select_statement* sql_select_statement) { m_sql_select_statement = sql_select_statement; }
	inline	sql_update_statement*	get_sql_update_statement () { return m_sql_update_statement; }
	inline	void	set_sql_update_statement (sql_update_statement* sql_update_statement) { m_sql_update_statement = sql_update_statement; }
	inline	sql_insert_statement*	get_sql_insert_statement () { return m_sql_insert_statement; }
	inline	void	set_sql_insert_statement (sql_insert_statement* sql_insert_statement) { m_sql_insert_statement = sql_insert_statement; }
	inline	sql_delete_statement*	get_sql_delete_statement () { return m_sql_delete_statement; }
	inline	void	set_sql_delete_statement (sql_delete_statement* sql_delete_statement) { m_sql_delete_statement = sql_delete_statement; }
	inline	sql_set_statement*	get_sql_set_statement () { return m_sql_set_statement; }
	inline	void	set_sql_set_statement (sql_set_statement* sql_set_statement) { m_sql_set_statement = sql_set_statement; }

public:
	static const u_int	g_sql_statement_1 = 1;
	static const u_int	g_sql_statement_2 = 2;
	static const u_int	g_sql_statement_3 = 3;
	static const u_int	g_sql_statement_4 = 4;
	static const u_int	g_sql_statement_5 = 5;

private:
	sql_select_statement*	m_sql_select_statement;
	sql_update_statement*	m_sql_update_statement;
	sql_insert_statement*	m_sql_insert_statement;
	sql_delete_statement*	m_sql_delete_statement;
	sql_set_statement*	m_sql_set_statement;

};

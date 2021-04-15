#pragma once

#include "SqlSyntax.h"
#include "sql_table.h"
#include "sql_tables.h"

class	sql_tables : public SqlSyntax
{
public:
	sql_tables (sql_table* c_sql_table);
	sql_tables (sql_tables* c_sql_tables, char* c_char, sql_table* c_sql_table);
	virtual ~sql_tables();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_table*	get_sql_table () { return m_sql_table; }
	inline	void	set_sql_table (sql_table* sql_table) { m_sql_table = sql_table; }
	inline	sql_tables*	get_sql_tables () { return m_sql_tables; }
	inline	void	set_sql_tables (sql_tables* sql_tables) { m_sql_tables = sql_tables; }
	sql_tables*	revert_sql_tables (sql_tables* p_sql_tables, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_tables_1 = 1;
	static const u_int	g_sql_tables_2 = 2;

private:
	sql_table*	m_sql_table;
	sql_tables*	m_sql_tables;
	string	m_char;

};

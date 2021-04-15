#pragma once

#include "SqlSyntax.h"
#include "sql_column_name.h"
#include "sql_table.h"
#include "sql_function_name.h"

class	sql_column : public SqlSyntax
{
public:
	sql_column (sql_column_name* c_sql_column_name);
	sql_column (sql_table* c_sql_table, char* c_char, sql_column_name* c_sql_column_name);
	sql_column (sql_function_name* c_sql_function_name, char* c_char_1, sql_column_name* c_sql_column_name, char* c_char_2);
	sql_column (sql_function_name* c_sql_function_name, char* c_char_1, sql_table* c_sql_table, char* c_char_2, sql_column_name* c_sql_column_name, char* c_char_3);
	virtual ~sql_column();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_column_name*	get_sql_column_name () { return m_sql_column_name; }
	inline	void	set_sql_column_name (sql_column_name* sql_column_name) { m_sql_column_name = sql_column_name; }
	inline	sql_table*	get_sql_table () { return m_sql_table; }
	inline	void	set_sql_table (sql_table* sql_table) { m_sql_table = sql_table; }
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }
	inline	sql_function_name*	get_sql_function_name () { return m_sql_function_name; }
	inline	void	set_sql_function_name (sql_function_name* sql_function_name) { m_sql_function_name = sql_function_name; }
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	string	get_char_3 () { return m_char_3; }
	inline	void	set_char_3 (char* str) { m_char_3 = str; }

public:
	static const u_int	g_sql_column_1 = 1;
	static const u_int	g_sql_column_2 = 2;
	static const u_int	g_sql_column_3 = 3;
	static const u_int	g_sql_column_4 = 4;

private:
	sql_column_name*	m_sql_column_name;
	sql_table*	m_sql_table;
	string	m_char;
	sql_function_name*	m_sql_function_name;
	string	m_char_1;
	string	m_char_2;
	string	m_char_3;

};

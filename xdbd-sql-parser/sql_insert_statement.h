#pragma once

#include "SqlSyntax.h"
#include "sql_table_part.h"
#include "sql_columns_part.h"
#include "sql_values_part.h"

class	sql_insert_statement : public SqlSyntax
{
public:
	sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, sql_columns_part* c_sql_columns_part, char* c_char_4, char* c_char_5, char* c_char_6, sql_values_part* c_sql_values_part, char* c_char_7, char* c_char_8);
	sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, sql_columns_part* c_sql_columns_part, char* c_char_4, char* c_char_5, char* c_char_6, sql_values_part* c_sql_values_part, char* c_char_7);
	sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, char* c_char_4, sql_values_part* c_sql_values_part, char* c_char_5, char* c_char_6);
	sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, char* c_char_4, sql_values_part* c_sql_values_part, char* c_char_5);
	virtual ~sql_insert_statement();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_table_part*	get_sql_table_part () { return m_sql_table_part; }
	inline	void	set_sql_table_part (sql_table_part* sql_table_part) { m_sql_table_part = sql_table_part; }
	inline	string	get_char_3 () { return m_char_3; }
	inline	void	set_char_3 (char* str) { m_char_3 = str; }
	inline	sql_columns_part*	get_sql_columns_part () { return m_sql_columns_part; }
	inline	void	set_sql_columns_part (sql_columns_part* sql_columns_part) { m_sql_columns_part = sql_columns_part; }
	inline	string	get_char_4 () { return m_char_4; }
	inline	void	set_char_4 (char* str) { m_char_4 = str; }
	inline	string	get_char_5 () { return m_char_5; }
	inline	void	set_char_5 (char* str) { m_char_5 = str; }
	inline	string	get_char_6 () { return m_char_6; }
	inline	void	set_char_6 (char* str) { m_char_6 = str; }
	inline	sql_values_part*	get_sql_values_part () { return m_sql_values_part; }
	inline	void	set_sql_values_part (sql_values_part* sql_values_part) { m_sql_values_part = sql_values_part; }
	inline	string	get_char_7 () { return m_char_7; }
	inline	void	set_char_7 (char* str) { m_char_7 = str; }
	inline	string	get_char_8 () { return m_char_8; }
	inline	void	set_char_8 (char* str) { m_char_8 = str; }

public:
	static const u_int	g_sql_insert_statement_1 = 1;
	static const u_int	g_sql_insert_statement_2 = 2;
	static const u_int	g_sql_insert_statement_3 = 3;
	static const u_int	g_sql_insert_statement_4 = 4;

private:
	string	m_char_1;
	string	m_char_2;
	sql_table_part*	m_sql_table_part;
	string	m_char_3;
	sql_columns_part*	m_sql_columns_part;
	string	m_char_4;
	string	m_char_5;
	string	m_char_6;
	sql_values_part*	m_sql_values_part;
	string	m_char_7;
	string	m_char_8;

};

#pragma once

#include "SqlSyntax.h"
#include "sql_columns_part.h"
#include "sql_tables_part.h"
#include "sql_expression_part.h"
#include "sql_order_by.h"

class	sql_select_statement : public SqlSyntax
{
public:
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, sql_order_by* c_sql_order_by, char* c_char_4);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, char* c_char_4);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, sql_order_by* c_sql_order_by);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, sql_order_by* c_sql_order_by, char* c_char_3);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, sql_order_by* c_sql_order_by);
	sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part);
	virtual ~sql_select_statement();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	sql_columns_part*	get_sql_columns_part () { return m_sql_columns_part; }
	inline	void	set_sql_columns_part (sql_columns_part* sql_columns_part) { m_sql_columns_part = sql_columns_part; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_tables_part*	get_sql_tables_part () { return m_sql_tables_part; }
	inline	void	set_sql_tables_part (sql_tables_part* sql_tables_part) { m_sql_tables_part = sql_tables_part; }
	inline	string	get_char_3 () { return m_char_3; }
	inline	void	set_char_3 (char* str) { m_char_3 = str; }
	inline	sql_expression_part*	get_sql_expression_part () { return m_sql_expression_part; }
	inline	void	set_sql_expression_part (sql_expression_part* sql_expression_part) { m_sql_expression_part = sql_expression_part; }
	inline	sql_order_by*	get_sql_order_by () { return m_sql_order_by; }
	inline	void	set_sql_order_by (sql_order_by* sql_order_by) { m_sql_order_by = sql_order_by; }
	inline	string	get_char_4 () { return m_char_4; }
	inline	void	set_char_4 (char* str) { m_char_4 = str; }

public:
	static const u_int	g_sql_select_statement_1 = 1;
	static const u_int	g_sql_select_statement_2 = 2;
	static const u_int	g_sql_select_statement_3 = 3;
	static const u_int	g_sql_select_statement_4 = 4;
	static const u_int	g_sql_select_statement_5 = 5;
	static const u_int	g_sql_select_statement_6 = 6;
	static const u_int	g_sql_select_statement_7 = 7;
	static const u_int	g_sql_select_statement_8 = 8;

private:
	string	m_char_1;
	sql_columns_part*	m_sql_columns_part;
	string	m_char_2;
	sql_tables_part*	m_sql_tables_part;
	string	m_char_3;
	sql_expression_part*	m_sql_expression_part;
	sql_order_by*	m_sql_order_by;
	string	m_char_4;

};

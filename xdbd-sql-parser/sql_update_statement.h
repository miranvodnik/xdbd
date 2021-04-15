#pragma once

#include "SqlSyntax.h"
#include "sql_table_part.h"
#include "sql_eq_expressions_part.h"
#include "sql_expression_part.h"

class	sql_update_statement : public SqlSyntax
{
public:
	sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3, sql_expression_part* c_sql_expression_part, char* c_char_4);
	sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3, sql_expression_part* c_sql_expression_part);
	sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3);
	sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part);
	virtual ~sql_update_statement();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	sql_table_part*	get_sql_table_part () { return m_sql_table_part; }
	inline	void	set_sql_table_part (sql_table_part* sql_table_part) { m_sql_table_part = sql_table_part; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_eq_expressions_part*	get_sql_eq_expressions_part () { return m_sql_eq_expressions_part; }
	inline	void	set_sql_eq_expressions_part (sql_eq_expressions_part* sql_eq_expressions_part) { m_sql_eq_expressions_part = sql_eq_expressions_part; }
	inline	string	get_char_3 () { return m_char_3; }
	inline	void	set_char_3 (char* str) { m_char_3 = str; }
	inline	sql_expression_part*	get_sql_expression_part () { return m_sql_expression_part; }
	inline	void	set_sql_expression_part (sql_expression_part* sql_expression_part) { m_sql_expression_part = sql_expression_part; }
	inline	string	get_char_4 () { return m_char_4; }
	inline	void	set_char_4 (char* str) { m_char_4 = str; }

public:
	static const u_int	g_sql_update_statement_1 = 1;
	static const u_int	g_sql_update_statement_2 = 2;
	static const u_int	g_sql_update_statement_3 = 3;
	static const u_int	g_sql_update_statement_4 = 4;

private:
	string	m_char_1;
	sql_table_part*	m_sql_table_part;
	string	m_char_2;
	sql_eq_expressions_part*	m_sql_eq_expressions_part;
	string	m_char_3;
	sql_expression_part*	m_sql_expression_part;
	string	m_char_4;

};

#pragma once

#include "SqlSyntax.h"
#include "sql_primary_expression.h"

class	sql_lt_expression : public SqlSyntax
{
public:
	sql_lt_expression (sql_primary_expression* c_sql_primary_expression_1, char* c_char, sql_primary_expression* c_sql_primary_expression_2);
	virtual ~sql_lt_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_primary_expression*	get_sql_primary_expression_1 () { return m_sql_primary_expression_1; }
	inline	void	set_sql_primary_expression_1 (sql_primary_expression* sql_primary_expression_1) { m_sql_primary_expression_1 = sql_primary_expression_1; }
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }
	inline	sql_primary_expression*	get_sql_primary_expression_2 () { return m_sql_primary_expression_2; }
	inline	void	set_sql_primary_expression_2 (sql_primary_expression* sql_primary_expression_2) { m_sql_primary_expression_2 = sql_primary_expression_2; }

public:
	static const u_int	g_sql_lt_expression_1 = 1;

private:
	sql_primary_expression*	m_sql_primary_expression_1;
	string	m_char;
	sql_primary_expression*	m_sql_primary_expression_2;

};

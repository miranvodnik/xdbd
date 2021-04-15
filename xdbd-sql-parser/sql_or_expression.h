#pragma once

#include "SqlSyntax.h"
#include "sql_and_expression.h"
#include "sql_or_expression.h"

class	sql_or_expression : public SqlSyntax
{
public:
	sql_or_expression (sql_and_expression* c_sql_and_expression);
	sql_or_expression (sql_or_expression* c_sql_or_expression, char* c_char, sql_and_expression* c_sql_and_expression);
	virtual ~sql_or_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_and_expression*	get_sql_and_expression () { return m_sql_and_expression; }
	inline	void	set_sql_and_expression (sql_and_expression* sql_and_expression) { m_sql_and_expression = sql_and_expression; }
	inline	sql_or_expression*	get_sql_or_expression () { return m_sql_or_expression; }
	inline	void	set_sql_or_expression (sql_or_expression* sql_or_expression) { m_sql_or_expression = sql_or_expression; }
	sql_or_expression*	revert_sql_or_expression (sql_or_expression* p_sql_or_expression, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_or_expression_1 = 1;
	static const u_int	g_sql_or_expression_2 = 2;

private:
	sql_and_expression*	m_sql_and_expression;
	sql_or_expression*	m_sql_or_expression;
	string	m_char;

};

#pragma once

#include "SqlSyntax.h"
#include "sql_eq_expression.h"
#include "sql_eq_expressions.h"

class	sql_eq_expressions : public SqlSyntax
{
public:
	sql_eq_expressions (sql_eq_expression* c_sql_eq_expression);
	sql_eq_expressions (sql_eq_expressions* c_sql_eq_expressions, sql_eq_expression* c_sql_eq_expression);
	sql_eq_expressions (sql_eq_expressions* c_sql_eq_expressions, char* c_char, sql_eq_expression* c_sql_eq_expression);
	virtual ~sql_eq_expressions();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_eq_expression*	get_sql_eq_expression () { return m_sql_eq_expression; }
	inline	void	set_sql_eq_expression (sql_eq_expression* sql_eq_expression) { m_sql_eq_expression = sql_eq_expression; }
	inline	sql_eq_expressions*	get_sql_eq_expressions () { return m_sql_eq_expressions; }
	inline	void	set_sql_eq_expressions (sql_eq_expressions* sql_eq_expressions) { m_sql_eq_expressions = sql_eq_expressions; }
	sql_eq_expressions*	revert_sql_eq_expressions (sql_eq_expressions* p_sql_eq_expressions, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_eq_expressions_1 = 1;
	static const u_int	g_sql_eq_expressions_2 = 2;
	static const u_int	g_sql_eq_expressions_3 = 3;

private:
	sql_eq_expression*	m_sql_eq_expression;
	sql_eq_expressions*	m_sql_eq_expressions;
	string	m_char;

};

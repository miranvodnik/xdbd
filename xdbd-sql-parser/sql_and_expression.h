#pragma once

#include "SqlSyntax.h"
#include "sql_comp_expression.h"
#include "sql_and_expression.h"

class	sql_and_expression : public SqlSyntax
{
public:
	sql_and_expression (sql_comp_expression* c_sql_comp_expression);
	sql_and_expression (sql_and_expression* c_sql_and_expression, char* c_char, sql_comp_expression* c_sql_comp_expression);
	virtual ~sql_and_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_comp_expression*	get_sql_comp_expression () { return m_sql_comp_expression; }
	inline	void	set_sql_comp_expression (sql_comp_expression* sql_comp_expression) { m_sql_comp_expression = sql_comp_expression; }
	inline	sql_and_expression*	get_sql_and_expression () { return m_sql_and_expression; }
	inline	void	set_sql_and_expression (sql_and_expression* sql_and_expression) { m_sql_and_expression = sql_and_expression; }
	sql_and_expression*	revert_sql_and_expression (sql_and_expression* p_sql_and_expression, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_and_expression_1 = 1;
	static const u_int	g_sql_and_expression_2 = 2;

private:
	sql_comp_expression*	m_sql_comp_expression;
	sql_and_expression*	m_sql_and_expression;
	string	m_char;

};

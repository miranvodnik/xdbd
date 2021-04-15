#pragma once

#include "SqlSyntax.h"
#include "sql_or_expression.h"

class	sql_expression : public SqlSyntax
{
public:
	sql_expression (sql_or_expression* c_sql_or_expression);
	virtual ~sql_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_or_expression*	get_sql_or_expression () { return m_sql_or_expression; }
	inline	void	set_sql_or_expression (sql_or_expression* sql_or_expression) { m_sql_or_expression = sql_or_expression; }

public:
	static const u_int	g_sql_expression_1 = 1;

private:
	sql_or_expression*	m_sql_or_expression;

};

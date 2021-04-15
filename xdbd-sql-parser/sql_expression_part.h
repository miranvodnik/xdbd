#pragma once

#include "SqlSyntax.h"
#include "sql_expression.h"

class	sql_expression_part : public SqlSyntax
{
public:
	sql_expression_part (sql_expression* c_sql_expression);
	virtual ~sql_expression_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_expression*	get_sql_expression () { return m_sql_expression; }
	inline	void	set_sql_expression (sql_expression* sql_expression) { m_sql_expression = sql_expression; }

public:
	static const u_int	g_sql_expression_part_1 = 1;

private:
	sql_expression*	m_sql_expression;

};

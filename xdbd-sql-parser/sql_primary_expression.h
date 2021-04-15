#pragma once

#include "SqlSyntax.h"
#include "sql_column.h"
#include "sql_constant.h"

class	sql_primary_expression : public SqlSyntax
{
public:
	sql_primary_expression (sql_column* c_sql_column);
	sql_primary_expression (sql_constant* c_sql_constant);
	virtual ~sql_primary_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_column*	get_sql_column () { return m_sql_column; }
	inline	void	set_sql_column (sql_column* sql_column) { m_sql_column = sql_column; }
	inline	sql_constant*	get_sql_constant () { return m_sql_constant; }
	inline	void	set_sql_constant (sql_constant* sql_constant) { m_sql_constant = sql_constant; }

public:
	static const u_int	g_sql_primary_expression_1 = 1;
	static const u_int	g_sql_primary_expression_2 = 2;

private:
	sql_column*	m_sql_column;
	sql_constant*	m_sql_constant;

};

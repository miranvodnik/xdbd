#pragma once

#include "SqlSyntax.h"
#include "sql_values.h"

class	sql_values_part : public SqlSyntax
{
public:
	sql_values_part (sql_values* c_sql_values);
	virtual ~sql_values_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_values*	get_sql_values () { return m_sql_values; }
	inline	void	set_sql_values (sql_values* sql_values) { m_sql_values = sql_values; }

public:
	static const u_int	g_sql_values_part_1 = 1;

private:
	sql_values*	m_sql_values;

};

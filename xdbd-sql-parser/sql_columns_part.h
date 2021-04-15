#pragma once

#include "SqlSyntax.h"
#include "sql_columns.h"

class	sql_columns_part : public SqlSyntax
{
public:
	sql_columns_part ();
	sql_columns_part (sql_columns* c_sql_columns);
	virtual ~sql_columns_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_columns*	get_sql_columns () { return m_sql_columns; }
	inline	void	set_sql_columns (sql_columns* sql_columns) { m_sql_columns = sql_columns; }

public:
	static const u_int	g_sql_columns_part_1 = 1;
	static const u_int	g_sql_columns_part_2 = 2;

private:
	sql_columns*	m_sql_columns;

};

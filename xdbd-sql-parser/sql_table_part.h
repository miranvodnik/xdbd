#pragma once

#include "SqlSyntax.h"
#include "sql_table.h"

class	sql_table_part : public SqlSyntax
{
public:
	sql_table_part (sql_table* c_sql_table);
	virtual ~sql_table_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_table*	get_sql_table () { return m_sql_table; }
	inline	void	set_sql_table (sql_table* sql_table) { m_sql_table = sql_table; }

public:
	static const u_int	g_sql_table_part_1 = 1;

private:
	sql_table*	m_sql_table;

};

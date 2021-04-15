#pragma once

#include "SqlSyntax.h"
#include "sql_tables.h"

class	sql_tables_part : public SqlSyntax
{
public:
	sql_tables_part (sql_tables* c_sql_tables);
	virtual ~sql_tables_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_tables*	get_sql_tables () { return m_sql_tables; }
	inline	void	set_sql_tables (sql_tables* sql_tables) { m_sql_tables = sql_tables; }

public:
	static const u_int	g_sql_tables_part_1 = 1;

private:
	sql_tables*	m_sql_tables;

};

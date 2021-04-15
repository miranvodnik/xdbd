#pragma once

#include "SqlSyntax.h"
#include "sql_columns.h"

class	sql_order_by : public SqlSyntax
{
public:
	sql_order_by (char* c_char_1, char* c_char_2, sql_columns* c_sql_columns);
	virtual ~sql_order_by();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_columns*	get_sql_columns () { return m_sql_columns; }
	inline	void	set_sql_columns (sql_columns* sql_columns) { m_sql_columns = sql_columns; }

public:
	static const u_int	g_sql_order_by_1 = 1;

private:
	string	m_char_1;
	string	m_char_2;
	sql_columns*	m_sql_columns;

};

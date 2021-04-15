#pragma once

#include "SqlSyntax.h"
#include "sql_column.h"
#include "sql_as_part.h"
#include "sql_columns.h"

class	sql_columns : public SqlSyntax
{
public:
	sql_columns (sql_column* c_sql_column, sql_as_part* c_sql_as_part);
	sql_columns (sql_columns* c_sql_columns, char* c_char, sql_column* c_sql_column, sql_as_part* c_sql_as_part);
	virtual ~sql_columns();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_column*	get_sql_column () { return m_sql_column; }
	inline	void	set_sql_column (sql_column* sql_column) { m_sql_column = sql_column; }
	inline	sql_as_part*	get_sql_as_part () { return m_sql_as_part; }
	inline	void	set_sql_as_part (sql_as_part* sql_as_part) { m_sql_as_part = sql_as_part; }
	inline	sql_columns*	get_sql_columns () { return m_sql_columns; }
	inline	void	set_sql_columns (sql_columns* sql_columns) { m_sql_columns = sql_columns; }
	sql_columns*	revert_sql_columns (sql_columns* p_sql_columns, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_columns_1 = 1;
	static const u_int	g_sql_columns_2 = 2;

private:
	sql_column*	m_sql_column;
	sql_as_part*	m_sql_as_part;
	sql_columns*	m_sql_columns;
	string	m_char;

};

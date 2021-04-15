#pragma once

#include "SqlSyntax.h"
#include "sql_ordering.h"

class	sql_column_name : public SqlSyntax
{
public:
	sql_column_name (unsigned int kind, char* c_char);
	sql_column_name (char* c_char, sql_ordering* c_sql_ordering);
	virtual ~sql_column_name();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }
	inline	sql_ordering*	get_sql_ordering () { return m_sql_ordering; }
	inline	void	set_sql_ordering (sql_ordering* sql_ordering) { m_sql_ordering = sql_ordering; }

public:
	static const u_int	g_sql_column_name_1 = 1;
	static const u_int	g_sql_column_name_2 = 2;
	static const u_int	g_sql_column_name_3 = 3;

private:
	string	m_char;
	sql_ordering*	m_sql_ordering;

};

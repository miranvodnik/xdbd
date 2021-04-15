#pragma once

#include "SqlSyntax.h"
#include "sql_constant.h"
#include "sql_values.h"

class	sql_values : public SqlSyntax
{
public:
	sql_values (sql_constant* c_sql_constant);
	sql_values (sql_values* c_sql_values, char* c_char, sql_constant* c_sql_constant);
	virtual ~sql_values();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_constant*	get_sql_constant () { return m_sql_constant; }
	inline	void	set_sql_constant (sql_constant* sql_constant) { m_sql_constant = sql_constant; }
	inline	sql_values*	get_sql_values () { return m_sql_values; }
	inline	void	set_sql_values (sql_values* sql_values) { m_sql_values = sql_values; }
	sql_values*	revert_sql_values (sql_values* p_sql_values, u_int kind);
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_values_1 = 1;
	static const u_int	g_sql_values_2 = 2;

private:
	sql_constant*	m_sql_constant;
	sql_values*	m_sql_values;
	string	m_char;

};

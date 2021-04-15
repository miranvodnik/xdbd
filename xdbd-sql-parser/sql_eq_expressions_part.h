#pragma once

#include "SqlSyntax.h"
#include "sql_eq_expressions.h"

class	sql_eq_expressions_part : public SqlSyntax
{
public:
	sql_eq_expressions_part ();
	sql_eq_expressions_part (sql_eq_expressions* c_sql_eq_expressions);
	sql_eq_expressions_part (char* c_char_1, sql_eq_expressions* c_sql_eq_expressions, char* c_char_2);
	virtual ~sql_eq_expressions_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_eq_expressions*	get_sql_eq_expressions () { return m_sql_eq_expressions; }
	inline	void	set_sql_eq_expressions (sql_eq_expressions* sql_eq_expressions) { m_sql_eq_expressions = sql_eq_expressions; }
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }

public:
	static const u_int	g_sql_eq_expressions_part_1 = 1;
	static const u_int	g_sql_eq_expressions_part_2 = 2;
	static const u_int	g_sql_eq_expressions_part_3 = 3;

private:
	sql_eq_expressions*	m_sql_eq_expressions;
	string	m_char_1;
	string	m_char_2;

};

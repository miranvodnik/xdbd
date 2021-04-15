#pragma once

#include "SqlSyntax.h"
#include "sql_primary_expression.h"

class	sql_is_ne_expression : public SqlSyntax
{
public:
	sql_is_ne_expression (sql_primary_expression* c_sql_primary_expression_1, char* c_char_1, char* c_char_2, sql_primary_expression* c_sql_primary_expression_2);
	virtual ~sql_is_ne_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_primary_expression*	get_sql_primary_expression_1 () { return m_sql_primary_expression_1; }
	inline	void	set_sql_primary_expression_1 (sql_primary_expression* sql_primary_expression_1) { m_sql_primary_expression_1 = sql_primary_expression_1; }
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_primary_expression*	get_sql_primary_expression_2 () { return m_sql_primary_expression_2; }
	inline	void	set_sql_primary_expression_2 (sql_primary_expression* sql_primary_expression_2) { m_sql_primary_expression_2 = sql_primary_expression_2; }

public:
	static const u_int	g_sql_is_ne_expression_1 = 1;

private:
	sql_primary_expression*	m_sql_primary_expression_1;
	string	m_char_1;
	string	m_char_2;
	sql_primary_expression*	m_sql_primary_expression_2;

};

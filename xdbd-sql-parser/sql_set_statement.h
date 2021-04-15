#pragma once

#include "SqlSyntax.h"
#include "sql_catalog.h"

class	sql_set_statement : public SqlSyntax
{
public:
	sql_set_statement (char* c_char_1, char* c_char_2, sql_catalog* c_sql_catalog);
	virtual ~sql_set_statement();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_catalog*	get_sql_catalog () { return m_sql_catalog; }
	inline	void	set_sql_catalog (sql_catalog* sql_catalog) { m_sql_catalog = sql_catalog; }

public:
	static const u_int	g_sql_set_statement_1 = 1;

private:
	string	m_char_1;
	string	m_char_2;
	sql_catalog*	m_sql_catalog;

};

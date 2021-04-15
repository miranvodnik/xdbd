#pragma once

#include "SqlSyntax.h"

class	sql_table : public SqlSyntax
{
public:
	sql_table (char* c_char);
	virtual ~sql_table();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_table_1 = 1;

private:
	string	m_char;

};

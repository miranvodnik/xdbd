#pragma once

#include "SqlSyntax.h"

class	sql_constant : public SqlSyntax
{
public:
	sql_constant (unsigned int kind, char* c_char);
	virtual ~sql_constant();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_constant_1 = 1;
	static const u_int	g_sql_constant_2 = 2;
	static const u_int	g_sql_constant_3 = 3;
	static const u_int	g_sql_constant_4 = 4;

private:
	string	m_char;

};

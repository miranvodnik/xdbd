#pragma once

#include "SqlSyntax.h"

class	sql_function_name : public SqlSyntax
{
public:
	sql_function_name (char* c_char);
	virtual ~sql_function_name();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_function_name_1 = 1;

private:
	string	m_char;

};

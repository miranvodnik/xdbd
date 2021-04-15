#pragma once

#include "SqlSyntax.h"

class	sql_catalog : public SqlSyntax
{
public:
	sql_catalog (char* c_char);
	virtual ~sql_catalog();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char () { return m_char; }
	inline	void	set_char (char* str) { m_char = str; }

public:
	static const u_int	g_sql_catalog_1 = 1;

private:
	string	m_char;

};

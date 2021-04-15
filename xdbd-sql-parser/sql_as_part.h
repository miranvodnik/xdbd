#pragma once

#include "SqlSyntax.h"

class	sql_as_part : public SqlSyntax
{
public:
	sql_as_part ();
	sql_as_part (char* c_char_1, char* c_char_2);
	virtual ~sql_as_part();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }

public:
	static const u_int	g_sql_as_part_1 = 1;
	static const u_int	g_sql_as_part_2 = 2;

private:
	string	m_char_1;
	string	m_char_2;

};

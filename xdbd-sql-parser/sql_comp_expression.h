#pragma once

#include "SqlSyntax.h"
#include "sql_eq_expression.h"
#include "sql_ne_expression.h"
#include "sql_lt_expression.h"
#include "sql_le_expression.h"
#include "sql_gt_expression.h"
#include "sql_ge_expression.h"
#include "sql_like_expression.h"
#include "sql_expression.h"
#include "sql_is_eq_expression.h"
#include "sql_is_ne_expression.h"

class	sql_comp_expression : public SqlSyntax
{
public:
	sql_comp_expression (sql_eq_expression* c_sql_eq_expression);
	sql_comp_expression (sql_ne_expression* c_sql_ne_expression);
	sql_comp_expression (sql_lt_expression* c_sql_lt_expression);
	sql_comp_expression (sql_le_expression* c_sql_le_expression);
	sql_comp_expression (sql_gt_expression* c_sql_gt_expression);
	sql_comp_expression (sql_ge_expression* c_sql_ge_expression);
	sql_comp_expression (sql_like_expression* c_sql_like_expression);
	sql_comp_expression (char* c_char_1, sql_expression* c_sql_expression, char* c_char_2);
	sql_comp_expression (sql_is_eq_expression* c_sql_is_eq_expression);
	sql_comp_expression (sql_is_ne_expression* c_sql_is_ne_expression);
	virtual ~sql_comp_expression();
	void Traverse (void* parseCtx);
private:
	void _init ();

public:
	inline	sql_eq_expression*	get_sql_eq_expression () { return m_sql_eq_expression; }
	inline	void	set_sql_eq_expression (sql_eq_expression* sql_eq_expression) { m_sql_eq_expression = sql_eq_expression; }
	inline	sql_ne_expression*	get_sql_ne_expression () { return m_sql_ne_expression; }
	inline	void	set_sql_ne_expression (sql_ne_expression* sql_ne_expression) { m_sql_ne_expression = sql_ne_expression; }
	inline	sql_lt_expression*	get_sql_lt_expression () { return m_sql_lt_expression; }
	inline	void	set_sql_lt_expression (sql_lt_expression* sql_lt_expression) { m_sql_lt_expression = sql_lt_expression; }
	inline	sql_le_expression*	get_sql_le_expression () { return m_sql_le_expression; }
	inline	void	set_sql_le_expression (sql_le_expression* sql_le_expression) { m_sql_le_expression = sql_le_expression; }
	inline	sql_gt_expression*	get_sql_gt_expression () { return m_sql_gt_expression; }
	inline	void	set_sql_gt_expression (sql_gt_expression* sql_gt_expression) { m_sql_gt_expression = sql_gt_expression; }
	inline	sql_ge_expression*	get_sql_ge_expression () { return m_sql_ge_expression; }
	inline	void	set_sql_ge_expression (sql_ge_expression* sql_ge_expression) { m_sql_ge_expression = sql_ge_expression; }
	inline	sql_like_expression*	get_sql_like_expression () { return m_sql_like_expression; }
	inline	void	set_sql_like_expression (sql_like_expression* sql_like_expression) { m_sql_like_expression = sql_like_expression; }
	inline	string	get_char_1 () { return m_char_1; }
	inline	void	set_char_1 (char* str) { m_char_1 = str; }
	inline	sql_expression*	get_sql_expression () { return m_sql_expression; }
	inline	void	set_sql_expression (sql_expression* sql_expression) { m_sql_expression = sql_expression; }
	inline	string	get_char_2 () { return m_char_2; }
	inline	void	set_char_2 (char* str) { m_char_2 = str; }
	inline	sql_is_eq_expression*	get_sql_is_eq_expression () { return m_sql_is_eq_expression; }
	inline	void	set_sql_is_eq_expression (sql_is_eq_expression* sql_is_eq_expression) { m_sql_is_eq_expression = sql_is_eq_expression; }
	inline	sql_is_ne_expression*	get_sql_is_ne_expression () { return m_sql_is_ne_expression; }
	inline	void	set_sql_is_ne_expression (sql_is_ne_expression* sql_is_ne_expression) { m_sql_is_ne_expression = sql_is_ne_expression; }

public:
	static const u_int	g_sql_comp_expression_1 = 1;
	static const u_int	g_sql_comp_expression_2 = 2;
	static const u_int	g_sql_comp_expression_3 = 3;
	static const u_int	g_sql_comp_expression_4 = 4;
	static const u_int	g_sql_comp_expression_5 = 5;
	static const u_int	g_sql_comp_expression_6 = 6;
	static const u_int	g_sql_comp_expression_7 = 7;
	static const u_int	g_sql_comp_expression_8 = 8;
	static const u_int	g_sql_comp_expression_9 = 9;
	static const u_int	g_sql_comp_expression_10 = 10;

private:
	sql_eq_expression*	m_sql_eq_expression;
	sql_ne_expression*	m_sql_ne_expression;
	sql_lt_expression*	m_sql_lt_expression;
	sql_le_expression*	m_sql_le_expression;
	sql_gt_expression*	m_sql_gt_expression;
	sql_ge_expression*	m_sql_ge_expression;
	sql_like_expression*	m_sql_like_expression;
	string	m_char_1;
	sql_expression*	m_sql_expression;
	string	m_char_2;
	sql_is_eq_expression*	m_sql_is_eq_expression;
	sql_is_ne_expression*	m_sql_is_ne_expression;

};

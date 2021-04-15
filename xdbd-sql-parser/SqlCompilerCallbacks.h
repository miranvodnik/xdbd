#pragma once

#include "parse-syntax.h"
#include <stack>
using namespace std;

enum	SqlCallbackReason
{
	DefaultCallbackReason = 0,
	TraversalPrologueCallbackReason = 1,
	TraversalEpilogueCallbackReason = 2
};

class SqlCompilerCallbacks
{
	typedef	stack < SqlSyntax* >	pstack;
public:
	SqlCompilerCallbacks () {}
	virtual	~SqlCompilerCallbacks () {}

	virtual	void	Invoke_sql_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_statement* c_sql_statement) = 0;
	virtual	void	Invoke_sql_select_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_select_statement* c_sql_select_statement) = 0;
	virtual	void	Invoke_sql_update_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_update_statement* c_sql_update_statement) = 0;
	virtual	void	Invoke_sql_insert_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_insert_statement* c_sql_insert_statement) = 0;
	virtual	void	Invoke_sql_delete_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_delete_statement* c_sql_delete_statement) = 0;
	virtual	void	Invoke_sql_set_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_set_statement* c_sql_set_statement) = 0;
	virtual	void	Invoke_sql_catalog_Callback (SqlCallbackReason reason, unsigned int kind, sql_catalog* c_sql_catalog) = 0;
	virtual	void	Invoke_sql_columns_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_columns_part* c_sql_columns_part) = 0;
	virtual	void	Invoke_sql_columns_Callback (SqlCallbackReason reason, unsigned int kind, sql_columns* c_sql_columns) = 0;
	virtual	void	Invoke_sql_column_Callback (SqlCallbackReason reason, unsigned int kind, sql_column* c_sql_column) = 0;
	virtual	void	Invoke_sql_column_name_Callback (SqlCallbackReason reason, unsigned int kind, sql_column_name* c_sql_column_name) = 0;
	virtual	void	Invoke_sql_function_name_Callback (SqlCallbackReason reason, unsigned int kind, sql_function_name* c_sql_function_name) = 0;
	virtual	void	Invoke_sql_ordering_Callback (SqlCallbackReason reason, unsigned int kind, sql_ordering* c_sql_ordering) = 0;
	virtual	void	Invoke_sql_as_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_as_part* c_sql_as_part) = 0;
	virtual	void	Invoke_sql_tables_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_tables_part* c_sql_tables_part) = 0;
	virtual	void	Invoke_sql_tables_Callback (SqlCallbackReason reason, unsigned int kind, sql_tables* c_sql_tables) = 0;
	virtual	void	Invoke_sql_table_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_table_part* c_sql_table_part) = 0;
	virtual	void	Invoke_sql_table_Callback (SqlCallbackReason reason, unsigned int kind, sql_table* c_sql_table) = 0;
	virtual	void	Invoke_sql_values_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_values_part* c_sql_values_part) = 0;
	virtual	void	Invoke_sql_values_Callback (SqlCallbackReason reason, unsigned int kind, sql_values* c_sql_values) = 0;
	virtual	void	Invoke_sql_constant_Callback (SqlCallbackReason reason, unsigned int kind, sql_constant* c_sql_constant) = 0;
	virtual	void	Invoke_sql_eq_expressions_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_eq_expressions_part* c_sql_eq_expressions_part) = 0;
	virtual	void	Invoke_sql_eq_expressions_Callback (SqlCallbackReason reason, unsigned int kind, sql_eq_expressions* c_sql_eq_expressions) = 0;
	virtual	void	Invoke_sql_primary_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_primary_expression* c_sql_primary_expression) = 0;
	virtual	void	Invoke_sql_eq_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_eq_expression* c_sql_eq_expression) = 0;
	virtual	void	Invoke_sql_is_eq_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_is_eq_expression* c_sql_is_eq_expression) = 0;
	virtual	void	Invoke_sql_ne_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_ne_expression* c_sql_ne_expression) = 0;
	virtual	void	Invoke_sql_is_ne_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_is_ne_expression* c_sql_is_ne_expression) = 0;
	virtual	void	Invoke_sql_lt_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_lt_expression* c_sql_lt_expression) = 0;
	virtual	void	Invoke_sql_le_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_le_expression* c_sql_le_expression) = 0;
	virtual	void	Invoke_sql_gt_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_gt_expression* c_sql_gt_expression) = 0;
	virtual	void	Invoke_sql_ge_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_ge_expression* c_sql_ge_expression) = 0;
	virtual	void	Invoke_sql_like_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_like_expression* c_sql_like_expression) = 0;
	virtual	void	Invoke_sql_comp_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_comp_expression* c_sql_comp_expression) = 0;
	virtual	void	Invoke_sql_and_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_and_expression* c_sql_and_expression) = 0;
	virtual	void	Invoke_sql_or_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_or_expression* c_sql_or_expression) = 0;
	virtual	void	Invoke_sql_expression_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_expression_part* c_sql_expression_part) = 0;
	virtual	void	Invoke_sql_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_expression* c_sql_expression) = 0;
	virtual	void	Invoke_sql_order_by_Callback (SqlCallbackReason reason, unsigned int kind, sql_order_by* c_sql_order_by) = 0;

	inline void push (SqlSyntax* p_syntax) { m_stack.push (p_syntax); }
	inline void pop () { m_stack.pop(); }
	inline SqlSyntax* top () { return m_stack.top(); }
private:
	pstack m_stack;
};

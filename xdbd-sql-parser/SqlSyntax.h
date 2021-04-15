#pragma once

#include "StdAfx.h"

typedef enum _SqlSyntaxContext
{
	SqlFirstSyntaxContext,
	sql_statement_Context,
	sql_select_statement_Context,
	sql_update_statement_Context,
	sql_insert_statement_Context,
	sql_delete_statement_Context,
	sql_set_statement_Context,
	sql_catalog_Context,
	sql_columns_part_Context,
	sql_columns_Context,
	sql_column_Context,
	sql_column_name_Context,
	sql_function_name_Context,
	sql_ordering_Context,
	sql_as_part_Context,
	sql_tables_part_Context,
	sql_tables_Context,
	sql_table_part_Context,
	sql_table_Context,
	sql_values_part_Context,
	sql_values_Context,
	sql_constant_Context,
	sql_eq_expressions_part_Context,
	sql_eq_expressions_Context,
	sql_primary_expression_Context,
	sql_eq_expression_Context,
	sql_is_eq_expression_Context,
	sql_ne_expression_Context,
	sql_is_ne_expression_Context,
	sql_lt_expression_Context,
	sql_le_expression_Context,
	sql_gt_expression_Context,
	sql_ge_expression_Context,
	sql_like_expression_Context,
	sql_comp_expression_Context,
	sql_and_expression_Context,
	sql_or_expression_Context,
	sql_expression_part_Context,
	sql_expression_Context,
	sql_order_by_Context,
	SqlLastSyntaxContext
} SqlSyntaxContext;

class	SqlSyntax
{
public:
	SqlSyntax (unsigned int kind, SqlSyntaxContext context);
	virtual	~SqlSyntax ();
	inline unsigned int kind () { return m_kind; }
	inline void* getAny () { return m_any; }
	inline void setAny (void* any) { m_any = any; }
	inline SqlSyntaxContext context () { return m_context; }
	inline SqlSyntax* parent () { return m_parent; }
	inline void parent (SqlSyntax* parent) { m_parent = parent; }
protected:
	unsigned int m_kind;
	SqlSyntaxContext m_context;
	SqlSyntax* m_parent;
	void* m_any;
};

class	sql_statement;
class	sql_select_statement;
class	sql_update_statement;
class	sql_insert_statement;
class	sql_delete_statement;
class	sql_set_statement;
class	sql_catalog;
class	sql_columns_part;
class	sql_columns;
class	sql_column;
class	sql_column_name;
class	sql_function_name;
class	sql_ordering;
class	sql_as_part;
class	sql_tables_part;
class	sql_tables;
class	sql_table_part;
class	sql_table;
class	sql_values_part;
class	sql_values;
class	sql_constant;
class	sql_eq_expressions_part;
class	sql_eq_expressions;
class	sql_primary_expression;
class	sql_eq_expression;
class	sql_is_eq_expression;
class	sql_ne_expression;
class	sql_is_ne_expression;
class	sql_lt_expression;
class	sql_le_expression;
class	sql_gt_expression;
class	sql_ge_expression;
class	sql_like_expression;
class	sql_comp_expression;
class	sql_and_expression;
class	sql_or_expression;
class	sql_expression_part;
class	sql_expression;
class	sql_order_by;

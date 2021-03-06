#pragma once

void*	sql_statement_1 (void* parseCtx, void* c_sql_select_statement);
void*	sql_statement_2 (void* parseCtx, void* c_sql_update_statement);
void*	sql_statement_3 (void* parseCtx, void* c_sql_insert_statement);
void*	sql_statement_4 (void* parseCtx, void* c_sql_delete_statement);
void*	sql_statement_5 (void* parseCtx, void* c_sql_set_statement);
void*	sql_select_statement_1 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_sql_order_by, void* c_char_4);
void*	sql_select_statement_2 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_char_4);
void*	sql_select_statement_3 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_sql_order_by);
void*	sql_select_statement_4 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part);
void*	sql_select_statement_5 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_sql_order_by, void* c_char_3);
void*	sql_select_statement_6 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3);
void*	sql_select_statement_7 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_sql_order_by);
void*	sql_select_statement_8 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part);
void*	sql_update_statement_1 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3, void* c_sql_expression_part, void* c_char_4);
void*	sql_update_statement_2 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3, void* c_sql_expression_part);
void*	sql_update_statement_3 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3);
void*	sql_update_statement_4 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part);
void*	sql_insert_statement_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_columns_part, void* c_char_4, void* c_char_5, void* c_char_6, void* c_sql_values_part, void* c_char_7, void* c_char_8);
void*	sql_insert_statement_2 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_columns_part, void* c_char_4, void* c_char_5, void* c_char_6, void* c_sql_values_part, void* c_char_7);
void*	sql_insert_statement_3 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_char_4, void* c_sql_values_part, void* c_char_5, void* c_char_6);
void*	sql_insert_statement_4 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_char_4, void* c_sql_values_part, void* c_char_5);
void*	sql_delete_statement_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_expression_part, void* c_char_4);
void*	sql_delete_statement_2 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_expression_part);
void*	sql_delete_statement_3 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3);
void*	sql_delete_statement_4 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part);
void*	sql_set_statement_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_catalog);
void*	sql_catalog_1 (void* parseCtx, void* c_char);
void*	sql_columns_part_1 (void* parseCtx);
void*	sql_columns_part_2 (void* parseCtx, void* c_sql_columns);
void*	sql_columns_1 (void* parseCtx, void* c_sql_column, void* c_sql_as_part);
void*	sql_columns_2 (void* parseCtx, void* c_sql_columns, void* c_char, void* c_sql_column, void* c_sql_as_part);
void*	sql_column_1 (void* parseCtx, void* c_sql_column_name);
void*	sql_column_2 (void* parseCtx, void* c_sql_table, void* c_char, void* c_sql_column_name);
void*	sql_column_3 (void* parseCtx, void* c_sql_function_name, void* c_char_1, void* c_sql_column_name, void* c_char_2);
void*	sql_column_4 (void* parseCtx, void* c_sql_function_name, void* c_char_1, void* c_sql_table, void* c_char_2, void* c_sql_column_name, void* c_char_3);
void*	sql_column_name_1 (void* parseCtx, void* c_char);
void*	sql_column_name_2 (void* parseCtx, void* c_char);
void*	sql_column_name_3 (void* parseCtx, void* c_char, void* c_sql_ordering);
void*	sql_function_name_1 (void* parseCtx, void* c_char);
void*	sql_ordering_1 (void* parseCtx, void* c_char);
void*	sql_ordering_2 (void* parseCtx, void* c_char);
void*	sql_ordering_3 (void* parseCtx, void* c_char);
void*	sql_ordering_4 (void* parseCtx, void* c_char);
void*	sql_as_part_1 (void* parseCtx);
void*	sql_as_part_2 (void* parseCtx, void* c_char_1, void* c_char_2);
void*	sql_tables_part_1 (void* parseCtx, void* c_sql_tables);
void*	sql_tables_1 (void* parseCtx, void* c_sql_table);
void*	sql_tables_2 (void* parseCtx, void* c_sql_tables, void* c_char, void* c_sql_table);
void*	sql_table_part_1 (void* parseCtx, void* c_sql_table);
void*	sql_table_1 (void* parseCtx, void* c_char);
void*	sql_values_part_1 (void* parseCtx, void* c_sql_values);
void*	sql_values_1 (void* parseCtx, void* c_sql_constant);
void*	sql_values_2 (void* parseCtx, void* c_sql_values, void* c_char, void* c_sql_constant);
void*	sql_constant_1 (void* parseCtx, void* c_char);
void*	sql_constant_2 (void* parseCtx, void* c_char);
void*	sql_constant_3 (void* parseCtx, void* c_char);
void*	sql_constant_4 (void* parseCtx, void* c_char);
void*	sql_eq_expressions_part_1 (void* parseCtx);
void*	sql_eq_expressions_part_2 (void* parseCtx, void* c_sql_eq_expressions);
void*	sql_eq_expressions_part_3 (void* parseCtx, void* c_char_1, void* c_sql_eq_expressions, void* c_char_2);
void*	sql_eq_expressions_1 (void* parseCtx, void* c_sql_eq_expression);
void*	sql_eq_expressions_2 (void* parseCtx, void* c_sql_eq_expressions, void* c_sql_eq_expression);
void*	sql_eq_expressions_3 (void* parseCtx, void* c_sql_eq_expressions, void* c_char, void* c_sql_eq_expression);
void*	sql_primary_expression_1 (void* parseCtx, void* c_sql_column);
void*	sql_primary_expression_2 (void* parseCtx, void* c_sql_constant);
void*	sql_eq_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_is_eq_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_ne_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_is_ne_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char_1, void* c_char_2, void* c_sql_primary_expression_2);
void*	sql_lt_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_le_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_gt_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_ge_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_like_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2);
void*	sql_comp_expression_1 (void* parseCtx, void* c_sql_eq_expression);
void*	sql_comp_expression_2 (void* parseCtx, void* c_sql_ne_expression);
void*	sql_comp_expression_3 (void* parseCtx, void* c_sql_lt_expression);
void*	sql_comp_expression_4 (void* parseCtx, void* c_sql_le_expression);
void*	sql_comp_expression_5 (void* parseCtx, void* c_sql_gt_expression);
void*	sql_comp_expression_6 (void* parseCtx, void* c_sql_ge_expression);
void*	sql_comp_expression_7 (void* parseCtx, void* c_sql_like_expression);
void*	sql_comp_expression_8 (void* parseCtx, void* c_char_1, void* c_sql_expression, void* c_char_2);
void*	sql_comp_expression_9 (void* parseCtx, void* c_sql_is_eq_expression);
void*	sql_comp_expression_10 (void* parseCtx, void* c_sql_is_ne_expression);
void*	sql_and_expression_1 (void* parseCtx, void* c_sql_comp_expression);
void*	sql_and_expression_2 (void* parseCtx, void* c_sql_and_expression, void* c_char, void* c_sql_comp_expression);
void*	sql_or_expression_1 (void* parseCtx, void* c_sql_and_expression);
void*	sql_or_expression_2 (void* parseCtx, void* c_sql_or_expression, void* c_char, void* c_sql_and_expression);
void*	sql_expression_part_1 (void* parseCtx, void* c_sql_expression);
void*	sql_expression_1 (void* parseCtx, void* c_sql_or_expression);
void*	sql_order_by_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_columns);

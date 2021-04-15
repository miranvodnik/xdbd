
%output  "sql-select.tab.c"
%defines "sql-select.tab.h"
%define api.pure full
%lex-param   { yyscan_t scanner}
%parse-param { yyscan_t scanner }
%parse-param { void* sqlCtx }

%{
	#include "yystype.h"
	#include "sql-select.lex.h"
	#include "parse-functions.h"
	static	char*	defVal = "";
	extern	char*	RetrieveLastName (void* sqlCtx);
	extern	char*	RetrieveLastNumber (void* sqlCtx);
	extern	char*	RetrieveLastString (void* sqlCtx);
%}

%union
{
	void*	sql_part;
};

%type <sql_part> sql_statement
%type <sql_part> sql_select_statement
%type <sql_part> sql_update_statement
%type <sql_part> sql_insert_statement
%type <sql_part> sql_delete_statement
%type <sql_part> sql_set_statement
%type <sql_part> sql_catalog
%type <sql_part> sql_columns_part
%type <sql_part> sql_columns
%type <sql_part> sql_column
%type <sql_part> sql_column_name
%type <sql_part> sql_function_name
%type <sql_part> sql_ordering
%type <sql_part> sql_as_part
%type <sql_part> sql_tables_part
%type <sql_part> sql_tables
%type <sql_part> sql_table_part
%type <sql_part> sql_table
%type <sql_part> sql_values_part
%type <sql_part> sql_values
%type <sql_part> sql_constant
%type <sql_part> sql_eq_expressions_part
%type <sql_part> sql_eq_expressions
%type <sql_part> sql_primary_expression
%type <sql_part> sql_eq_expression
%type <sql_part> sql_is_eq_expression
%type <sql_part> sql_ne_expression
%type <sql_part> sql_is_ne_expression
%type <sql_part> sql_lt_expression
%type <sql_part> sql_le_expression
%type <sql_part> sql_gt_expression
%type <sql_part> sql_ge_expression
%type <sql_part> sql_like_expression
%type <sql_part> sql_comp_expression
%type <sql_part> sql_and_expression
%type <sql_part> sql_or_expression
%type <sql_part> sql_expression_part
%type <sql_part> sql_expression
%type <sql_part> sql_order_by

%token	SELECT
%token	UPDATE
%token	INSERT
%token	DELETE
%token	CATALOG
%token	FROM
%token	WHERE
%token	INTO
%token	TNULL
%token	SET
%token	VALUES
%token	ORDER
%token	BY
%token	ASC
%token	ASCENDING
%token	DESC
%token	DESCENDING
%token	AS
%token	SEMICOLON
%token	COMMA
%token	NAME
%token	DOT
%token	NUMBER
%token	STRING
%token	ASTERISK
%token	QUESTION_MARK
%token	EQ
%token	IS
%token	NE
%token	NOT
%token	LT
%token	LE
%token	GT
%token	GE
%token	LBR
%token	RBR
%token	AND
%token	OR
%token	LIKE

%start	sql_statement

%%

sql_statement
	:	sql_select_statement
	{
		$$ = sql_statement_1 (sqlCtx, $1);
	}
	|	sql_update_statement
	{
		$$ = sql_statement_2 (sqlCtx, $1);
	}
	|	sql_insert_statement
	{
		$$ = sql_statement_3 (sqlCtx, $1);
	}
	|	sql_delete_statement
	{
		$$ = sql_statement_4 (sqlCtx, $1);
	}
	|	sql_set_statement
	{
		$$ = sql_statement_5 (sqlCtx, $1);
	}
	;

sql_select_statement
	:	SELECT sql_columns_part FROM sql_tables_part WHERE sql_expression_part sql_order_by SEMICOLON
	{
		$$ = sql_select_statement_1 (sqlCtx, defVal, $2, defVal, $4, defVal, $6, $7, defVal);
	}
	|	SELECT sql_columns_part FROM sql_tables_part WHERE sql_expression_part SEMICOLON
	{
		$$ = sql_select_statement_2 (sqlCtx, defVal, $2, defVal, $4, defVal, $6, defVal);
	}
	|	SELECT sql_columns_part FROM sql_tables_part WHERE sql_expression_part sql_order_by
	{
		$$ = sql_select_statement_3 (sqlCtx, defVal, $2, defVal, $4, defVal, $6, $7);
	}
	|	SELECT sql_columns_part FROM sql_tables_part WHERE sql_expression_part
	{
		$$ = sql_select_statement_4 (sqlCtx, defVal, $2, defVal, $4, defVal, $6);
	}
	|	SELECT sql_columns_part FROM sql_tables_part sql_order_by SEMICOLON
	{
		$$ = sql_select_statement_5 (sqlCtx, defVal, $2, defVal, $4, $5, defVal);
	}
	|	SELECT sql_columns_part FROM sql_tables_part SEMICOLON
	{
		$$ = sql_select_statement_6 (sqlCtx, defVal, $2, defVal, $4, defVal);
	}
	|	SELECT sql_columns_part FROM sql_tables_part sql_order_by
	{
		$$ = sql_select_statement_7 (sqlCtx, defVal, $2, defVal, $4, $5);
	}
	|	SELECT sql_columns_part FROM sql_tables_part
	{
		$$ = sql_select_statement_8 (sqlCtx, defVal, $2, defVal, $4);
	}
	;

sql_update_statement
	:	UPDATE sql_table_part SET sql_eq_expressions_part WHERE sql_expression_part SEMICOLON
	{
		$$ = sql_update_statement_1 (sqlCtx, defVal, $2, defVal, $4, defVal, $6, defVal);
	}
	|	UPDATE sql_table_part SET sql_eq_expressions_part WHERE sql_expression_part
	{
		$$ = sql_update_statement_2 (sqlCtx, defVal, $2, defVal, $4, defVal, $6);
	}
	|	UPDATE sql_table_part SET sql_eq_expressions_part SEMICOLON
	{
		$$ = sql_update_statement_3 (sqlCtx, defVal, $2, defVal, $4, defVal);
	}
	|	UPDATE sql_table_part SET sql_eq_expressions_part
	{
		$$ = sql_update_statement_4 (sqlCtx, defVal, $2, defVal, $4);
	}
	;

sql_insert_statement
	:	INSERT INTO sql_table_part LBR sql_columns_part RBR VALUES LBR sql_values_part RBR SEMICOLON
	{
		$$ = sql_insert_statement_1 (sqlCtx, defVal, defVal, $3, defVal, $5, defVal, defVal, defVal, $9, defVal, defVal);
	}
	|	INSERT INTO sql_table_part LBR sql_columns_part RBR VALUES LBR sql_values_part RBR
	{
		$$ = sql_insert_statement_2 (sqlCtx, defVal, defVal, $3, defVal, $5, defVal, defVal, defVal, $9, defVal);
	}
	|	INSERT INTO sql_table_part VALUES LBR sql_values_part RBR SEMICOLON
	{
		$$ = sql_insert_statement_3 (sqlCtx, defVal, defVal, $3, defVal, defVal, $6, defVal, defVal);
	}
	|	INSERT INTO sql_table_part VALUES LBR sql_values_part RBR
	{
		$$ = sql_insert_statement_4 (sqlCtx, defVal, defVal, $3, defVal, defVal, $6, defVal);
	}
	;

sql_delete_statement
	:	DELETE FROM sql_table_part WHERE sql_expression_part SEMICOLON
	{
		$$ = sql_delete_statement_1 (sqlCtx, defVal, defVal, $3, defVal, $5, defVal);
	}
	|	DELETE FROM sql_table_part WHERE sql_expression_part
	{
		$$ = sql_delete_statement_2 (sqlCtx, defVal, defVal, $3, defVal, $5);
	}
	|	DELETE FROM sql_table_part SEMICOLON
	{
		$$ = sql_delete_statement_3 (sqlCtx, defVal, defVal, $3, defVal);
	}
	|	DELETE FROM sql_table_part
	{
		$$ = sql_delete_statement_4 (sqlCtx, defVal, defVal, $3);
	}
	;

sql_set_statement
	:	SET CATALOG sql_catalog
	{
		$$ = sql_set_statement_1 (sqlCtx, defVal, defVal, $3);
	}
	;

sql_catalog
	:	NAME
	{
		$$ = sql_catalog_1 (sqlCtx, (void*)RetrieveLastName(sqlCtx));
	}
	;

sql_columns_part
	:
	{
		$$ = sql_columns_part_1 (sqlCtx);
	}
	|	sql_columns
	{
		$$ = sql_columns_part_2 (sqlCtx, $1);
	}
	;
	
sql_columns
	:	sql_column sql_as_part
	{
		$$ = sql_columns_1 (sqlCtx, $1, $2);
	}
	|	sql_columns COMMA sql_column sql_as_part
	{
		$$ = sql_columns_2 (sqlCtx, $1, defVal, $3, $4);
	}
	;

sql_column
	:	sql_column_name
	{
		$$ = sql_column_1 (sqlCtx, $1);
	}
	|	sql_table DOT sql_column_name
	{
		$$ = sql_column_2 (sqlCtx, $1, defVal, $3);
	}
	|	sql_function_name LBR sql_column_name RBR
	{
		$$ = sql_column_3 (sqlCtx, $1, defVal, $3, defVal);
	}
	|	sql_function_name LBR sql_table DOT sql_column_name RBR
	{
		$$ = sql_column_4 (sqlCtx, $1, defVal, $3, defVal, $5, defVal);
	}
	;

sql_column_name
	:	NAME
	{
		$$ = sql_column_name_1 (sqlCtx, (void*)RetrieveLastName(sqlCtx));
	}
	|	ASTERISK
	{
		$$ = sql_column_name_2 (sqlCtx, defVal);
	}
	|	NAME sql_ordering
	{
		$$ = sql_column_name_3 (sqlCtx, (void*)RetrieveLastName(sqlCtx), $2);
	}
	;

sql_function_name
	:	NAME
	{
		$$ = sql_function_name_1 (sqlCtx, (void*)RetrieveLastName(sqlCtx));
	}
	;

sql_ordering
	:	ASC
	{
		$$ = sql_ordering_1 (sqlCtx, defVal);
	}
	|	ASCENDING
	{
		$$ = sql_ordering_2 (sqlCtx, defVal);
	}
	|	DESC
	{
		$$ = sql_ordering_3 (sqlCtx, defVal);
	}
	|	DESCENDING
	{
		$$ = sql_ordering_4 (sqlCtx, defVal);
	}
	;

sql_as_part
	:
	{
		$$ = sql_as_part_1 (sqlCtx);
	}
	|	AS NAME
	{
		$$ = sql_as_part_2 (sqlCtx, defVal, (void*)RetrieveLastName(sqlCtx));
	}
	;

sql_tables_part
	:
	sql_tables
	{
		$$ = sql_tables_part_1 (sqlCtx, $1);
	}
	;
	
sql_tables
	:	sql_table
	{
		$$ = sql_tables_1 (sqlCtx, $1);
	}
	|	sql_tables COMMA sql_table
	{
		$$ = sql_tables_2 (sqlCtx, $1, defVal, $3);
	}
	;

sql_table_part
	:	sql_table
	{
		$$ = sql_table_part_1 (sqlCtx, $1);
	}
	;
	
sql_table
	:	NAME
	{
		$$ = sql_table_1 (sqlCtx, (void*)RetrieveLastName(sqlCtx));
	}
	;

sql_values_part
	:	sql_values
	{
		$$ = sql_values_part_1 (sqlCtx, $1);
	}
	;

sql_values
	:	sql_constant
	{
		$$ = sql_values_1 (sqlCtx, $1);
	}
	|	sql_values COMMA sql_constant
	{
		$$ = sql_values_2 (sqlCtx, $1, defVal, $3);
	}
	;

sql_constant
	:	NUMBER
	{
		$$ = sql_constant_1 (sqlCtx, (void*)RetrieveLastNumber(sqlCtx));
	}
	|	STRING
	{
		$$ = sql_constant_2 (sqlCtx, (void*)RetrieveLastString(sqlCtx));
	}
	|	TNULL
	{
		$$ = sql_constant_3 (sqlCtx, defVal);
	}
	|	QUESTION_MARK
	{
		$$ = sql_constant_4 (sqlCtx, defVal);
	}
	;

sql_eq_expressions_part
	:
	{
		$$ = sql_eq_expressions_part_1 (sqlCtx);
	}
	|	sql_eq_expressions
	{
		$$ = sql_eq_expressions_part_2 (sqlCtx, $1);
	}
	|	LBR sql_eq_expressions RBR
	{
		$$ = sql_eq_expressions_part_3 (sqlCtx, defVal, $2, defVal);
	}
	;
	
sql_eq_expressions
	:	sql_eq_expression
	{
		$$ = sql_eq_expressions_1 (sqlCtx, $1);
	}
	|	sql_eq_expressions sql_eq_expression
	{
		$$ = sql_eq_expressions_2 (sqlCtx, $1, $2);
	}
	|	sql_eq_expressions COMMA sql_eq_expression
	{
		$$ = sql_eq_expressions_3 (sqlCtx, $1, defVal, $3);
	}
	;

sql_primary_expression
	:	sql_column
	{
		$$ = sql_primary_expression_1 (sqlCtx, $1);
	}
	|	sql_constant
	{
		$$ = sql_primary_expression_2 (sqlCtx, $1);
	}
	;

sql_eq_expression
	:	sql_primary_expression EQ sql_primary_expression
	{
		$$ = sql_eq_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_is_eq_expression
	:	sql_primary_expression IS sql_primary_expression
	{
		$$ = sql_is_eq_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_ne_expression
	:	sql_primary_expression NE sql_primary_expression
	{
		$$ = sql_ne_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_is_ne_expression
	:	sql_primary_expression IS NOT sql_primary_expression
	{
		$$ = sql_is_ne_expression_1 (sqlCtx, $1, defVal, defVal, $4);
	}
	;

sql_lt_expression
	:	sql_primary_expression LT sql_primary_expression
	{
		$$ = sql_lt_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_le_expression
	:	sql_primary_expression LE sql_primary_expression
	{
		$$ = sql_le_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_gt_expression
	:	sql_primary_expression GT sql_primary_expression
	{
		$$ = sql_gt_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_ge_expression
	:	sql_primary_expression GE sql_primary_expression
	{
		$$ = sql_ge_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_like_expression
	:	sql_primary_expression LIKE sql_primary_expression
	{
		$$ = sql_like_expression_1 (sqlCtx, $1, defVal, $3);
	}
	;

sql_comp_expression
	:	sql_eq_expression
	{
		$$ = sql_comp_expression_1 (sqlCtx, $1);
	}
	|	sql_ne_expression
	{
		$$ = sql_comp_expression_2 (sqlCtx, $1);
	}
	|	sql_lt_expression
	{
		$$ = sql_comp_expression_3 (sqlCtx, $1);
	}
	|	sql_le_expression
	{
		$$ = sql_comp_expression_4 (sqlCtx, $1);
	}
	|	sql_gt_expression
	{
		$$ = sql_comp_expression_5 (sqlCtx, $1);
	}
	|	sql_ge_expression
	{
		$$ = sql_comp_expression_6 (sqlCtx, $1);
	}
	|	sql_like_expression
	{
		$$ = sql_comp_expression_7 (sqlCtx, $1);
	}
	|	LBR sql_expression RBR
	{
		$$ = sql_comp_expression_8 (sqlCtx, defVal, $2, defVal);
	}
	|	sql_is_eq_expression
	{
		$$ = sql_comp_expression_9 (sqlCtx, $1);
	}
	|	sql_is_ne_expression
	{
		$$ = sql_comp_expression_10 (sqlCtx, $1);
	}
	;

sql_and_expression
	:	sql_comp_expression
	{
		$$ = sql_and_expression_1 (sqlCtx, $1);
	}
	|	sql_and_expression AND sql_comp_expression
	{
		$$ = sql_and_expression_2 (sqlCtx, $1, defVal, $3);
	}
	;

sql_or_expression
	:	sql_and_expression
	{
		$$ = sql_or_expression_1 (sqlCtx, $1);
	}
	|	sql_or_expression OR sql_and_expression
	{
		$$ = sql_or_expression_2 (sqlCtx, $1, defVal, $3);
	}
	;
	
sql_expression_part
	:
	sql_expression
	{
		$$ = sql_expression_part_1 (sqlCtx, $1);
	}
	;
	
sql_expression
	:	sql_or_expression
	{
		$$ = sql_expression_1 (sqlCtx, $1);
	}
	;

sql_order_by
	:	ORDER BY sql_columns
	{
		$$ = sql_order_by_1 (sqlCtx, defVal, defVal, $3);
	}
	;

%%

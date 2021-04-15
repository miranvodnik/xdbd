#include "SqlCompilerCallbacks.h"
#include "sql_update_statement.h"


sql_update_statement::sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3, sql_expression_part* c_sql_expression_part, char* c_char_4) : SqlSyntax (1, sql_update_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_table_part = c_sql_table_part;
	m_char_2 = c_char_2;
	m_sql_eq_expressions_part = c_sql_eq_expressions_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
	m_char_4 = c_char_4;
}

sql_update_statement::sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3, sql_expression_part* c_sql_expression_part) : SqlSyntax (2, sql_update_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_table_part = c_sql_table_part;
	m_char_2 = c_char_2;
	m_sql_eq_expressions_part = c_sql_eq_expressions_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
}

sql_update_statement::sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part, char* c_char_3) : SqlSyntax (3, sql_update_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_table_part = c_sql_table_part;
	m_char_2 = c_char_2;
	m_sql_eq_expressions_part = c_sql_eq_expressions_part;
	m_char_3 = c_char_3;
}

sql_update_statement::sql_update_statement (char* c_char_1, sql_table_part* c_sql_table_part, char* c_char_2, sql_eq_expressions_part* c_sql_eq_expressions_part) : SqlSyntax (4, sql_update_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_table_part = c_sql_table_part;
	m_char_2 = c_char_2;
	m_sql_eq_expressions_part = c_sql_eq_expressions_part;
}

sql_update_statement::~sql_update_statement ()
{
	delete	m_sql_table_part;
	delete	m_sql_eq_expressions_part;
	delete	m_sql_expression_part;
	_init ();
}

void	sql_update_statement::_init ()
{
	m_sql_table_part = 0;
	m_sql_eq_expressions_part = 0;
	m_sql_expression_part = 0;
}

void sql_update_statement::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_update_statement_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_update_statement_1:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_eq_expressions_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
		break;
		case	g_sql_update_statement_2:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_eq_expressions_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
		break;
		case	g_sql_update_statement_3:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_eq_expressions_part->Traverse (parseCtx);
		break;
		case	g_sql_update_statement_4:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_eq_expressions_part->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_update_statement_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_update_statement_1 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3, void* c_sql_expression_part, void* c_char_4)
{
	sql_update_statement*	p_sql_update_statement = new sql_update_statement ((char*)c_char_1, (sql_table_part*)c_sql_table_part, (char*)c_char_2, (sql_eq_expressions_part*)c_sql_eq_expressions_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part, (char*)c_char_4);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_update_statement);
	((sql_eq_expressions_part*)c_sql_eq_expressions_part)->parent (p_sql_update_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_update_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_update_statement_Callback (DefaultCallbackReason, 1, p_sql_update_statement);
	return	p_sql_update_statement;
}

void*	sql_update_statement_2 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3, void* c_sql_expression_part)
{
	sql_update_statement*	p_sql_update_statement = new sql_update_statement ((char*)c_char_1, (sql_table_part*)c_sql_table_part, (char*)c_char_2, (sql_eq_expressions_part*)c_sql_eq_expressions_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_update_statement);
	((sql_eq_expressions_part*)c_sql_eq_expressions_part)->parent (p_sql_update_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_update_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_update_statement_Callback (DefaultCallbackReason, 2, p_sql_update_statement);
	return	p_sql_update_statement;
}

void*	sql_update_statement_3 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part, void* c_char_3)
{
	sql_update_statement*	p_sql_update_statement = new sql_update_statement ((char*)c_char_1, (sql_table_part*)c_sql_table_part, (char*)c_char_2, (sql_eq_expressions_part*)c_sql_eq_expressions_part, (char*)c_char_3);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_update_statement);
	((sql_eq_expressions_part*)c_sql_eq_expressions_part)->parent (p_sql_update_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_update_statement_Callback (DefaultCallbackReason, 3, p_sql_update_statement);
	return	p_sql_update_statement;
}

void*	sql_update_statement_4 (void* parseCtx, void* c_char_1, void* c_sql_table_part, void* c_char_2, void* c_sql_eq_expressions_part)
{
	sql_update_statement*	p_sql_update_statement = new sql_update_statement ((char*)c_char_1, (sql_table_part*)c_sql_table_part, (char*)c_char_2, (sql_eq_expressions_part*)c_sql_eq_expressions_part);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_update_statement);
	((sql_eq_expressions_part*)c_sql_eq_expressions_part)->parent (p_sql_update_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_update_statement_Callback (DefaultCallbackReason, 4, p_sql_update_statement);
	return	p_sql_update_statement;
}

}

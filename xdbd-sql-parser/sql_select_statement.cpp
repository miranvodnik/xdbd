#include "SqlCompilerCallbacks.h"
#include "sql_select_statement.h"


sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, sql_order_by* c_sql_order_by, char* c_char_4) : SqlSyntax (1, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
	m_sql_order_by = c_sql_order_by;
	m_char_4 = c_char_4;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, char* c_char_4) : SqlSyntax (2, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
	m_char_4 = c_char_4;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part, sql_order_by* c_sql_order_by) : SqlSyntax (3, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
	m_sql_order_by = c_sql_order_by;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3, sql_expression_part* c_sql_expression_part) : SqlSyntax (4, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_char_3 = c_char_3;
	m_sql_expression_part = c_sql_expression_part;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, sql_order_by* c_sql_order_by, char* c_char_3) : SqlSyntax (5, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_sql_order_by = c_sql_order_by;
	m_char_3 = c_char_3;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, char* c_char_3) : SqlSyntax (6, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_char_3 = c_char_3;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part, sql_order_by* c_sql_order_by) : SqlSyntax (7, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
	m_sql_order_by = c_sql_order_by;
}

sql_select_statement::sql_select_statement (char* c_char_1, sql_columns_part* c_sql_columns_part, char* c_char_2, sql_tables_part* c_sql_tables_part) : SqlSyntax (8, sql_select_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_columns_part = c_sql_columns_part;
	m_char_2 = c_char_2;
	m_sql_tables_part = c_sql_tables_part;
}

sql_select_statement::~sql_select_statement ()
{
	delete	m_sql_columns_part;
	delete	m_sql_tables_part;
	delete	m_sql_expression_part;
	delete	m_sql_order_by;
	_init ();
}

void	sql_select_statement::_init ()
{
	m_sql_columns_part = 0;
	m_sql_tables_part = 0;
	m_sql_expression_part = 0;
	m_sql_order_by = 0;
}

void sql_select_statement::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_select_statement_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_select_statement_1:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
			m_sql_order_by->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_2:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_3:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
			m_sql_order_by->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_4:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_expression_part->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_5:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_order_by->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_6:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_7:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
			m_sql_order_by->Traverse (parseCtx);
		break;
		case	g_sql_select_statement_8:
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_tables_part->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_select_statement_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_select_statement_1 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_sql_order_by, void* c_char_4)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part, (sql_order_by*)c_sql_order_by, (char*)c_char_4);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_select_statement);
	((sql_order_by*)c_sql_order_by)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 1, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_2 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_char_4)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part, (char*)c_char_4);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 2, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_3 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part, void* c_sql_order_by)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part, (sql_order_by*)c_sql_order_by);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_select_statement);
	((sql_order_by*)c_sql_order_by)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 3, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_4 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3, void* c_sql_expression_part)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (char*)c_char_3, (sql_expression_part*)c_sql_expression_part);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_expression_part*)c_sql_expression_part)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 4, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_5 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_sql_order_by, void* c_char_3)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (sql_order_by*)c_sql_order_by, (char*)c_char_3);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_order_by*)c_sql_order_by)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 5, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_6 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_char_3)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (char*)c_char_3);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 6, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_7 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part, void* c_sql_order_by)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part, (sql_order_by*)c_sql_order_by);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((sql_order_by*)c_sql_order_by)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 7, p_sql_select_statement);
	return	p_sql_select_statement;
}

void*	sql_select_statement_8 (void* parseCtx, void* c_char_1, void* c_sql_columns_part, void* c_char_2, void* c_sql_tables_part)
{
	sql_select_statement*	p_sql_select_statement = new sql_select_statement ((char*)c_char_1, (sql_columns_part*)c_sql_columns_part, (char*)c_char_2, (sql_tables_part*)c_sql_tables_part);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_select_statement);
	((sql_tables_part*)c_sql_tables_part)->parent (p_sql_select_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_select_statement_Callback (DefaultCallbackReason, 8, p_sql_select_statement);
	return	p_sql_select_statement;
}

}

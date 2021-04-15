#include "SqlCompilerCallbacks.h"
#include "sql_insert_statement.h"


sql_insert_statement::sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, sql_columns_part* c_sql_columns_part, char* c_char_4, char* c_char_5, char* c_char_6, sql_values_part* c_sql_values_part, char* c_char_7, char* c_char_8) : SqlSyntax (1, sql_insert_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_table_part = c_sql_table_part;
	m_char_3 = c_char_3;
	m_sql_columns_part = c_sql_columns_part;
	m_char_4 = c_char_4;
	m_char_5 = c_char_5;
	m_char_6 = c_char_6;
	m_sql_values_part = c_sql_values_part;
	m_char_7 = c_char_7;
	m_char_8 = c_char_8;
}

sql_insert_statement::sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, sql_columns_part* c_sql_columns_part, char* c_char_4, char* c_char_5, char* c_char_6, sql_values_part* c_sql_values_part, char* c_char_7) : SqlSyntax (2, sql_insert_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_table_part = c_sql_table_part;
	m_char_3 = c_char_3;
	m_sql_columns_part = c_sql_columns_part;
	m_char_4 = c_char_4;
	m_char_5 = c_char_5;
	m_char_6 = c_char_6;
	m_sql_values_part = c_sql_values_part;
	m_char_7 = c_char_7;
}

sql_insert_statement::sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, char* c_char_4, sql_values_part* c_sql_values_part, char* c_char_5, char* c_char_6) : SqlSyntax (3, sql_insert_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_table_part = c_sql_table_part;
	m_char_3 = c_char_3;
	m_char_4 = c_char_4;
	m_sql_values_part = c_sql_values_part;
	m_char_5 = c_char_5;
	m_char_6 = c_char_6;
}

sql_insert_statement::sql_insert_statement (char* c_char_1, char* c_char_2, sql_table_part* c_sql_table_part, char* c_char_3, char* c_char_4, sql_values_part* c_sql_values_part, char* c_char_5) : SqlSyntax (4, sql_insert_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_table_part = c_sql_table_part;
	m_char_3 = c_char_3;
	m_char_4 = c_char_4;
	m_sql_values_part = c_sql_values_part;
	m_char_5 = c_char_5;
}

sql_insert_statement::~sql_insert_statement ()
{
	delete	m_sql_table_part;
	delete	m_sql_columns_part;
	delete	m_sql_values_part;
	_init ();
}

void	sql_insert_statement::_init ()
{
	m_sql_table_part = 0;
	m_sql_columns_part = 0;
	m_sql_values_part = 0;
}

void sql_insert_statement::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_insert_statement_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_insert_statement_1:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_values_part->Traverse (parseCtx);
		break;
		case	g_sql_insert_statement_2:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_columns_part->Traverse (parseCtx);
			m_sql_values_part->Traverse (parseCtx);
		break;
		case	g_sql_insert_statement_3:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_values_part->Traverse (parseCtx);
		break;
		case	g_sql_insert_statement_4:
			m_sql_table_part->Traverse (parseCtx);
			m_sql_values_part->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_insert_statement_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_insert_statement_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_columns_part, void* c_char_4, void* c_char_5, void* c_char_6, void* c_sql_values_part, void* c_char_7, void* c_char_8)
{
	sql_insert_statement*	p_sql_insert_statement = new sql_insert_statement ((char*)c_char_1, (char*)c_char_2, (sql_table_part*)c_sql_table_part, (char*)c_char_3, (sql_columns_part*)c_sql_columns_part, (char*)c_char_4, (char*)c_char_5, (char*)c_char_6, (sql_values_part*)c_sql_values_part, (char*)c_char_7, (char*)c_char_8);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_insert_statement);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_insert_statement);
	((sql_values_part*)c_sql_values_part)->parent (p_sql_insert_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_insert_statement_Callback (DefaultCallbackReason, 1, p_sql_insert_statement);
	return	p_sql_insert_statement;
}

void*	sql_insert_statement_2 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_sql_columns_part, void* c_char_4, void* c_char_5, void* c_char_6, void* c_sql_values_part, void* c_char_7)
{
	sql_insert_statement*	p_sql_insert_statement = new sql_insert_statement ((char*)c_char_1, (char*)c_char_2, (sql_table_part*)c_sql_table_part, (char*)c_char_3, (sql_columns_part*)c_sql_columns_part, (char*)c_char_4, (char*)c_char_5, (char*)c_char_6, (sql_values_part*)c_sql_values_part, (char*)c_char_7);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_insert_statement);
	((sql_columns_part*)c_sql_columns_part)->parent (p_sql_insert_statement);
	((sql_values_part*)c_sql_values_part)->parent (p_sql_insert_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_insert_statement_Callback (DefaultCallbackReason, 2, p_sql_insert_statement);
	return	p_sql_insert_statement;
}

void*	sql_insert_statement_3 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_char_4, void* c_sql_values_part, void* c_char_5, void* c_char_6)
{
	sql_insert_statement*	p_sql_insert_statement = new sql_insert_statement ((char*)c_char_1, (char*)c_char_2, (sql_table_part*)c_sql_table_part, (char*)c_char_3, (char*)c_char_4, (sql_values_part*)c_sql_values_part, (char*)c_char_5, (char*)c_char_6);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_insert_statement);
	((sql_values_part*)c_sql_values_part)->parent (p_sql_insert_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_insert_statement_Callback (DefaultCallbackReason, 3, p_sql_insert_statement);
	return	p_sql_insert_statement;
}

void*	sql_insert_statement_4 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_table_part, void* c_char_3, void* c_char_4, void* c_sql_values_part, void* c_char_5)
{
	sql_insert_statement*	p_sql_insert_statement = new sql_insert_statement ((char*)c_char_1, (char*)c_char_2, (sql_table_part*)c_sql_table_part, (char*)c_char_3, (char*)c_char_4, (sql_values_part*)c_sql_values_part, (char*)c_char_5);
	((sql_table_part*)c_sql_table_part)->parent (p_sql_insert_statement);
	((sql_values_part*)c_sql_values_part)->parent (p_sql_insert_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_insert_statement_Callback (DefaultCallbackReason, 4, p_sql_insert_statement);
	return	p_sql_insert_statement;
}

}

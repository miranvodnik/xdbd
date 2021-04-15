#include "SqlCompilerCallbacks.h"
#include "sql_column.h"


sql_column::sql_column (sql_column_name* c_sql_column_name) : SqlSyntax (1, sql_column_Context)
{
	_init ();
	m_sql_column_name = c_sql_column_name;
}

sql_column::sql_column (sql_table* c_sql_table, char* c_char, sql_column_name* c_sql_column_name) : SqlSyntax (2, sql_column_Context)
{
	_init ();
	m_sql_table = c_sql_table;
	m_char = c_char;
	m_sql_column_name = c_sql_column_name;
}

sql_column::sql_column (sql_function_name* c_sql_function_name, char* c_char_1, sql_column_name* c_sql_column_name, char* c_char_2) : SqlSyntax (3, sql_column_Context)
{
	_init ();
	m_sql_function_name = c_sql_function_name;
	m_char_1 = c_char_1;
	m_sql_column_name = c_sql_column_name;
	m_char_2 = c_char_2;
}

sql_column::sql_column (sql_function_name* c_sql_function_name, char* c_char_1, sql_table* c_sql_table, char* c_char_2, sql_column_name* c_sql_column_name, char* c_char_3) : SqlSyntax (4, sql_column_Context)
{
	_init ();
	m_sql_function_name = c_sql_function_name;
	m_char_1 = c_char_1;
	m_sql_table = c_sql_table;
	m_char_2 = c_char_2;
	m_sql_column_name = c_sql_column_name;
	m_char_3 = c_char_3;
}

sql_column::~sql_column ()
{
	delete	m_sql_column_name;
	delete	m_sql_table;
	delete	m_sql_function_name;
	_init ();
}

void	sql_column::_init ()
{
	m_sql_column_name = 0;
	m_sql_table = 0;
	m_sql_function_name = 0;
}

void sql_column::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_column_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_column_1:
			m_sql_column_name->Traverse (parseCtx);
		break;
		case	g_sql_column_2:
			m_sql_table->Traverse (parseCtx);
			m_sql_column_name->Traverse (parseCtx);
		break;
		case	g_sql_column_3:
			m_sql_function_name->Traverse (parseCtx);
			m_sql_column_name->Traverse (parseCtx);
		break;
		case	g_sql_column_4:
			m_sql_function_name->Traverse (parseCtx);
			m_sql_table->Traverse (parseCtx);
			m_sql_column_name->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_column_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_column_1 (void* parseCtx, void* c_sql_column_name)
{
	sql_column*	p_sql_column = new sql_column ((sql_column_name*)c_sql_column_name);
	((sql_column_name*)c_sql_column_name)->parent (p_sql_column);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_Callback (DefaultCallbackReason, 1, p_sql_column);
	return	p_sql_column;
}

void*	sql_column_2 (void* parseCtx, void* c_sql_table, void* c_char, void* c_sql_column_name)
{
	sql_column*	p_sql_column = new sql_column ((sql_table*)c_sql_table, (char*)c_char, (sql_column_name*)c_sql_column_name);
	((sql_table*)c_sql_table)->parent (p_sql_column);
	((sql_column_name*)c_sql_column_name)->parent (p_sql_column);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_Callback (DefaultCallbackReason, 2, p_sql_column);
	return	p_sql_column;
}

void*	sql_column_3 (void* parseCtx, void* c_sql_function_name, void* c_char_1, void* c_sql_column_name, void* c_char_2)
{
	sql_column*	p_sql_column = new sql_column ((sql_function_name*)c_sql_function_name, (char*)c_char_1, (sql_column_name*)c_sql_column_name, (char*)c_char_2);
	((sql_function_name*)c_sql_function_name)->parent (p_sql_column);
	((sql_column_name*)c_sql_column_name)->parent (p_sql_column);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_Callback (DefaultCallbackReason, 3, p_sql_column);
	return	p_sql_column;
}

void*	sql_column_4 (void* parseCtx, void* c_sql_function_name, void* c_char_1, void* c_sql_table, void* c_char_2, void* c_sql_column_name, void* c_char_3)
{
	sql_column*	p_sql_column = new sql_column ((sql_function_name*)c_sql_function_name, (char*)c_char_1, (sql_table*)c_sql_table, (char*)c_char_2, (sql_column_name*)c_sql_column_name, (char*)c_char_3);
	((sql_function_name*)c_sql_function_name)->parent (p_sql_column);
	((sql_table*)c_sql_table)->parent (p_sql_column);
	((sql_column_name*)c_sql_column_name)->parent (p_sql_column);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_Callback (DefaultCallbackReason, 4, p_sql_column);
	return	p_sql_column;
}

}

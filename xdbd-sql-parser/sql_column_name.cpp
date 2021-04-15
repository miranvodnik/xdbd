#include "SqlCompilerCallbacks.h"
#include "sql_column_name.h"


sql_column_name::sql_column_name (unsigned int kind, char* c_char) : SqlSyntax (kind, sql_column_name_Context)
{
	_init ();
	m_char = c_char;
}

sql_column_name::sql_column_name (char* c_char, sql_ordering* c_sql_ordering) : SqlSyntax (3, sql_column_name_Context)
{
	_init ();
	m_char = c_char;
	m_sql_ordering = c_sql_ordering;
}

sql_column_name::~sql_column_name ()
{
	delete	m_sql_ordering;
	_init ();
}

void	sql_column_name::_init ()
{
	m_sql_ordering = 0;
}

void sql_column_name::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_column_name_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_column_name_1:
		break;
		case	g_sql_column_name_2:
		break;
		case	g_sql_column_name_3:
			m_sql_ordering->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_column_name_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_column_name_1 (void* parseCtx, void* c_char)
{
	sql_column_name*	p_sql_column_name = new sql_column_name (1, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_name_Callback (DefaultCallbackReason, 1, p_sql_column_name);
	return	p_sql_column_name;
}

void*	sql_column_name_2 (void* parseCtx, void* c_char)
{
	sql_column_name*	p_sql_column_name = new sql_column_name (2, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_name_Callback (DefaultCallbackReason, 2, p_sql_column_name);
	return	p_sql_column_name;
}

void*	sql_column_name_3 (void* parseCtx, void* c_char, void* c_sql_ordering)
{
	sql_column_name*	p_sql_column_name = new sql_column_name ((char*)c_char, (sql_ordering*)c_sql_ordering);
	((sql_ordering*)c_sql_ordering)->parent (p_sql_column_name);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_column_name_Callback (DefaultCallbackReason, 3, p_sql_column_name);
	return	p_sql_column_name;
}

}

#include "SqlCompilerCallbacks.h"
#include "sql_function_name.h"


sql_function_name::sql_function_name (char* c_char) : SqlSyntax (1, sql_function_name_Context)
{
	_init ();
	m_char = c_char;
}

sql_function_name::~sql_function_name ()
{
	_init ();
}

void	sql_function_name::_init ()
{
}

void sql_function_name::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_function_name_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_function_name_1:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_function_name_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_function_name_1 (void* parseCtx, void* c_char)
{
	sql_function_name*	p_sql_function_name = new sql_function_name ((char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_function_name_Callback (DefaultCallbackReason, 1, p_sql_function_name);
	return	p_sql_function_name;
}

}

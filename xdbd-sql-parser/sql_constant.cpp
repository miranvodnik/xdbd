#include "SqlCompilerCallbacks.h"
#include "sql_constant.h"


sql_constant::sql_constant (unsigned int kind, char* c_char) : SqlSyntax (kind, sql_constant_Context)
{
	_init ();
	m_char = c_char;
}

sql_constant::~sql_constant ()
{
	_init ();
}

void	sql_constant::_init ()
{
}

void sql_constant::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_constant_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_constant_1:
		break;
		case	g_sql_constant_2:
		break;
		case	g_sql_constant_3:
		break;
		case	g_sql_constant_4:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_constant_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_constant_1 (void* parseCtx, void* c_char)
{
	sql_constant*	p_sql_constant = new sql_constant (1, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_constant_Callback (DefaultCallbackReason, 1, p_sql_constant);
	return	p_sql_constant;
}

void*	sql_constant_2 (void* parseCtx, void* c_char)
{
	sql_constant*	p_sql_constant = new sql_constant (2, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_constant_Callback (DefaultCallbackReason, 2, p_sql_constant);
	return	p_sql_constant;
}

void*	sql_constant_3 (void* parseCtx, void* c_char)
{
	sql_constant*	p_sql_constant = new sql_constant (3, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_constant_Callback (DefaultCallbackReason, 3, p_sql_constant);
	return	p_sql_constant;
}

void*	sql_constant_4 (void* parseCtx, void* c_char)
{
	sql_constant*	p_sql_constant = new sql_constant (4, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_constant_Callback (DefaultCallbackReason, 4, p_sql_constant);
	return	p_sql_constant;
}

}

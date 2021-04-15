#include "SqlCompilerCallbacks.h"
#include "sql_ordering.h"


sql_ordering::sql_ordering (unsigned int kind, char* c_char) : SqlSyntax (kind, sql_ordering_Context)
{
	_init ();
	m_char = c_char;
}

sql_ordering::~sql_ordering ()
{
	_init ();
}

void	sql_ordering::_init ()
{
}

void sql_ordering::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_ordering_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_ordering_1:
		break;
		case	g_sql_ordering_2:
		break;
		case	g_sql_ordering_3:
		break;
		case	g_sql_ordering_4:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_ordering_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_ordering_1 (void* parseCtx, void* c_char)
{
	sql_ordering*	p_sql_ordering = new sql_ordering (1, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_ordering_Callback (DefaultCallbackReason, 1, p_sql_ordering);
	return	p_sql_ordering;
}

void*	sql_ordering_2 (void* parseCtx, void* c_char)
{
	sql_ordering*	p_sql_ordering = new sql_ordering (2, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_ordering_Callback (DefaultCallbackReason, 2, p_sql_ordering);
	return	p_sql_ordering;
}

void*	sql_ordering_3 (void* parseCtx, void* c_char)
{
	sql_ordering*	p_sql_ordering = new sql_ordering (3, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_ordering_Callback (DefaultCallbackReason, 3, p_sql_ordering);
	return	p_sql_ordering;
}

void*	sql_ordering_4 (void* parseCtx, void* c_char)
{
	sql_ordering*	p_sql_ordering = new sql_ordering (4, (char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_ordering_Callback (DefaultCallbackReason, 4, p_sql_ordering);
	return	p_sql_ordering;
}

}

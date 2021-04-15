#include "SqlCompilerCallbacks.h"
#include "sql_as_part.h"


sql_as_part::sql_as_part () : SqlSyntax (1, sql_as_part_Context)
{
	_init ();
}

sql_as_part::sql_as_part (char* c_char_1, char* c_char_2) : SqlSyntax (2, sql_as_part_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
}

sql_as_part::~sql_as_part ()
{
	_init ();
}

void	sql_as_part::_init ()
{
}

void sql_as_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_as_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_as_part_1:
		break;
		case	g_sql_as_part_2:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_as_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_as_part_1 (void* parseCtx)
{
	sql_as_part*	p_sql_as_part = new sql_as_part ();
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_as_part_Callback (DefaultCallbackReason, 1, p_sql_as_part);
	return	p_sql_as_part;
}

void*	sql_as_part_2 (void* parseCtx, void* c_char_1, void* c_char_2)
{
	sql_as_part*	p_sql_as_part = new sql_as_part ((char*)c_char_1, (char*)c_char_2);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_as_part_Callback (DefaultCallbackReason, 2, p_sql_as_part);
	return	p_sql_as_part;
}

}

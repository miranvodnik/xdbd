#include "SqlCompilerCallbacks.h"
#include "sql_eq_expressions_part.h"


sql_eq_expressions_part::sql_eq_expressions_part () : SqlSyntax (1, sql_eq_expressions_part_Context)
{
	_init ();
}

sql_eq_expressions_part::sql_eq_expressions_part (sql_eq_expressions* c_sql_eq_expressions) : SqlSyntax (2, sql_eq_expressions_part_Context)
{
	_init ();
	m_sql_eq_expressions = c_sql_eq_expressions;
}

sql_eq_expressions_part::sql_eq_expressions_part (char* c_char_1, sql_eq_expressions* c_sql_eq_expressions, char* c_char_2) : SqlSyntax (3, sql_eq_expressions_part_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_eq_expressions = c_sql_eq_expressions;
	m_char_2 = c_char_2;
}

sql_eq_expressions_part::~sql_eq_expressions_part ()
{
	delete	m_sql_eq_expressions;
	_init ();
}

void	sql_eq_expressions_part::_init ()
{
	m_sql_eq_expressions = 0;
}

void sql_eq_expressions_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_eq_expressions_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_eq_expressions_part_1:
		break;
		case	g_sql_eq_expressions_part_2:
			m_sql_eq_expressions->Traverse (parseCtx);
		break;
		case	g_sql_eq_expressions_part_3:
			m_sql_eq_expressions->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_eq_expressions_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_eq_expressions_part_1 (void* parseCtx)
{
	sql_eq_expressions_part*	p_sql_eq_expressions_part = new sql_eq_expressions_part ();
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_part_Callback (DefaultCallbackReason, 1, p_sql_eq_expressions_part);
	return	p_sql_eq_expressions_part;
}

void*	sql_eq_expressions_part_2 (void* parseCtx, void* c_sql_eq_expressions)
{
	sql_eq_expressions_part*	p_sql_eq_expressions_part = new sql_eq_expressions_part ((sql_eq_expressions*)c_sql_eq_expressions);
	((sql_eq_expressions*)c_sql_eq_expressions)->parent (p_sql_eq_expressions_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_part_Callback (DefaultCallbackReason, 2, p_sql_eq_expressions_part);
	return	p_sql_eq_expressions_part;
}

void*	sql_eq_expressions_part_3 (void* parseCtx, void* c_char_1, void* c_sql_eq_expressions, void* c_char_2)
{
	sql_eq_expressions_part*	p_sql_eq_expressions_part = new sql_eq_expressions_part ((char*)c_char_1, (sql_eq_expressions*)c_sql_eq_expressions, (char*)c_char_2);
	((sql_eq_expressions*)c_sql_eq_expressions)->parent (p_sql_eq_expressions_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_part_Callback (DefaultCallbackReason, 3, p_sql_eq_expressions_part);
	return	p_sql_eq_expressions_part;
}

}

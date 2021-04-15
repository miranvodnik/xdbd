#include "SqlCompilerCallbacks.h"
#include "sql_eq_expressions.h"


sql_eq_expressions::sql_eq_expressions (sql_eq_expression* c_sql_eq_expression) : SqlSyntax (1, sql_eq_expressions_Context)
{
	_init ();
	m_sql_eq_expression = c_sql_eq_expression;
}

sql_eq_expressions::sql_eq_expressions (sql_eq_expressions* c_sql_eq_expressions, sql_eq_expression* c_sql_eq_expression) : SqlSyntax (2, sql_eq_expressions_Context)
{
	_init ();
	m_sql_eq_expressions = c_sql_eq_expressions;
	m_sql_eq_expression = c_sql_eq_expression;
}

sql_eq_expressions::sql_eq_expressions (sql_eq_expressions* c_sql_eq_expressions, char* c_char, sql_eq_expression* c_sql_eq_expression) : SqlSyntax (3, sql_eq_expressions_Context)
{
	_init ();
	m_sql_eq_expressions = c_sql_eq_expressions;
	m_char = c_char;
	m_sql_eq_expression = c_sql_eq_expression;
}

sql_eq_expressions::~sql_eq_expressions ()
{
	delete	m_sql_eq_expression;
	delete	m_sql_eq_expressions;
	_init ();
}

void	sql_eq_expressions::_init ()
{
	m_sql_eq_expression = 0;
	m_sql_eq_expressions = 0;
}

void sql_eq_expressions::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_eq_expressions_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_eq_expressions_1:
			m_sql_eq_expression->Traverse (parseCtx);
		break;
		case	g_sql_eq_expressions_2:
			m_sql_eq_expressions->Traverse (parseCtx);
			m_sql_eq_expression->Traverse (parseCtx);
		break;
		case	g_sql_eq_expressions_3:
			m_sql_eq_expressions->Traverse (parseCtx);
			m_sql_eq_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_eq_expressions_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_eq_expressions*	sql_eq_expressions::revert_sql_eq_expressions (sql_eq_expressions* p_sql_eq_expressions, u_int kind)
{
	if (this == 0)
		return	p_sql_eq_expressions;
	sql_eq_expressions*	q_sql_eq_expressions = m_sql_eq_expressions->revert_sql_eq_expressions(this, m_kind);
	m_kind = kind;
	m_sql_eq_expressions = p_sql_eq_expressions;
	return	q_sql_eq_expressions;
}

extern "C"
{

void*	sql_eq_expressions_1 (void* parseCtx, void* c_sql_eq_expression)
{
	sql_eq_expressions*	p_sql_eq_expressions = new sql_eq_expressions ((sql_eq_expression*)c_sql_eq_expression);
	((sql_eq_expression*)c_sql_eq_expression)->parent (p_sql_eq_expressions);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_Callback (DefaultCallbackReason, 1, p_sql_eq_expressions);
	return	p_sql_eq_expressions;
}

void*	sql_eq_expressions_2 (void* parseCtx, void* c_sql_eq_expressions, void* c_sql_eq_expression)
{
	sql_eq_expressions*	p_sql_eq_expressions = new sql_eq_expressions ((sql_eq_expressions*)c_sql_eq_expressions, (sql_eq_expression*)c_sql_eq_expression);
	((sql_eq_expressions*)c_sql_eq_expressions)->parent (p_sql_eq_expressions);
	((sql_eq_expression*)c_sql_eq_expression)->parent (p_sql_eq_expressions);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_Callback (DefaultCallbackReason, 2, p_sql_eq_expressions);
	return	p_sql_eq_expressions;
}

void*	sql_eq_expressions_3 (void* parseCtx, void* c_sql_eq_expressions, void* c_char, void* c_sql_eq_expression)
{
	sql_eq_expressions*	p_sql_eq_expressions = new sql_eq_expressions ((sql_eq_expressions*)c_sql_eq_expressions, (char*)c_char, (sql_eq_expression*)c_sql_eq_expression);
	((sql_eq_expressions*)c_sql_eq_expressions)->parent (p_sql_eq_expressions);
	((sql_eq_expression*)c_sql_eq_expression)->parent (p_sql_eq_expressions);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_eq_expressions_Callback (DefaultCallbackReason, 3, p_sql_eq_expressions);
	return	p_sql_eq_expressions;
}

}

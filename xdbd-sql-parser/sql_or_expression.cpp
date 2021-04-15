#include "SqlCompilerCallbacks.h"
#include "sql_or_expression.h"


sql_or_expression::sql_or_expression (sql_and_expression* c_sql_and_expression) : SqlSyntax (1, sql_or_expression_Context)
{
	_init ();
	m_sql_and_expression = c_sql_and_expression;
}

sql_or_expression::sql_or_expression (sql_or_expression* c_sql_or_expression, char* c_char, sql_and_expression* c_sql_and_expression) : SqlSyntax (2, sql_or_expression_Context)
{
	_init ();
	m_sql_or_expression = c_sql_or_expression;
	m_char = c_char;
	m_sql_and_expression = c_sql_and_expression;
}

sql_or_expression::~sql_or_expression ()
{
	delete	m_sql_and_expression;
	delete	m_sql_or_expression;
	_init ();
}

void	sql_or_expression::_init ()
{
	m_sql_and_expression = 0;
	m_sql_or_expression = 0;
}

void sql_or_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_or_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_or_expression_1:
			m_sql_and_expression->Traverse (parseCtx);
		break;
		case	g_sql_or_expression_2:
			m_sql_or_expression->Traverse (parseCtx);
			m_sql_and_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_or_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_or_expression*	sql_or_expression::revert_sql_or_expression (sql_or_expression* p_sql_or_expression, u_int kind)
{
	if (this == 0)
		return	p_sql_or_expression;
	sql_or_expression*	q_sql_or_expression = m_sql_or_expression->revert_sql_or_expression(this, m_kind);
	m_kind = kind;
	m_sql_or_expression = p_sql_or_expression;
	return	q_sql_or_expression;
}

extern "C"
{

void*	sql_or_expression_1 (void* parseCtx, void* c_sql_and_expression)
{
	sql_or_expression*	p_sql_or_expression = new sql_or_expression ((sql_and_expression*)c_sql_and_expression);
	((sql_and_expression*)c_sql_and_expression)->parent (p_sql_or_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_or_expression_Callback (DefaultCallbackReason, 1, p_sql_or_expression);
	return	p_sql_or_expression;
}

void*	sql_or_expression_2 (void* parseCtx, void* c_sql_or_expression, void* c_char, void* c_sql_and_expression)
{
	sql_or_expression*	p_sql_or_expression = new sql_or_expression ((sql_or_expression*)c_sql_or_expression, (char*)c_char, (sql_and_expression*)c_sql_and_expression);
	((sql_or_expression*)c_sql_or_expression)->parent (p_sql_or_expression);
	((sql_and_expression*)c_sql_and_expression)->parent (p_sql_or_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_or_expression_Callback (DefaultCallbackReason, 2, p_sql_or_expression);
	return	p_sql_or_expression;
}

}

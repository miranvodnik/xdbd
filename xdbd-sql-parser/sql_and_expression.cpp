#include "SqlCompilerCallbacks.h"
#include "sql_and_expression.h"


sql_and_expression::sql_and_expression (sql_comp_expression* c_sql_comp_expression) : SqlSyntax (1, sql_and_expression_Context)
{
	_init ();
	m_sql_comp_expression = c_sql_comp_expression;
}

sql_and_expression::sql_and_expression (sql_and_expression* c_sql_and_expression, char* c_char, sql_comp_expression* c_sql_comp_expression) : SqlSyntax (2, sql_and_expression_Context)
{
	_init ();
	m_sql_and_expression = c_sql_and_expression;
	m_char = c_char;
	m_sql_comp_expression = c_sql_comp_expression;
}

sql_and_expression::~sql_and_expression ()
{
	delete	m_sql_comp_expression;
	delete	m_sql_and_expression;
	_init ();
}

void	sql_and_expression::_init ()
{
	m_sql_comp_expression = 0;
	m_sql_and_expression = 0;
}

void sql_and_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_and_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_and_expression_1:
			m_sql_comp_expression->Traverse (parseCtx);
		break;
		case	g_sql_and_expression_2:
			m_sql_and_expression->Traverse (parseCtx);
			m_sql_comp_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_and_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_and_expression*	sql_and_expression::revert_sql_and_expression (sql_and_expression* p_sql_and_expression, u_int kind)
{
	if (this == 0)
		return	p_sql_and_expression;
	sql_and_expression*	q_sql_and_expression = m_sql_and_expression->revert_sql_and_expression(this, m_kind);
	m_kind = kind;
	m_sql_and_expression = p_sql_and_expression;
	return	q_sql_and_expression;
}

extern "C"
{

void*	sql_and_expression_1 (void* parseCtx, void* c_sql_comp_expression)
{
	sql_and_expression*	p_sql_and_expression = new sql_and_expression ((sql_comp_expression*)c_sql_comp_expression);
	((sql_comp_expression*)c_sql_comp_expression)->parent (p_sql_and_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_and_expression_Callback (DefaultCallbackReason, 1, p_sql_and_expression);
	return	p_sql_and_expression;
}

void*	sql_and_expression_2 (void* parseCtx, void* c_sql_and_expression, void* c_char, void* c_sql_comp_expression)
{
	sql_and_expression*	p_sql_and_expression = new sql_and_expression ((sql_and_expression*)c_sql_and_expression, (char*)c_char, (sql_comp_expression*)c_sql_comp_expression);
	((sql_and_expression*)c_sql_and_expression)->parent (p_sql_and_expression);
	((sql_comp_expression*)c_sql_comp_expression)->parent (p_sql_and_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_and_expression_Callback (DefaultCallbackReason, 2, p_sql_and_expression);
	return	p_sql_and_expression;
}

}

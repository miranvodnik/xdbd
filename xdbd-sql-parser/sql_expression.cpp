#include "SqlCompilerCallbacks.h"
#include "sql_expression.h"


sql_expression::sql_expression (sql_or_expression* c_sql_or_expression) : SqlSyntax (1, sql_expression_Context)
{
	_init ();
	m_sql_or_expression = c_sql_or_expression;
}

sql_expression::~sql_expression ()
{
	delete	m_sql_or_expression;
	_init ();
}

void	sql_expression::_init ()
{
	m_sql_or_expression = 0;
}

void sql_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_expression_1:
			m_sql_or_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_expression_1 (void* parseCtx, void* c_sql_or_expression)
{
	sql_expression*	p_sql_expression = new sql_expression ((sql_or_expression*)c_sql_or_expression);
	((sql_or_expression*)c_sql_or_expression)->parent (p_sql_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_expression_Callback (DefaultCallbackReason, 1, p_sql_expression);
	return	p_sql_expression;
}

}

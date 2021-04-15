#include "SqlCompilerCallbacks.h"
#include "sql_expression_part.h"


sql_expression_part::sql_expression_part (sql_expression* c_sql_expression) : SqlSyntax (1, sql_expression_part_Context)
{
	_init ();
	m_sql_expression = c_sql_expression;
}

sql_expression_part::~sql_expression_part ()
{
	delete	m_sql_expression;
	_init ();
}

void	sql_expression_part::_init ()
{
	m_sql_expression = 0;
}

void sql_expression_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_expression_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_expression_part_1:
			m_sql_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_expression_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_expression_part_1 (void* parseCtx, void* c_sql_expression)
{
	sql_expression_part*	p_sql_expression_part = new sql_expression_part ((sql_expression*)c_sql_expression);
	((sql_expression*)c_sql_expression)->parent (p_sql_expression_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_expression_part_Callback (DefaultCallbackReason, 1, p_sql_expression_part);
	return	p_sql_expression_part;
}

}

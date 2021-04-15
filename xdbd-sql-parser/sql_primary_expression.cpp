#include "SqlCompilerCallbacks.h"
#include "sql_primary_expression.h"


sql_primary_expression::sql_primary_expression (sql_column* c_sql_column) : SqlSyntax (1, sql_primary_expression_Context)
{
	_init ();
	m_sql_column = c_sql_column;
}

sql_primary_expression::sql_primary_expression (sql_constant* c_sql_constant) : SqlSyntax (2, sql_primary_expression_Context)
{
	_init ();
	m_sql_constant = c_sql_constant;
}

sql_primary_expression::~sql_primary_expression ()
{
	delete	m_sql_column;
	delete	m_sql_constant;
	_init ();
}

void	sql_primary_expression::_init ()
{
	m_sql_column = 0;
	m_sql_constant = 0;
}

void sql_primary_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_primary_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_primary_expression_1:
			m_sql_column->Traverse (parseCtx);
		break;
		case	g_sql_primary_expression_2:
			m_sql_constant->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_primary_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_primary_expression_1 (void* parseCtx, void* c_sql_column)
{
	sql_primary_expression*	p_sql_primary_expression = new sql_primary_expression ((sql_column*)c_sql_column);
	((sql_column*)c_sql_column)->parent (p_sql_primary_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_primary_expression_Callback (DefaultCallbackReason, 1, p_sql_primary_expression);
	return	p_sql_primary_expression;
}

void*	sql_primary_expression_2 (void* parseCtx, void* c_sql_constant)
{
	sql_primary_expression*	p_sql_primary_expression = new sql_primary_expression ((sql_constant*)c_sql_constant);
	((sql_constant*)c_sql_constant)->parent (p_sql_primary_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_primary_expression_Callback (DefaultCallbackReason, 2, p_sql_primary_expression);
	return	p_sql_primary_expression;
}

}

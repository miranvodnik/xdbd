#include "SqlCompilerCallbacks.h"
#include "sql_gt_expression.h"


sql_gt_expression::sql_gt_expression (sql_primary_expression* c_sql_primary_expression_1, char* c_char, sql_primary_expression* c_sql_primary_expression_2) : SqlSyntax (1, sql_gt_expression_Context)
{
	_init ();
	m_sql_primary_expression_1 = c_sql_primary_expression_1;
	m_char = c_char;
	m_sql_primary_expression_2 = c_sql_primary_expression_2;
}

sql_gt_expression::~sql_gt_expression ()
{
	delete	m_sql_primary_expression_1;
	delete	m_sql_primary_expression_2;
	_init ();
}

void	sql_gt_expression::_init ()
{
	m_sql_primary_expression_1 = 0;
	m_sql_primary_expression_2 = 0;
}

void sql_gt_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_gt_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_gt_expression_1:
			m_sql_primary_expression_1->Traverse (parseCtx);
			m_sql_primary_expression_2->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_gt_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_gt_expression_1 (void* parseCtx, void* c_sql_primary_expression_1, void* c_char, void* c_sql_primary_expression_2)
{
	sql_gt_expression*	p_sql_gt_expression = new sql_gt_expression ((sql_primary_expression*)c_sql_primary_expression_1, (char*)c_char, (sql_primary_expression*)c_sql_primary_expression_2);
	((sql_primary_expression*)c_sql_primary_expression_1)->parent (p_sql_gt_expression);
	((sql_primary_expression*)c_sql_primary_expression_2)->parent (p_sql_gt_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_gt_expression_Callback (DefaultCallbackReason, 1, p_sql_gt_expression);
	return	p_sql_gt_expression;
}

}

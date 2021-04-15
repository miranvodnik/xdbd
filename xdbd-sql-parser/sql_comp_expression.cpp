#include "SqlCompilerCallbacks.h"
#include "sql_comp_expression.h"


sql_comp_expression::sql_comp_expression (sql_eq_expression* c_sql_eq_expression) : SqlSyntax (1, sql_comp_expression_Context)
{
	_init ();
	m_sql_eq_expression = c_sql_eq_expression;
}

sql_comp_expression::sql_comp_expression (sql_ne_expression* c_sql_ne_expression) : SqlSyntax (2, sql_comp_expression_Context)
{
	_init ();
	m_sql_ne_expression = c_sql_ne_expression;
}

sql_comp_expression::sql_comp_expression (sql_lt_expression* c_sql_lt_expression) : SqlSyntax (3, sql_comp_expression_Context)
{
	_init ();
	m_sql_lt_expression = c_sql_lt_expression;
}

sql_comp_expression::sql_comp_expression (sql_le_expression* c_sql_le_expression) : SqlSyntax (4, sql_comp_expression_Context)
{
	_init ();
	m_sql_le_expression = c_sql_le_expression;
}

sql_comp_expression::sql_comp_expression (sql_gt_expression* c_sql_gt_expression) : SqlSyntax (5, sql_comp_expression_Context)
{
	_init ();
	m_sql_gt_expression = c_sql_gt_expression;
}

sql_comp_expression::sql_comp_expression (sql_ge_expression* c_sql_ge_expression) : SqlSyntax (6, sql_comp_expression_Context)
{
	_init ();
	m_sql_ge_expression = c_sql_ge_expression;
}

sql_comp_expression::sql_comp_expression (sql_like_expression* c_sql_like_expression) : SqlSyntax (7, sql_comp_expression_Context)
{
	_init ();
	m_sql_like_expression = c_sql_like_expression;
}

sql_comp_expression::sql_comp_expression (char* c_char_1, sql_expression* c_sql_expression, char* c_char_2) : SqlSyntax (8, sql_comp_expression_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_sql_expression = c_sql_expression;
	m_char_2 = c_char_2;
}

sql_comp_expression::sql_comp_expression (sql_is_eq_expression* c_sql_is_eq_expression) : SqlSyntax (9, sql_comp_expression_Context)
{
	_init ();
	m_sql_is_eq_expression = c_sql_is_eq_expression;
}

sql_comp_expression::sql_comp_expression (sql_is_ne_expression* c_sql_is_ne_expression) : SqlSyntax (10, sql_comp_expression_Context)
{
	_init ();
	m_sql_is_ne_expression = c_sql_is_ne_expression;
}

sql_comp_expression::~sql_comp_expression ()
{
	delete	m_sql_eq_expression;
	delete	m_sql_ne_expression;
	delete	m_sql_lt_expression;
	delete	m_sql_le_expression;
	delete	m_sql_gt_expression;
	delete	m_sql_ge_expression;
	delete	m_sql_like_expression;
	delete	m_sql_expression;
	delete	m_sql_is_eq_expression;
	delete	m_sql_is_ne_expression;
	_init ();
}

void	sql_comp_expression::_init ()
{
	m_sql_eq_expression = 0;
	m_sql_ne_expression = 0;
	m_sql_lt_expression = 0;
	m_sql_le_expression = 0;
	m_sql_gt_expression = 0;
	m_sql_ge_expression = 0;
	m_sql_like_expression = 0;
	m_sql_expression = 0;
	m_sql_is_eq_expression = 0;
	m_sql_is_ne_expression = 0;
}

void sql_comp_expression::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_comp_expression_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_comp_expression_1:
			m_sql_eq_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_2:
			m_sql_ne_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_3:
			m_sql_lt_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_4:
			m_sql_le_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_5:
			m_sql_gt_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_6:
			m_sql_ge_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_7:
			m_sql_like_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_8:
			m_sql_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_9:
			m_sql_is_eq_expression->Traverse (parseCtx);
		break;
		case	g_sql_comp_expression_10:
			m_sql_is_ne_expression->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_comp_expression_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_comp_expression_1 (void* parseCtx, void* c_sql_eq_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_eq_expression*)c_sql_eq_expression);
	((sql_eq_expression*)c_sql_eq_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 1, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_2 (void* parseCtx, void* c_sql_ne_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_ne_expression*)c_sql_ne_expression);
	((sql_ne_expression*)c_sql_ne_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 2, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_3 (void* parseCtx, void* c_sql_lt_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_lt_expression*)c_sql_lt_expression);
	((sql_lt_expression*)c_sql_lt_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 3, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_4 (void* parseCtx, void* c_sql_le_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_le_expression*)c_sql_le_expression);
	((sql_le_expression*)c_sql_le_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 4, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_5 (void* parseCtx, void* c_sql_gt_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_gt_expression*)c_sql_gt_expression);
	((sql_gt_expression*)c_sql_gt_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 5, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_6 (void* parseCtx, void* c_sql_ge_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_ge_expression*)c_sql_ge_expression);
	((sql_ge_expression*)c_sql_ge_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 6, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_7 (void* parseCtx, void* c_sql_like_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_like_expression*)c_sql_like_expression);
	((sql_like_expression*)c_sql_like_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 7, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_8 (void* parseCtx, void* c_char_1, void* c_sql_expression, void* c_char_2)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((char*)c_char_1, (sql_expression*)c_sql_expression, (char*)c_char_2);
	((sql_expression*)c_sql_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 8, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_9 (void* parseCtx, void* c_sql_is_eq_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_is_eq_expression*)c_sql_is_eq_expression);
	((sql_is_eq_expression*)c_sql_is_eq_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 9, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

void*	sql_comp_expression_10 (void* parseCtx, void* c_sql_is_ne_expression)
{
	sql_comp_expression*	p_sql_comp_expression = new sql_comp_expression ((sql_is_ne_expression*)c_sql_is_ne_expression);
	((sql_is_ne_expression*)c_sql_is_ne_expression)->parent (p_sql_comp_expression);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_comp_expression_Callback (DefaultCallbackReason, 10, p_sql_comp_expression);
	return	p_sql_comp_expression;
}

}

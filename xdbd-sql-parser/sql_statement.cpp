#include "SqlCompilerCallbacks.h"
#include "sql_statement.h"


sql_statement::sql_statement (sql_select_statement* c_sql_select_statement) : SqlSyntax (1, sql_statement_Context)
{
	_init ();
	m_sql_select_statement = c_sql_select_statement;
}

sql_statement::sql_statement (sql_update_statement* c_sql_update_statement) : SqlSyntax (2, sql_statement_Context)
{
	_init ();
	m_sql_update_statement = c_sql_update_statement;
}

sql_statement::sql_statement (sql_insert_statement* c_sql_insert_statement) : SqlSyntax (3, sql_statement_Context)
{
	_init ();
	m_sql_insert_statement = c_sql_insert_statement;
}

sql_statement::sql_statement (sql_delete_statement* c_sql_delete_statement) : SqlSyntax (4, sql_statement_Context)
{
	_init ();
	m_sql_delete_statement = c_sql_delete_statement;
}

sql_statement::sql_statement (sql_set_statement* c_sql_set_statement) : SqlSyntax (5, sql_statement_Context)
{
	_init ();
	m_sql_set_statement = c_sql_set_statement;
}

sql_statement::~sql_statement ()
{
	delete	m_sql_select_statement;
	delete	m_sql_update_statement;
	delete	m_sql_insert_statement;
	delete	m_sql_delete_statement;
	delete	m_sql_set_statement;
	_init ();
}

void	sql_statement::_init ()
{
	m_sql_select_statement = 0;
	m_sql_update_statement = 0;
	m_sql_insert_statement = 0;
	m_sql_delete_statement = 0;
	m_sql_set_statement = 0;
}

void sql_statement::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_statement_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_statement_1:
			m_sql_select_statement->Traverse (parseCtx);
		break;
		case	g_sql_statement_2:
			m_sql_update_statement->Traverse (parseCtx);
		break;
		case	g_sql_statement_3:
			m_sql_insert_statement->Traverse (parseCtx);
		break;
		case	g_sql_statement_4:
			m_sql_delete_statement->Traverse (parseCtx);
		break;
		case	g_sql_statement_5:
			m_sql_set_statement->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_statement_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_statement_1 (void* parseCtx, void* c_sql_select_statement)
{
	sql_statement*	p_sql_statement = new sql_statement ((sql_select_statement*)c_sql_select_statement);
	((sql_select_statement*)c_sql_select_statement)->parent (p_sql_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_statement_Callback (DefaultCallbackReason, 1, p_sql_statement);
	return	p_sql_statement;
}

void*	sql_statement_2 (void* parseCtx, void* c_sql_update_statement)
{
	sql_statement*	p_sql_statement = new sql_statement ((sql_update_statement*)c_sql_update_statement);
	((sql_update_statement*)c_sql_update_statement)->parent (p_sql_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_statement_Callback (DefaultCallbackReason, 2, p_sql_statement);
	return	p_sql_statement;
}

void*	sql_statement_3 (void* parseCtx, void* c_sql_insert_statement)
{
	sql_statement*	p_sql_statement = new sql_statement ((sql_insert_statement*)c_sql_insert_statement);
	((sql_insert_statement*)c_sql_insert_statement)->parent (p_sql_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_statement_Callback (DefaultCallbackReason, 3, p_sql_statement);
	return	p_sql_statement;
}

void*	sql_statement_4 (void* parseCtx, void* c_sql_delete_statement)
{
	sql_statement*	p_sql_statement = new sql_statement ((sql_delete_statement*)c_sql_delete_statement);
	((sql_delete_statement*)c_sql_delete_statement)->parent (p_sql_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_statement_Callback (DefaultCallbackReason, 4, p_sql_statement);
	return	p_sql_statement;
}

void*	sql_statement_5 (void* parseCtx, void* c_sql_set_statement)
{
	sql_statement*	p_sql_statement = new sql_statement ((sql_set_statement*)c_sql_set_statement);
	((sql_set_statement*)c_sql_set_statement)->parent (p_sql_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_statement_Callback (DefaultCallbackReason, 5, p_sql_statement);
	return	p_sql_statement;
}

}

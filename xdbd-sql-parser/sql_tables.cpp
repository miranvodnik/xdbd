#include "SqlCompilerCallbacks.h"
#include "sql_tables.h"


sql_tables::sql_tables (sql_table* c_sql_table) : SqlSyntax (1, sql_tables_Context)
{
	_init ();
	m_sql_table = c_sql_table;
}

sql_tables::sql_tables (sql_tables* c_sql_tables, char* c_char, sql_table* c_sql_table) : SqlSyntax (2, sql_tables_Context)
{
	_init ();
	m_sql_tables = c_sql_tables;
	m_char = c_char;
	m_sql_table = c_sql_table;
}

sql_tables::~sql_tables ()
{
	delete	m_sql_table;
	delete	m_sql_tables;
	_init ();
}

void	sql_tables::_init ()
{
	m_sql_table = 0;
	m_sql_tables = 0;
}

void sql_tables::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_tables_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_tables_1:
			m_sql_table->Traverse (parseCtx);
		break;
		case	g_sql_tables_2:
			m_sql_tables->Traverse (parseCtx);
			m_sql_table->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_tables_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_tables*	sql_tables::revert_sql_tables (sql_tables* p_sql_tables, u_int kind)
{
	if (this == 0)
		return	p_sql_tables;
	sql_tables*	q_sql_tables = m_sql_tables->revert_sql_tables(this, m_kind);
	m_kind = kind;
	m_sql_tables = p_sql_tables;
	return	q_sql_tables;
}

extern "C"
{

void*	sql_tables_1 (void* parseCtx, void* c_sql_table)
{
	sql_tables*	p_sql_tables = new sql_tables ((sql_table*)c_sql_table);
	((sql_table*)c_sql_table)->parent (p_sql_tables);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_tables_Callback (DefaultCallbackReason, 1, p_sql_tables);
	return	p_sql_tables;
}

void*	sql_tables_2 (void* parseCtx, void* c_sql_tables, void* c_char, void* c_sql_table)
{
	sql_tables*	p_sql_tables = new sql_tables ((sql_tables*)c_sql_tables, (char*)c_char, (sql_table*)c_sql_table);
	((sql_tables*)c_sql_tables)->parent (p_sql_tables);
	((sql_table*)c_sql_table)->parent (p_sql_tables);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_tables_Callback (DefaultCallbackReason, 2, p_sql_tables);
	return	p_sql_tables;
}

}

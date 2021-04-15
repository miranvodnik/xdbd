#include "SqlCompilerCallbacks.h"
#include "sql_tables_part.h"


sql_tables_part::sql_tables_part (sql_tables* c_sql_tables) : SqlSyntax (1, sql_tables_part_Context)
{
	_init ();
	m_sql_tables = c_sql_tables;
}

sql_tables_part::~sql_tables_part ()
{
	delete	m_sql_tables;
	_init ();
}

void	sql_tables_part::_init ()
{
	m_sql_tables = 0;
}

void sql_tables_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_tables_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_tables_part_1:
			m_sql_tables->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_tables_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_tables_part_1 (void* parseCtx, void* c_sql_tables)
{
	sql_tables_part*	p_sql_tables_part = new sql_tables_part ((sql_tables*)c_sql_tables);
	((sql_tables*)c_sql_tables)->parent (p_sql_tables_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_tables_part_Callback (DefaultCallbackReason, 1, p_sql_tables_part);
	return	p_sql_tables_part;
}

}

#include "SqlCompilerCallbacks.h"
#include "sql_table_part.h"


sql_table_part::sql_table_part (sql_table* c_sql_table) : SqlSyntax (1, sql_table_part_Context)
{
	_init ();
	m_sql_table = c_sql_table;
}

sql_table_part::~sql_table_part ()
{
	delete	m_sql_table;
	_init ();
}

void	sql_table_part::_init ()
{
	m_sql_table = 0;
}

void sql_table_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_table_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_table_part_1:
			m_sql_table->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_table_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_table_part_1 (void* parseCtx, void* c_sql_table)
{
	sql_table_part*	p_sql_table_part = new sql_table_part ((sql_table*)c_sql_table);
	((sql_table*)c_sql_table)->parent (p_sql_table_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_table_part_Callback (DefaultCallbackReason, 1, p_sql_table_part);
	return	p_sql_table_part;
}

}

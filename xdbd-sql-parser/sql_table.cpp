#include "SqlCompilerCallbacks.h"
#include "sql_table.h"


sql_table::sql_table (char* c_char) : SqlSyntax (1, sql_table_Context)
{
	_init ();
	m_char = c_char;
}

sql_table::~sql_table ()
{
	_init ();
}

void	sql_table::_init ()
{
}

void sql_table::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_table_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_table_1:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_table_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_table_1 (void* parseCtx, void* c_char)
{
	sql_table*	p_sql_table = new sql_table ((char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_table_Callback (DefaultCallbackReason, 1, p_sql_table);
	return	p_sql_table;
}

}

#include "SqlCompilerCallbacks.h"
#include "sql_columns_part.h"


sql_columns_part::sql_columns_part () : SqlSyntax (1, sql_columns_part_Context)
{
	_init ();
}

sql_columns_part::sql_columns_part (sql_columns* c_sql_columns) : SqlSyntax (2, sql_columns_part_Context)
{
	_init ();
	m_sql_columns = c_sql_columns;
}

sql_columns_part::~sql_columns_part ()
{
	delete	m_sql_columns;
	_init ();
}

void	sql_columns_part::_init ()
{
	m_sql_columns = 0;
}

void sql_columns_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_columns_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_columns_part_1:
		break;
		case	g_sql_columns_part_2:
			m_sql_columns->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_columns_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_columns_part_1 (void* parseCtx)
{
	sql_columns_part*	p_sql_columns_part = new sql_columns_part ();
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_columns_part_Callback (DefaultCallbackReason, 1, p_sql_columns_part);
	return	p_sql_columns_part;
}

void*	sql_columns_part_2 (void* parseCtx, void* c_sql_columns)
{
	sql_columns_part*	p_sql_columns_part = new sql_columns_part ((sql_columns*)c_sql_columns);
	((sql_columns*)c_sql_columns)->parent (p_sql_columns_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_columns_part_Callback (DefaultCallbackReason, 2, p_sql_columns_part);
	return	p_sql_columns_part;
}

}

#include "SqlCompilerCallbacks.h"
#include "sql_columns.h"


sql_columns::sql_columns (sql_column* c_sql_column, sql_as_part* c_sql_as_part) : SqlSyntax (1, sql_columns_Context)
{
	_init ();
	m_sql_column = c_sql_column;
	m_sql_as_part = c_sql_as_part;
}

sql_columns::sql_columns (sql_columns* c_sql_columns, char* c_char, sql_column* c_sql_column, sql_as_part* c_sql_as_part) : SqlSyntax (2, sql_columns_Context)
{
	_init ();
	m_sql_columns = c_sql_columns;
	m_char = c_char;
	m_sql_column = c_sql_column;
	m_sql_as_part = c_sql_as_part;
}

sql_columns::~sql_columns ()
{
	delete	m_sql_column;
	delete	m_sql_as_part;
	delete	m_sql_columns;
	_init ();
}

void	sql_columns::_init ()
{
	m_sql_column = 0;
	m_sql_as_part = 0;
	m_sql_columns = 0;
}

void sql_columns::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_columns_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_columns_1:
			m_sql_column->Traverse (parseCtx);
			m_sql_as_part->Traverse (parseCtx);
		break;
		case	g_sql_columns_2:
			m_sql_columns->Traverse (parseCtx);
			m_sql_column->Traverse (parseCtx);
			m_sql_as_part->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_columns_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_columns*	sql_columns::revert_sql_columns (sql_columns* p_sql_columns, u_int kind)
{
	if (this == 0)
		return	p_sql_columns;
	sql_columns*	q_sql_columns = m_sql_columns->revert_sql_columns(this, m_kind);
	m_kind = kind;
	m_sql_columns = p_sql_columns;
	return	q_sql_columns;
}

extern "C"
{

void*	sql_columns_1 (void* parseCtx, void* c_sql_column, void* c_sql_as_part)
{
	sql_columns*	p_sql_columns = new sql_columns ((sql_column*)c_sql_column, (sql_as_part*)c_sql_as_part);
	((sql_column*)c_sql_column)->parent (p_sql_columns);
	((sql_as_part*)c_sql_as_part)->parent (p_sql_columns);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_columns_Callback (DefaultCallbackReason, 1, p_sql_columns);
	return	p_sql_columns;
}

void*	sql_columns_2 (void* parseCtx, void* c_sql_columns, void* c_char, void* c_sql_column, void* c_sql_as_part)
{
	sql_columns*	p_sql_columns = new sql_columns ((sql_columns*)c_sql_columns, (char*)c_char, (sql_column*)c_sql_column, (sql_as_part*)c_sql_as_part);
	((sql_columns*)c_sql_columns)->parent (p_sql_columns);
	((sql_column*)c_sql_column)->parent (p_sql_columns);
	((sql_as_part*)c_sql_as_part)->parent (p_sql_columns);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_columns_Callback (DefaultCallbackReason, 2, p_sql_columns);
	return	p_sql_columns;
}

}

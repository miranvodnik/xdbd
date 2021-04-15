#include "SqlCompilerCallbacks.h"
#include "sql_order_by.h"


sql_order_by::sql_order_by (char* c_char_1, char* c_char_2, sql_columns* c_sql_columns) : SqlSyntax (1, sql_order_by_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_columns = c_sql_columns;
}

sql_order_by::~sql_order_by ()
{
	delete	m_sql_columns;
	_init ();
}

void	sql_order_by::_init ()
{
	m_sql_columns = 0;
}

void sql_order_by::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_order_by_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_order_by_1:
			m_sql_columns->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_order_by_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_order_by_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_columns)
{
	sql_order_by*	p_sql_order_by = new sql_order_by ((char*)c_char_1, (char*)c_char_2, (sql_columns*)c_sql_columns);
	((sql_columns*)c_sql_columns)->parent (p_sql_order_by);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_order_by_Callback (DefaultCallbackReason, 1, p_sql_order_by);
	return	p_sql_order_by;
}

}

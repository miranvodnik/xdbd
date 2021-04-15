#include "SqlCompilerCallbacks.h"
#include "sql_values_part.h"


sql_values_part::sql_values_part (sql_values* c_sql_values) : SqlSyntax (1, sql_values_part_Context)
{
	_init ();
	m_sql_values = c_sql_values;
}

sql_values_part::~sql_values_part ()
{
	delete	m_sql_values;
	_init ();
}

void	sql_values_part::_init ()
{
	m_sql_values = 0;
}

void sql_values_part::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_values_part_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_values_part_1:
			m_sql_values->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_values_part_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_values_part_1 (void* parseCtx, void* c_sql_values)
{
	sql_values_part*	p_sql_values_part = new sql_values_part ((sql_values*)c_sql_values);
	((sql_values*)c_sql_values)->parent (p_sql_values_part);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_values_part_Callback (DefaultCallbackReason, 1, p_sql_values_part);
	return	p_sql_values_part;
}

}

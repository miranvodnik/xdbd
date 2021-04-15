#include "SqlCompilerCallbacks.h"
#include "sql_values.h"


sql_values::sql_values (sql_constant* c_sql_constant) : SqlSyntax (1, sql_values_Context)
{
	_init ();
	m_sql_constant = c_sql_constant;
}

sql_values::sql_values (sql_values* c_sql_values, char* c_char, sql_constant* c_sql_constant) : SqlSyntax (2, sql_values_Context)
{
	_init ();
	m_sql_values = c_sql_values;
	m_char = c_char;
	m_sql_constant = c_sql_constant;
}

sql_values::~sql_values ()
{
	delete	m_sql_constant;
	delete	m_sql_values;
	_init ();
}

void	sql_values::_init ()
{
	m_sql_constant = 0;
	m_sql_values = 0;
}

void sql_values::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_values_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_values_1:
			m_sql_constant->Traverse (parseCtx);
		break;
		case	g_sql_values_2:
			m_sql_values->Traverse (parseCtx);
			m_sql_constant->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_values_Callback (TraversalEpilogueCallbackReason, kind, this);
}

sql_values*	sql_values::revert_sql_values (sql_values* p_sql_values, u_int kind)
{
	if (this == 0)
		return	p_sql_values;
	sql_values*	q_sql_values = m_sql_values->revert_sql_values(this, m_kind);
	m_kind = kind;
	m_sql_values = p_sql_values;
	return	q_sql_values;
}

extern "C"
{

void*	sql_values_1 (void* parseCtx, void* c_sql_constant)
{
	sql_values*	p_sql_values = new sql_values ((sql_constant*)c_sql_constant);
	((sql_constant*)c_sql_constant)->parent (p_sql_values);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_values_Callback (DefaultCallbackReason, 1, p_sql_values);
	return	p_sql_values;
}

void*	sql_values_2 (void* parseCtx, void* c_sql_values, void* c_char, void* c_sql_constant)
{
	sql_values*	p_sql_values = new sql_values ((sql_values*)c_sql_values, (char*)c_char, (sql_constant*)c_sql_constant);
	((sql_values*)c_sql_values)->parent (p_sql_values);
	((sql_constant*)c_sql_constant)->parent (p_sql_values);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_values_Callback (DefaultCallbackReason, 2, p_sql_values);
	return	p_sql_values;
}

}

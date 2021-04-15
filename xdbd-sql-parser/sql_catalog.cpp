#include "SqlCompilerCallbacks.h"
#include "sql_catalog.h"


sql_catalog::sql_catalog (char* c_char) : SqlSyntax (1, sql_catalog_Context)
{
	_init ();
	m_char = c_char;
}

sql_catalog::~sql_catalog ()
{
	_init ();
}

void	sql_catalog::_init ()
{
}

void sql_catalog::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_catalog_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_catalog_1:
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_catalog_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_catalog_1 (void* parseCtx, void* c_char)
{
	sql_catalog*	p_sql_catalog = new sql_catalog ((char*)c_char);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_catalog_Callback (DefaultCallbackReason, 1, p_sql_catalog);
	return	p_sql_catalog;
}

}

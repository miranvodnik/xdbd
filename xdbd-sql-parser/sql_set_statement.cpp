#include "SqlCompilerCallbacks.h"
#include "sql_set_statement.h"


sql_set_statement::sql_set_statement (char* c_char_1, char* c_char_2, sql_catalog* c_sql_catalog) : SqlSyntax (1, sql_set_statement_Context)
{
	_init ();
	m_char_1 = c_char_1;
	m_char_2 = c_char_2;
	m_sql_catalog = c_sql_catalog;
}

sql_set_statement::~sql_set_statement ()
{
	delete	m_sql_catalog;
	_init ();
}

void	sql_set_statement::_init ()
{
	m_sql_catalog = 0;
}

void sql_set_statement::Traverse (void* parseCtx)
{
	unsigned int kind = this->kind();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_set_statement_Callback (TraversalPrologueCallbackReason, kind, this);
	((SqlCompilerCallbacks*)parseCtx)->push(this);
	switch (kind)
	{
		case	g_sql_set_statement_1:
			m_sql_catalog->Traverse (parseCtx);
		break;
	}
	((SqlCompilerCallbacks*)parseCtx)->pop();
	((SqlCompilerCallbacks*)parseCtx)->Invoke_sql_set_statement_Callback (TraversalEpilogueCallbackReason, kind, this);
}

extern "C"
{

void*	sql_set_statement_1 (void* parseCtx, void* c_char_1, void* c_char_2, void* c_sql_catalog)
{
	sql_set_statement*	p_sql_set_statement = new sql_set_statement ((char*)c_char_1, (char*)c_char_2, (sql_catalog*)c_sql_catalog);
	((sql_catalog*)c_sql_catalog)->parent (p_sql_set_statement);
	((SqlCompilerCallbacks*) parseCtx)->Invoke_sql_set_statement_Callback (DefaultCallbackReason, 1, p_sql_set_statement);
	return	p_sql_set_statement;
}

}

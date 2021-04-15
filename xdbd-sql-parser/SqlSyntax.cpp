#include "SqlSyntax.h"

SqlSyntax::SqlSyntax (unsigned int kind, SqlSyntaxContext context)
{
	m_kind = kind;
	m_context = context;
	m_parent = 0;
	m_any = 0;
}

SqlSyntax::~SqlSyntax () {}

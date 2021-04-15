/*
 * XdbdSqlCatalogFunction.cpp
 *
 *  Created on: 4. feb. 2016
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <XdbdSqlCatalogFunction.h>

namespace xml_db_daemon
{

XdbdSqlCatalogFunction::XdbdSqlCatalogFunction(sql_statement* p_sql_statement, int ecount)
{
	Reset ();
	m_sql_statement = p_sql_statement;
}

XdbdSqlCatalogFunction::~XdbdSqlCatalogFunction()
{
}

void	XdbdSqlCatalogFunction::Reset()
{
	m_sql_statement = 0;
	m_sql_set_statement = 0;
	m_sql_catalog = 0;
	m_shmSize = 0;
	m_hdrSize = 0;
}

int XdbdSqlCatalogFunction::BindParameters (XdbdShmSegment* shmSegment)
{
	return	0;
}

int	XdbdSqlCatalogFunction::ExecuteCatalogStatement (XdbdShmSegment* shmSegment)
{
	return	0;
}

int	XdbdSqlCatalogFunction::CreatePreparedSet (XdbdShmSegment* shmSegment)
{
	int	result = -1;

	do
	{
		m_hdrSize = xdbdalign (6 * sizeof (int), long);

		unsigned char*	shmAddr = (unsigned char*) shmSegment->Resize (m_hdrSize);

		unsigned char*	addr = shmAddr;
		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_hdrSize, errno);
			break;
		}

		unsigned char*	buff = addr;
		((int*)buff) [g_shmSizeIndex] = m_hdrSize;
		((int*)buff) [g_parCountIndex] = 0;
		((int*)buff) [g_parSizeIndex] = 0;
		((int*)buff) [g_colCountIndex] = 0;
		((int*)buff) [g_colSizeIndex] = 0;
		((int*)buff) [g_rowCountIndex] = 0;

		shmSegment->hdrSize(m_hdrSize);
		result = 0;
#if defined (_DEBUG)
		dump (addr, shmsize);
#endif
	}
	while (false);

	return	result;
}

int	XdbdSqlCatalogFunction::Prepare (XdbdShmSegment* shmSegment, bool create)
{
	m_sql_statement->Traverse(this);
	if (create)
		CreatePreparedSet (shmSegment);
	return	(m_sql_catalog == 0) ? -1 : 0;
}

int	XdbdSqlCatalogFunction::Execute (XdbdShmSegment* shmSegment)
{
	return	0;
}

#define	CLASSNAME	XdbdSqlCatalogFunction

#define	sql_catalog_CallbackDefined
void	CLASSNAME::Invoke_sql_catalog_Callback (SqlCallbackReason reason, unsigned int kind, sql_catalog* p_sql_catalog)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		m_sql_catalog = p_sql_catalog;
		break;
	case	TraversalEpilogueCallbackReason:
		break;
	}
}

#include "SqlCompilerCallbackImpls.template"

} /* namespace xml_db_daemon */

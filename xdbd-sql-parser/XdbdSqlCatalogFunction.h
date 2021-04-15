/*
 * XdbdSqlCatalogFunction.h
 *
 *  Created on: 4. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdMessages.h"
#include "XdbdCommon.h"
#include "SqlCompilerCallbacks.h"
#include "sql_statement.h"
#include "XdbdXmlDataBase.h"

#include <SqlCompilerCallbacks.h>

namespace xml_db_daemon
{

class XdbdSqlCatalogFunction: public SqlCompilerCallbacks
{
public:
	XdbdSqlCatalogFunction(sql_statement* p_sql_statement, int ecount);
	virtual ~XdbdSqlCatalogFunction();

public:
	int	Prepare (XdbdShmSegment* shmSegment, bool create);
	int	Execute (XdbdShmSegment* shmSegment);

private:
	void	Reset();
	int BindParameters (XdbdShmSegment* shmSegment);
	int	ExecuteCatalogStatement (XdbdShmSegment* shmSegment);
	int	CreatePreparedSet (XdbdShmSegment* shmSegment);

#include "SqlCompilerCallbackDecls.h"

private:
	sql_statement*	m_sql_statement;
	sql_set_statement*	m_sql_set_statement;
	sql_catalog*	m_sql_catalog;
	int	m_shmSize;
	int	m_hdrSize;
};

} /* namespace xml_db_daemon */

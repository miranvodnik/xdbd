/*
 * XdbdSqlInsertFunction.h
 *
 *  Created on: 13. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdMessages.h"
#include "XdbdCommon.h"
#include "SqlCompilerCallbacks.h"
#include "sql_statement.h"
#include "XdbdXmlDataBase.h"
#include "XdbdInsertTriggerJob.h"

namespace xml_db_daemon
{

class	XdbdShmSegment;

class XdbdSqlInsertFunction: public SqlCompilerCallbacks
{
public:
	XdbdSqlInsertFunction(sql_statement* p_sql_statement, int ecount);
	virtual ~XdbdSqlInsertFunction();

public:
	int	Prepare (XdbdShmSegment* shmSegment, bool create);
	int	Execute (XdbdShmSegment* shmSegment);

private:
	void	Reset();
	int BindParameters (XdbdShmSegment* shmSegment);
	int	CreatePreparedSet (XdbdShmSegment* shmSegment);
	int	InsertResultSet (XdbdShmSegment* shmSegment);

#include "SqlCompilerCallbackDecls.h"

private:
	sql_statement*	m_sql_statement;
	sql_insert_statement*	m_sql_insert_statement;
	sql_table_part*	m_sql_table_part;
	sql_columns_part*	m_sql_columns_part;
	sql_columns*	m_sql_columns;
	sql_columns*	m_sql_columns_current;
	sql_values_part*	m_sql_values_part;
	sql_values*	m_sql_values;
	XdbdXmlTableInfo*	m_tabinfo;
	XdbdXmlTableInfo::col_iterator	m_cit;
	int	m_columnCounter;
	int	m_resultSize;
	int	m_eindex;
	compset	m_dynpars;
	int	m_shmSize;
	int	m_hdrSize;
	int	m_parCount;
	int	m_parsSize;
	int	m_colCount;
	int	m_colsSize;
	int	m_rowCount;
};

} /* namespace xml_db_daemon */

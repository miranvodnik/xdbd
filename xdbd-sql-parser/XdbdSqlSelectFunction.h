/*
 * XdbdSqlSelectFunction.h
 *
 *  Created on: 13. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sys/mman.h>
#include "XdbdMessages.h"
#include "XdbdSqlComparison.h"
#include "XdbdXmlDataBase.h"
#include "XdbdSqlResultInfo.h"
#include "SqlCompilerCallbacks.h"
#include "sql_statement.h"
#include "sql_columns.h"
#include "sql_tables.h"
#include "sql_expression.h"

#include <string>
#include <deque>
#include <vector>
#include <set>
using namespace std;

namespace xml_db_daemon
{

class	XdbdShmSegment;

class XdbdSqlSelectFunction: public SqlCompilerCallbacks
{
public:
	XdbdSqlSelectFunction(sql_statement* p_sql_statement, int ecount);
	virtual ~XdbdSqlSelectFunction();

public:
	int	Prepare (XdbdShmSegment* shmSegment, bool create);
	int	Execute (XdbdShmSegment* shmSegment);

private:
	void	Reset();
	int BindParameters (XdbdShmSegment* shmSegment);
	int	ExecuteConditionalSelectStatement (XdbdShmSegment* shmSegment);
	int	ExecuteUnconditionalSelectStatement (XdbdShmSegment* shmSegment);
	int	CreatePreparedSet (XdbdShmSegment* shmSegment);
	int	CreateResultSet (XdbdShmSegment* shmSegment, int count, set < unsigned char* > &resultSet);
	int	ExecuteCompChecks (XdbdSqlComparison* scomp, XdbdXmlColumnInfo::colrng range, unsigned long v, set < unsigned char* > & resultSet);

#include "SqlCompilerCallbackDecls.h"

private:
	sql_statement*	m_sql_statement;
	sql_select_statement*	m_sql_select_statement;
	sql_columns_part*	m_sql_columns_part;
	sql_tables_part*	m_sql_tables_part;
	sql_tables*	m_sql_tables;
	tablist	m_sqlTables;
	collist	m_sqlColumns;
	reslist	m_sqlResultInfo;
	int	m_resultSize;
	int	m_functionCount;
	int	m_ecount;
	int	m_eindex;
	cmpset	m_cmpset;
	cmpptr	m_cmpptr;
	compset	m_compset;
	compset	m_dynpars;
	int	m_shmSize;
	int	m_hdrSize;
	int	m_parCount;
	int	m_parsSize;
	int	m_colCount;
	int	m_colsSize;
	int	m_rowCount;
	int	m_chdrSize;
};

} /* namespace xml_db_daemon */

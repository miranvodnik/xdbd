/*
 * XdbdSqlDeleteFunction.h
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
#include "XdbdDeleteTriggerJob.h"

namespace xml_db_daemon
{

class	XdbdShmSegment;

class XdbdSqlDeleteFunction: public SqlCompilerCallbacks
{
public:
	XdbdSqlDeleteFunction(sql_statement* p_sql_statement, int ecount);
	virtual ~XdbdSqlDeleteFunction();

public:
	int	Prepare (XdbdShmSegment* shmSegment, bool create);
	int	Execute (XdbdShmSegment* shmSegment);

private:
	void	Reset();
	int BindParameters (XdbdShmSegment* shmSegment);
	int	ExecuteConditionalDeleteStatement (XdbdShmSegment* shmSegment);
	int	ExecuteUnconditionalDeleteStatement (XdbdShmSegment* shmSegment);
	int	CreatePreparedSet (XdbdShmSegment* shmSegment);
	int	DeleteResultSet (XdbdShmSegment* shmSegment, int count, set < unsigned char* > &resultSet);
	int	ExecuteCompChecks (XdbdSqlComparison* scomp, XdbdXmlColumnInfo::colrng range, unsigned long v, set < unsigned char* > & resultSet);

#include "SqlCompilerCallbackDecls.h"

private:
	sql_statement*	m_sql_statement;
	sql_delete_statement*	m_sql_delete_statement;
	sql_table*	m_sql_table;
	sql_expression*	m_sql_expression;
	XdbdXmlTableInfo*	m_tabinfo;
	int	m_resultSize;
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
};

} /* namespace xml_db_daemon */

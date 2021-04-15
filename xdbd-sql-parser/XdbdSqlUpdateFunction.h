/*
 * XdbdSqlUpdateFunction.h
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
#include "XdbdUpdateTriggerJob.h"

namespace xml_db_daemon
{

class	XdbdShmSegment;

class XdbdSqlUpdateFunction: public SqlCompilerCallbacks
{
public:
	XdbdSqlUpdateFunction(sql_statement* p_sql_statement, int ecount);
	virtual ~XdbdSqlUpdateFunction();

public:
	int	Prepare (XdbdShmSegment* shmSegment, bool create);
	int	Execute (XdbdShmSegment* shmSegment);

private:
	void	Reset();
	int BindParameters (XdbdShmSegment* shmSegment);
	int	ExecuteConditionalUpdateStatement (XdbdShmSegment* shmSegment);
	int	ExecuteUnconditionalUpdateStatement (XdbdShmSegment* shmSegment);
	int	CreatePreparedSet (XdbdShmSegment* shmSegment);
	int	UpdateResultSet (XdbdShmSegment* shmSegment, int count, set < unsigned char* > &resultSet);
	int	ExecuteCompChecks (XdbdSqlComparison* scomp, XdbdXmlColumnInfo::colrng range, unsigned long v, set < unsigned char* > & resultSet);

#include "SqlCompilerCallbackDecls.h"

private:
	sql_statement*	m_sql_statement;
	sql_update_statement*	m_sql_update_statement;
	sql_table*	m_sql_table;
	sql_eq_expressions_part*	m_sql_eq_expressions_part;
	XdbdXmlTableInfo*	m_tabinfo;
	int	m_resultSize;
	int	m_eindex;
	cmpset	m_cmpset;
	cmpptr	m_cmpptr;
	compset	m_compset;
	compset	m_dynpars;
	cvallist	m_cvallist;
	int	m_updCount;
	int	m_shmSize;
	int	m_hdrSize;
	int	m_parCount;
	int	m_parsSize;
	int	m_colCount;
};

} /* namespace xml_db_daemon */

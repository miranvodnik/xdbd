/*
 * XdbdSqlSelectFunction.cpp
 *
 *  Created on: 13. nov. 2015
 *      Author: miran
 */

#include <unistd.h>
#include <sqlxdbd.h>
#include <sqlext.h>
#include <sys/types.h>

#include "XdbdCommon.h"
#include "XdbdSqlSelectFunction.h"
#include <string.h>
#include <iostream>
#include <iomanip>
using namespace std;

#if defined (_DEBUG)
static	void	dump (unsigned char* addr, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if ((i % 16) == 0)
			printf ("\n");
		if ((i % 8) == 0)
			printf (" ");
		printf (" %02x", addr[i]);
	}
	printf ("\n");
}
#endif

namespace xml_db_daemon
{

XdbdSqlSelectFunction::XdbdSqlSelectFunction(sql_statement* p_sql_statement, int ecount)
{
	Reset();
	m_sql_statement = p_sql_statement;
	m_ecount = ecount;
}

XdbdSqlSelectFunction::~XdbdSqlSelectFunction()
{
	m_cmpptr.clear();
	for (compset::iterator it = m_compset.begin(); it != m_compset.end(); ++it)
	{
		delete	*it;
	}
	m_compset.clear();
	for (compset::iterator it = m_dynpars.begin(); it != m_dynpars.end(); ++it)
	{
		// delete	*it;	all XdbdSqlComparison objects are already deleted within m_compset
	}
	m_dynpars.clear();
	Reset();
}

void	XdbdSqlSelectFunction::Reset()
{
	m_sql_statement = 0;
	m_sql_select_statement = 0;
	m_sql_columns_part = 0;
	m_sql_tables_part = 0;
	m_sql_tables = 0;
	m_resultSize = 0;
	m_ecount = 0;
	m_eindex = 0;
	m_shmSize = 0;
	m_hdrSize = 0;
	m_colCount = 0;
	m_colsSize = 0;
	m_parCount = 0;
	m_parsSize = 0;
	m_rowCount = 0;
	m_chdrSize = 0;
}

int	XdbdSqlSelectFunction::Prepare (XdbdShmSegment* shmSegment, bool create)
{
	m_sql_statement->Traverse(this);
	if (create)
		CreatePreparedSet (shmSegment);
	return	((m_sql_select_statement == 0) || (m_sql_tables == 0)) ? -1 : 0;
}

int XdbdSqlSelectFunction::BindParameters (XdbdShmSegment* shmSegment)
{
	int	csize = m_compset.size();
	for (int i = 0; i < csize; ++i)
	{
		XdbdSqlComparison*	comp = m_compset[i];
		XdbdXmlTableInfo*	tabinfo = comp->tabinfo();
		if (tabinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("null table info"));
			continue;
		}
		XdbdXmlColumnInfo*	colinfo = comp->colinfo();
		if (colinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("null column info"));
			continue;
		}
		int	index = colinfo->index();
		unsigned char*	recData = colinfo->columnToRecord(comp->data());
		if (chkcolflag (recData, 2, index, COLPARVALUE) != 0)
			continue;
		unsigned char*	data = new unsigned char [tabinfo->resSize()];
		memset (data, 0, tabinfo->resSize());
		XdbdXmlColumnInfo::colrng	range = XdbdXmlDataBase::ExecComparison(colinfo, colinfo->recordToColumn(data), comp->data(), (chkcolflag (recData, 2, index, COLNULLVALUE)) ? SQL_NULL_DATA : colinfo->realLength(), comp->sqlSyntaxContext());
		comp->data(data + colinfo->position());
		comp->range(range);
	}

	unsigned char*	addr = (unsigned char*) shmSegment->shmAddr();

	addr += 6 * sizeof (int);
	unsigned char*	pars = xdbdalign (addr + (m_parCount + m_colCount) * 3 * sizeof (int), unsigned char*);
	int	offset = 0;
	for (int i = 0; (i < m_parCount); ++i, addr += 3 * sizeof (int))
	{
		int	psize = ((int*)addr)[1];
		int	plen  = ((int*)addr)[2];
		XdbdSqlComparison*	comp = m_dynpars[i];
		XdbdXmlTableInfo*	tabinfo = comp->tabinfo();
		if (tabinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("null table info"));
			continue;
		}
		XdbdXmlColumnInfo*	colinfo = comp->colinfo();
		if (colinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("null column info"));
			continue;
		}
		unsigned char*	data = new unsigned char [tabinfo->resSize()];
		memset (data, 0, tabinfo->resSize());
		XdbdXmlColumnInfo::colrng	range = XdbdXmlDataBase::ExecComparison(colinfo, data + colinfo->position(), pars + offset, plen, comp->sqlSyntaxContext());
		comp->data(data + colinfo->position());
		comp->range(range);
		shmSegment->DisplayParameter (i, pars + offset, plen, colinfo->colType());
		offset += psize;
	}
	return	0;
}

int	XdbdSqlSelectFunction::Execute (XdbdShmSegment* shmSegment)
{
	int	result = -1;

	try
	{
		for (tablist::iterator it = m_sqlTables.begin(); it != m_sqlTables.end(); ++it)
		{
			(*it)->LockTable();
		}
		BindParameters (shmSegment);
		result = (m_sql_select_statement->kind() > sql_select_statement::g_sql_select_statement_4) ?
			ExecuteUnconditionalSelectStatement (shmSegment) : ExecuteConditionalSelectStatement (shmSegment);
		for (tablist::iterator it = m_sqlTables.begin(); it != m_sqlTables.end(); ++it)
		{
			(*it)->UnlockTable();
		}
	}
	catch (...)
	{
		for (tablist::iterator it = m_sqlTables.begin(); it != m_sqlTables.end(); ++it)
		{
			(*it)->UnlockTable();
		}
	}

	return	result;
}

int	XdbdSqlSelectFunction::ExecuteConditionalSelectStatement (XdbdShmSegment* shmSegment)
{
	set < unsigned char* >	resultSet;
	int	setcount = 0;
	for (cmpset::iterator ci = m_cmpset.begin(); ci != m_cmpset.end(); ++ci, ++setcount)
	{
		XdbdSqlComparison*	scomp = 0;
		unsigned long	v = *ci;
		while (true)
		{
			unsigned long	mask = 1;
			bool	inc = false;
			for (int i = 0; i < m_eindex; ++i, mask <<= 1)
			{
				XdbdSqlComparison*	comp = m_compset[i];
				inc = comp->incIndex();
				if ((v & mask) == 0)
					continue;
				if (inc)
					continue;
				if ((scomp == 0) || (comp->resultCount() < scomp->resultCount()))
					scomp = comp;
			}
			if (scomp != 0)
				break;
		}
		if ((scomp == 0) || (scomp->resultCount() == 0))
			continue;
		XdbdXmlColumnInfo::colrng	range = scomp->range();
		if (scomp->invert())
		{
			XdbdXmlColumnInfo*	scolinfo = scomp->colinfo();
			XdbdXmlColumnInfo::colrng	rng;
			rng = XdbdXmlColumnInfo::colrng (scolinfo->begin(), range.first);
			ExecuteCompChecks (scomp, rng, v, resultSet);
			rng = XdbdXmlColumnInfo::colrng (range.second, scolinfo->end());
			ExecuteCompChecks (scomp, rng, v, resultSet);
		}
		else
			ExecuteCompChecks (scomp, range, v, resultSet);
	}

	int	count = resultSet.size();

	CreateResultSet (shmSegment, count, resultSet);

	return	0;
}

int	XdbdSqlSelectFunction::ExecuteCompChecks (XdbdSqlComparison* scomp, XdbdXmlColumnInfo::colrng range, unsigned long v, set < unsigned char* > & resultSet)
{
	for (XdbdXmlColumnInfo::colidx idx = range.first; idx != range.second; ++idx)
	{
		unsigned char*	rowaddr = idx->second;
		if (*rowaddr == 'D')
			continue;
		int	i;
		unsigned long	mask = 1;
		for (i = 0; i < m_eindex; ++i, mask <<= 1)
		{
			if ((v & mask) == 0)
				continue;
			XdbdSqlComparison*	comp = m_compset[i];
			if (comp == scomp)
				continue;
			if (comp->tabinfo() != scomp->tabinfo())
				continue;
			XdbdXmlColumnInfo*	colinfo = comp->colinfo();
			if (colinfo->isTrue())
				continue;
			unsigned char*	data = colinfo->recordToColumn(rowaddr);
			colset < XdbdXmlColumnInfo > ::cmpcol	fcomp (colinfo);
			bool	accept = false;
			switch (comp->sqlSyntaxContext())
			{
			case	sql_eq_expression_Context:
				{
					accept = (!fcomp (data, comp->data()) && !fcomp (comp->data(), data));
				}
				break;
			case	sql_ne_expression_Context:
				{
					accept = (fcomp (data, comp->data()) || fcomp (comp->data(), data));
				}
				break;
			case	sql_lt_expression_Context:
				{
					accept = (fcomp (data, comp->data()));
				}
				break;
			case	sql_le_expression_Context:
				{
					accept = (!fcomp (comp->data(), data));
				}
				break;
			case	sql_gt_expression_Context:
				{
					accept = (fcomp (comp->data(), data));
				}
				break;
			case	sql_ge_expression_Context:
				{
					accept = (!fcomp (data, comp->data()));
				}
				break;
			case	sql_like_expression_Context:
				{
					accept = true;	// (!fcomp (data, comp->data()));
				}
			case	sql_is_eq_expression_Context:
				{
					accept = (!fcomp (data, comp->data()) && !fcomp (comp->data(), data));
				}
				break;
			case	sql_is_ne_expression_Context:
				{
					accept = (fcomp (data, comp->data()) || fcomp (comp->data(), data));
				}
				break;
			default:
				break;
			}
			if (!accept)
				break;
		}
		if (i != m_eindex)
			continue;
		if (resultSet.find(rowaddr) == resultSet.end())
			resultSet.insert(rowaddr);
	}
	return	0;
}

int	XdbdSqlSelectFunction::ExecuteUnconditionalSelectStatement (XdbdShmSegment* shmSegment)
{
	int	count = 0;
	set < unsigned char* >	resultSet;

	for (tablist::iterator it = m_sqlTables.begin(); it != m_sqlTables.end(); ++it)
	{
		XdbdXmlTableInfo*	tabinfo = *it;
		XdbdXmlColumnInfo*	colinfo = *(tabinfo->col_begin());
		for (XdbdXmlColumnInfo::colidx cit = colinfo->begin(); (count < 100) && (cit != colinfo->end()); ++cit, ++count)
			resultSet.insert(cit->second);
	}

	CreateResultSet (shmSegment, count, resultSet);

	return	0;
}

int	XdbdSqlSelectFunction::CreateResultSet (XdbdShmSegment* shmSegment, int count, set < unsigned char* > &resultSet)
{
	if (((m_colCount <= 0) || (m_resultSize <= 0)) && (count <= 0))
		return	0;

	if (m_sqlResultInfo.size() <= (size_t) m_functionCount)
		count = 1;

	do
	{
		unsigned char*	addr = (unsigned char*) shmSegment->shmAddr();
		if (addr == MAP_FAILED)
			CreatePreparedSet(shmSegment);

		addr = (unsigned char*) shmSegment->shmAddr();
		if (addr == MAP_FAILED)
			break;

		m_shmSize  = m_hdrSize + count * m_resultSize;

		addr = (unsigned char*) shmSegment->Resize(m_shmSize);
		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_shmSize, errno);
			break;
		}

		unsigned char*	ptr = addr + m_hdrSize;
		memset (ptr, 0, m_resultSize);

		((int*)addr)[g_rowCountIndex] = count;
		for (set < unsigned char* > ::iterator it = resultSet.begin(); it != resultSet.end(); ++it)
		{
			int	index = 0;
			unsigned char*	baseAddr = ptr;
			unsigned char*	rowaddr = *it;
			*rowaddr = 'S';
			ptr += m_chdrSize;
			for (reslist::iterator resit = m_sqlResultInfo.begin(); resit != m_sqlResultInfo.end(); ++resit, ++index)
			{
				XdbdSqlResultInfo*	resinfo = *resit;
				XdbdXmlColumnInfo*	colinfo = resinfo->colinfo();
				switch (resinfo->getFunction())
				{
				case	XdbdSqlResultInfo::XdbdSqlNoFunction:
					if (chkcolflag (rowaddr, 2, colinfo->index(), COLNULLVALUE) != 0)
					{
						if (colinfo->nullable())
							setcolflag (baseAddr, 2, index, COLNULLABLE);
						else
							xdbdErrReport (SC_XDBD, SC_WRN, err_info ("null value of nonnullable column '%s.%s'"), resinfo->tabinfo()->name().c_str(), colinfo->name().c_str());
						setcolflag (baseAddr, 2, index, COLNULLVALUE);
					}
					else
					{
						unsigned char*	coladdr = colinfo->recordToColumn(rowaddr);
						memcpy (ptr, coladdr, colinfo->maxLength());
					}
					ptr += colinfo->realLength();
					break;
				case	XdbdSqlResultInfo::XdbdSqlCountFunction:
				case	XdbdSqlResultInfo::XdbdSqlMinFunction:
				case	XdbdSqlResultInfo::XdbdSqlMaxFunction:
					*((int*)ptr) = resultSet.size();
					ptr += xdbdalign (sizeof (int), long);
					break;
				}
			}
		}

#if defined (_DEBUG)
		dump (addr, shmSegment->shmSize());
#endif
	}
	while (false);

	return	0;
}

int	XdbdSqlSelectFunction::CreatePreparedSet (XdbdShmSegment* shmSegment)
{
	int	result = -1;

	do
	{
		m_parCount = m_dynpars.size();
		m_hdrSize = xdbdalign (((m_colCount + m_parCount) * 3 + 6) * sizeof (int), long) + xdbdalign (m_parsSize, long);
		if (m_hdrSize <= 0)
			break;

		unsigned char*	addr = (unsigned char*) shmSegment->Resize(m_hdrSize);

		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_WRN, err_info ("mmap (%d) failed, errno = %d"), m_hdrSize, errno);
			break;
		}

		unsigned char*	buff = addr;
		((int*)buff) [g_shmSizeIndex] = m_hdrSize;
		((int*)buff) [g_parCountIndex] = m_parCount;
		((int*)buff) [g_parSizeIndex] = m_parsSize;
		((int*)buff) [g_colCountIndex] = m_colCount;
		((int*)buff) [g_colSizeIndex] = m_resultSize;
		((int*)buff) [g_rowCountIndex] = 0;

		buff += 6 * sizeof (int);

		for (compset::iterator it = m_dynpars.begin(); it != m_dynpars.end(); ++it, buff += 3 * sizeof (int))
		{
			XdbdXmlColumnInfo*	colinfo = (*it)->colinfo();
			((int*)buff)[0] = (int) colinfo->colType();
			((int*)buff)[1] = (int) colinfo->realLength();
			((int*)buff)[2] = (int) colinfo->maxLength();
		}

		for (reslist::iterator it = m_sqlResultInfo.begin(); it != m_sqlResultInfo.end(); ++it, buff += 3 * sizeof (int))
		{
			XdbdSqlResultInfo*	resinfo = *it;
			XdbdXmlColumnInfo*	colinfo = resinfo->colinfo();
			switch (resinfo->getFunction())
			{
			case	XdbdSqlResultInfo::XdbdSqlNoFunction:
				((int*)buff)[0] = (int) colinfo->colType();
				((int*)buff)[1] = (int) colinfo->realLength();
				((int*)buff)[2] = (int) colinfo->maxLength();
				break;
			case	XdbdSqlResultInfo::XdbdSqlCountFunction:
			case	XdbdSqlResultInfo::XdbdSqlMinFunction:
			case	XdbdSqlResultInfo::XdbdSqlMaxFunction:
				((int*)buff)[0] = (int) SQL_INTEGER;
				((int*)buff)[1] = (int) xdbdalign (sizeof (int), long);
				((int*)buff)[2] = (int) sizeof (int);
				break;
			}
		}

		shmSegment->hdrSize(m_hdrSize);
		result = 0;
#if defined (_DEBUG)
		dump (addr, shmSegment->shmSize());
#endif
	}
	while (false);

	return	result;
}

#define	CLASSNAME	XdbdSqlSelectFunction

#define	sql_select_statement_CallbackDefined
void	CLASSNAME::Invoke_sql_select_statement_Callback (SqlCallbackReason reason, unsigned int kind, sql_select_statement* p_sql_select_statement)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		m_sql_select_statement = p_sql_select_statement;
		break;
	case	TraversalEpilogueCallbackReason:
		{
			XdbdSqlResultInfo*	p_XdbdSqlResultInfo;
			m_resultSize = 0;
			m_functionCount = 0;
			for (tablist::iterator tit = m_sqlTables.begin(); tit != m_sqlTables.end(); tit++)
			{
				XdbdXmlTableInfo*	tabinfo = *tit;
				for (vector<sql_column*>::iterator cit = m_sqlColumns.begin(); cit != m_sqlColumns.end(); cit++)
				{
					sql_column*	p_sql_column = *cit;
					sql_column_name*	p_sql_column_name = p_sql_column->get_sql_column_name();
					sql_function_name*	p_sql_function_name = p_sql_column->get_sql_function_name();
					const char*	function = "";
					if (p_sql_function_name != 0)
						function = p_sql_function_name->get_char().c_str();
					if ((p_sql_column_name->kind() == sql_column_name::g_sql_column_name_1) || (p_sql_column_name->kind() == sql_column_name::g_sql_column_name_3))
					{
						XdbdXmlColumnInfo*	colinfo = tabinfo->find (p_sql_column_name->get_char());
						if (colinfo == 0)
						{
							xdbdErrReport (SC_XDBD, SC_WRN, err_info ("unknown column name '%s.%s'"), tabinfo->name().c_str(), p_sql_column_name->get_char().c_str());
							continue;
						}
						m_sqlResultInfo.push_back (p_XdbdSqlResultInfo = new XdbdSqlResultInfo (tabinfo, colinfo, function));
						if (p_XdbdSqlResultInfo->getFunction() != XdbdSqlResultInfo::XdbdSqlNoFunction)
						{
							++m_functionCount;
							m_resultSize += xdbdalign (sizeof (int), long);
						}
						else
							m_resultSize += colinfo->realLength();
						++m_colCount;
					}
					else
					{
						for (XdbdXmlTableInfo::col_iterator it = tabinfo->col_begin(); it != tabinfo->col_end(); it = tabinfo->col_next(it))
						{
							XdbdXmlColumnInfo*	colinfo = *it;
							m_sqlResultInfo.push_back (p_XdbdSqlResultInfo = new XdbdSqlResultInfo (tabinfo, colinfo, function));
							if (p_XdbdSqlResultInfo->getFunction() != XdbdSqlResultInfo::XdbdSqlNoFunction)
							{
								++m_functionCount;
								m_resultSize += xdbdalign (sizeof (int), long);
							}
							else
								m_resultSize += colinfo->realLength();
							++m_colCount;
						}
					}
				}
			}
			m_resultSize += (m_chdrSize = xdbdalign (2 + (m_colCount + 3) / 2, long));
			m_parsSize = 0;
			for (compset::iterator it = m_dynpars.begin(); it != m_dynpars.end(); ++it)
			{
				XdbdXmlColumnInfo*	colinfo = (*it)->colinfo();
				if (colinfo == 0)
					continue;
				m_parsSize += colinfo->realLength();
			}
		}
		break;
	}
}

#define	sql_columns_part_CallbackDefined
void	CLASSNAME::Invoke_sql_columns_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_columns_part* p_sql_columns_part)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		m_sql_columns_part = p_sql_columns_part;
		break;
	case	TraversalEpilogueCallbackReason:
		m_sql_columns_part = 0;
		break;
	}
}

#define	sql_column_CallbackDefined
void	CLASSNAME::Invoke_sql_column_Callback (SqlCallbackReason reason, unsigned int kind, sql_column* p_sql_column)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		if (m_sql_columns_part != 0)
		{
			m_sqlColumns.push_back(p_sql_column);
		}
		break;
	case	TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_tables_part_CallbackDefined
void	CLASSNAME::Invoke_sql_tables_part_Callback (SqlCallbackReason reason, unsigned int kind, sql_tables_part* p_sql_tables_part)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		m_sql_tables_part = p_sql_tables_part;
		m_sql_tables = p_sql_tables_part->get_sql_tables();
		break;
	case	TraversalEpilogueCallbackReason:
		m_sql_tables_part = 0;
		break;
	}
}

#define	sql_table_CallbackDefined
void	CLASSNAME::Invoke_sql_table_Callback (SqlCallbackReason reason, unsigned int kind, sql_table* p_sql_table)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		if (m_sql_tables_part != 0)
		{
			XdbdXmlTableInfo*	tabinfo = XdbdXmlDataBase::GetTableInfo(p_sql_table->get_char());
			if (tabinfo != 0)
				m_sqlTables.push_back(tabinfo);
		}
		break;
	case	TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_or_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_or_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_or_expression* p_sql_or_expression)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		break;
	case	TraversalEpilogueCallbackReason:
		if (kind == sql_or_expression::g_sql_or_expression_2)
		{
			cmpsize	rsize = m_cmpptr.back();
			m_cmpptr.pop_back();
			cmpsize	lsize = m_cmpptr.back();
			m_cmpptr.pop_back();
			m_cmpptr.push_back(make_pair (lsize.first, rsize.second));
		}
		break;
	}
}

#define	sql_and_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_and_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_and_expression* p_sql_and_expression)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		break;
	case	TraversalEpilogueCallbackReason:
		if (kind == sql_and_expression::g_sql_and_expression_2)
		{
			cmpsize	rsize = m_cmpptr.back();
			m_cmpptr.pop_back();
			cmpsize	lsize = m_cmpptr.back();
			m_cmpptr.pop_back();
			cmpset	cset;
			for (cmpset::size_type l = lsize.first; l < lsize.second; ++l)
			{
				for (cmpset::size_type r = rsize.first; r < rsize.second; ++r)
				{
					unsigned long	v = m_cmpset[l] | m_cmpset[r];
					cset.push_back(v);
				}
			}
			for (cmpset::size_type l = lsize.first; l < lsize.second; ++l)
			{
				m_cmpset.pop_back();
			}
			for (cmpset::size_type r = rsize.first; r < rsize.second; ++r)
			{
				m_cmpset.pop_back();
			}
			cmpset::size_type	size = m_cmpset.size();
			for (cmpset::iterator it = cset.begin(); it != cset.end(); ++it)
			{
				m_cmpset.push_back(*it);
			}
			m_cmpptr.push_back(make_pair (size, m_cmpset.size()));
			cset.clear();
		}
		break;
	}
}

#define	sql_comp_expression_CallbackDefined
void	CLASSNAME::Invoke_sql_comp_expression_Callback (SqlCallbackReason reason, unsigned int kind, sql_comp_expression* p_sql_comp_expression)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		break;
	case	TraversalEpilogueCallbackReason:
		{
			if ((kind == sql_comp_expression::g_sql_comp_expression_7) ||
				(kind == sql_comp_expression::g_sql_comp_expression_8))
				break;

			XdbdXmlTableInfo*	tabinfo = 0;
			XdbdXmlColumnInfo*	colinfo = 0;
			unsigned char*	data = 0;
			ssize_t	dsize = 0;
			SqlSyntaxContext	sqlSyntaxContext = SqlFirstSyntaxContext;
			bool	invert = false;
			XdbdXmlColumnInfo::colrng	range = XdbdXmlDataBase::PrepareComparison (m_sqlTables, p_sql_comp_expression, tabinfo, colinfo, data, dsize, sqlSyntaxContext, invert);
			XdbdSqlComparison*	comp;
			if (colinfo != 0)
			{
				if (data != 0)
					comp = new XdbdSqlComparison (range, tabinfo, colinfo, colinfo->recordToColumn (data), dsize, sqlSyntaxContext, invert);
				else
					comp = new XdbdSqlComparison (range, tabinfo, colinfo, 0, dsize, sqlSyntaxContext, invert);
			}
			else
			{
				if (data != 0) delete [] data;
				comp = new XdbdSqlComparison (range, tabinfo, colinfo, 0, dsize, sqlSyntaxContext, invert);
			}
			m_compset.push_back (comp);
			if ((((ssize_t)sql_constant::g_sql_constant_4) + dsize) == 0)
				m_dynpars.push_back (comp);

			cmpset::size_type	size = m_cmpset.size();
			unsigned long	v = 0;
			v |= (1 << m_eindex++);
			m_cmpset.push_back(v);
			m_cmpptr.push_back(make_pair(size, size + 1));
		}
		break;
	}
}

#include "SqlCompilerCallbackImpls.template"

} /* namespace xml_db_daemon */

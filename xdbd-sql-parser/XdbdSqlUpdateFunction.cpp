/*
 * XdbdSqlUpdateFunction.cpp
 *
 *  Created on: 13. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "XdbdSqlUpdateFunction.h"
#include "XdbdMessageQueue.h"

namespace xml_db_daemon
{

XdbdSqlUpdateFunction::XdbdSqlUpdateFunction (sql_statement* p_sql_statement, int ecount)
{
	Reset ();
	m_sql_statement = p_sql_statement;
}

XdbdSqlUpdateFunction::~XdbdSqlUpdateFunction ()
{
	m_cmpptr.clear ();
	for (compset::iterator it = m_compset.begin (); it != m_compset.end (); ++it)
	{
		delete *it;
	}
	m_compset.clear ();
	int i = 0;
	for (compset::iterator it = m_dynpars.begin (); it != m_dynpars.end (); ++it, ++i)
	{
		if (i < m_updCount)
			delete *it;	// all XdbdSqlComparison objects are already deleted within m_compset
	}
	m_dynpars.clear ();
	Reset ();
}

void XdbdSqlUpdateFunction::Reset ()
{
	m_sql_statement = 0;
	m_sql_update_statement = 0;
	m_sql_table = 0;
	m_sql_eq_expressions_part = 0;
	m_tabinfo = 0;
	m_eindex = 0;
	m_shmSize = 0;
	m_hdrSize = 0;
	m_updCount = 0;
	m_parCount = 0;
	m_parsSize = 0;
	m_colCount = 0;
}

int XdbdSqlUpdateFunction::Prepare (XdbdShmSegment* shmSegment, bool create)
{
	m_sql_statement->Traverse (this);
	if (create)
		CreatePreparedSet (shmSegment);
	return ((m_tabinfo == 0) || (m_sql_table == 0)) ? -1 : 0;
}

int XdbdSqlUpdateFunction::BindParameters (XdbdShmSegment* shmSegment)
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
		if (chkcolflag (colinfo->columnToRecord(comp->data()), 2, colinfo->index(), COLPARVALUE) != 0)
			continue;
		unsigned char*	data = new unsigned char [tabinfo->resSize()];
		memset (data, 0, tabinfo->resSize());
		XdbdXmlColumnInfo::colrng	range = XdbdXmlDataBase::ExecComparison(colinfo, colinfo->recordToColumn(data), comp->data(), colinfo->realLength(), comp->sqlSyntaxContext());
		comp->data(data + colinfo->position());
		comp->range(range);
	}

	unsigned char* addr = (unsigned char*) shmSegment->shmAddr ();

	addr += 6 * sizeof(int);
	unsigned char* pars = xdbdalign(addr + (m_parCount + m_colCount) * 3 * sizeof(int), unsigned char*);
	int offset = 0;
	for (int i = 0; (i < m_parCount); ++i, addr += 3 * sizeof(int))
	{
		int psize = ((int*) addr) [1];
		int plen = ((int*) addr) [2];
		XdbdSqlComparison* comp = m_dynpars [i];
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
		if (i < m_updCount)
		{
			if (plen != SQL_NULL_DATA)
				memcpy (data + colinfo->position(), pars + offset, plen);
			else
				setcolflag(data, 2, colinfo->index(), COLNULLVALUE);
			comp->data (data + colinfo->position());
			comp->range (XdbdXmlColumnInfo::colrng ());
		}
		else
		{
			XdbdXmlColumnInfo::colrng	range = XdbdXmlDataBase::ExecComparison(colinfo, data + colinfo->position(), pars + offset, plen, comp->sqlSyntaxContext());
			comp->data (data + colinfo->position());
			comp->range (range);
		}
		shmSegment->DisplayParameter(i, pars + offset, plen, colinfo->colType());
		offset += psize;
	}
	return	0;
}

int XdbdSqlUpdateFunction::Execute (XdbdShmSegment* shmSegment)
{
	int	result = -1;
	if (m_tabinfo == 0)
		return	-1;

	try
	{
		m_tabinfo->LockTable();
		BindParameters (shmSegment);
		result = (m_sql_update_statement->kind () > sql_update_statement::g_sql_update_statement_2) ?
			ExecuteUnconditionalUpdateStatement (shmSegment) : ExecuteConditionalUpdateStatement (shmSegment);
		m_tabinfo->UnlockTable();
	}
	catch (...)
	{
		m_tabinfo->UnlockTable();
	}

	return result;
}

int XdbdSqlUpdateFunction::ExecuteConditionalUpdateStatement (XdbdShmSegment* shmSegment)
{
	set < unsigned char* > resultSet;
	int setcount = 0;
	for (cmpset::iterator ci = m_cmpset.begin (); ci != m_cmpset.end (); ++ci, ++setcount)
	{
		XdbdSqlComparison* scomp = 0;
		unsigned long v = *ci;
		while (true)
		{
			unsigned long mask = 1;
			bool inc = false;
			for (int i = 0; i < m_eindex; ++i, mask <<= 1)
			{
				XdbdSqlComparison* comp = m_compset [i];
				inc = comp->incIndex ();
				if ((v & mask) == 0)
					continue;
				if (inc)
					continue;
				if ((scomp == 0) || (comp->resultCount () < scomp->resultCount ()))
					scomp = comp;
			}
			if (scomp != 0)
				break;
		}
		if ((scomp == 0) || (scomp->resultCount () == 0))
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

	int count = resultSet.size ();

	UpdateResultSet (shmSegment, count, resultSet);
	return 0;
}

int	XdbdSqlUpdateFunction::ExecuteCompChecks (XdbdSqlComparison* scomp, XdbdXmlColumnInfo::colrng range, unsigned long v, set < unsigned char* > & resultSet)
{
	for (XdbdXmlColumnInfo::colidx idx = range.first; idx != range.second; ++idx)
	{
		unsigned char* rowaddr = idx->second;
		if (*rowaddr == 'D')
			continue;
		int i;
		unsigned long mask = 1;
		for (i = 0; i < m_eindex; ++i, mask <<= 1)
		{
			if ((v & mask) == 0)
				continue;
			XdbdSqlComparison* comp = m_compset [i];
			if (comp == scomp)
				continue;
			if (comp->tabinfo () != scomp->tabinfo ())
				continue;
			XdbdXmlColumnInfo* colinfo = comp->colinfo ();
			if (colinfo->isTrue())
				continue;
			unsigned char* data = colinfo->recordToColumn (rowaddr);
			colset < XdbdXmlColumnInfo > ::cmpcol fcomp (colinfo);
			bool accept = false;
			switch (comp->sqlSyntaxContext ())
			{
			case sql_eq_expression_Context:
				{
					accept = (!fcomp (data, comp->data ()) && !fcomp (comp->data (), data));
				}
				break;
			case sql_ne_expression_Context:
				{
					accept = (fcomp (data, comp->data ()) || fcomp (comp->data (), data));
				}
				break;
			case sql_lt_expression_Context:
				{
					accept = (fcomp (data, comp->data ()));
				}
				break;
			case sql_le_expression_Context:
				{
					accept = (!fcomp (comp->data (), data));
				}
				break;
			case sql_gt_expression_Context:
				{
					accept = (fcomp (comp->data (), data));
				}
				break;
			case sql_ge_expression_Context:
				{
					accept = (!fcomp (data, comp->data ()));
				}
				break;
			case sql_like_expression_Context:
				{
					accept = true; // (!fcomp (data, comp->data()));
				}
				break;
			case sql_is_eq_expression_Context:
				{
					accept = (!fcomp (data, comp->data ()) && !fcomp (comp->data (), data));
				}
				break;
			case sql_is_ne_expression_Context:
				{
					accept = (fcomp (data, comp->data ()) || fcomp (comp->data (), data));
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
		if (resultSet.find (rowaddr) == resultSet.end ())
			resultSet.insert (rowaddr);
	}
	return	0;
}

int XdbdSqlUpdateFunction::ExecuteUnconditionalUpdateStatement (XdbdShmSegment* shmSegment)
{
	int count = 0;
	set < unsigned char* > resultSet;

	for (XdbdXmlTableInfo::col_iterator cit = m_tabinfo->col_begin(); (count < 100) && (cit != m_tabinfo->col_end()); cit++)
	{
		XdbdXmlColumnInfo*	colinfo = *cit;
		for (XdbdXmlColumnInfo::colidx idx = colinfo->begin(); (count < 100) && (idx != colinfo->end()); ++count, ++idx)
		{
			resultSet.insert (idx->second);
		}
	}

	UpdateResultSet (shmSegment, count, resultSet);
	return 0;
}

int XdbdSqlUpdateFunction::CreatePreparedSet (XdbdShmSegment* shmSegment)
{
	int result = -1;

	do
	{
		m_parCount = m_dynpars.size ();
		m_colCount = m_cvallist.size ();

		m_hdrSize = xdbdalign(((m_colCount + m_parCount) * 3 + 6) * sizeof(int),
			long) + xdbdalign(m_parsSize + m_resultSize, long);
		if (m_hdrSize <= 0)
			break;

		unsigned char* shmAddr = (unsigned char*) shmSegment->Resize(m_hdrSize);

		unsigned char* addr = shmAddr;
		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_hdrSize, errno);
			break;
		}

		unsigned char* buff = addr;
		((int*) buff) [g_shmSizeIndex] = m_hdrSize;
		((int*) buff) [g_parCountIndex] = m_parCount;
		((int*) buff) [g_parSizeIndex] = m_parsSize;
		((int*) buff) [g_colCountIndex] = m_colCount;
		((int*) buff) [g_colSizeIndex] = m_resultSize;
		((int*) buff) [g_rowCountIndex] = 0;

		buff += 6 * sizeof(int);

		for (compset::iterator it = m_dynpars.begin (); it != m_dynpars.end (); ++it, buff += 3 * sizeof(int))
		{
			XdbdXmlColumnInfo* colinfo = (*it)->colinfo ();
			((int*) buff) [0] = (int) colinfo->colType ();
			((int*) buff) [1] = (int) colinfo->realLength ();
			((int*) buff) [2] = (int) colinfo->maxLength ();
		}

		for (cvallist::iterator it = m_cvallist.begin (); it != m_cvallist.end (); ++it, buff += 3 * sizeof(int))
		{
			XdbdXmlColumnInfo* colinfo = it->first;
			((int*) buff) [0] = (int) colinfo->colType ();
			((int*) buff) [1] = (int) colinfo->realLength ();
			((int*) buff) [2] = (int) colinfo->maxLength ();
		}

		shmSegment->hdrSize (m_hdrSize);
		result = 0;
#if defined (_DEBUG)
		dump (addr, shmsize);
#endif
	}
	while (false);

	return result;
}

int XdbdSqlUpdateFunction::UpdateResultSet (XdbdShmSegment* shmSegment, int count, set < unsigned char* > &resultSet)
{
	do
	{
		int rescount = m_cvallist.size ();

		if ((rescount <= 0) || (m_resultSize <= 0))
			break;

		unsigned char* addr = (unsigned char*) shmSegment->shmAddr ();
		if (addr == MAP_FAILED)
			CreatePreparedSet (shmSegment);

		addr = (unsigned char*) shmSegment->shmAddr ();
		if (addr == MAP_FAILED)
			break;

		m_shmSize = m_hdrSize + count * m_resultSize;

		addr = (unsigned char*) shmSegment->Resize(m_shmSize);
		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_shmSize, errno);
			break;
		}

		int index = 0;
		int ressize = m_tabinfo->resSize ();
		unsigned char* rowdata = (unsigned char*) malloc (ressize);
		memset (rowdata, 0, ressize);
		for (cvallist::iterator it = m_cvallist.begin (); it != m_cvallist.end (); ++it)
		{
			XdbdXmlColumnInfo* colinfo = it->first;
			sql_constant* p_sql_constant = it->second;
			unsigned char* coladdr = colinfo->recordToColumn (rowdata);
			char* dataAddr;

			clrcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
			if (p_sql_constant->kind () == sql_constant::g_sql_constant_3)	// NULL valued parameter
			{
				if (colinfo->nullable())
					setcolflag(rowdata, 2, colinfo->index(), COLNULLABLE);
				else
					xdbdErrReport (SC_XDBD, SC_WRN, err_info ("setting null value to non-nullable column '%s.%s'"), m_tabinfo->name().c_str(), colinfo->name().c_str());
				setcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
			}
			else if (p_sql_constant->kind () == sql_constant::g_sql_constant_4)	// dynamically valued parameter '?'
			{
				dataAddr = (char*) m_dynpars [index++]->data ();
				if (chkcolflag(colinfo->columnToRecord((unsigned char*) dataAddr), 2, colinfo->index(), COLNULLVALUE))
					setcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
				else
				switch (colinfo->colType ())
				{
				case SQL_SMALLINT:
					*((SQLSMALLINT*) coladdr) = *((SQLSMALLINT*) dataAddr);
					break;
				case SQL_INTEGER:
					if (sizeof(int) != sizeof(long))
					{
						switch (colinfo->maxLength ())
						{
						case sizeof(int):
							*((SQLINTEGER*) coladdr) = *((SQLINTEGER*) dataAddr);
							break;
						case sizeof(long):
							*((SQLLEN*) coladdr) = *((SQLLEN*) dataAddr);
							break;
						}
					}
					else
						*((SQLINTEGER*) coladdr) = *((SQLINTEGER*) dataAddr);
					break;
				case SQL_VARCHAR:
					memcpy ((SQLVARCHAR*) coladdr, dataAddr, colinfo->maxLength ());
					break;
				}
			}
			else	// explicitly valued parameter
			{
				string dataStr = p_sql_constant->get_char ();
				char* dataAddr = (char*) dataStr.c_str ();
				if (chkcolflag(colinfo->columnToRecord((unsigned char*) dataAddr), 2, colinfo->index(), COLNULLVALUE))
					setcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
				else
				switch (colinfo->colType ())
				{
				case SQL_SMALLINT:
					*((SQLSMALLINT*) coladdr) = (SQLSMALLINT) strtol (dataAddr, 0, 0);
					break;
				case SQL_INTEGER:
					if (sizeof(int) != sizeof(long))
					{
						switch (colinfo->maxLength ())
						{
						case sizeof(int):
							*((SQLINTEGER*) coladdr) = (SQLINTEGER) strtol (dataAddr, 0, 0);
							break;
						case sizeof(long):
							*((SQLLEN*) coladdr) = (SQLLEN) strtol (dataAddr, 0, 0);
							break;
						}
					}
					else
						*((SQLINTEGER*) coladdr) = (SQLINTEGER) strtol (dataAddr, 0, 0);
					break;
				case SQL_VARCHAR:
					memcpy ((SQLVARCHAR*) coladdr, dataAddr, colinfo->maxLength ());
					break;
				}
			}
		}

		unsigned char* ptr = addr + m_hdrSize;
		for (set < unsigned char* >::iterator it = resultSet.begin (); it != resultSet.end (); ++it)
		{
			int index = 0;
			unsigned char* baseAddr = ptr;
			unsigned char* rowaddr = *it;
			unsigned char* oldAndNewData = (unsigned char*) malloc (2 * ressize);
			memcpy (oldAndNewData, rowaddr, ressize);
			for (cvallist::iterator resit = m_cvallist.begin (); resit != m_cvallist.end (); ++resit, ++index)
			{
				XdbdXmlColumnInfo* colinfo = resit->first;
				colinfo->update (rowaddr, rowdata);
				if (chkcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE) != 0)
				{
					if (colinfo->nullable())
					{
						setcolflag(baseAddr, 2, colinfo->index(), COLNULLABLE);
						setcolflag(rowaddr, 2, colinfo->index(), COLNULLABLE);
					}
					setcolflag(baseAddr, 2, colinfo->index(), COLNULLVALUE);
					setcolflag(rowaddr, 2, colinfo->index(), COLNULLVALUE);
				}
				else
				{
					unsigned char* coladdr = colinfo->recordToColumn (rowaddr);
					memcpy (ptr, coladdr, colinfo->maxLength ());
					clrcolflag(baseAddr, 2, colinfo->index(), COLNULLVALUE);
					clrcolflag(rowaddr, 2, colinfo->index(), COLNULLVALUE);
				}
				ptr += colinfo->realLength ();
			}
			memcpy (rowaddr, "UU", sizeof(short));
			memcpy (oldAndNewData + ressize, rowaddr, ressize);
			if (XdbdXmlDataBase::TriggerStatus())
				XdbdMessageQueue::Put (new XdbdUpdateTriggerJob (m_tabinfo, oldAndNewData, ressize));
			else
				XdbdXmlDataBase::SaveTrgJob (new XdbdUpdateTriggerJob (m_tabinfo, oldAndNewData, ressize));
		}
		free (rowdata);
#if defined (_DEBUG)
		dump (addr, shmsize);
#endif
	}
	while (false);
	return 0;
}

#define	CLASSNAME	XdbdSqlUpdateFunction

#define	sql_update_statement_CallbackDefined
void CLASSNAME::Invoke_sql_update_statement_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_update_statement* p_sql_update_statement)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		m_updCount = 0;
		m_sql_update_statement = p_sql_update_statement;
		break;
	case TraversalEpilogueCallbackReason:
		{
			m_parsSize = 0;
			for (compset::iterator it = m_dynpars.begin (); it != m_dynpars.end (); ++it)
			{
				XdbdXmlColumnInfo* colinfo = (*it)->colinfo ();
				if (colinfo == 0)
					continue;
				m_parsSize += colinfo->realLength ();
			}
			m_resultSize = 0;
			for (cvallist::iterator cit = m_cvallist.begin (); cit != m_cvallist.end (); cit++)
			{
				XdbdXmlColumnInfo* colinfo = cit->first;
				m_resultSize += colinfo->realLength ();
			}
		}
		break;
	}
}

#define	sql_table_part_CallbackDefined
void CLASSNAME::Invoke_sql_table_part_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_table_part* p_sql_table_part)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		{
			m_sql_table = p_sql_table_part->get_sql_table ();
			m_tabinfo = XdbdXmlDataBase::GetTableInfo (m_sql_table->get_char ());
		}
		break;
	case TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_eq_expressions_part_CallbackDefined
void CLASSNAME::Invoke_sql_eq_expressions_part_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_eq_expressions_part* p_sql_eq_expressions_part)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		{
			m_sql_eq_expressions_part = p_sql_eq_expressions_part;
		}
		break;
	case TraversalEpilogueCallbackReason:
		{
			m_sql_eq_expressions_part = 0;
		}
		break;
	}
}

#define	sql_or_expression_CallbackDefined
void CLASSNAME::Invoke_sql_or_expression_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_or_expression* p_sql_or_expression)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		break;
	case TraversalEpilogueCallbackReason:
		if (kind == sql_or_expression::g_sql_or_expression_2)
		{
			cmpsize rsize = m_cmpptr.back ();
			m_cmpptr.pop_back ();
			cmpsize lsize = m_cmpptr.back ();
			m_cmpptr.pop_back ();
			m_cmpptr.push_back (make_pair (lsize.first, rsize.second));
		}
		break;
	}
}

#define	sql_and_expression_CallbackDefined
void CLASSNAME::Invoke_sql_and_expression_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_and_expression* p_sql_and_expression)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		break;
	case TraversalEpilogueCallbackReason:
		if (kind == sql_and_expression::g_sql_and_expression_2)
		{
			cmpsize rsize = m_cmpptr.back ();
			m_cmpptr.pop_back ();
			cmpsize lsize = m_cmpptr.back ();
			m_cmpptr.pop_back ();
			cmpset cset;
			for (cmpset::size_type l = lsize.first; l < lsize.second; ++l)
			{
				for (cmpset::size_type r = rsize.first; r < rsize.second; ++r)
				{
					unsigned long v = m_cmpset [l] | m_cmpset [r];
					cset.push_back (v);
				}
			}
			for (cmpset::size_type l = lsize.first; l < lsize.second; ++l)
			{
				m_cmpset.pop_back ();
			}
			for (cmpset::size_type r = rsize.first; r < rsize.second; ++r)
			{
				m_cmpset.pop_back ();
			}
			cmpset::size_type size = m_cmpset.size ();
			for (cmpset::iterator it = cset.begin (); it != cset.end (); ++it)
			{
				m_cmpset.push_back (*it);
			}
			m_cmpptr.push_back (make_pair (size, m_cmpset.size ()));
			cset.clear ();
		}
		break;
	}
}

#define	sql_comp_expression_CallbackDefined
void CLASSNAME::Invoke_sql_comp_expression_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_comp_expression* p_sql_comp_expression)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		break;
	case TraversalEpilogueCallbackReason:
		if (m_sql_eq_expressions_part == 0)
		{
			if (kind > sql_comp_expression::g_sql_comp_expression_7)
				break;

			XdbdXmlColumnInfo* colinfo = 0;
			unsigned char* data = 0;
			ssize_t dsize = 0;
			SqlSyntaxContext sqlSyntaxContext = SqlFirstSyntaxContext;
			bool	invert = false;
			XdbdXmlColumnInfo::colrng range = XdbdXmlDataBase::PrepareComparison (m_tabinfo, p_sql_comp_expression,
				colinfo, data, dsize, sqlSyntaxContext, invert);
			XdbdSqlComparison* comp = new XdbdSqlComparison (range, m_tabinfo, colinfo, (colinfo == 0) ? 0 : data + colinfo->position(), dsize, sqlSyntaxContext, invert);
			m_compset.push_back (comp);
			if ((((ssize_t) sql_constant::g_sql_constant_4) + dsize) == 0)
				m_dynpars.push_back (comp);

			cmpset::size_type size = m_cmpset.size ();
			unsigned long v = 0;
			v |= (1 << m_eindex++);
			m_cmpset.push_back (v);
			m_cmpptr.push_back (make_pair (size, size + 1));
		}
		break;
	}
}

#define	sql_eq_expression_CallbackDefined
void CLASSNAME::Invoke_sql_eq_expression_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_eq_expression* p_sql_eq_expression)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		break;
	case TraversalEpilogueCallbackReason:
		if (m_sql_eq_expressions_part != 0)
		{
			sql_primary_expression* p_sql_primary_expression_1 = p_sql_eq_expression->get_sql_primary_expression_1 ();
			sql_primary_expression* p_sql_primary_expression_2 = p_sql_eq_expression->get_sql_primary_expression_2 ();
			if (p_sql_primary_expression_1->kind () != sql_primary_expression::g_sql_primary_expression_1)
				break;
			if (p_sql_primary_expression_2->kind () != sql_primary_expression::g_sql_primary_expression_2)
				break;
			sql_column* p_sql_column = p_sql_primary_expression_1->get_sql_column ();
			sql_constant* p_sql_constant = p_sql_primary_expression_2->get_sql_constant ();
			sql_table* p_sql_table = p_sql_column->get_sql_table ();
			if (p_sql_table != 0)
			{
				if (XdbdXmlDataBase::GetTableInfo (m_sql_table->get_char ()) != m_tabinfo)
					break;
			}
			sql_column_name* p_sql_column_name = p_sql_column->get_sql_column_name ();
			XdbdXmlColumnInfo* colinfo = m_tabinfo->find (p_sql_column_name->get_char ());
			if (colinfo == 0)
				break;
			m_cvallist.push_back (make_pair (colinfo, p_sql_constant));
			if (p_sql_constant->kind () == sql_constant::g_sql_constant_4)
			{
				++m_updCount;
				m_dynpars.push_back (
					new XdbdSqlComparison (XdbdXmlColumnInfo::colrng (), m_tabinfo, colinfo, 0, colinfo->maxLength (),
						p_sql_eq_expression->context (), false));
			}
		}
		break;
	}
}

#include "SqlCompilerCallbackImpls.template"

} /* namespace xml_db_daemon */

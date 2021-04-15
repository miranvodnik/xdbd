/*
 * XdbdSqlInsertFunction.cpp
 *
 *  Created on: 13. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "XdbdSqlInsertFunction.h"
#include "XdbdMessageQueue.h"

namespace xml_db_daemon
{

XdbdSqlInsertFunction::XdbdSqlInsertFunction (sql_statement* p_sql_statement, int ecount)
{
	Reset ();
	m_sql_statement = p_sql_statement;
}

XdbdSqlInsertFunction::~XdbdSqlInsertFunction ()
{
	for (compset::iterator it = m_dynpars.begin (); it != m_dynpars.end (); ++it)
	{
		delete *it;
	}
	m_dynpars.clear ();
	Reset ();
}

void XdbdSqlInsertFunction::Reset ()
{
	m_sql_statement = 0;
	m_sql_insert_statement = 0;
	m_sql_table_part = 0;
	m_sql_columns_part = 0;
	m_sql_columns = 0;
	m_sql_columns_current = 0;
	m_sql_values_part = 0;
	m_sql_values = 0;
	m_tabinfo = 0;
	m_columnCounter = 0;
	m_resultSize = 0;
	m_eindex = 0;
	m_shmSize = 0;
	m_hdrSize = 0;
	m_parCount = 0;
	m_parsSize = 0;
	m_colCount = 0;
	m_colsSize = 0;
	m_rowCount = 0;
}

int XdbdSqlInsertFunction::Prepare (XdbdShmSegment* shmSegment, bool create)
{
	try
	{
		m_sql_statement->Traverse (this);
		if (create)
			CreatePreparedSet (shmSegment);
	}
	catch (int)
	{
		return -1;
	}
	return ((m_sql_table_part == 0) || (m_sql_values_part == 0)) ? -1 : 0;
}

int XdbdSqlInsertFunction::BindParameters (XdbdShmSegment* shmSegment)
{
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
		unsigned char* data = new unsigned char [tabinfo->resSize()];
		memset (data, 0, tabinfo->resSize());
		if (plen != SQL_NULL_DATA)
			memcpy (data + colinfo->position(), pars + offset, plen);
		else
			setcolflag(data, 2, colinfo->index(), COLNULLVALUE);
		comp->data (data + colinfo->position());
		comp->range (XdbdXmlColumnInfo::colrng ());
		shmSegment->DisplayParameter(i, pars + offset, plen, colinfo->colType());
		offset += psize;
	}
	return	0;
}

int XdbdSqlInsertFunction::Execute (XdbdShmSegment* shmSegment)
{
	int	result = -1;
	if (m_tabinfo == 0)
		return	result;

	try
	{
		m_tabinfo->LockTable();
		BindParameters (shmSegment);
		result = InsertResultSet (shmSegment);
		m_tabinfo->UnlockTable();
	}
	catch (...)
	{
		m_tabinfo->UnlockTable();
	}

	return result;
}

int XdbdSqlInsertFunction::CreatePreparedSet (XdbdShmSegment* shmSegment)
{
	int result = -1;

	do
	{
		m_parCount = m_dynpars.size ();
		m_colCount = m_tabinfo->resCount ();

		m_hdrSize = xdbdalign(((m_colCount + m_parCount) * 3 + 6) * sizeof(int),
			long) + xdbdalign (m_parsSize + m_resultSize, long);
		if (m_hdrSize <= 0)
			break;

		unsigned char* addr = (unsigned char*) shmSegment->Resize (m_hdrSize);

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

		for (XdbdXmlTableInfo::col_iterator it = m_tabinfo->col_begin (); it != m_tabinfo->col_end ();
			++it, buff += 3 * sizeof(int))
		{
			XdbdXmlColumnInfo* colinfo = *it;
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

int XdbdSqlInsertFunction::InsertResultSet (XdbdShmSegment* shmSegment)
{
	do
	{
		unsigned char* addr = (unsigned char*) shmSegment->shmAddr ();
		if (addr == MAP_FAILED)
			CreatePreparedSet (shmSegment);

		addr = (unsigned char*) shmSegment->shmAddr ();
		if (addr == MAP_FAILED)
			break;

		m_shmSize = m_hdrSize + m_resultSize;

		addr = (unsigned char*) shmSegment->Resize (m_shmSize);
		if (addr == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_shmSize, errno);
			break;
		}

		unsigned char* rowdata = m_tabinfo->getFreeCol ();
		if (rowdata == 0)
			break;

		int index = 0;

		if (m_sql_columns_part == 0)
		{
			m_cit = m_tabinfo->col_begin ();
		}
		else
		{
			m_sql_columns_current = m_sql_columns;
		}

		memset (rowdata, 0, m_tabinfo->resSize());

		// set column values for all columns in SQL statement
		for (sql_values* it = m_sql_values; it != 0; it = it->get_sql_values ())
		{
			XdbdXmlColumnInfo* colinfo = 0;
			sql_constant* p_sql_constant = it->get_sql_constant ();

			if (m_sql_columns_part == 0)
			{
				if (m_cit == m_tabinfo->col_end ())
					break;
				colinfo = *m_cit;
				++m_cit;
			}
			else
			{
				if (m_sql_columns_current == 0)
					break;
				colinfo = m_tabinfo->find (
					m_sql_columns_current->get_sql_column ()->get_sql_column_name ()->get_char ());
				m_sql_columns_current = m_sql_columns_current->get_sql_columns ();
			}

			if (colinfo == 0)
				continue;

			unsigned char* coladdr = colinfo->recordToColumn (rowdata);
			char* dataAddr;
			if (p_sql_constant->kind () == sql_constant::g_sql_constant_3)	// NULL valued parameter
			{
				if (colinfo->nullable ())
					setcolflag(rowdata, 2, colinfo->index(), COLNULLABLE);
			}
			else	if (p_sql_constant->kind () == sql_constant::g_sql_constant_4)	// dynamically valued parameter '?'
			{
				dataAddr = (char*) m_dynpars [index++]->data ();
				if (chkcolflag(colinfo->columnToRecord((unsigned char*) dataAddr), 2, colinfo->index(), COLNULLVALUE))
					setcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
				else
				{
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
					clrcolflag (rowdata, 2, colinfo->index(), COLNULLVALUE);
				}
			}
			else	// explicitly valued parameter
			{
				string dataStr = p_sql_constant->get_char ();
				char* dataAddr = (char*) dataStr.c_str ();
				if (chkcolflag(colinfo->columnToRecord((unsigned char*) dataAddr), 2, colinfo->index(), COLNULLVALUE))
					setcolflag(rowdata, 2, colinfo->index(), COLNULLVALUE);
				else
				{
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
					clrcolflag (rowdata, 2, colinfo->index(), COLNULLVALUE);
				}
			}
			colinfo->insert (rowdata);
		}

		// set column values for all columns not set with SQL statement
		for (XdbdXmlTableInfo::col_iterator cit = m_tabinfo->col_begin(); cit != m_tabinfo->col_end(); ++cit)
		{
			XdbdXmlColumnInfo* colinfo = *cit;
			if (chkcolflag (rowdata, 2, colinfo->index(), COLNULLABLE))
				continue;	// nullable column set NULL value
			if (!chkcolflag (rowdata, 2, colinfo->index(), COLNULLVALUE))
				continue;	// column set non NULL value

			// yet undefined column value
			unsigned char* coladdr = colinfo->recordToColumn (rowdata);
			// auto increment column?
			if (colinfo->autoInc())
			{
				clrcolflag (rowdata, 2, colinfo->index(), COLNULLVALUE);
				colinfo->uniqueAutoValue(coladdr);
			}
			// not predefined
			else	if (colinfo->nullDefValue())
			{
				if (colinfo->nullable ())
					setcolflag(rowdata, 2, colinfo->index(), COLNULLABLE);
				else
					xdbdErrReport(SC_XDBD, SC_WRN, err_info ("setting null value to non-nullable column '%s' in table '%s'"), colinfo->name().c_str(), m_tabinfo->name().c_str());
			}
			// predefined
			else
			{
				char* dataAddr = (char*) colinfo->defVal().c_str();
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
				clrcolflag (rowdata, 2, colinfo->index(), COLNULLVALUE);
			}
			colinfo->insert (rowdata);
		}

		int ressize = m_tabinfo->resSize ();
		unsigned char* newData = (unsigned char*) malloc (ressize);
		memcpy (rowdata, "II", sizeof(short));
		memcpy (newData, rowdata, ressize);
		if (XdbdXmlDataBase::TriggerStatus())
			XdbdMessageQueue::Put (new XdbdInsertTriggerJob (m_tabinfo, newData, ressize));
		else
			XdbdXmlDataBase::SaveTrgJob (new XdbdInsertTriggerJob (m_tabinfo, newData, ressize));

#if defined (_DEBUG)
		dump (addr, shmsize);
#endif
	}
	while (false);
	return 0;
}

#define	CLASSNAME	XdbdSqlInsertFunction

#define	sql_insert_statement_CallbackDefined
void CLASSNAME::Invoke_sql_insert_statement_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_insert_statement* p_sql_insert_statement)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
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
			for (XdbdXmlTableInfo::col_iterator it = m_tabinfo->col_begin (); it != m_tabinfo->col_end (); ++it)
				m_resultSize += (*it)->realLength ();
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
			m_sql_table_part = p_sql_table_part;
			if ((m_tabinfo = XdbdXmlDataBase::GetTableInfo (m_sql_table_part->get_sql_table ()->get_char ())) == 0)
				throw 1;
		}
		break;
	case TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_columns_part_CallbackDefined
void CLASSNAME::Invoke_sql_columns_part_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_columns_part* p_sql_columns_part)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		{

			m_sql_columns_part = p_sql_columns_part;
			m_sql_columns = p_sql_columns_part->get_sql_columns ()->revert_sql_columns (0,
				sql_columns::g_sql_columns_1);
			p_sql_columns_part->set_sql_columns (m_sql_columns);
		}
		break;
	case TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_values_part_CallbackDefined
void CLASSNAME::Invoke_sql_values_part_Callback (SqlCallbackReason reason, unsigned int kind,
	sql_values_part* p_sql_values_part)
{
	switch (reason)
	{
	case DefaultCallbackReason:
		break;
	case TraversalPrologueCallbackReason:
		{
			m_sql_values_part = p_sql_values_part;
			m_sql_values = p_sql_values_part->get_sql_values ()->revert_sql_values (0, sql_values::g_sql_values_1);
			p_sql_values_part->set_sql_values (m_sql_values);
			if (m_sql_columns_part == 0)
			{
				m_cit = m_tabinfo->col_begin ();
			}
			else
			{
				m_sql_columns_current = m_sql_columns;
			}
		}
		break;
	case TraversalEpilogueCallbackReason:
		break;
	}
}

#define	sql_values_CallbackDefined
void	CLASSNAME::Invoke_sql_values_Callback (SqlCallbackReason reason, unsigned int kind, sql_values* p_sql_values)
{
	switch (reason)
	{
	case	DefaultCallbackReason:
		break;
	case	TraversalPrologueCallbackReason:
		{
			sql_constant*	p_sql_constant = p_sql_values->get_sql_constant();

			++m_columnCounter;
			XdbdXmlColumnInfo* colinfo = 0;
			if (m_sql_columns_part == 0)
			{
				if (m_cit == m_tabinfo->col_end ())
					break;
				colinfo = *m_cit;
				++m_cit;
			}
			else
			{
				if (m_sql_columns_current == 0)
					break;
				colinfo = m_tabinfo->find (
					m_sql_columns_current->get_sql_column ()->get_sql_column_name ()->get_char ());
				m_sql_columns_current = m_sql_columns_current->get_sql_columns ();
			}
			if (p_sql_constant->kind () != sql_constant::g_sql_constant_4)
				break;
			if (colinfo == 0)
				break;
			m_dynpars.push_back (
				new XdbdSqlComparison (XdbdXmlColumnInfo::colrng (), m_tabinfo, colinfo, 0, colinfo->maxLength (),
					p_sql_constant->context (), false));
		}
		break;
	case	TraversalEpilogueCallbackReason:
		break;
	}
}

#include "SqlCompilerCallbackImpls.template"

} /* namespace xml_db_daemon */

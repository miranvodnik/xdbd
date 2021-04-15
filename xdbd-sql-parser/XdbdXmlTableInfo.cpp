/*
 * XdbdXmlTableInfo.cpp
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include "XdbdXmlTableInfo.h"
#include "XdbdXmlDataBase.h"
#include <string.h>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

int XdbdXmlTableInfo::g_refCount = 1;
SQLHENV	XdbdXmlTableInfo::g_henv = SQL_NULL_HENV;
SQLHDBC XdbdXmlTableInfo::g_hdbc = SQL_NULL_HDBC;
bool XdbdXmlTableInfo::g_connected = false;
const u_char	XdbdXmlTableInfo::g_deadPtr[] = { 0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad };

XdbdXmlTableInfo::XdbdXmlTableInfo (const char* name)
{
	Reset ();
	m_name = name;
	sem_init (&m_sem, 0, 1);
}

XdbdXmlTableInfo::~XdbdXmlTableInfo ()
{
	saveShmData();

	for (shmset::iterator it = m_shmset.begin (); it != m_shmset.end (); ++it)
	{
		shmdescr p = *it;
		munmap (p.addr, p.size);
		shm_unlink (p.name.c_str ());
	}

	for (collist::iterator it = m_collist.begin (); it != m_collist.end (); ++it)
		delete *it;
	m_collist.clear ();

	if (m_hselectStm != SQL_NULL_HSTMT) SQLFreeHandle (SQL_HANDLE_STMT, m_hselectStm);
	if (m_hupdateStm != SQL_NULL_HSTMT) SQLFreeHandle (SQL_HANDLE_STMT, m_hupdateStm);
	if (m_hinsertStm != SQL_NULL_HSTMT) SQLFreeHandle (SQL_HANDLE_STMT, m_hinsertStm);
	if (m_hdeleteStm != SQL_NULL_HSTMT) SQLFreeHandle (SQL_HANDLE_STMT, m_hdeleteStm);

	if (m_rowData != 0)
		free (m_rowData);
	if (m_nullInd != 0)
		free (m_nullInd);
	Reset ();
}

void	XdbdXmlTableInfo::Reset ()
{
	m_colhdrSize = 0;
	m_resSize = 0;
	m_free = 0;
	m_hselectStm = SQL_NULL_HSTMT;
	m_hupdateStm = SQL_NULL_HSTMT;
	m_hinsertStm = SQL_NULL_HSTMT;
	m_hdeleteStm = SQL_NULL_HSTMT;
	m_selectPrepared = false;
	m_updatePrepared = false;
	m_insertPrepared = false;
	m_deletePrepared = false;
	m_rowData = 0;
	m_colData = 0;
	m_parData = 0;
	m_nullInd = 0;
}

void	XdbdXmlTableInfo::LockTable ()
{
	if (sem_wait (&m_sem) != 0)
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot lock table '%s', sem_wait() failed, errno = %d"), m_name.c_str(), errno);
}

void	XdbdXmlTableInfo::UnlockTable ()
{
	if (sem_post (&m_sem) != 0)
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot unlock table '%s', sem_post() failed, errno = %d"), m_name.c_str(), errno);
}

XdbdXmlColumnInfo* XdbdXmlTableInfo::find (string name)
{
	for (collist::iterator it = m_collist.begin (); it != m_collist.end (); ++it)
	{
		XdbdXmlColumnInfo* colinfo = *it;
		if (strcasecmp (colinfo->name ().c_str(), name.c_str()) == 0)
			return colinfo;
	}
	return 0;
}

char* XdbdXmlTableInfo::findNodeContent (xmlNodePtr node, const char*path [])
{
	if (*path == 0)
		return (char*) node->content;
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		if (strcmp (*path, (char*) child->name) != 0)
			continue;
		return findNodeContent (child, path + 1);
	}
	return 0;
}

char* XdbdXmlTableInfo::findAttrContent (xmlNodePtr node, const char*attr)
{
	if (*attr == 0)
		return (char*) 0;
	for (xmlAttrPtr child = node->properties; child != 0; child = child->next)
	{
		if (strcmp (attr, (char*) child->name) != 0)
			continue;
		return	(char*) child->children->content;
	}
	return 0;
}

int XdbdXmlTableInfo::IndexXmlTable (xmlXPathContextPtr xmlCtx, string name)
{
#if defined (XDBD_PERFTEST)
	struct timeval	tbegin, tend;
	struct timeval	txml = {0, 0}, tdb = {0, 0};
	struct timeval	txmlbegin, txmlend, tdbbegin, tdbend;

	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST

	string pathExpr = "//" + name;
	xmlXPathObjectPtr obj = xmlXPathEvalExpression ((xmlChar*) pathExpr.c_str (), xmlCtx);
	if (obj == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot evaluate xml path '//%s' for table '%s', xmlXPathEvalExpression () failed"), name.c_str(), m_name.c_str());
		return -1;
	}

	xmlNodeSetPtr nodes = obj->nodesetval;
	int nodenr = (nodes != 0) ? nodes->nodeNr : 0;

#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: node set created for '//%s' for table '%s', nodes = %d, time = %d.%06d"), name.c_str(), m_name.c_str(), nodenr, tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST

	if (nodenr <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("no nodes for xml path '//%s' for table '%s'"), name.c_str(), m_name.c_str());
		getFreeCol();
	}

#if defined (XDBD_PERFTEST)
	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST

	for (int nr = 0; nr < nodenr; ++nr)
	{
#if defined (XDBD_PERFTEST)
		gettimeofday (&txmlbegin, 0);
#endif	// XDBD_PERFTEST

		xmlNodePtr node = nodes->nodeTab [nr];

#if defined (XDBD_PERFTEST)
		gettimeofday (&txmlend, 0);
		txmlend.tv_sec -= txmlbegin.tv_sec;
		if ((txmlend.tv_usec -= txmlbegin.tv_usec) < 0)
		{
			txmlend.tv_usec += 1000 * 1000;
			txmlend.tv_sec--;
		}
		txml.tv_sec += txmlend.tv_sec;
		if ((txml.tv_usec += txmlend.tv_usec) >= 1000 * 1000)
		{
			txml.tv_usec -= 1000 * 1000;
			txml.tv_sec++;
		}
#endif	// XDBD_PERFTEST

		if (node == 0)
			continue;

		unsigned char* addr = getFreeCol();
		unsigned char* ptr = addr;

		if (addr == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate db column for table '%s'"), m_name.c_str());
			break;
		}

		int index = 0;
		*((char*) ptr) = m_colhdrSize;
		ptr += m_colhdrSize;
		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit, ++index)
		{
#if defined (XDBD_PERFTEST)
			gettimeofday (&txmlbegin, 0);
#endif	// XDBD_PERFTEST

			XdbdXmlColumnInfo* colinfo = *resit;
			char* content;
			{
				string	colName = (*resit)->name ();
				const char* path [] =
					{ colName.c_str(), "text", 0 };
				if ((content = findNodeContent (node, path)) == 0)
					content = findAttrContent (node, path[0]);
			}

#if defined (XDBD_PERFTEST)
			gettimeofday (&txmlend, 0);
			txmlend.tv_sec -= txmlbegin.tv_sec;
			if ((txmlend.tv_usec -= txmlbegin.tv_usec) < 0)
			{
				txmlend.tv_usec += 1000 * 1000;
				txmlend.tv_sec--;
			}
			txml.tv_sec += txmlend.tv_sec;
			if ((txml.tv_usec += txmlend.tv_usec) >= 1000 * 1000)
			{
				txml.tv_usec -= 1000 * 1000;
				txml.tv_sec++;
			}

			gettimeofday (&tdbbegin, 0);
#endif	// XDBD_PERFTEST

			if (content == 0)
			{
				if (colinfo->primary())
				{
					clrcolflag(addr, 2, index, COLNULLVALUE);
					colinfo->uniqueAutoValue(ptr);
					colinfo->insert (ptr, addr);
					ptr += colinfo->realLength ();
					continue;
				}
				if (colinfo->nullDefValue())
				{
					setcolflag(addr, 2, index, COLNULLVALUE);
					if (colinfo->nullable ())
						setcolflag(addr, 2, index, COLNULLABLE);
					colinfo->insert (ptr, addr);
					ptr += colinfo->realLength ();
					continue;
				}
				else
					content = (char*) colinfo->defVal().c_str();
				clrcolflag(addr, 2, index, COLNULLVALUE);
				continue;
			}

			switch (colinfo->colType ())
			{
			case SQL_SMALLINT:
				*((SQLSMALLINT*) ptr) = (SQLSMALLINT) atoi (content);
				break;
			case SQL_INTEGER:
				if (sizeof(int) != sizeof(long))
				{
					switch (colinfo->maxLength ())
					{
					case sizeof(int):
						*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
						break;
					case sizeof(long):
						*((SQLLEN*) ptr) = (SQLLEN) atol (content);
						break;
					default:
						break;
					}
				}
				else
					*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
				break;
			case SQL_VARCHAR:
				memcpy (ptr, content, colinfo->maxLength ());
				break;
			default:
				break;
			}
			clrcolflag(addr, 2, index, COLNULLVALUE);
			colinfo->insert (ptr, addr);
			ptr += colinfo->realLength ();

#if defined (XDBD_PERFTEST)
			gettimeofday (&tdbend, 0);
			tdbend.tv_sec -= tdbbegin.tv_sec;
			if ((tdbend.tv_usec -= tdbbegin.tv_usec) < 0)
			{
				tdbend.tv_usec += 1000 * 1000;
				tdbend.tv_sec--;
			}
			tdb.tv_sec += tdbend.tv_sec;
			if ((tdb.tv_usec += tdbend.tv_usec) >= 1000 * 1000)
			{
				tdb.tv_usec -= 1000 * 1000;
				tdb.tv_sec++;
			}
#endif	// XDBD_PERFTEST
		}
	}

#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: node set loaded for '//%s' for table '%s', time = %d.%06d"), name.c_str(), m_name.c_str(), tend.tv_sec, tend.tv_usec);
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: node set timing for '//%s' for table '%s', xmltime = %d.%06d, dbtime = %d.%06d"), name.c_str(), m_name.c_str(), txml.tv_sec, txml.tv_usec, tdb.tv_sec, tdb.tv_usec);
#endif	// XDBD_PERFTEST

	xmlXPathFreeObject (obj);

	generateSelectStatement ();
	generateUpdateStatement ();
	generateInsertStatement ();
	generateDeleteStatement ();

	return 0;
}

void XdbdXmlTableInfo::connectDB ()
{
	if ((g_henv == SQL_NULL_HENV) && (SQLAllocHandle (SQL_HANDLE_ENV, 0, &g_henv) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLENV handle"), m_name.c_str());
	}

	else if ((g_hdbc == SQL_NULL_HDBC) && (SQLAllocHandle (SQL_HANDLE_DBC, g_henv, &g_hdbc) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLDBC handle"), m_name.c_str());
	}

	else if (!g_connected
		&& (SQLConnect (g_hdbc, (SQLCHAR*) "localhost", SQL_NTS, (SQLCHAR*) "user", SQL_NTS, (SQLCHAR*) "user",
		SQL_NTS) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLConnect()"), m_name.c_str());
	}

	else
	{
		g_connected = true;
		if (m_rowData == 0)
		{
			m_rowData = (char*) malloc (2 * m_resSize);
			m_colData = m_rowData;
			m_parData = m_rowData + m_resSize;
		}
	}
}

void XdbdXmlTableInfo::generateSelectStatement ()
{
	const char* opCode = "";
	m_selectStm = "SELECT ";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (colinfo->primary ())
			continue;
		m_selectStm += opCode;
		m_selectStm += colinfo->name ();
		opCode = ", ";
	}
	m_selectStm += " FROM ";
	m_selectStm += m_name;
	m_selectStm += " WHERE (";
	opCode = "";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary ())
			continue;
		m_selectStm += opCode;
		m_selectStm += "(";
		m_selectStm += colinfo->name ();
		m_selectStm += " = ?)";
		opCode = " AND ";
	}
	m_selectStm += ")";
	xdbdErrReport (SC_XDBD, SC_APL, err_info ("internal select statement '%s' created"), m_selectStm.c_str());
}

void XdbdXmlTableInfo::connectSelectStatement ()
{
	connectDB ();

	if (!g_connected)
		return;

	if ((m_hselectStm == SQL_NULL_HSTMT) && (SQLAllocHandle (SQL_HANDLE_STMT, g_hdbc, &m_hselectStm) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLSTM handle for statement '%s'"), m_selectStm.c_str());
		return;
	}

	else if (!m_selectPrepared)
	{
		if (SQLPrepare (m_hselectStm, (SQLCHAR*) m_selectStm.c_str (), SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLPrepare (%s)"), m_selectStm.c_str());
			return;
		}

		m_selectPrepared = true;

		int index = 0;
		int parnr = 0;

		if (m_nullInd == 0)
			m_nullInd = (SQLLEN*) malloc (m_collist.size() * sizeof (SQLLEN));

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (!colinfo->primary ())
			{
				if (SQLBindCol (m_hselectStm, ++index, colinfo->colType (), m_colData + colinfo->position (),
					colinfo->realLength (), m_nullInd + colinfo->index()) != SQL_SUCCESS)
					xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind column '%s' for statement '%s'"), colinfo->name().c_str(), m_selectStm.c_str());
			}
			else
			{
				if (SQLBindParam (m_hselectStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (),
					0, m_parData + colinfo->position (), 0) != SQL_SUCCESS)
					xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_selectStm.c_str());
			}
		}
	}
}

void XdbdXmlTableInfo::generateUpdateStatement ()
{
	const char* opCode = "";
	m_updateStm = "UPDATE ";
	m_updateStm += m_name;
	m_updateStm += " SET ";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (colinfo->primary ())
			continue;
		m_updateStm += opCode;
		m_updateStm += colinfo->name ();
		m_updateStm += " = ?";
		opCode = ", ";
	}
	m_updateStm += " WHERE (";
	opCode = "";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary ())
			continue;
		m_updateStm += opCode;
		m_updateStm += "(";
		m_updateStm += colinfo->name ();
		m_updateStm += " = ?)";
		opCode = " AND ";
	}
	m_updateStm += ")";
	xdbdErrReport (SC_XDBD, SC_APL, err_info ("internal update statement '%s' created"), m_updateStm.c_str());
}

void XdbdXmlTableInfo::connectUpdateStatement ()
{
	connectDB ();

	if (!g_connected)
		return;

	if ((m_hupdateStm == SQL_NULL_HSTMT) && (SQLAllocHandle (SQL_HANDLE_STMT, g_hdbc, &m_hupdateStm) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLSTM handle for statement '%s'"), m_updateStm.c_str());
		return;
	}

	else if (!m_updatePrepared)
	{
		if (SQLPrepare (m_hupdateStm, (SQLCHAR*) m_updateStm.c_str (), SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLPrepare (%s)"), m_updateStm.c_str());
			return;
		}

		m_updatePrepared = true;

		int parnr = 0;

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (colinfo->primary ())
				continue;
			if (SQLBindParam (m_hupdateStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0,
				m_parData + colinfo->position (), 0) != SQL_SUCCESS)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_updateStm.c_str());
		}

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (!colinfo->primary ())
				continue;
			if (SQLBindParam (m_hupdateStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0,
				m_parData + colinfo->position (), 0) != SQL_SUCCESS)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_updateStm.c_str());
		}
	}
}

void XdbdXmlTableInfo::generateInsertStatement ()
{
	const char* opCode = "";
	m_insertStm = "INSERT INTO ";
	m_insertStm += m_name;
	m_insertStm += " (";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		m_insertStm += opCode;
		m_insertStm += colinfo->name ();
		opCode = ", ";
	}
	m_insertStm += ") VALUES (";
	opCode = "";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		m_insertStm += opCode;
		m_insertStm += "?";
		opCode = ", ";
	}
	m_insertStm += ")";
	xdbdErrReport (SC_XDBD, SC_APL, err_info ("internal insert statement '%s' created"), m_insertStm.c_str());
}

void XdbdXmlTableInfo::connectInsertStatement ()
{
	connectDB ();

	if (!g_connected)
		return;

	if ((m_hinsertStm == SQL_NULL_HSTMT) && (SQLAllocHandle (SQL_HANDLE_STMT, g_hdbc, &m_hinsertStm) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLSTM handle for statement '%s'"), m_insertStm.c_str());
		return;
	}

	else if (!m_insertPrepared)
	{
		if (SQLPrepare (m_hinsertStm, (SQLCHAR*) m_insertStm.c_str (), SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLPrepare (%s)"), m_insertStm.c_str());
			return;
		}

		m_insertPrepared = true;

		int parnr = 0;

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (colinfo->primary ())
				continue;
			if (SQLBindParam (m_hupdateStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0,
				m_parData + colinfo->position (), 0) != SQL_SUCCESS)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_updateStm.c_str());
		}

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (!colinfo->primary ())
				continue;
			if (SQLBindParam (m_hupdateStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0,
				m_parData + colinfo->position (), 0) != SQL_SUCCESS)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_updateStm.c_str());
		}
	}
}

void XdbdXmlTableInfo::generateDeleteStatement ()
{
	const char* opCode = " ";
	m_deleteStm = "DELETE FROM ";
	m_deleteStm += m_name;
	m_deleteStm += " WHERE";
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary())
			continue;
		m_deleteStm += opCode;
		m_deleteStm += colinfo->name ();
		m_deleteStm += " = ?";
		opCode = " AND ";
	}
	xdbdErrReport (SC_XDBD, SC_APL, err_info ("internal delete statement '%s' created"), m_deleteStm.c_str());
}

void XdbdXmlTableInfo::connectDeleteStatement ()
{
	connectDB ();

	if (!g_connected)
		return;

	if ((m_hdeleteStm == SQL_NULL_HSTMT) && (SQLAllocHandle (SQL_HANDLE_STMT, g_hdbc, &m_hdeleteStm) != SQL_SUCCESS))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate internal SQLSTM handle for statement '%s'"), m_deleteStm.c_str());
		return;
	}

	else if (!m_deletePrepared)
	{
		if (SQLPrepare (m_hdeleteStm, (SQLCHAR*) m_deleteStm.c_str (), SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLPrepare (%s)"), m_deleteStm.c_str());
			return;
		}

		m_deletePrepared = true;

		int parnr = 0;

		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (!colinfo->primary())
				continue;
			if (SQLBindParam (m_hdeleteStm, ++parnr, colinfo->colType (), colinfo->colType (), colinfo->realLength (),
				0, m_parData + colinfo->position (), 0) != SQL_SUCCESS)
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameter for column '%s' for statement '%s'"), colinfo->name().c_str(), m_deleteStm.c_str());
		}
	}
}

int	XdbdXmlTableInfo::bindSelectParameters (xmlNodePtr node)
{
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary ())
			continue;

		bool	binary = false;
		char*	content;
		{
			string	colName = colinfo->name ();
			const char* path [] =
				{ colName.c_str(), "text", 0 };
			if ((content = findNodeContent (node, path)) == 0)
				content = findAttrContent (node, path[0]);
		}
		if (content == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("primary column '%s' in xml data for table '%s' has undefined value"), colinfo->name().c_str(), m_name.c_str());
			return	-1;
		}

		char* ptr = m_parData + colinfo->position ();
		switch (colinfo->colType ())
		{
		case SQL_SMALLINT:
			*((SQLSMALLINT*) ptr) = (SQLSMALLINT) (binary ? *((SQLSMALLINT*) content) : atoi (content));
			break;
		case SQL_INTEGER:
			if (sizeof(int) != sizeof(long))
			{
				switch (colinfo->maxLength ())
				{
				case sizeof(int):
					*((SQLINTEGER*) ptr) = (SQLINTEGER) (binary ? *((SQLINTEGER*) content) : atoi (content));
					break;
				case sizeof(long):
					*((SQLLEN*) ptr) = (SQLLEN) (binary ? *((SQLLEN*) content) : atol (content));
					break;
				default:
					break;
				}
			}
			else
				*((SQLINTEGER*) ptr) = (SQLINTEGER) (binary ? *((SQLINTEGER*) content) : atoi (content));
			break;
		case SQL_VARCHAR:
			memcpy (ptr, content, colinfo->maxLength ());
			break;
		default:
			break;
		}
		if (binary)
			free (content);
	}
	return	0;
}

int	XdbdXmlTableInfo::checkSelectResults (xmlNodePtr node)
{
	int	count = 0;
	bool	diff = false;

	do
	{
		memset (m_nullInd, 0, m_collist.size() * sizeof (SQLLEN));
		if (SQLFetch (m_hselectStm) == SQL_NO_DATA)
			break;

		++count;
		for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
		{
			XdbdXmlColumnInfo* colinfo = *resit;
			if (colinfo->primary ())
				continue;

			int index = colinfo->index();
			char* content;
			{
				string	colName = colinfo->name ();
				const char* path [] =
					{ colName.c_str(), "text", 0 };
				if ((content = findNodeContent (node, path)) == 0)
					content = findAttrContent (node, path[0]);
			}

			if (content == 0)
			{
				if (m_nullInd[index] != SQL_NULL_DATA)
					diff = true;
				continue;
			}

			if (m_nullInd[index] == SQL_NULL_DATA)
			{
				diff = true;
				continue;
			}

			char* ptr = m_colData + colinfo->position ();
			switch (colinfo->colType ())
			{
			case SQL_SMALLINT:
				diff = diff || ((*((SQLSMALLINT*) ptr) != (SQLSMALLINT) atoi (content)));
				break;
			case SQL_INTEGER:
				if (sizeof(int) != sizeof(long))
				{
					switch (colinfo->maxLength ())
					{
					case sizeof(int):
						diff = diff || ((*((SQLINTEGER*) ptr) != (SQLINTEGER) atoi (content)));
						break;
					case sizeof(long):
						diff = diff || ((*((SQLLEN*) ptr) != (SQLLEN) atol (content)));
						break;
					default:
						break;
					}
				}
				else
					diff = diff || ((*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content)));
				break;
			case SQL_VARCHAR:
				diff = diff || ((strncmp (ptr, content, colinfo->maxLength ()) != 0));
				break;
			default:
				break;
			}
		}
	}
	while (false);

	return	diff ? -count : count;
}

void	XdbdXmlTableInfo::bindUpdateParameters (xmlNodePtr node)
{
	int parnr = 0;

	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (colinfo->primary())
			continue;

		char* content;
		{
			string	colName = colinfo->name ();
			const char* path [] =
				{ colName.c_str(), "text", 0 };
			if ((content = findNodeContent (node, path)) == 0)
				content = findAttrContent (node, path[0]);
		}
		if (content == 0)
		{
			SQLBindParam(m_hupdateStm, parnr + 1, SQL_UNKNOWN_TYPE, SQL_UNKNOWN_TYPE, 0, 0, (SQLPOINTER) (long) (parnr), (SQLLEN*) SQL_NULL_DATA);
			++parnr;
			continue;
		}

		SQLBindParam (m_hupdateStm, parnr + 1, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0, m_parData + colinfo->position (), 0);
		++parnr;

		char* ptr = m_parData + colinfo->position ();
		switch (colinfo->colType ())
		{
		case SQL_SMALLINT:
			*((SQLSMALLINT*) ptr) = (SQLSMALLINT) atoi (content);
			break;
		case SQL_INTEGER:
			if (sizeof(int) != sizeof(long))
			{
				switch (colinfo->maxLength ())
				{
				case sizeof(int):
					*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
					break;
				case sizeof(long):
					*((SQLLEN*) ptr) = (SQLLEN) atol (content);
					break;
				default:
					break;
				}
			}
			else
				*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
			break;
		case SQL_VARCHAR:
			memcpy (ptr, content, colinfo->maxLength ());
			break;
		default:
			break;
		}
	}

	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary())
			continue;

		char* content;
		{
			string	colName = colinfo->name ();
			const char* path [] =
				{ colName.c_str(), "text", 0 };
			if ((content = findNodeContent (node, path)) == 0)
				content = findAttrContent (node, path[0]);
		}
		if (content == 0)
		{
			SQLBindParam(m_hupdateStm, parnr + 1, SQL_UNKNOWN_TYPE, SQL_UNKNOWN_TYPE, 0, 0, (SQLPOINTER) (long) (parnr), (SQLLEN*) SQL_NULL_DATA);
			++parnr;
			continue;
		}

		SQLBindParam (m_hupdateStm, parnr + 1, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0, m_parData + colinfo->position (), 0);
		++parnr;

		char* ptr = m_parData + colinfo->position ();
		switch (colinfo->colType ())
		{
		case SQL_SMALLINT:
			*((SQLSMALLINT*) ptr) = (SQLSMALLINT) atoi (content);
			break;
		case SQL_INTEGER:
			if (sizeof(int) != sizeof(long))
			{
				switch (colinfo->maxLength ())
				{
				case sizeof(int):
					*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
					break;
				case sizeof(long):
					*((SQLLEN*) ptr) = (SQLLEN) atol (content);
					break;
				default:
					break;
				}
			}
			else
				*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
			break;
		case SQL_VARCHAR:
			memcpy (ptr, content, colinfo->maxLength ());
			break;
		default:
			break;
		}
	}
}

void	XdbdXmlTableInfo::bindInsertParameters (xmlNodePtr node)
{
	int parnr = 0;

	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		char* content;
		{
			string	colName = colinfo->name ();
			const char* path [] =
				{ colName.c_str(), "text", 0 };
			if ((content = findNodeContent (node, path)) == 0)
				content = findAttrContent (node, path[0]);
		}

		char* ptr = m_parData + colinfo->position ();
		if (content == 0)
		{
			SQLBindParam(m_hinsertStm, parnr + 1, SQL_UNKNOWN_TYPE, SQL_UNKNOWN_TYPE, 0, 0, (SQLPOINTER) (long) (parnr), (SQLLEN*) SQL_NULL_DATA);
			++parnr;
			if (false)
			{
				if (!colinfo->autoInc())
				{
					memset (ptr, 0, colinfo->realLength());
					xdbdErrReport (SC_XDBD, SC_WRN, err_info ("column '%s' in table '%s' has undefined value"), colinfo->name().c_str(), m_name.c_str());
					continue;
				}
				colinfo->uniqueAutoValue ((unsigned char*) ptr);
			}
			continue;
		}

		SQLBindParam (m_hinsertStm, parnr + 1, colinfo->colType (), colinfo->colType (), colinfo->realLength (), 0, m_parData + colinfo->position (), 0);
		++parnr;

		switch (colinfo->colType ())
		{
		case SQL_SMALLINT:
			*((SQLSMALLINT*) ptr) = (SQLSMALLINT) atoi (content);
			break;
		case SQL_INTEGER:
			if (sizeof(int) != sizeof(long))
			{
				switch (colinfo->maxLength ())
				{
				case sizeof(int):
					*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
					break;
				case sizeof(long):
					*((SQLLEN*) ptr) = (SQLLEN) atol (content);
					break;
				default:
					break;
				}
			}
			else
				*((SQLINTEGER*) ptr) = (SQLINTEGER) atoi (content);
			break;
		case SQL_VARCHAR:
			memcpy (ptr, content, colinfo->maxLength ());
			break;
		default:
			break;
		}
	}
}

void	XdbdXmlTableInfo::bindDeleteParameters (unsigned char* rowAddr)
{
	for (collist::iterator resit = m_collist.begin (); resit != m_collist.end (); ++resit)
	{
		XdbdXmlColumnInfo* colinfo = *resit;
		if (!colinfo->primary())
			continue;

		char* content = (char*) colinfo->recordToColumn (rowAddr);
		char* ptr = m_parData + colinfo->position ();
		int	length = colinfo->maxLength();

		memcpy (ptr, content, length);
	}
}

int XdbdXmlTableInfo::ReloadXmlTable (xmlXPathContextPtr xmlCtx, string name)
{
#if defined(XDBD_PERFTEST)
	int	selectCounter = 0;
	int	updateCounter = 0;
	int	insertCounter = 0;
	int	deleteCounter = 0;
#endif // XDBD_PERFTEST

	string pathExpr = "//" + name;
	xmlXPathObjectPtr obj = xmlXPathEvalExpression ((xmlChar*) pathExpr.c_str (), xmlCtx);
	if (obj == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot eval xml path '//%s' for table '%s'"), name.c_str(), m_name.c_str());
		return -1;
	}

	xmlNodeSetPtr nodes = obj->nodesetval;
	int nodenr = (nodes != 0) ? nodes->nodeNr : 0;

	if (m_resSize <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info ("empty node set for xml path '//%s' for table '%s'"), name.c_str(), m_name.c_str());
		return -1;
	}

	connectSelectStatement ();
	connectUpdateStatement ();
	connectInsertStatement ();
	connectDeleteStatement ();

	//	select arbitrary column (first one is quite good) and iterate all rows
	XdbdXmlColumnInfo*	colinfo = m_collist[0];
	for (XdbdXmlColumnInfo::colidx idx = colinfo->begin(); idx != colinfo->end(); ++idx)
	{
		unsigned char*	rowAddr = idx->second;
		memset (rowAddr, 0, 2);
	}

	for (int nr = 0; nr < nodenr; ++nr)
	{
		xmlNodePtr node = nodes->nodeTab [nr];
		if (node == 0)
			continue;

		if (bindSelectParameters(node) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot bind parameters for internal '%s'"), m_selectStm.c_str());
			break;
		}
		if (SQLExecute (m_hselectStm) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLExecute (%s)"), m_selectStm.c_str());
			break;
		}
#if defined(XDBD_PERFTEST)
		++selectCounter;
#endif // XDBD_PERFTEST
		int	check = checkSelectResults(node);
		if (check < 0)
		{
			bindUpdateParameters (node);
			if (SQLExecute (m_hupdateStm) != SQL_SUCCESS)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLExecute (%s)"), m_updateStm.c_str());
				break;
			}
#if defined(XDBD_PERFTEST)
			++updateCounter;
#endif // XDBD_PERFTEST
		}
		else	if (check == 0)
		{
			bindInsertParameters (node);
			if (SQLExecute (m_hinsertStm) != SQL_SUCCESS)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLExecute (%s)"), m_insertStm.c_str());
				break;
			}
#if defined(XDBD_PERFTEST)
			++insertCounter;
#endif // XDBD_PERFTEST
		}
	}

	int	count = 0;
	for (XdbdXmlColumnInfo::colidx idx = colinfo->begin(); idx != colinfo->end(); ++idx)
	{
		unsigned char*	rowAddr = idx->second;
		if (*rowAddr != 0)
			continue;
		++count;
	}

	while (count > 0)
	{
		unsigned char*	rows = (unsigned char*) malloc (count * m_resSize);
		if (rows == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate %d * %d bytes for deleted rows"), count, m_resSize);
			break;
		}
		unsigned char*	rowPtr = rows;
		for (XdbdXmlColumnInfo::colidx idx = colinfo->begin(); idx != colinfo->end(); ++idx)
		{
			unsigned char*	rowAddr = idx->second;
			if (*rowAddr != 0)
				continue;
			memcpy (rowPtr, rowAddr, m_resSize);
			rowPtr += m_resSize;
		}
		int	i = 0;
		for (i = 0, rowPtr = rows; i < count; rowPtr += m_resSize, ++i)
		{
			bindDeleteParameters (rowPtr);
			if (SQLExecute (m_hdeleteStm) != SQL_SUCCESS)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot invoke internal SQLExecute (%s)"), m_deleteStm.c_str());
				continue;
			}
#if defined(XDBD_PERFTEST)
			++deleteCounter;
#endif // XDBD_PERFTEST
		}
		free (rows);
		break;
	}

#if defined(XDBD_PERFTEST)
	if (selectCounter > 0) xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reload table '%s', %d '%s' statements executed"), m_name.c_str(), selectCounter, m_selectStm.c_str());
	if (updateCounter > 0) xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reload table '%s', %d '%s' statements executed"), m_name.c_str(), updateCounter, m_updateStm.c_str());
	if (insertCounter > 0) xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reload table '%s', %d '%s' statements executed"), m_name.c_str(), insertCounter, m_insertStm.c_str());
	if (deleteCounter > 0) xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reload table '%s', %d '%s' statements executed"), m_name.c_str(), deleteCounter, m_deleteStm.c_str());
#endif // XDBD_PERFTEST
	xmlXPathFreeObject (obj);

	return 0;
}

void XdbdXmlTableInfo::AlignColPositions ()
{
	cout << "TABLE " << m_name << endl;
	cout << "{" << endl;
	for (shmset::iterator it = m_shmset.begin(); it != m_shmset.end(); ++it)
		cout << "\tSHM = " << it->name << endl;
	m_resSize = m_colhdrSize = xdbdalign(2 + (m_collist.size () + 3) / 2, long);
	cout << "\tHDR = " << m_colhdrSize << endl;
	for (collist::iterator it = m_collist.begin (); it != m_collist.end (); ++it)
	{
		(*it)->alignPosition (m_colhdrSize);
		m_resSize += (*it)->realLength ();
		cout << "\tCOL " << (*it)->name() << " (POS = " << (*it)->position() << ", SIZE = " << (*it)->realLength() << ")" << endl;
	}
	cout << "\tRESSIZE = " << m_resSize << endl;
	cout << "}" << endl;
}

int XdbdXmlTableInfo::allocShmChunk ()
{
	int rowcnt = 1024;
	char shmName [64];
	sprintf (shmName, "/xdbd-table-%08d", g_refCount++);
	shm_unlink (shmName);

	int shmFd;
	if ((shmFd = shm_open (shmName, O_RDWR | O_CREAT, 0666)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create shared memory chunk '%s', shm_open() failed, errno = %d"), shmName, errno);
		return -1;
	}

	if (ftruncate (shmFd, rowcnt * m_resSize) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot allocate space for shared memory chunk '%s', ftruncate (%d * %d) failed, errno = %d"), shmName, rowcnt, m_resSize, errno);
		close (shmFd);
		return -1;
	}

	unsigned char* addr = (unsigned char*) mmap (0, rowcnt * m_resSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	if (addr == MAP_FAILED)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot map shared memory chunk '%s', mmap (%d * %d) failed, errno = %d"), shmName, rowcnt, m_resSize, errno);
		close (shmFd);
		return -1;
	}
	{
		shmdescr	shm = { shmName, rowcnt * m_resSize, addr };
		m_shmset.push_back (shm);
	}
	close (shmFd);

	for (int i = 0; i < rowcnt; ++i, addr += m_resSize)
		freeCol (addr);

	cout << "TABLE " << m_name << ", SHM = " << shmName << endl;
	return 0;
}

unsigned char* XdbdXmlTableInfo::getFreeCol ()
{
	if ((m_free == 0) && (allocShmChunk () < 0))
		return 0;
	unsigned char* addr = m_free;
	if (addr != 0)
	{
		m_free = *((unsigned char**) addr);
		*((unsigned char**)addr) = 0;
	}
	return addr;
}

void	XdbdXmlTableInfo::saveShmData ()
{
	string	datPath = XdbdXmlDataBase::XdbdDatDataBasePath();
	datPath += '/';
	datPath += m_name;
	datPath += ".shm";

	while (m_free != 0)
	{
		unsigned char*	addr = m_free;
		if (addr != 0)
			m_free = *((unsigned char**)addr);
		if (addr != 0)
			memcpy ((unsigned char**) addr, g_deadPtr, sizeof addr);
	}

	unlink (datPath.c_str());

	int	fd = open (datPath.c_str(), O_RDWR | O_CREAT, 0666);
	lseek (fd, 0, SEEK_SET);
	if (fd < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create file, open (%s) failed, errno = %d"), datPath.c_str(), errno);
		return;
	}

	for (shmset::iterator it = m_shmset.begin(); it != m_shmset.end(); ++it)
	{
		shmdescr	shm = *it;
		int	shmFd = shm_open (shm.name.c_str(), O_RDONLY, 0666);
		if (shmFd < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot open shm chunk for reading, shm_open (%s) failed, errno = %d"), shm.name.c_str(), errno);
			continue;
		}
		if (sendfile (fd, shmFd, 0, shm.size) != (ssize_t) shm.size)
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot transfer shm chunk '%s' to file '%s', sendfile (%d) failed, errno = %d"), shm.name.c_str(), datPath.c_str(), shm.size, errno);
		close (shmFd);
	}
	close (fd);
}

void	XdbdXmlTableInfo::restoreShmData ()
{
	string	datPath = XdbdXmlDataBase::XdbdDatDataBasePath();
	datPath += '/';
	datPath += m_name;
	datPath += ".shm";

	struct	stat	sbuf;
	if (stat (datPath.c_str(), &sbuf) < 0)
	{
		if (errno != ENOENT)
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("stat (%s) failed, errno = %d"), datPath.c_str(), errno);
		return;
	}

	int	fd = open (datPath.c_str(), O_RDONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("open (%s) failed, errno = %d"), datPath.c_str(), errno);
		return;
	}

	char shmName [64];
	sprintf (shmName, "/xdbd-table-%08d", g_refCount++);
	shm_unlink (shmName);

	int shmFd;
	if ((shmFd = shm_open (shmName, O_RDWR | O_CREAT, 0666)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create shared memory chunk, shm_open (%s) failed, errno = %d"), shmName, errno);
		close (fd);
		return;
	}

	if ((sbuf.st_size % m_resSize) != 0)
	{
		xdbdErrReport (SC_XDBD, SC_WRN, err_info ("file size of '%s' is not multiple of db column size for table '%s'"), datPath.c_str(), m_name.c_str(), errno);
	}

	if (sendfile (shmFd, fd, 0, sbuf.st_size) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot rewrite shared memory chunk '%s', sendfile (%d) failed, errno = %d"), shmName, sbuf.st_size, errno);
		close (fd);
		close (shmFd);
		return;
	}

	if (ftruncate (shmFd, sbuf.st_size) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot resize shared memory chunk '%s', ftruncate (%d) failed, errno = %d"), shmName, sbuf.st_size, errno);
		close (fd);
		close (shmFd);
		return;
	}

	unsigned char* addr = (unsigned char*) mmap (0, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	if (addr == MAP_FAILED)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot map shared memory chunk '%s', mmap (%d) failed, errno = %d"), shmName, sbuf.st_size, errno);
		close (fd);
		close (shmFd);
		return;
	}
	{
		shmdescr	shm = { shmName, (size_t) sbuf.st_size, addr };
		m_shmset.push_back (shm);
	}

	close (shmFd);
	close (fd);

	for (int rowcount = sbuf.st_size / m_resSize; rowcount > 0; --rowcount, addr += m_resSize)
	{
		if (memcmp ((unsigned char**) addr, g_deadPtr, sizeof addr) == 0)
		{
			freeCol (addr);
			continue;
		}
		for (collist::iterator it = m_collist.begin(); it != m_collist.end(); ++it)
		{
			XdbdXmlColumnInfo*	colinfo = *it;
			colinfo->insert (addr);
		}
	}
}

} /* namespace xml_db_daemon */

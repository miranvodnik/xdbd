/*
 * XdbdOdbcStatement.cpp
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#include "XdbdCommon.h"
#include "XdbdOdbcConnection.h"

namespace xml_db_daemon
{

const int	XdbdOdbcStatement::g_maxIdLen;

XdbdOdbcStatement::XdbdOdbcStatement(XdbdOdbcHandle* parent, xdbd_stmt_t p_statement) : XdbdOdbcHandle (parent)
{
	Reset ();
	m_attrAppRowDesc = (SQLHANDLE) ((XdbdOdbcConnection*)parent)->createDescriptor();
	m_attrAppParamDesc = (SQLHANDLE) ((XdbdOdbcConnection*)parent)->createDescriptor();
	m_attrCursorScrollable = SQL_FALSE;
	m_attrCursorSensitivity = SQL_UNSPECIFIED;
	m_attrImpRowDesc = (SQLHANDLE) ((XdbdOdbcConnection*)parent)->createDescriptor();
	m_attrImpParamDesc = (SQLHANDLE) ((XdbdOdbcConnection*)parent)->createDescriptor();
	m_attrMetadataId = SQL_FALSE;
	m_statement = p_statement;
	strncpy ((char*) m_cursorName, "cursor", g_maxIdLen);
}

XdbdOdbcStatement::~XdbdOdbcStatement()
{
	((XdbdOdbcConnection*)parent())->deleteDescriptor((XdbdOdbcDescriptor*) m_attrAppRowDesc);
	((XdbdOdbcConnection*)parent())->deleteDescriptor((XdbdOdbcDescriptor*) m_attrAppParamDesc);
	((XdbdOdbcConnection*)parent())->deleteDescriptor((XdbdOdbcDescriptor*) m_attrImpRowDesc);
	((XdbdOdbcConnection*)parent())->deleteDescriptor((XdbdOdbcDescriptor*) m_attrImpParamDesc);
	Reset ();
}

void XdbdOdbcStatement::Reset ()
{
	m_attrAppRowDesc = SQL_NULL_HDESC;
	m_attrAppParamDesc = SQL_NULL_HDESC;
	m_attrCursorScrollable = SQL_FALSE;
	m_attrCursorSensitivity = SQL_UNSPECIFIED;
	m_attrImpRowDesc = SQL_NULL_HDESC;
	m_attrImpParamDesc = SQL_NULL_HDESC;
	m_attrMetadataId = SQL_FALSE;
	m_statement = 0;
	m_parCount = 0;
	m_colCount = 0;
	m_rowCount = 0;
	m_rowSize = 0;
	m_hdrSize = 0;
	m_rowIndex = 0;
	m_currentRecordSet = 0;
	m_parAtExec = -1;
}

int	XdbdOdbcStatement::LoadDescriptors ()
{
	XdbdStatement*	stmt = (XdbdStatement*) m_statement;
	unsigned char*	addr = stmt->shmAddr();
	int	shmSize = stmt->shmSize();

	if (addr == (unsigned char*) -1)
		return	-1;

	if (shmSize < (int) (6 * sizeof (int)))
		return	-1;

	shmSize  = ((int*)addr) [0];
	int	parCount = ((int*)addr) [1];
	int	parsSize = ((int*)addr) [2];
	int	colCount = ((int*)addr) [3];

	if (shmSize < (((parCount + colCount) * 3) + 6) * (int) sizeof (int) + parsSize)
		return	-1;

	m_currentRecordSet = addr + ((int*)addr)[0];

	addr += 6 * sizeof (int);

	m_parCount = parCount;
	// create application parameter descriptor with empty records
	XdbdOdbcDescriptor*	appParDesc = (XdbdOdbcDescriptor*) m_attrAppParamDesc;
	appParDesc->descCount (parCount + 1);

	// create implementation parameter descriptor and initialize its records
	XdbdOdbcDescriptor*	impParDesc = (XdbdOdbcDescriptor*) m_attrImpParamDesc;
	impParDesc->descCount (parCount + 1);
	for (int i = 0; i < m_parCount; ++i, addr += 3 * sizeof (int))
	{
		impParDesc->descType (i + 1, ((int*)addr)[0]);
		impParDesc->descLength (i + 1, ((int*)addr)[2]);
	}

	m_colCount = colCount;
	// create application columns descriptor with empty records
	XdbdOdbcDescriptor*	appRowDesc = (XdbdOdbcDescriptor*) m_attrAppRowDesc;
	appRowDesc->descCount (colCount + 1);

	// create implementation columns descriptor and initialize its records
	XdbdOdbcDescriptor*	impRowDesc = (XdbdOdbcDescriptor*) m_attrImpRowDesc;
	impRowDesc->descCount (colCount + 1);
	m_rowSize = m_hdrSize = xdbdalign (2 + (m_colCount + 3) / 2, long);
	for (int i = 0; i < m_colCount; ++i, addr += 3 * sizeof (int))
	{
		// remember column data type
		impRowDesc->descType (i + 1, ((int*)addr)[0]);
		// remember column data actually reserved space (>= number of bytes)
		impRowDesc->descLength (i + 1, ((int*)addr)[1]);
		// remember column data byte length
		impRowDesc->descOctetLength (i + 1, ((int*)addr)[2]);
		// remember column data offset within fetched record
		impRowDesc->descDataPtr(i + 1, (SQLPOINTER)(long)m_rowSize);
		m_rowSize += ((int*)addr)[1];
	}

	return	0;
}

int	XdbdOdbcStatement::BindDynamicPars (int parAtExec)
{
	XdbdStatement*	stmt = (XdbdStatement*) m_statement;
	unsigned char*	addr = stmt->shmAddr();
	unsigned char*	pars = xdbdalign (addr + ((m_parCount + m_colCount) * 3 + 6) * sizeof (int), unsigned char*);
	addr += 6 * sizeof (int);

	for (int i = 0; i < parAtExec; ++i, pars += ((int*)addr)[1], addr += 3 * sizeof (int));
	XdbdOdbcDescriptor*	appParamDesc = (XdbdOdbcDescriptor*) m_attrAppParamDesc;
	for (int i = parAtExec; (i < m_parCount) && (i < appParamDesc->descCount()); ++i, addr += 3 * sizeof (int))
	{
		SQLINTEGER	length = (SQLINTEGER) (SQLLEN) appParamDesc->descOctetLengthPtr(i+1);
		if (length == SQL_DATA_AT_EXEC)
		{
			m_parAtExec = i;
			return	SQL_NEED_DATA;
		}
		if (length == SQL_NULL_DATA)
		{
			((int*)addr)[2] = SQL_NULL_DATA;
			pars += ((int*)addr)[1];
			continue;
		}
		else
			((int*)addr)[2] = ((int*)addr)[1];
		SQLPOINTER	parval = appParamDesc->descDataPtr(i+1);
		if (parval == 0)
			return	SQL_ERROR;

		int	ptype = ((int*)addr)[0];
		int	psize = ((int*)addr)[1];
		int	pmaxl = ((int*)addr)[2];
		switch (ptype)
		{
		case	SQL_SMALLINT:
			*((SQLSMALLINT*)pars) = *((SQLSMALLINT*)parval);
			break;
		case	SQL_INTEGER:
			if (sizeof (int) != sizeof (long))
			{
				switch (pmaxl)
				{
				case	sizeof (int):
					*((SQLINTEGER*)pars) = *((SQLINTEGER*)parval);
					break;
				case	sizeof (long):
					*((SQLLEN*)pars) = *((SQLLEN*)parval);
					break;
				default:
					break;
				}
			}
			else
			{
				*((SQLINTEGER*)pars) = *((SQLINTEGER*)parval);
			}
			break;
		case	SQL_VARCHAR:
			memcpy (pars, parval, pmaxl);
			break;
		}
		pars += psize;
	}
	m_parAtExec = appParamDesc->descCount();
	return SQL_SUCCESS;
}

int	XdbdOdbcStatement::Execute ()
{
	int	status;

	m_parAtExec = 0;
	if ((status = BindDynamicPars(m_parAtExec)) != SQL_SUCCESS)
		return	status;
	return ((XdbdStatement*)m_statement)->Execute();
}

int	XdbdOdbcStatement::InitFetch ()
{
	XdbdStatement*	stmt = (XdbdStatement*) m_statement;
	unsigned char*	addr = stmt->shmAddr();

	m_currentRecordSet = addr + ((int*)addr)[0];
	m_rowCount = ((int*)addr)[5];
	m_rowIndex = -1;
	return	0;
}

int	XdbdOdbcStatement::FetchNextRow ()
{
	++m_rowIndex;
	if ((m_currentRecordSet == 0) || (m_rowIndex < 0) || (m_rowIndex >= m_rowCount))
	{
		--m_rowIndex;
		return	SQL_NO_DATA;
	}

	XdbdOdbcDescriptor*	impRowDesc = (XdbdOdbcDescriptor*) m_attrImpRowDesc;
	XdbdOdbcDescriptor*	appRowDesc = (XdbdOdbcDescriptor*) m_attrAppRowDesc;
	unsigned char*	baseData = m_currentRecordSet + m_rowIndex * m_rowSize;
	unsigned char*	data = baseData + m_hdrSize;

	for (int i = 0; i < m_colCount; data += impRowDesc->descLength (i + 1), ++i)
	{
		SQLPOINTER	appDataPtr = appRowDesc->descDataPtr (i + 1);
		if (appDataPtr == 0)
			continue;
		SQLPOINTER	lenPtr = appRowDesc->descOctetLengthPtr(i+1);
		if (chkcolflag (baseData, 2, i, COLNULLVALUE) != 0)
		{
			if (lenPtr != 0)
				*((SQLLEN*)lenPtr) = SQL_NULL_DATA;
		}
		else
		{
			SQLINTEGER	len;
			memcpy (appDataPtr, data, len = appRowDesc->descOctetLength (i + 1));
			if (lenPtr != 0)
				*((SQLLEN*)lenPtr) = (SQLLEN) len;
		}
	}
	return	SQL_SUCCESS;
}

int	XdbdOdbcStatement::FetchRow (SQLSMALLINT FetchOrientation, SQLLEN FetchOffset)
{
	SQLLEN	index = m_rowIndex;

	switch (FetchOrientation)
	{
	case	SQL_FETCH_NEXT:
		break;
	case	SQL_FETCH_PRIOR:
		--index;
		break;
	case	SQL_FETCH_FIRST:
		index = 0;
		break;
	case	SQL_FETCH_LAST:
		index = m_rowCount - 1;
		break;
	case	SQL_FETCH_ABSOLUTE:
		index = FetchOffset;
		break;
	case	SQL_FETCH_RELATIVE:
		index = m_rowIndex + FetchOffset;
		break;
	default:
		index = -1;
	}

	if ((index < 0) || (index >= m_rowCount))
		return	SQL_NO_DATA;

	m_rowIndex = index - 1;
	return	FetchNextRow();
}

int	XdbdOdbcStatement::ParamData (SQLPOINTER* value)
{
	if (m_parAtExec < 0)
	{
		*value = 0;
		return	SQL_ERROR;
	}
	XdbdOdbcDescriptor*	appRowDesc = (XdbdOdbcDescriptor*) m_attrAppParamDesc;
	if (m_parAtExec < appRowDesc->descCount())
	{
		*value = appRowDesc->descDataPtr(m_parAtExec + 1);
		return	SQL_NEED_DATA;
	}
	else
	{
		*value = 0;
		return	SQL_SUCCESS;
	}
}

int	XdbdOdbcStatement::PutData (SQLPOINTER data, SQLLEN length)
{
	int status;

	XdbdOdbcDescriptor*	appRowDesc = (XdbdOdbcDescriptor*) m_attrAppParamDesc;
	if ((m_parAtExec < 0) || (m_parAtExec >= appRowDesc->descCount()))
		return	SQL_ERROR;

	appRowDesc->descDataPtr(m_parAtExec + 1, data);
	appRowDesc->descOctetLengthPtr(m_parAtExec + 1, (SQLPOINTER)length);
	return BindDynamicPars (m_parAtExec++);
}

} /* namespace xml_db_daemon */

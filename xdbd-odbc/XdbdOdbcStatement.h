/*
 * XdbdOdbcStatement.h
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdApi.h"
#include "XdbdStatement.h"
#include "XdbdOdbcDescriptor.h"

namespace xml_db_daemon
{

class XdbdOdbcStatement: public XdbdOdbcHandle
{
public:
	XdbdOdbcStatement(XdbdOdbcHandle* parent, xdbd_stmt_t p_statement);
	virtual ~XdbdOdbcStatement();
	void Reset ();
	int	LoadDescriptors ();
	int	InitFetch ();
	int	FetchNextRow ();
	int	FetchRow (SQLSMALLINT FetchOrientation, SQLLEN FetchOffset);
	int	BindDynamicPars (int parAtExec);
	inline int	BindParam (SQLUSMALLINT parnr, SQLSMALLINT valtype, SQLSMALLINT partype, SQLULEN parlen, SQLSMALLINT scale, SQLPOINTER parval, SQLLEN length)
		{ return ((parnr < 1) || (parnr > m_parCount)) ? -1 : 0; }
	inline int	BindCol (SQLUSMALLINT colnr, SQLSMALLINT coltype, SQLPOINTER colval, SQLLEN collen, SQLLEN* length)
		{ return	((colnr < 1) || (colnr > m_colCount)) ? -1 : 0; }
	int	ParamData (SQLPOINTER* value);
	int	PutData (SQLPOINTER data, SQLLEN length);
	int	Execute ();

	inline SQLHANDLE attrAppRowDesc () { return m_attrAppRowDesc; }
	inline void attrAppRowDesc (SQLHANDLE attrAppRowDesc) { if (m_attrAppRowDesc != SQL_NULL_HDESC) delete (XdbdOdbcHandle*) m_attrAppRowDesc; m_attrAppRowDesc = attrAppRowDesc; }
	inline SQLHANDLE attrAppParamDesc () { return m_attrAppParamDesc; }
	inline void attrAppParamDesc (SQLHANDLE attrAppParamDesc) { if (m_attrAppParamDesc != SQL_NULL_HDESC) delete (XdbdOdbcHandle*) m_attrAppParamDesc; m_attrAppParamDesc = attrAppParamDesc; }
	inline SQLINTEGER attrCursorScrollable () { return m_attrCursorScrollable; }
	inline void attrCursorScrollable (SQLINTEGER attrCursorScrollable) { m_attrCursorScrollable = attrCursorScrollable; }
	inline SQLINTEGER attrCursorSensitivity () { return m_attrCursorSensitivity; }
	inline void attrCursorSensitivity (SQLINTEGER attrCursorSensitivity) { m_attrCursorSensitivity = attrCursorSensitivity; }
	inline SQLHANDLE attrImpRowDesc () { return m_attrImpRowDesc; }
	inline SQLHANDLE attrImpParamDesc () { return m_attrImpParamDesc; }
	inline SQLINTEGER attrMetadataId () { return m_attrMetadataId; }
	inline void attrMetadataId (SQLINTEGER attrMetadataId) { m_attrMetadataId = attrMetadataId; }
	inline xdbd_stmt_t statement () { return m_statement; }
	inline unsigned char* currentRecordSet () { return m_currentRecordSet; }
	inline void setCursorName (SQLCHAR* cursorName, SQLINTEGER length) { strncpy ((char*) m_cursorName, (char*) cursorName, min (g_maxIdLen, length)); m_cursorName[g_maxIdLen-1] = 0; }
	inline void getCursorName (SQLCHAR* cursorName, SQLINTEGER length) { strncpy ((char*) cursorName, (char*) m_cursorName, min (g_maxIdLen, length)); cursorName[length-1] = 0; }
	inline int getCursorLength () { return strnlen ((char*) m_cursorName, g_maxIdLen); }
	inline int closeCursor () { int c = m_cursorName[0]; /* m_cursorName[0] = 0; */ return (c == 0) ? -1 : 0; }
public:
	static const int	g_maxIdLen = 128;
private:
	SQLHANDLE	m_attrAppRowDesc;
	SQLHANDLE	m_attrAppParamDesc;
	SQLINTEGER	m_attrCursorScrollable;
	SQLINTEGER	m_attrCursorSensitivity;
	SQLHANDLE	m_attrImpRowDesc;
	SQLHANDLE	m_attrImpParamDesc;
	SQLINTEGER	m_attrMetadataId;
	xdbd_stmt_t	m_statement;
	SQLCHAR	m_cursorName[g_maxIdLen];
	int	m_parCount;
	int	m_colCount;
	int	m_rowCount;
	int	m_rowSize;
	int	m_hdrSize;
	int	m_rowIndex;
	unsigned char*	m_currentRecordSet;
	int	m_parAtExec;
};

} /* namespace xml_db_daemon */

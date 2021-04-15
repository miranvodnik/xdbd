/*
 * XdbdOdbcDescriptor.h
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdOdbcHandle.h"
#include <vector>
using namespace std;

namespace xml_db_daemon
{

class XdbdOdbcDescriptor: public XdbdOdbcHandle
{
	class Record
	{
	public:
		Record () {}
		Record (const Record&r) { *this = r; }
		~Record () {}
	public:
		SQLSMALLINT	m_descType;
		SQLINTEGER	m_descLength;
		SQLINTEGER	m_descOctetLength;
		SQLSMALLINT	m_descPrecision;
		SQLSMALLINT	m_descScale;
		SQLSMALLINT	m_descDatetimeIntervalCode;
		SQLSMALLINT	m_descNullable;
		SQLCHAR	m_descName[128];
		SQLSMALLINT	m_descUnnamed;
		SQLPOINTER	m_descDataPtr;
		SQLPOINTER	m_descOctetLengthPtr;
		SQLPOINTER	m_descIndicatorPtr;
	};
	typedef vector < Record >	reclist;
public:
	XdbdOdbcDescriptor(XdbdOdbcHandle* parent, SQLSMALLINT descAllocType);
	virtual ~XdbdOdbcDescriptor();
	void Reset ();

	inline void copy (XdbdOdbcDescriptor* src) { m_reclist = src->m_reclist; m_descCount = src->m_descCount; }
	void create (SQLINTEGER index, bool resize);
	inline SQLSMALLINT descCount () { return m_descCount; }
	inline void descCount (SQLSMALLINT descCount) { create (descCount, true); m_descCount = descCount; }
	inline SQLSMALLINT descAllocType () { return m_descAllocType; }
	inline SQLSMALLINT descType (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descType; }
	inline void descType (SQLINTEGER index, SQLSMALLINT descType) { create (index, false); m_reclist[index - 1].m_descType = descType; }
	inline SQLINTEGER descLength (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descLength; }
	inline void descLength (SQLINTEGER index, SQLINTEGER descLength) { create (index, false); m_reclist[index - 1].m_descLength = descLength; }
	inline SQLINTEGER descOctetLength (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descOctetLength; }
	inline void descOctetLength (SQLINTEGER index, SQLINTEGER descOctetLength) { create (index, false); m_reclist[index - 1].m_descOctetLength = descOctetLength; }
	inline SQLSMALLINT descPrecision (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descPrecision; }
	inline void descPrecision (SQLINTEGER index, SQLSMALLINT descPrecision) { create (index, false); m_reclist[index - 1].m_descPrecision = descPrecision; }
	inline SQLSMALLINT descScale (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descScale; }
	inline void descScale (SQLINTEGER index, SQLSMALLINT descScale) { create (index, false); m_reclist[index - 1].m_descScale = descScale; }
	inline SQLSMALLINT descDatetimeIntervalCode (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descDatetimeIntervalCode; }
	inline void descDatetimeIntervalCode (SQLINTEGER index, SQLSMALLINT descDatetimeIntervalCode) { create (index, false); m_reclist[index - 1].m_descDatetimeIntervalCode = descDatetimeIntervalCode; }
	inline SQLSMALLINT descNullable (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descNullable; }
	inline void descNullable (SQLINTEGER index, SQLSMALLINT descNullable) { create (index, false); m_reclist[index - 1].m_descNullable = descNullable; }
	inline SQLCHAR* descName (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descName; }
	inline void descName (SQLINTEGER index, SQLCHAR* descName) { create (index, false); strncpy ((char*) m_reclist[index - 1].m_descName, (char*) descName, 128); }
	inline SQLSMALLINT descUnnamed (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descUnnamed; }
	inline void descUnnamed (SQLINTEGER index, SQLSMALLINT descUnnamed) { create (index, false); m_reclist[index - 1].m_descUnnamed = descUnnamed; }
	inline SQLPOINTER descDataPtr (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descDataPtr; }
	inline void descDataPtr (SQLINTEGER index, SQLPOINTER descDataPtr) { create (index, false); m_reclist[index - 1].m_descDataPtr = descDataPtr; }
	inline SQLPOINTER descOctetLengthPtr (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descOctetLengthPtr; }
	inline void descOctetLengthPtr (SQLINTEGER index, SQLPOINTER descOctetLengthPtr) { create (index, false); m_reclist[index - 1].m_descOctetLengthPtr = descOctetLengthPtr; }
	inline SQLPOINTER descIndicatorPtr (SQLINTEGER index) { create (index, false); return m_reclist[index - 1].m_descIndicatorPtr; }
	inline void descIndicatorPtr (SQLINTEGER index, SQLPOINTER descIndicatorPtr) { create (index, false); m_reclist[index - 1].m_descIndicatorPtr = descIndicatorPtr; }
private:
	SQLSMALLINT	m_descCount;
	SQLSMALLINT	m_descAllocType;
	reclist	m_reclist;
};

} /* namespace xml_db_daemon */

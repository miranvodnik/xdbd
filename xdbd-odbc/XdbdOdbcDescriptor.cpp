/*
 * XdbdOdbcDescriptor.cpp
 *
 *  Created on: 14. dec. 2015
 *      Author: miran
 */

#include "XdbdOdbcDescriptor.h"

namespace xml_db_daemon
{

XdbdOdbcDescriptor::XdbdOdbcDescriptor(XdbdOdbcHandle* parent, SQLSMALLINT descAllocType) : XdbdOdbcHandle (parent)
{
	Reset ();
	m_descAllocType = descAllocType;
}

XdbdOdbcDescriptor::~XdbdOdbcDescriptor()
{
	m_reclist.clear();
	Reset ();
}

void XdbdOdbcDescriptor::Reset ()
{
	m_descCount = 0;
	m_descAllocType = 0;
}

void XdbdOdbcDescriptor::create (SQLINTEGER index, bool resize)
{
	for (SQLSMALLINT i = m_descCount; i < index; ++i)
	{
		Record	r;
		memset (&r, 0, sizeof r);
		m_reclist.push_back(r);
	}
	if (!resize)
		return;
	for (SQLSMALLINT i = index; i < m_descCount; ++i)
		m_reclist.pop_back();
}

} /* namespace xml_db_daemon */

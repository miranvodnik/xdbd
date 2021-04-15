/*
 * XdbdXmlColumnInfo.cpp
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <string.h>
#include "XdbdXmlColumnInfo.h"
#include "XdbdXmlDataBase.h"
#include <iostream>
#include <sstream>
#include <climits>
using namespace std;

namespace xml_db_daemon
{

XdbdXmlColumnInfo::XdbdXmlColumnInfo(const char* name, const char* vtype, const char* defVal, int minOccurs, int maxOccurs, int position, int minLength, int maxLength, int minValue, int maxValue, int index)
{
	m_name = name;
	m_type = vtype;
	if (!(m_nullDefVal = ((defVal == 0) || (strncasecmp (defVal, "null", 4) == 0))))
		m_defVal = defVal;
	m_minOccurs = minOccurs;
	m_maxOccurs = maxOccurs;
	m_position = position;
	m_minLength = minLength;
	m_maxLength = maxLength;
	if ((m_minValue = minValue) > maxValue)
		m_minValue = maxValue;
	if ((m_maxValue = maxValue) < minValue)
		m_maxValue = minValue;
	m_index = index;
	if (maxLength < 0)
	{
		if (strcmp (vtype, "xs:short") == 0)
			m_minLength = m_maxLength = sizeof (short), m_colType = SQL_SMALLINT;
		else	if (strcmp (vtype, "xs:int") == 0)
			m_minLength = m_maxLength = sizeof (int), m_colType = SQL_INTEGER;
		else	if (strcmp (vtype, "xs:long") == 0)
			m_minLength = m_maxLength = sizeof (long), m_colType = SQL_INTEGER;
		else
			m_minLength = m_maxLength = sizeof (long), m_colType = SQL_INTEGER;
	}
	else
		m_colType = SQL_VARCHAR;
	m_colset = new colset < XdbdXmlColumnInfo > (this);
	m_realLength = xdbdalign (m_maxLength, long);

	m_nullable = (m_minOccurs == 0);
	m_pkey = false;
	m_true = (strcasecmp (name, "nodeid") == 0) ? XdbdXmlDataBase::IgnoreNodeid() : false;
	for (m_autoInc = false; (name = strstr (name, "_id")) != 0; name += 3)
		m_autoInc = (strcmp (name, "_id") == 0);
	m_autoVal = new unsigned char [m_maxLength];
}

XdbdXmlColumnInfo::~XdbdXmlColumnInfo()
{
	delete	[] m_autoVal;
}

void	XdbdXmlColumnInfo::DisplayValue (const char* name, const unsigned char* __x)
{
	stringstream	str;
	unsigned char* __ux = columnToRecord ((unsigned char*) __x);

	str << name << " = ";
	if (chkcolflag (__ux, 2, index(), COLNULLVALUE) != 0)
		str << " null";
	else
		switch (m_colType)
		{
		case	SQL_SMALLINT:
			str << *((SQLSMALLINT*)__x);
			break;
		case	SQL_INTEGER:
			if (sizeof (int) != sizeof (long))
			{
				switch (m_maxLength)
				{
				case	sizeof (int):
					str << *((SQLINTEGER*)__x);
					break;
				case	sizeof (long):
					str << *((SQLLEN*)__x);
					break;
				default:
					str << "NaN";
					break;
				}
			}
			else
				str << *((SQLINTEGER*)__x);
			break;
		case	SQL_VARCHAR:
			str << (const char*)__x;
			break;
		default:
			str << "NaN";
			break;
		}
	str << endl << ends;

	const char*	msg = str.str().c_str();
	xdbdErrReport( SC_XDBD, SC_ERR, msg);
}

void	XdbdXmlColumnInfo::uniqueAutoValue (unsigned char* ptr)
{
	colidx	end = m_colset->end();
	if (m_colset->begin() != end)
		--end;
	if (end != m_colset->end())
	{
		memcpy (m_autoVal, recordToColumn (end->second), m_maxLength);
		switch (m_colType)
		{
		case	SQL_SMALLINT:
			(*((SQLSMALLINT*) m_autoVal)) ++;
			break;
		case	SQL_INTEGER:
			if (sizeof (int) != sizeof (long))
			{
				switch (m_maxLength)
				{
				case	sizeof (int):
					(*((SQLINTEGER*) m_autoVal)) ++;
					break;
				case	sizeof (long):
					(*((SQLLEN*) m_autoVal)) ++;
					break;
				default:
					(*((SQLINTEGER*) m_autoVal)) ++;
					break;
				}
			}
			else
				(*((SQLINTEGER*) m_autoVal)) ++;
			break;
		case	SQL_VARCHAR:
			incAutoBuffer (m_autoVal, m_maxLength);
			break;
		default:
			(*((SQLINTEGER*) m_autoVal)) ++;
			break;
		}
	}
	else
	{
		switch (m_colType)
		{
		case	SQL_SMALLINT:
			(*((SQLSMALLINT*) m_autoVal)) = 1;
			break;
		case	SQL_INTEGER:
			if (sizeof (int) != sizeof (long))
			{
				switch (m_maxLength)
				{
				case	sizeof (int):
					(*((SQLINTEGER*) m_autoVal)) = 1;
					break;
				case	sizeof (long):
					(*((SQLLEN*) m_autoVal)) = 1;
					break;
				default:
					(*((SQLINTEGER*) m_autoVal)) = 1;
					break;
				}
			}
			else
				(*((SQLINTEGER*) m_autoVal)) = 1;
			break;
		case	SQL_VARCHAR:
			for (int i = 0; i < m_maxLength; ++i)
				m_autoVal[i] = '0';
			break;
		default:
			(*((SQLINTEGER*) m_autoVal)) = 1;
			break;
		}
	}
	memcpy (ptr, m_autoVal, m_maxLength);
}

void	XdbdXmlColumnInfo::incAutoBuffer (unsigned char* autoBuffer, int len)
{
	if (len <= 0)
		return;
	--len;
	if (++(autoBuffer[len]) <= '9')
		return;
	autoBuffer[len] = '0';
	incAutoBuffer(autoBuffer, len);
}

bool XdbdXmlColumnInfo::update (unsigned char* dst, unsigned char* src)
{
	memcpy (dst + m_position, src + m_position, m_maxLength);
	colrng	rng = m_colset->equal_range(dst + m_position);
	colidx	it;
	for (it = rng.first; (it != rng.second) && (it->second != dst); ++it);
	if (it == rng.second)
		return	false;
	m_colset->erase(it);
	insert (dst + m_position, dst);
	return	true;
}

bool XdbdXmlColumnInfo::insert (unsigned char* dst)
{
	insert (dst + m_position, dst);
	return	true;
}

bool XdbdXmlColumnInfo::dilite (unsigned char* dst)
{
	colrng	rng = m_colset->equal_range(dst + m_position);
	colidx	it;
	for (it = rng.first; (it != rng.second) && (it->second != dst); ++it);
	if (it == rng.second)
		return	false;
	m_colset->erase(it);
	return	true;
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::eq_range (unsigned char* data)
{
	return m_colset->equal_range(data);
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::ne_range (unsigned char* data)
{
	return m_colset->equal_range(data);
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::lt_range (unsigned char* data)
{
	if (m_colset->empty())
		return make_pair (m_colset->end(), m_colset->end());
	return make_pair (m_colset->begin(), m_colset->lower_bound(data));
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::le_range (unsigned char* data)
{
	if (m_colset->empty())
		return make_pair (m_colset->end(), m_colset->end());
	return make_pair (m_colset->begin(), m_colset->upper_bound(data));
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::gt_range (unsigned char* data)
{
	if (m_colset->empty())
		return make_pair (m_colset->end(), m_colset->end());
	return make_pair (m_colset->upper_bound(data), m_colset->end());
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::ge_range (unsigned char* data)
{
	if (m_colset->empty())
		return make_pair (m_colset->end(), m_colset->end());
	return make_pair (m_colset->lower_bound(data), m_colset->end());
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::like_range (unsigned char* data)
{
	return make_pair (m_colset->begin(), m_colset->end());
}

XdbdXmlColumnInfo::colrng XdbdXmlColumnInfo::no_range (unsigned char* data)
{
	return make_pair (m_colset->end(), m_colset->end());
}

bool	XdbdXmlColumnInfo::find (unsigned char* data)
{
	return	m_colset->find (data) != m_colset->end ();
}

} /* namespace xml_db_daemon */

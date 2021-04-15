/*
 * XdbdXmlColumnInfo.h
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sqlxdbd.h>
#include <string.h>
#include "XdbdCommon.h"
#include <string>
#include <map>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

template < typename T > class xshortcmp : public less < T >
{
public:
	xshortcmp (size_t size = sizeof (short)) : m_size (size) {}
    bool operator()(const T& __x, const T& __y) const { return *((short*)__x) < *((short*)__y); }
private:
	size_t	m_size;
};

template < typename T > class xintcmp : public less < T >
{
public:
	xintcmp (size_t size = sizeof (int)) : m_size (size) {}
    bool operator()(const T& __x, const T& __y) const { return *((int*)__x) < *((int*)__y); }
private:
	size_t	m_size;
};

template < typename T > class xlongcmp : public less < T >
{
public:
	xlongcmp (size_t size = sizeof (long)) : m_size (size) {}
    bool operator()(const T& __x, const T& __y) const { return *((long*)__x) < *((long*)__y); }
private:
	size_t	m_size;
};

template < typename T > class xstringcmp : public less < T >
{
public:
	xstringcmp (size_t size) : m_size (size) {}
    bool operator()(const T& __x, const T& __y) const { return strncmp ((const char*)__x, (const char*)__y, m_size); }
private:
	size_t	m_size;
};

template < typename T > class xblobcmp : public less < T >
{
public:
	xblobcmp (size_t size) : m_size (size) {}
    bool operator()(const T& __x, const T& __y) const { return memcmp ((const char*)__x, (const char*)__y, m_size); }
private:
	size_t	m_size;
};

template < typename T, typename U > class colcmp : public less < T >
{
public:
	colcmp (U* colinfo) : m_colinfo (colinfo), m_size (colinfo->maxLength()), m_ctype (colinfo->colType()) {}
    inline bool operator()(const T& __x, const T& __y) const
	{
    	T	__ux = m_colinfo->columnToRecord (__x);
    	T	__uy = m_colinfo->columnToRecord (__y);

#if defined(_DEBUG)
    	m_colinfo->DisplayValue ("x", __x);
    	m_colinfo->DisplayValue ("y", __y);
#endif

    	if (chkcolflag (__uy, 2, m_colinfo->index(), COLNULLVALUE) != 0)
			return	false;
		if (chkcolflag (__ux, 2, m_colinfo->index(), COLNULLVALUE) != 0)
			return true;

    	switch (m_ctype)
    	{
    	case	SQL_SMALLINT:
    		return	*((SQLSMALLINT*)__x) < *((SQLSMALLINT*)__y);
    		break;
    	case	SQL_INTEGER:
    		if (sizeof (int) != sizeof (long))
    		{
    			switch (m_size)
    			{
    			case	sizeof (int):
    	    		return	*((SQLINTEGER*)__x) < *((SQLINTEGER*)__y);
    				break;
    			case	sizeof (long):
    	    		return	*((SQLLEN*)__x) < *((SQLLEN*)__y);
    				break;
    			default:
    				return	false;
    				break;
    			}
    		}
    		else
        		return	*((SQLINTEGER*)__x) < *((SQLINTEGER*)__y);
    		break;
    	case	SQL_VARCHAR:
    		return	strncmp ((const char*)__x, (const char*)__y, m_size) < 0;
    		break;
    	}
    	return	false;
    }
private:
    U*	m_colinfo;
	size_t	m_size;
	int	m_ctype;
};

template < typename T > class colset : public multimap < unsigned char*, unsigned char*, colcmp < unsigned char*, T > >
{
public:
	typedef colcmp < unsigned char*, T >	cmpcol;
	typedef multimap < unsigned char*, unsigned char*, cmpcol >	colmap;
public:
	colset (T* t) : colmap (cmpcol (t)) {}
};

class XdbdXmlColumnInfo
{
public:
	typedef colset < XdbdXmlColumnInfo > ::iterator	colidx;
	typedef pair < colidx, colidx >	colrng;
	XdbdXmlColumnInfo(const char* name, const char* vtype, const char* defVal, int minOccurs, int maxOccurs, int position, int minLength, int maxLength, int minValue, int maxValue, int index);
	virtual ~XdbdXmlColumnInfo();
	void	DisplayValue (const char* name, const unsigned char* __x);
	inline string name() { return m_name; }
	inline string vtype() { return m_type; }
	inline int	minOccurs () { return m_minOccurs; }
	inline int	maxOccurs () { return m_maxOccurs; }
	inline bool	nullable () { return m_nullable; }
	inline bool	primary () { return m_pkey; }
	inline void primary (bool primary) { m_pkey = primary; }
	inline bool isTrue () { return m_true; }
	inline bool	autoInc () { return m_autoInc; }
	inline int position() { return m_position; }
	inline void	alignPosition (int position) { m_position += position; }
	inline int	minLength() { return m_minLength; }
	inline int	maxLength() { return m_maxLength; }
	inline int	minValue() { return m_minValue; }
	inline int	maxValue() { return m_maxValue; }
	inline int	realLength() { return m_realLength; }
	inline int	colType() { return m_colType; }
	inline int	index () { return m_index; }
	inline bool	nullDefValue () { return m_nullDefVal; }
	inline string	defVal () { return m_defVal; }
	inline unsigned char*	columnToRecord (unsigned char* coladdr) { return coladdr - m_position; }
	inline unsigned char*	recordToColumn (unsigned char* recaddr) { return recaddr + m_position; }
	inline unsigned char*	columnToColumn (unsigned char* coladdr, XdbdXmlColumnInfo* colinfo) { return coladdr - m_position + colinfo->position(); }
	inline void insert (unsigned char* key, unsigned char* val) { if (m_pkey || XdbdCommon::indexAll()) m_colset->insert (colset < XdbdXmlColumnInfo > ::value_type (make_pair(key, val))); }
	bool update (unsigned char* dst, unsigned char* src);
	bool insert (unsigned char* dst);
	bool dilite (unsigned char* dst);	// delete is reserved word
	colrng eq_range (unsigned char* data);
	colrng ne_range (unsigned char* data);
	colrng lt_range (unsigned char* data);
	colrng le_range (unsigned char* data);
	colrng gt_range (unsigned char* data);
	colrng ge_range (unsigned char* data);
	colrng like_range (unsigned char* data);
	colrng no_range (unsigned char* data);
	bool find (unsigned char* data);
	inline colidx begin () { return m_colset->begin(); }
	inline colidx end () { return m_colset->end(); }
	int check (colrng rng)
	{
		int cnt;
		colidx idx;
		for (cnt = 0, idx = rng.first; idx != rng.second; ++idx, ++cnt);
		return	cnt;
	}
	void	incAutoBuffer (unsigned char* autoBuffer, int len);
	void	uniqueAutoValue (unsigned char* ptr);
private:
	string	m_name;
	string	m_type;
	string	m_defVal;
	int	m_minOccurs;
	int	m_maxOccurs;
	int	m_position;
	int	m_minLength;
	int	m_maxLength;
	int	m_minValue;
	int	m_maxValue;
	int	m_realLength;
	int	m_colType;
	int	m_index;
	unsigned char*	m_autoVal;

	bool	m_nullDefVal;
	bool	m_nullable;
	bool	m_pkey;
	bool	m_true;
	bool	m_autoInc;
	colset < XdbdXmlColumnInfo > *	m_colset;
};

} /* namespace xml_db_daemon */

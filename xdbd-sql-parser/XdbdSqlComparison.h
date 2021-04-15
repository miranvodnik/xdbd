/*
 * XdbdSqlComparison.h
 *
 *  Created on: 10. dec. 2015
 *      Author: miran
 */

#pragma once

#include "sql_statement.h"
#include "XdbdXmlTableInfo.h"
#include "XdbdSqlResultInfo.h"

#include <string>
#include <deque>
#include <vector>
#include <set>
using namespace std;

namespace xml_db_daemon
{

class XdbdSqlComparison
{
public:
	XdbdSqlComparison (XdbdXmlColumnInfo::colrng range, XdbdXmlTableInfo* tabinfo, XdbdXmlColumnInfo* colinfo, unsigned char* data, size_t size, SqlSyntaxContext sqlSyntaxContext, bool invert):
		m_range (range), m_sqlSyntaxContext (sqlSyntaxContext), m_tabinfo (tabinfo), m_colinfo (colinfo), m_data (data), m_size (size), m_invert (((colinfo == 0) || colinfo->isTrue()) ? false : invert)
	{
		if (m_invert)
			m_colidx = m_colinfo->begin();
		else
			m_colidx = range.first;
		m_resultCount = 0;
		m_low = true;
//		for (XdbdXmlColumnInfo::colidx idx = range.first; idx != range.second; ++idx, ++m_resultCount);
	}
	~XdbdSqlComparison () { if (m_data != 0) delete [] m_colinfo->columnToRecord(m_data); m_data = 0;}
	inline int resultCount () { return m_resultCount; }
	inline XdbdXmlColumnInfo::colrng range () { return m_range; }
	inline void range (XdbdXmlColumnInfo::colrng range) { m_range = range; m_colidx = range.first; }
	inline SqlSyntaxContext sqlSyntaxContext () { return m_sqlSyntaxContext; }
	inline XdbdXmlTableInfo* tabinfo () { return m_tabinfo; }
	inline XdbdXmlColumnInfo* colinfo () { return m_colinfo; }
	inline unsigned char* data () { return m_data; }
	inline void data (unsigned char* data) { if (m_data != 0) delete [] m_colinfo->columnToRecord(m_data); m_data = data; }
	inline size_t size () { return m_size; }
	inline bool invert () { return m_invert; }
	bool incIndex ()
	{
		if (m_invert)
		{
			while (true)
			{
				if (m_low)
				{
					if (m_colidx == m_range.first)
					{
						m_low = false;
						m_colidx = m_range.second;
						continue;
					}
				}
				else
				{
					if (m_colidx == m_colinfo->end())
						return	false;
				}
				break;
			}
		}
		else
		{
			if (m_colidx == m_range.second)
				return false;
		}
		if (m_colidx == m_colinfo->end())
			return	false;
		++m_colidx;
		++m_resultCount;
		return true;
	}
private:
	XdbdXmlColumnInfo::colrng	m_range;
	SqlSyntaxContext m_sqlSyntaxContext;
	XdbdXmlTableInfo*	m_tabinfo;
	XdbdXmlColumnInfo*	m_colinfo;
	unsigned char*	m_data;
	size_t	m_size;
	XdbdXmlColumnInfo::colidx	m_colidx;
	int	m_resultCount;
	bool	m_invert;
	bool	m_low;
};

typedef	enum
{
	NoExprPrecedence,
	OrExprPrecedence,
	AndExprPrecedence,
	CompExprPrecedence,
	NotExprPrecedence,
}	ExprPrecedence;

typedef deque < unsigned long >	cmpset;
typedef	pair < cmpset::size_type, cmpset::size_type >	cmpsize;
typedef	deque < cmpsize >	cmpptr;
typedef pair < string, ExprPrecedence >	exprpair;
typedef deque < exprpair >	strstack;
typedef	pair < XdbdXmlColumnInfo*, sql_constant* >	cvalrec;
typedef vector < cvalrec >	cvallist;
typedef vector < XdbdXmlTableInfo* >	tablist;
typedef vector < sql_column* >	collist;
typedef vector < XdbdSqlResultInfo* >	reslist;
typedef vector < XdbdSqlComparison* >	compset;

} /* namespace xml_db_daemon */

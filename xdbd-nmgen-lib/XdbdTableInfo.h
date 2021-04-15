/*
 * XdbdTableInfo.h
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#pragma once

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "XdbdColumnInfo.h"

#include <vector>
#include <map>
using namespace std;

namespace xml_db_daemon
{

/*! @brief DB table info
 *
 *  holds collection of column descriptions and defines
 *  some simple functions to access internal structures
 *
 */
class XdbdTableInfo
{
public:
	typedef vector < XdbdColumnInfo* >	collist;	//!< type: list of column references
	typedef collist::iterator	col_iterator;	//!< iterator through column list
	XdbdTableInfo (const char* name);
	virtual ~XdbdTableInfo();
	void	AlignColPositions ();
	inline void add (XdbdColumnInfo* col) { m_collist.push_back(col); }	//!< save reference to column descriptor
	XdbdColumnInfo* find (string name);
	inline string name () { return m_name; }	//!< get table name
	inline col_iterator col_begin () { return m_collist.begin(); }	//!< get beginning of column list
	inline col_iterator col_end () { return m_collist.end(); }	//!< get end of column list
	inline col_iterator col_next (col_iterator it) { return ++it; }	//!< iterate through column list
	inline int	resCount () { return m_collist.size(); }	//!< get number of columns
	inline int	resSize () { return m_resSize; }	//!< get size of raw columns line value
	inline int	colhdrSize () { return m_colhdrSize; }	//!< get header size of row columns line value
private:
	string	m_name;	//!< table name
	collist	m_collist;	//!< column list
	u_int	m_colhdrSize;	//!< row columns line header size
	u_int	m_resSize;	//!< row columns line size
};

} /* namespace xml_db_daemon */

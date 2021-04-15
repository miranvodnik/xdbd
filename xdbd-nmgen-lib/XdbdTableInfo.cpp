/*
 * XdbdTableInfo.cpp
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include "XdbdTableInfo.h"
#include <string.h>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief XML table info constructor
 *
 *  it remembers table name and initializes internal
 *  data structures
 *
 *  @param name DB table name
 *
 */
XdbdTableInfo::XdbdTableInfo (const char* name)
{
	m_name = name;
	m_colhdrSize = 0;
	m_resSize = 0;
}

/*! @brief XML table info destructor
 *
 *  it releases collection of column info objects
 *
 */
XdbdTableInfo::~XdbdTableInfo()
{
	for (collist::iterator it = m_collist.begin(); it != m_collist.end(); ++it)
		delete	*it;
	m_collist.clear();
}

/*! @brief find column
 *
 *  given column name find reference to its descriptor
 *
 *  @param name column name
 *
 *  @return 0 column with that name does not exists
 *  @return other reference to XdbdColumnInfo instance
 *
 */
XdbdColumnInfo* XdbdTableInfo::find (string name)
{
	for (collist::iterator it = m_collist.begin(); it != m_collist.end(); ++it)
	{
		XdbdColumnInfo*	colinfo = *it;
		if (colinfo->name() == name)
			return	colinfo;
	}
	return	0;
}

/*! @brief align column positions
 *
 *  align all column values so that their positions will be
 *  multiples of sizeof(long)
 *
 */
void	XdbdTableInfo::AlignColPositions ()
{
	m_resSize = m_colhdrSize = xdbdalign (2 + (m_collist.size() + 3) / 2, long);
	for (collist::iterator it = m_collist.begin(); it != m_collist.end(); ++it)
	{
		(*it)->alignPosition (m_colhdrSize);
		m_resSize += (*it)->realLength();
	}
}

} /* namespace xml_db_daemon */

/*
 * XdbdColumnInfo.cpp
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#include "XdbdColumnInfo.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief create an instance of column info
 *
 *  it collects informations gathered from XSD description of some
 *  XML DB column
 *
 *  @param name column name gathered from <xs:element name> attribute
 *  @param vtype XML data type gathered from <xs:restriction base> attribute
 *  @param minOccurs minimal number of occurrences of column in table gathered
 *  from <xs:element minOccurs> attribute. Reasonable values are 0 and 1.
 *  @param maxOccurs maximal number of occurrences of column in table gathered
 *  from <xs:element maxOccurs> attribute. Reasonable values are 0 and 1.
 *  @param position column data position within field holding values of all
 *  columns of given table. This parameter is deduced by algorithm reading
 *  column info. It is accumulated value and depends on positions of all
 *  previous columns
 *  @param minLength minimal length of some columns (string data). Lengths
 *  of primitive data columns are deduced from their type (integer, long)
 *  @param maxLength maximal length of some columns (string data). Lengths
 *  of primitive data columns are deduced from their type (integer, long)
 *  @param index sequence number of column within given table
 *
 */
XdbdColumnInfo::XdbdColumnInfo(const char* name, const char* vtype, int minOccurs, int maxOccurs, int position, int minLength, int maxLength, int index)
{
	m_name = name;
	m_type = vtype;
	m_minOccurs = minOccurs;
	m_maxOccurs = maxOccurs;
	m_position = position;
	m_minLength = minLength;
	m_maxLength = maxLength;
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
	m_realLength = xdbdalign (m_maxLength, long);
	m_nullable = (minOccurs <= 0);
}

/*! @brief destructor does nothing special
 *
 */
XdbdColumnInfo::~XdbdColumnInfo()
{
}

} /* namespace xml_db_daemon */

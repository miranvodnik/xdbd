/*
 * XdbdColumnInfo.h
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sqlxdbd.h>
#include <string.h>
#include <string>
#include <map>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

#define	xdbdalign(A,T)	(T) ((((long) (A)) + (sizeof (T) - 1)) & ~(sizeof (T) - 1))

/*! @brief XML DB column info class for trigger source code generator
 *
 *  this class is used by trigger source code generator to correctly
 *  generate and optimize C code functions which handle DB trigger events
 *
 */
class XdbdColumnInfo
{
public:
	XdbdColumnInfo(const char* name, const char* vtype, int minOccurs, int maxOccurs, int position, int minLength, int maxLength, int index);
	virtual ~XdbdColumnInfo();
	inline string name() { return m_name; }	//!< get column name
	inline string vtype() { return m_type; }	//!< get column XML type
	inline int position() { return m_position; }	//!< get column position within table
	inline void	alignPosition (int position) { m_position += position; }	//!< align column position to fit associated data structure
	inline int	minLength() { return m_minLength; }	//!< get minimal length of column value
	inline int	maxLength() { return m_maxLength; }	//!< get maximal length of column value
	inline int	realLength() { return m_realLength; }	//!< get real length of column value
	inline int	colType() { return m_colType; }	//!< get SQL column type
	inline int	index () { return m_index; }	//!< get column index
	inline bool	nullable () { return m_nullable; }	//!< can have null value
private:
	string	m_name;	//!< column name
	string	m_type;	//!< column XML type
	int	m_minOccurs;	//!< minimal number of column occurrences
	int	m_maxOccurs;	//!< maximal number of column occurrences
	int	m_position;	//!< column position within table
	int	m_minLength;	//!< column data minimal length
	int	m_maxLength;	//!< column data maximal length
	int	m_realLength;	//!< column data real length
	int	m_colType;	//!< column SQL type
	int	m_index;	//!< column sequence number

	bool	m_nullable;	//!< can have null value
};

} /* namespace xml_db_daemon */

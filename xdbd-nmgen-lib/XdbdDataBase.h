/*
 * XdbdDataBase.h
 *
 *  Created on: 14. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdTableInfo.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace xml_db_daemon
{

typedef map < string, XdbdTableInfo* >	docmap;	//!< one to one (table name) <--> (table) mapping
typedef multimap < string, string >	tabmap;	//!< one to many (xsd file name) <--> (table name) mapping
typedef	pair < tabmap::iterator, tabmap::iterator >	tabrng;	//!< range of tables defined in given xsd file

/*! @brief 'database' of table and column info
 *
 *  it holds two lists:
 *  - list of table descriptions specified by corresponding XSD
 *  files. Every table description contains descriptions of all
 *  its columns
 *  - list of mappings between XSD file and table names
 */
class XdbdDataBase
{
private:
	XdbdDataBase();
	virtual ~XdbdDataBase();
public:
	/*! @brief load all XSD files
	 *
	 *  read all XSD files from specified directory and load
	 *  specifications of all tables and their columns into
	 *  internal data structures
	 *
	 *  @param xsdPath directory containing XSD files
	 *
	 *  @return 0 XSD files loaded
	 *  @return -1 cannot open specified directory
	 *
	 */
	inline static int	LoadXsdFiles (char* xsdPath) { return g_XdbdDataBase->_LoadXsdFiles (xsdPath); }

	/*! @brief acquire table info given its name
	 *
	 *  @param name table name
	 *
	 *  @return 0 table info cannot be retrieved probably because of
	 *  unknown table name
	 *  @return other reference to an instance of table info
	 *
	 */
	inline static XdbdTableInfo*	GetTableInfo (string name) { return g_XdbdDataBase->_GetTableInfo(name); }
	static inline docmap getdocmap () { return g_XdbdDataBase->m_docmap; }	//!< get document map reference
private:
	int	_LoadXsdFiles (char* xsdPath);
	XdbdTableInfo* _GetTableInfo (string  name);
	xmlAttrPtr	findAttribute (xmlNodePtr node, const char*path[], const char*name);
private:
	static string	g_XdbdNsPrefix;
	static string	g_XdbdNsUri;
	static XdbdDataBase*	g_XdbdDataBase;
	docmap	m_docmap;	//!< document map: mapping between table names and their info
	tabmap	m_tabmap;	//!< mapping between XSD file names and table names
	XdbdTableInfo*	m_currentTableInfo;	//!< current table info record
	XdbdColumnInfo*	m_currentColumnInfo;	//!< current column info record
};

} /* namespace xml_db_daemon */

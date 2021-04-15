/*
 * XdbdNMGenSystemTables.cpp
 *
 *  Created on: 25. feb. 2016
 *      Author: miran
 */

#include <unistd.h>
#include "XdbdNMGenSystemTables.h"

namespace xml_db_daemon
{

/*! @brief constructor of system table generator
 *
 *  it saves reference to array of controlling parameters
 *  and their number. Since parameters are not copied into
 *  internal structures of this object, application must
 *  guarantee that they are accessible through the whole
 *  life cycle of this object
 *
 */
XdbdNMGenSystemTables::XdbdNMGenSystemTables (int n, char*p [])
{
	m_n = n;
	m_p = p;
}

/*! @brief destructor of system table generator
 *
 */
XdbdNMGenSystemTables::~XdbdNMGenSystemTables ()
{
}

/*! @brief generate system tables
 *
 *  function creates two files named system.tables.xml and
 *  system.columns.xml. They contain definitions of tables
 *  and their columns used in XML data base. Function is
 *  controlled by parameters referenced by m_p. They should
 *  specify xsd files which contain XSD specifications which
 *  will be used to createXML DB files. Parameters are:
 *  - xsd <file-name>. it specifies name of XSD file which
 *  contains XSD definitions of any number of DB tables. There
 *  should be any number of these options
 *  - xml <directory-name> it specifies directory which will
 *  be used to store generated files, system.tables.xml and
 *  system.columns.xml.
 *
 *  @return 0 always. If there are any errors they are reported
 *  to console
 *
 */
int	XdbdNMGenSystemTables::GenerateSystemTables ()
{
	if (m_n < 2)
	{
		cerr << "Usage: " << m_p [0] << " [--xsd <xml-xsd-dir>] [--xml <xml-out-dir>]\n"
			<< endl;
		return 0;
	}

	bool xsdLoaded = false;
	string xmlDir = "";

	xmlInitParser ();

	for (int i = 1; i < m_n; ++i)
	{
		char* par;
		if (*(par = m_p [i]) == '-')
		{
			if (++i >= m_n)
				break;
			if (strcmp (par, "--xsd") == 0)
			{
				XdbdDataBase::LoadXsdFiles (m_p [i]);
				xsdLoaded = true;
			}
			else if (strcmp (par, "--xml") == 0)
			{
				xmlDir = m_p [i];
			}
			else
				cerr << "unknown option " << par << endl;
		}
	}

	ofstream	tabFile;
	ofstream	colFile;
	string	tabFileName = xmlDir + "/system.tables.xml";
	string	colFileName = xmlDir + "/system.columns.xml";
	tabFile.open(tabFileName.c_str());
	colFile.open(colFileName.c_str());

	if (!tabFile.is_open())
	{
		cerr << "Cannot open system.table file '" << tabFileName << "'" << endl;
		return	0;
	}

	if (!colFile.is_open())
	{
		cerr << "Cannot open system.columns file '" << colFileName << "'" << endl;
		return	0;
	}

	tabFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	tabFile << "<system.tables_root xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='system.tables.xsd'>" << endl;

	colFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	colFile << "<system.columns_root xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='system.columns.xsd'>" << endl;

	int	tabIndex = 0;
	int	colIndex = 0;
	docmap	dm = XdbdDataBase::getdocmap();
	docmap::iterator	bit = dm.begin();
	docmap::iterator	eit = dm.end();
	for (docmap::iterator dit = bit; dit != eit; ++dit)
	{
		XdbdTableInfo*	tabinfo = dit->second;
		if (tabinfo->name().find ("system.", 0) != string::npos)
			continue;
		++tabIndex;
		tabFile << "  <system.tables>" << endl;
		tabFile << "    <table_id>" << tabIndex << "</table_id>" << endl;
		tabFile << "    <table_name>" << tabinfo->name() << "</table_name>" << endl;
		tabFile << "    <table_type>" << "BASE TABLE" << "</table_type>" << endl;
		tabFile << "    <schema_id>" << 1 << "</schema_id>" << endl;
		tabFile << "    <remarks>" << tabinfo->name() << "</remarks>" << endl;
		tabFile << "  </system.tables>" << endl;

		int	ordinal = 0;
		for (XdbdTableInfo::col_iterator cit = tabinfo->col_begin(); cit != tabinfo->col_end(); ++cit)
		{
			XdbdColumnInfo*	colinfo = *cit;
			++ordinal;
			++colIndex;
			colFile << "  <system.columns>" << endl;
			colFile << "    <column_id>" << colIndex << "</column_id>" << endl;
			colFile << "    <column_name>" << colinfo->name() << "</column_name>" << endl;
			colFile << "    <table_id>" << tabIndex << "</table_id>" << endl;
			colFile << "    <data_type>" << colinfo->colType() << "</data_type>" << endl;
			colFile << "    <type_name>";
			switch (colinfo->colType())
			{
			case	SQL_SMALLINT:
				colFile << "SQLSMALLINT";
				break;
			case	SQL_INTEGER:
				colFile << "SQLINTEGER";
				break;
			case	SQL_VARCHAR:
				colFile << "SQLVARCHAR";
				break;
			default:
				colFile << "UNKNOWN";
				break;
			}
			colFile << "</type_name>" << endl;
			colFile << "    <column_size>" << colinfo->maxLength() << "</column_size>" << endl;
			colFile << "    <nullable>" << (colinfo->nullable() ? SQL_NULLABLE : SQL_NO_NULLS) << "</nullable>" << endl;
			colFile << "    <ordinal_position>" << colIndex << "</ordinal_position>" << endl;
			colFile << "  </system.columns>" << endl;
		}
	}

	tabFile << "</system.tables_root>" << endl;
	colFile << "</system.columns_root>" << endl;

	tabFile.close();
	colFile.close();

	xmlCleanupParser ();

	return 0;
}

} /* namespace xml_db_daemon */

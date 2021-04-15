/*
 * XdbdNMGenAppl.cpp
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenAppl.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "XdbdTableInfo.h"

#include <iostream>
#include <string>
#include <set>
using namespace std;

namespace xml_db_daemon
{

/*! @brief constructor for main descriptor
 *
 *  it initializes parameters and their count
 *
 *  @param n parameter count
 *  @param p parameter list
 *
 */
XdbdNMGenAppl::XdbdNMGenAppl (int n, char*p [])
{
	m_n = n;
	m_p = p;
}

/*! @brief destructor of main descriptor
 *
 */
XdbdNMGenAppl::~XdbdNMGenAppl ()
{
}

/*! @brief generate source files for all triggers
 *
 *  function loads XSD table specifications and generates
 *  source code files for specified triggers. These source files
 *  contain C code which will be executed whenever associated trigger
 *  fired an event. The whole process of code generation is controlled
 *  by parameters given to the constructor of this object. Parameters are:
 *
 *  - in : must be followed by the name of input directory. There
 *  should be many input directory names. Every one of them must
 *  be preceded with parameter --in. If there is no input directory
 *  present in command line, current working directory is taken.
 *  Input directories hold XML files containing specifications for
 *  trigger event handlers. Generated code will reflect this specifications
 *  combined with informations retrieved from XML database tables
 *
 *  - out : must be followed by the name of output directory. There
 *  should be many output directories present in command line but
 *  only the last one it will be taken into account . If there is
 *  no output directory present in command line, current working
 *  directory is taken. Output directory is used to save all generated
 *  files.
 *
 *  - xsd : must be followed by XSD directory name. There should be
 *  many XSD directory names present in command line every one
 *  preceded by --xsd. Specifications of database tables should thus
 *  be specified in many XSD directories. At least one XSD directory
 *  should be specified otherwise warning message will be displayed
 *  and the code will probably not be generated or it will not be
 *  generated correctly.
 *
 *  - trg : must be followed by name of directory which will be used
 *  to save XML files containing trigger database tables. These files
 *  must be generated and integrated with XML database otherwise
 *  triggers will not function.
 *
 *  @return 0 always
 *
 */
int XdbdNMGenAppl::GenerateTriggerSourceFiles ()
{
	if (m_n < 2)
	{
		cerr << "Usage: " << m_p [0] << " [--in <xml-input-dir>] [--out <xml-output-dir>] [--xsd <xml-xsd-dir>] [--trg <trigger-xml>]\n"
			<< endl;
		return 0;
	}

	char* cwd = getcwd (0, 0);
	vector < string > inDir;
	string outDir = cwd;
	bool xsdLoaded = false;
	string trgFileName = "";

	xmlInitParser ();

	for (int i = 1; i < m_n; ++i)
	{
		char* par;
		if (*(par = m_p [i]) == '-')
		{
			if (++i >= m_n)
				break;
			if (strcmp (par, "--in") == 0)
			{
				inDir.push_back (m_p [i]);
			}
			else if (strcmp (par, "--out") == 0)
			{
				outDir = m_p [i];
			}
			else if (strcmp (par, "--xsd") == 0)
			{
				XdbdDataBase::LoadXsdFiles (m_p [i]);
				xsdLoaded = true;
			}
			else if (strcmp (par, "--trg") == 0)
			{
				trgFileName = m_p [i];
			}
		}
	}

	if (!xsdLoaded)
		cerr << "Warning: no --xsd option" << endl;

	if (inDir.empty ())
		inDir.push_back (cwd);

	for (vector < string >::iterator it = inDir.begin (); it != inDir.end (); ++it)
	{
		char* dirName = (char*) (*it).c_str ();
		DIR* xmldir = opendir (dirName);
		if (xmldir == 0)
		{
			cerr << "Cannot open dir '" << dirName << "'\n" << endl;
			return 0;
		}

		struct dirent* entry;
		while ((entry = readdir (xmldir)) != 0)
		{
			if ((entry->d_type & DT_REG) == 0)
				continue;

			// Skip filenames that do not end in .xml..
			if (strcmp( &(entry->d_name[strlen(entry->d_name)-4]), ".xml" ) != 0) {
				cout << "(I): XdbdNMGenAppl.cpp: ignoring non-XML file " << dirName << "/" << entry->d_name << endl;
				continue;
			}

			ReadXsdTableSpec (dirName, entry->d_name);
		}
	}

	xmlCleanupParser ();

	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
	{
		void* data [] =
			{ (void*) GenerateAll, (char*) outDir.c_str () };
		(*it)->GenerateCode (1, data);
	}

	GenerateXsdTriggersSpec (trgFileName);
	return 0;
}

/*! @brief collect trigger info from specified XML file
 *
 *  function takes XML file name, parses it using XML parser
 *  implemented in xmlReadFile() and analyzes all 'notification'
 *  nodes found by XML parser
 *
 *  @param dir name of input directory
 *  @param fname name of XML file containing trigger specification
 *
 *  @return 0 informations gathered correctly
 *  @return -1 XML parser failed
 *
 */
int XdbdNMGenAppl::ReadXsdTableSpec (const char* dir, const char* fname)
{
	string entryName = dir;
	entryName += "/";
	entryName += fname;
	xmlDocPtr doc = xmlReadFile (entryName.c_str (), NULL, 0);
	if (doc == NULL)
	{
		cerr << "Cannot parse XML document '" << m_p[1] << "'\n" << endl;
		return -1;
	}

	for (xmlNodePtr node = doc->children; node != 0; node = node->next)
	{
		string name = (char*) node->name;
		if (name != g_notification)
			continue;
		XdbdNMGenNotification* notification = new XdbdNMGenNotification (fname);
		notification->ReadSubNodes (node);
		m_nmlist.push_back (notification);
	}

	xmlFreeDoc (doc);
	return 0;
}

/*! @brief generate XML database table for all triggers
 *
 *  @param fileName name of XML file containing database
 *  table for all triggers
 *
 *  @return 0 always
 *
 */
int	XdbdNMGenAppl::GenerateXsdTriggersSpec (string fileName)
{
	do
	{
		if (fileName.empty ())
			break;

		ofstream trgFile;
		trgFile.open (fileName.c_str ());
		if (!trgFile.is_open ())
			break;

		trgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
		trgFile << "<xdbd_trigger_root" << endl;
		trgFile << " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'" << endl;
		trgFile << " xsi:noNamespaceSchemaLocation='xdbd_trigger.xsd'>" << endl;

		int index = 1;
		for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it, ++index)
		{
			void* data [] =
				{ (void*) GenerateTriggerRecord, &trgFile, (void*) index };
			(*it)->GenerateCode (1, data);
		}

		trgFile << "</xdbd_trigger_root>" << endl;
		trgFile.close ();
	}
	while (false);

	return 0;
}

/*! @brief check column usage
 *
 *  recursively check if named column has been used at any node
 *
 *  @param name column name
 *
 *  @return true column has been used
 *  @return false column has not been used
 *
 */
bool XdbdNMGenAppl::CheckColumnUsage (const char* name)
{
	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
		if ((*it)->CheckColumnUsage(name))
			return	true;
	return	false;
}

} /* namespace xml_db_daemon */

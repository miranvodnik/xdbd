/*
 * XdbdDataBase.cpp
 *
 *  Created on: 14. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "XdbdDataBase.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

string	XdbdDataBase::g_XdbdNsPrefix = "xs";	//!< name space prefix for XSD files
string	XdbdDataBase::g_XdbdNsUri = "http://www.w3.org/2001/XMLSchema";	//!< XML schema URI
XdbdDataBase*	XdbdDataBase::g_XdbdDataBase = new XdbdDataBase ();	//!< one and only one instance of 'database'

/*! @brief create an instance of 'database'
 *
 *  it initializes current table and column info
 *  references to 0
 *
 */
XdbdDataBase::XdbdDataBase()
{
	m_currentTableInfo = 0;
	m_currentColumnInfo = 0;
}

/*! @brief destructor does nothing special
 *
 */
XdbdDataBase::~XdbdDataBase()
{
}

/*! @brief get table info reference
 *
 *  @param name table name
 *
 *  @return 0 table info reference cannot be acquired. Probably
 *  unknown table name
 *  @return other table info reference
 *
 */
XdbdTableInfo* XdbdDataBase::_GetTableInfo (string  name)
{
	docmap::iterator	it = m_docmap.find(name);
	return	(it != m_docmap.end()) ? it->second : 0;
}

/*! @brief search for XML attribute reference
 *
 *  function traverses recursively through the given path if
 *  that path actually exists. At the end of path it retrieves
 *  attribute given its name if it actually exists
 *
 *  @param node XML node - the root node of search
 *  @param path path through root to an element holding attribute
 *  @param name attribute name
 *
 *  @return 0 cannot find attribute: illegal path, attribute does
 *  not exist, ...
 *  @return other reference to xmlAttr structure describing attribute
 *
 */
xmlAttrPtr	XdbdDataBase::findAttribute (xmlNodePtr node, const char*path[], const char*name)
{
	if (*path == 0)
	{
		for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
		{
			if (strcmp (name, (char*) prop->name) != 0)
				continue;
			return	prop;
		}
		return	0;
	}
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		xmlAttrPtr	attr;
		if (strcmp (*path, (char*) child->name) != 0)
			continue;
		if ((attr = findAttribute(child, path + 1, name)) == 0)
			continue;
		return	attr;
	}
	return	0;
}

/*! @brief load all XSD files
 *
 *  function loads all XSD files from given directory
 *  gathering information about database tables and
 *  columns contained within them
 *
 *  @param xsdPath directory path containing XSD files
 *
 *  @return 0 all done
 *  @return -1 cannot open directory path
 *
 */
int	XdbdDataBase::_LoadXsdFiles (char* xsdPath)
{
	DIR*	dir = opendir (xsdPath);
	if (dir == 0)
		return	-1;
	struct	dirent	dir_entry;
	struct	dirent*	dir_result = 0;

	while (true)
	{
		memset (&dir_entry, 0, sizeof dir_entry);
		dir_entry.d_reclen = sizeof dir_entry;
		if (readdir_r (dir, &dir_entry, &dir_result) < 0)
			break;
		if (dir_result == 0)
			break;
		if ((dir_entry.d_type & DT_REG) == 0)
			continue;
		char*	ptr = strrchr (dir_entry.d_name, '.');
		if (ptr == 0)
			continue;
		if (strcmp (ptr, ".xsd") != 0)
			continue;

		string	xsdfname = xsdPath;
		xsdfname += '/';
		xsdfname += dir_entry.d_name;

		xmlDocPtr	xsdDoc = 0;
		xmlXPathContextPtr	xsdCtx = 0;
		xmlXPathObjectPtr	obj = 0;

		do
		{
			xsdDoc = xmlReadFile (xsdfname.c_str(), 0, 0);
			if (xsdDoc == 0)
			{
				cerr << "cannot parse '" << xsdfname << "'" << endl;
				break;
			}

			xsdCtx = xmlXPathNewContext (xsdDoc);
			if (xsdCtx == 0)
			{
				cerr << "cannot create xml context for '" << xsdfname << "'" << endl;
				break;
			}

			if (xmlXPathRegisterNs(xsdCtx, (const xmlChar*) g_XdbdNsPrefix.c_str(), (const xmlChar*) g_XdbdNsUri.c_str()) != 0)
			{
				cerr << "cannot set namespace for '" << xsdfname << "'" << endl;
				break;
			}

			obj = xmlXPathEvalExpression ((xmlChar*) "//xs:element", xsdCtx);
			if (obj == 0)
			{
				cerr << "cannot locate xs:element(s) '" << xsdfname << "'" << endl;
				break;
			}

			xmlNodeSetPtr nodes = obj->nodesetval;
			if (nodes == 0)
			{
				cerr << "empty set of xs:element(s) '" << xsdfname << "'" << endl;
				break;
			}

			int	columnIndex = 0;
			int	currentDepth = 0;
			int	elementDepth = 0;
			int	columnPosition = 0;
			for (int i = 0; i < nodes->nodeNr; ++i)
			{
				xmlAttrPtr	nameAttr;
				xmlAttrPtr	minOccurs;
				xmlAttrPtr	maxOccurs;
				xmlAttrPtr	baseAttr;
				xmlAttrPtr	minValAttr;
				xmlAttrPtr	maxValAttr;
				xmlNodePtr	node = nodes->nodeTab[i];
				if (node == 0)
					continue;
				int	depth;
				xmlNodePtr	parent;
				for (depth = 0, parent = node->parent; parent != 0; parent = parent->parent, ++depth);
				if (depth > currentDepth)
					++elementDepth, columnPosition = 0;
				else	if (depth < currentDepth)
					--elementDepth;
				currentDepth = depth;
				switch (elementDepth)
				{
				case	1:
					//	root name
					break;
				case	2:
					//	table name
					{
						do
						{
							columnIndex = 0;
							m_currentTableInfo = 0;
							{
								const char*	path[] = { 0 };
								nameAttr = findAttribute(node, path, "name");
							}
							if (nameAttr == 0)
								break;

							docmap::iterator	dit;
							if ((dit = m_docmap.find ((char*)nameAttr->children->content)) != m_docmap.end())
							{
								delete	dit->second;
								m_docmap.erase(dit);
								cerr << "redefinition of table '" << nameAttr->children->content << "' in file '" << xsdfname << "'" << endl;
							}
							else
								m_tabmap.insert (make_pair (dir_entry.d_name, (char*)nameAttr->children->content));
							m_docmap[(char*)nameAttr->children->content] = m_currentTableInfo = new XdbdTableInfo ((char*) nameAttr->children->content);
						}
						while (false);
					}
					break;
				case	3:
					//	column name
					{
						do
						{
							m_currentColumnInfo = 0;
							//	@name attribute must be present
							{
								const char*	path[] = { 0 };
								nameAttr = findAttribute(node, path, "name");
							}
							if (nameAttr == 0)
							{
								cerr << "file: '" << xsdfname << "', line: " << node->line << ", @name attribute not found, node = '" << node->name << "'" << endl;
								break;
							}

							//	@minOccurs attribute need not be present
							{
								const char*	path[] = { 0 };
								minOccurs = findAttribute(node, path, "minOccurs");
							}

							//	@maxOccurs attribute need not be present
							{
								const char*	path[] = { 0 };
								maxOccurs = findAttribute(node, path, "maxOccurs");
							}

							//	@base attribute must be present
							{
								const char*	path[] = { "simpleType", "restriction", 0 };
								baseAttr = findAttribute(node, path, "base");
							}
							if (baseAttr == 0)
							{
								cerr << "file: '" << xsdfname << "', line: " << node->line << ", @base attribute not found, path = '" << node->name << "/simpleType/restriction/base'" << endl;
								break;
							}

							char*	minlValue = 0;
							//	minLength need not be present
							{
								const char*	path[] = { "simpleType", "restriction", "minLength", 0 };
								minValAttr = findAttribute(node, path, "value");
							}
							if (minValAttr != 0)
								minlValue = (char*) minValAttr->children->content;

							char*	maxlValue = 0;
							//	maxLength need not be present
							{
								const char*	path[] = { "simpleType", "restriction", "maxLength", 0 };
								maxValAttr = findAttribute(node, path, "value");
							}
							if (maxValAttr != 0)
								maxlValue = (char*) maxValAttr->children->content;

							m_currentColumnInfo = new XdbdColumnInfo
								(
									(char*) nameAttr->children->content,
									(char*) baseAttr->children->content,
									(minOccurs == 0) ? 0 : atoi ((char*)minOccurs->children->content),
									(maxOccurs == 0) ? 1 : atoi ((char*)maxOccurs->children->content),
									columnPosition,
									(minlValue != 0) ? atoi (minlValue) : -1,
									(maxlValue != 0) ? atoi (maxlValue) : -1,
									columnIndex++
								);
							columnPosition += m_currentColumnInfo->realLength();
							if (m_currentTableInfo != 0)
								m_currentTableInfo->add(m_currentColumnInfo);
						}
						while (false);
					}
					break;
				}
			}
		}
		while (false);	// only one iteration of do loop

		if (xsdDoc != 0)
			xmlFreeDoc (xsdDoc);
		if (xsdCtx != 0)
			xmlXPathFreeContext (xsdCtx);
		if (obj != 0)
			xmlXPathFreeObject (obj);
	}
	closedir (dir);

	for (docmap::iterator it = m_docmap.begin(); it != m_docmap.end(); ++it)
		it->second->AlignColPositions ();
	return	0;
}

} /* namespace xml_db_daemon */

/*
 * XdbdNMGenField.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenField.h"
#include <string.h>

namespace xml_db_daemon
{

/*! @brief 'field' descriptor constructor
 *
 */
XdbdNMGenField::XdbdNMGenField ()
{
}

/*! @brief 'field' descriptor destructor
 *
 */
XdbdNMGenField::~XdbdNMGenField ()
{
}

/*! @brief read sub-nodes
 *
 *  'field' node has no valid sub-nodes. If there are any
 *  they are ignored. It can contain attributes which are
 *  read instead.
 *
 *  @param node reference to XML 'field' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenField::ReadSubNodes (xmlNodePtr node)
{
	ReadAttributes (node);
	return 0;
}

/*! @brief read attributes
 *
 *  the only valid attribute for this node is 'name'. Its value
 *  is saved in m_name property of this class. Other attributes
 *  are simply ignored
 *
 *  @param node reference to XML 'field' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenField::ReadAttributes (xmlNodePtr node)
{
	for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
	{
		string name = (char*) prop->name;
		if (name == g_name)
		{
			m_name = (char*) prop->children->content;
		}
		else
		{

		}
	}
	return 0;
}

/*! @brief check column usage
 *
 *  'name' attribute of 'field' node actually represents DB column
 *  that's why this function returns true when and only when the
 *  value of 'name' attribute matches column name
 *
 *  @param name name of DB column
 *
 *  @return true column name matches 'name' attribute
 *  @return false column name does not match 'name' attribute
 *
 */
bool XdbdNMGenField::CheckColumnUsage (const char* name)
{
	return	strcmp (m_name.c_str(), name) == 0;
}

} /* namespace xml_db_daemon */

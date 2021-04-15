/*
 * XdbdNMGenStruct.cpp
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenStruct.h"

namespace xml_db_daemon
{

/*! @brief 'struct' descriptor constructor
 *
 */
XdbdNMGenStruct::XdbdNMGenStruct ()
{
}

/*! @brief 'struct' descriptor destructor
 *
 */
XdbdNMGenStruct::~XdbdNMGenStruct ()
{
}

/*! @brief read sub-nodes
 *
 *  since 'struct' node contains no valid nodes, this function
 *  reads only its attributes
 *
 *  @param node reference to XML 'struct' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenStruct::ReadSubNodes (xmlNodePtr node)
{
	ReadAttributes (node);
	return 0;
}

/*! @brief read attributes
 *
 *  reads 'name', 'subtype' and 'type' attributes
 *
 *  @param node reference to XML 'struct' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenStruct::ReadAttributes (xmlNodePtr node)
{
	for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
	{
		string name = (char*) prop->name;
		if (name == g_name)
		{
			m_name = (char*) prop->children->content;
		}
		else if (name == g_subtype)
		{
			m_subtype = (char*) prop->children->content;
		}
		else if (name == g_type)
		{
			m_type = (char*) prop->children->content;
		}
		else
		{

		}
	}
	return 0;
}

/*! @brief generate code
 *
 *  generates two variable declarations, the second one is actually
 *  reference to the first one. Type of first variable is equal to the
 *  value of attribute 'type', type of the second is reference to
 *  that same type. Names of these variables are generated from 'name'
 *  attribute value, second variable's name being equal to that value
 *  and first variable's name being prepended string 'a_'. First variable
 *  is always accessed indirectly using it's reference saved in the second
 *  one
 *
 *  @param dataCount size of data[] table - should be 2
 *  @param data code generation parameters - code generation flag
 *  and reference to output file object
 *
 *  @return 0 always
 *
 */
int XdbdNMGenStruct::GenerateCode (int dataCount, void* data [])
{
	ofstream* outFile = (ofstream*) data [1];

	*outFile << "\t" << m_type << "\ta_" << m_name << ";" << endl;
	*outFile << "\t" << m_type << "*\t" << m_name << " = &a_" << m_name << ";" << endl;
	return 0;
}

/*! @brief check column usage
 *
 *  this node does not generate DB column names
 *
 *  @param name name of DB column
 *
 *  @return false always
 *
 */
bool XdbdNMGenStruct::CheckColumnUsage (const char* name)
{
	return	false;
}

} /* namespace xml_db_daemon */

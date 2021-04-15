/*
 * XdbdNMGenEventsInsert.cpp
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenEventsInsert.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief 'insert' descriptor constructor
 *
 */
XdbdNMGenEventsInsert::XdbdNMGenEventsInsert ()
{
}

/*! @brief 'insert' descriptor destructor
 *
 */
XdbdNMGenEventsInsert::~XdbdNMGenEventsInsert ()
{
}

/*! @brief read sub-nodes
 *
 *  valid sub-nodes of 'insert' node are 'code_text' and
 *  'msgsnd'. All of them (any number of instances of any
 *  of them) are saved in superclass's m_nmlist. Nodes with
 *  other names are ignored.
 *
 *  @param node reference to XML 'insert' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenEventsInsert::ReadSubNodes (xmlNodePtr node)
{
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		string name = (char*) child->name;
		if (name == g_code_text)
		{
			XdbdNMGenCodeText* code = new XdbdNMGenCodeText ();
			m_nmlist.push_back (code);
			code->ReadSubNodes (child);
		}
		else if (name == g_msgsnd)
		{
			XdbdNMGenMsgsnd* msg = new XdbdNMGenMsgsnd ();
			m_nmlist.push_back (msg);
			msg->ReadSubNodes (child);
		}
		else
		{

		}
	}
	return 0;
}

/*! @brief read attributes
 *
 *  since 'insert' node has no attributes, this function
 *  does nothing
 *
 *  @param node reference to XML 'insert' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenEventsInsert::ReadAttributes (xmlNodePtr node)
{
	return 0;
}

/*! @brief generate code
 *
 *  almost all code is generated by sub-nodes except for case
 *  label (case 2:) and curly braces surrounding case statement
 *  when generating implementation code (data[0] equal to
 *  GenerateImplementation). Case 2 is associated with trigger
 *  fired due to execution of SQL insert statement
 *
 *  @param dataCount size of data[] table - should be 2
 *  @param data code generation parameters - code generation flag
 *  and reference to output file object
 *
 *  @return 0 always
 *
 */
int XdbdNMGenEventsInsert::GenerateCode (int dataCount, void* data [])
{
	ofstream* outFile = (ofstream*) data [1];

	switch ((GenerateFlags) (long) data [0])
	{
	case GenerateMQDefinitions:
	case GenerateMQInitializations:
		for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
			(*it)->GenerateCode (dataCount, data);
		break;
	case GenerateImplementation:
		*outFile << "      case 2:" << endl;
		*outFile << "         {" << endl;
		*outFile << "            /*\tINSERT CODE\t*/" << endl;
		for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
			(*it)->GenerateCode (dataCount, data);
		*outFile << "         }" << endl;
		*outFile << "         break;" << endl;
		break;
	default:
		break;
	}
	return 0;
}

/*! @brief check column usage
 *
 *  recursively check usage of specific DB column in generated
 *  code
 *
 *  @param name name of DB column
 *
 *  @return true column name found in generated code
 *  @return false column name not found in generated code
 *
 */
bool XdbdNMGenEventsInsert::CheckColumnUsage (const char* name)
{
	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
		if ((*it)->CheckColumnUsage(name))
			return	true;
	return	false;
}

} /* namespace xml_db_daemon */

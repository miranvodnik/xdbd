/*
 * XdbdNMGenChangeMask.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenChangeMask.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief create 'change_mask' descriptor
 *
 *  initializes 'conditions' descriptor
 */
XdbdNMGenChangeMask::XdbdNMGenChangeMask ()
{
	m_conditions = 0;
}

/*! @brief delete 'change_mask' descriptor
 *
 *  deletes 'conditions' descriptor
 */
XdbdNMGenChangeMask::~XdbdNMGenChangeMask ()
{
	delete m_conditions;
}

/*! @brief read subnodes
 *
 *  read all possible sub-nodes, which should be:
 *  mostly one 'conditions' (if there are more only
 *  the last one is taken), any number of 'code_text'
 *  and 'msgsnd' nodes which are collected in recursive
 *  list m-nmlist. All nodes are further recursively
 *  read using ReadSubNodes() on all sub-nodes
 *
 *  @param node XML node associated with 'change_mask'
 *
 *  @return 0 always
 *
 */
int XdbdNMGenChangeMask::ReadSubNodes (xmlNodePtr node)
{
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		string name = (char*) child->name;
		if (name == g_conditions)
		{
			delete m_conditions;
			m_conditions = new XdbdNMGenConditions ();
			m_conditions->ReadSubNodes (child);
		}
		else if (name == g_code_text)
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
 *  since node 'change_mask' has no attributes, this function
 *  does nothing
 *
 *  @param node XML node associated with 'change_mask'
 *
 *  @return 0 always
 *
 */
int XdbdNMGenChangeMask::ReadAttributes (xmlNodePtr node)
{
	return 0;
}

/*! @brief generate code for 'change_mask' descriptor
 *
 *  @see XdbdNMGenBase
 *
 */
int XdbdNMGenChangeMask::GenerateCode (int dataCount, void* data [])
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
		//*outFile << "         {" << endl;
		if (m_conditions != 0)
		{
			m_conditions->GenerateCode (dataCount, data);
			*outFile << "            {" << endl;
			*outFile << "               /*\tCHANGE MASK CODE\t*/" << endl;
		}
		for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
			(*it)->GenerateCode (dataCount, data);
		if (m_conditions != 0)
			*outFile << "            }" << endl;
		//*outFile << "         }" << endl;
		break;
	default:
		break;
	}
	return 0;
}

/*! @brief check column usage in generated code
 *
 *  recursively check DB column usage
 *
 *  @see XdbdNMGenBase
 *
 */
bool XdbdNMGenChangeMask::CheckColumnUsage (const char* name)
{
	if ((m_conditions != 0) && m_conditions->CheckColumnUsage(name))
		return	true;
	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
		if ((*it)->CheckColumnUsage(name))
			return	true;
	return	false;
}

} /* namespace xml_db_daemon */

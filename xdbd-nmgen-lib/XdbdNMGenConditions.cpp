/*
 * XdbdNMGenConditions.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenConditions.h"
#include "XdbdTableInfo.h"

namespace xml_db_daemon
{

/*! @brief create 'conditions' descriptor
 *
 *  initializes 'all_fields' descriptor reference
 */
XdbdNMGenConditions::XdbdNMGenConditions ()
{
	m_allFields = 0;
}

/*! @brief delete 'conditions' descriptor
 *
 *  deletes 'all_fields' descriptor reference
 */
XdbdNMGenConditions::~XdbdNMGenConditions ()
{
	delete m_allFields;
}

/*! @brief read sub-nodes
 *
 *  reads all sub-nodes which should be 'all_fields' or
 *  'field' nodes. 'all_field' does not accumulate while
 *  'field' node does and are collected in superclass's
 *  m_nmlist.
 *
 *  @param node XML node reference
 *
 *  @return 0 always
 *
 */
int XdbdNMGenConditions::ReadSubNodes (xmlNodePtr node)
{
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		string name = (char*) child->name;
		if (name == g_all_fields)
		{
			delete m_allFields;
			m_allFields = new XdbdNMGenAllFields ();
			m_allFields->ReadSubNodes (child);
		}
		else if (name == g_field)
		{
			XdbdNMGenField* field = new XdbdNMGenField ();
			field->ReadSubNodes (child);
			m_nmlist.push_back (field);
		}
		else
		{

		}
	}
	return 0;
}

/*! @brief read attributes
 *
 *  since 'conditions' node has no attributes, this
 *  function does nothing
 *
 *  @param node 'conditions' node reference
 *
 *  @return 0 always
 *
 */
int XdbdNMGenConditions::ReadAttributes (xmlNodePtr node)
{
	return 0;
}

/*! @brief generate code
 *
 *  there are two cases to consider when generating code:
 *
 *  - 'all_fields' sub-node is present. All 'field' nodes are ignored
 *  in this case. Main purpose of this case is to construct special
 *  bit mask field (static unsigned char cond_mask[]) with length equal
 *  to raw DB line length of table affected by corresponding trigger.
 *  Bits valued 1 represent valid contents of DB data, while the bits
 *  valued 0 represent irrelevant data in raw DB line. Finally an if
 *  statement is generated which calls function equal () which compares
 *  old raw DB value with new one taking in account only relevant bits
 *  in both of them.
 *
 *  - when 'all_fields' node is not present an, if statement is
 *  generated comparing old and new values of all columns associated
 *  with 'field' sub-nodes.
 *
 */
int XdbdNMGenConditions::GenerateCode (int dataCount, void* data [])
{
	ofstream* outFile = (ofstream*) data [1];
	XdbdTableInfo* tabinfo = (XdbdTableInfo*) data [2];
	int resSize = tabinfo->resSize ();

	if (m_allFields)
	{
		unsigned char* recMask = new unsigned char [resSize];

		memset (recMask, 0, resSize);
		for (XdbdTableInfo::col_iterator it = tabinfo->col_begin (); it != tabinfo->col_end (); ++it)
		{
			XdbdColumnInfo* colinfo = *it;
			int index = colinfo->index ();
			int position = colinfo->position ();
			int minLen = colinfo->minLength ();
			if (minLen == 0)
				minLen = colinfo->maxLength ();
			for (int i = 0; i < minLen; ++i)
			{
				if ((position + i) < resSize)
				{
					recMask [position + i] = 0xff;
					continue;
				}
				cerr << "position (" << position << ") + i (" << i << ") >= resSize (" << resSize
					<< "), table = " << tabinfo->name() << ", column = " << colinfo->name() << endl;
			}
			if ((2 + (index >> 1)) < resSize)
			{
				*(recMask + 2 + (index >> 1)) |= ((index & 1) == 0) ? 1 : (1 << 4);
				continue;
			}
			cerr << "2 + (index (" << index << ") >> 1 >= resSize (" << resSize
				<< "), table = " << tabinfo->name() << ", column = " << colinfo->name() << endl;
		}

		*outFile << "            static unsigned char\tcond_mask[] = {";
		for (int i = 0; i < resSize; ++i)
		{
			if ((i % 16) == 0)
				*outFile << endl << "               ";
			*outFile << (int) recMask [i] << ", ";
		}
		*outFile << endl;
		*outFile << "            };" << endl;
		*outFile << endl;

		*outFile << "            if (equal (newLine, oldLine, cond_mask, sizeof cond_mask) != 0)" << endl;
		delete [] recMask;
	}
	else
	{
		int count = 0;
		bool ind = false;
		*outFile << "            if (" << endl;
		for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it, ind = true)
		{
			XdbdNMGenField* field = (XdbdNMGenField*) *it;
			string name = field->name ();
			XdbdColumnInfo* colinfo = tabinfo->find (name);
			if (colinfo == 0)
				continue;

			int ctype = colinfo->colType ();
			int minLen = colinfo->minLength ();
			if (minLen == 0)
				minLen = colinfo->maxLength ();
			*outFile << "            ";
			if (ind)
				*outFile << "|| ";
			if (ctype == SQL_VARCHAR)
				*outFile << "(strncmp ((char*) new_" << name << ", (char*) old_" << name << ", " << minLen << ") != 0)"
					<< endl;
			else
				*outFile << "(new_" << name << " != old_" << name << ")" << endl;
			++count;
		}
		if (count == 0)
			*outFile << "0" << endl;
		*outFile << "            )" << endl;
	}

	return 0;
}

/*! @brief check DB column
 *
 *  if 'all_fields' sub-node is present than actually no column
 *  is used explicitly and function fails otherwise the set of
 *  'fileld' sub-nodes (in fact m-nmlist) is checked for explicit
 *  column presence
 *
 *  @param name column name
 *
 *  @return true column is explicitly present
 *  @return false 'all_fields' is present or column is not
 *  explicitly present
 *
 */
bool XdbdNMGenConditions::CheckColumnUsage (const char* name)
{
	if (m_allFields)
		return	false;
	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
		if ((*it)->CheckColumnUsage(name))
			return	true;
	return	false;
}

} /* namespace xml_db_daemon */

/*
 * XdbdNMGenCodeText.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include <string.h>
#include "XdbdNMGenCodeText.h"

namespace xml_db_daemon
{

const char* XdbdNMGenCodeText::g_regexpr = "\\$[a-zA-Z0-9_]+\\.";
regex_t XdbdNMGenCodeText::g_reg;
XdbdNMGenCodeText* XdbdNMGenCodeText::g_initializer = new XdbdNMGenCodeText (true);

/*! @brief 'code_text' initializer
 *
 *  initializes regular expression used by all public instances of
 *  this class
 *
 *  @param doIt initialization flag, must be true to invoke
 *  regular expression initialization
 *
 */
XdbdNMGenCodeText::XdbdNMGenCodeText (bool doIt)
{
	if (doIt)
		regcomp (&g_reg, g_regexpr, REG_EXTENDED | REG_ICASE);
}

/*! @brief public instance initializer
 *
 *  it initializes text buffer which will be used to collect all
 *  code specified by single 'code_text' node
 *
 */
XdbdNMGenCodeText::XdbdNMGenCodeText ()
{
	m_outText = "";
}

/*! @brief destructor does nothing
 *
 */
XdbdNMGenCodeText::~XdbdNMGenCodeText ()
{
}

/*! @brief read sub-nodes
 *
 *  it actually reads only one sub-node, its text sub-node and
 *  produces C code: it resolves variable names (names beginning
 *  with $)and its 'new' and 'old' suffixes. Regular expression
 *  g_reg is used to search variable names
 *
 *  @param node XML node to be parsed
 *
 *  @return	0 always
 *
 */
int XdbdNMGenCodeText::ReadSubNodes (xmlNodePtr node)
{
	m_text = (char*) node->children->content;

	char* ptr = (char*) m_text.c_str ();
	regmatch_t match;
	memset (&match, 0, sizeof(regmatch_t));

	while (true)
	{
		if (regexec (&g_reg, ptr, 1, &match, 0) == 0)
		{
			*(ptr + match.rm_so) = 0;
			m_outText += ptr;
			*(ptr + match.rm_so) = '$';

//			for (regoff_t offset = match.rm_so + 1; offset < match.rm_eo - 1; ++offset)
//				ptr [offset] = toupper (ptr [offset]);

			if (strncmp (ptr + match.rm_eo, (char*) "new", 3) == 0)
			{
				*(ptr + match.rm_eo - 1) = 0;
				m_outText += "new_";
				m_outText += ptr + match.rm_so + 1;
				*(ptr + match.rm_eo - 1) = '.';
				ptr += match.rm_eo + 3;
			}
			else if (strncmp (ptr + match.rm_eo, (char*) "old", 3) == 0)
			{
				*(ptr + match.rm_eo - 1) = 0;
				m_outText += "old_";
				m_outText += ptr + match.rm_so + 1;
				*(ptr + match.rm_eo - 1) = '.';
				ptr += match.rm_eo + 3;
			}
			else
			{
				*(ptr + match.rm_eo) = 0;
				m_outText += ptr + match.rm_so;
				*(ptr + match.rm_eo) = '.';
				ptr += match.rm_eo;
			}
		}
		else
		{
			m_outText += ptr;
			break;
		}
	}
	return 0;
}

/*! @brief read attributes
 *
 *  since node 'code_text' has no attributes, this function
 *  does nothing
 *
 *  @param node XML node to be analyzed
 *
 *  @return 0 always
 *
 */
int XdbdNMGenCodeText::ReadAttributes (xmlNodePtr node)
{
	return 0;
}

/*! @brief generate code
 *
 *  it saves code generated in ReadNodes() into output file
 *
 *  @param dataCount number of data parameters
 *  @param data actual parameters ($see XdbdNMGenBase)
 *
 *  @return	0 always
 *
 */
int XdbdNMGenCodeText::GenerateCode (int dataCount, void* data [])
{
	ofstream* outFile = (ofstream*) data [1];

	switch ((GenerateFlags) (long) data [0])
	{
	case GenerateImplementation:
		*outFile << m_outText << endl;
		break;
	default:
		break;
	}
	return 0;
}

/*! @brief check DB column
 *
 *  check if generated text contains column name
 *
 *  @param name column name
 *
 *  @return true generated text contains column name
 *  @return false generated text does not contain column name
 *
 */
bool XdbdNMGenCodeText::CheckColumnUsage (const char* name)
{
	return	m_text.find(name) != string::npos;
}

} /* namespace xml_db_daemon */

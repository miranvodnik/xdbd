/*
 * XdbdNMGenNotification.cpp
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#include <stdio.h>
#include "XdbdNMGenNotification.h"

namespace xml_db_daemon
{

const char* XdbdNMGenNotification::g_includePoint        = "<%INCLUDE_POINT%>";	//!< marker used to generate #include statements
const char* XdbdNMGenNotification::g_prototypePoint      = "<%PROTOTYPE_POINT%>";	//!< marker used to generate function prototypes
const char* XdbdNMGenNotification::g_implementationPoint = "<%IMPLEMENTATION_POINT%>";	//!< marker used to generate function code
const char* XdbdNMGenNotification::g_template []         = {	//!< template of generated file
	"/******************************************************/",
	"/*                                                    */",
	"/* DO NOT EDIT!                                       */",
	"/* Generated file by: XdbdNMGenNotification.cpp       */", 
	"/*                                                    */",
	"/******************************************************/",
	"",
	"/************************************************************************/",
	"/* LIBRARIES                                                            */",
	"/************************************************************************/",
	"#ifdef SS_UNIX", "#  include \"sqlxdbd.h\"", "#else","#  include <windows.h>", "#endif", "#include <ctype.h>",
	"#include <stdio.h>", "#include <string.h>", "#include <wchar.h>", "#include <stddef.h>", "#include <stdlib.h>",
	"#include <sys/timeb.h>", "", "#include <sqlext.h>", "#include <assert.h>", "", "#include \"g2_appli.h\"",
	"#include \"g2_com.h\"",
	"#include <itcommon/ipcs_keys.h>",
	"#include \"notify_msg.h\"",
	g_includePoint,
	"#include \"clip_string.h\"	// clip_string(), equal()",
	"#include \"nmSolidNtwrk.h\"",
	"",
	"/************************************************************************/",
	"/* VARIABLE DECLARATIONS                                                */",
	"/************************************************************************/",
	"static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;",
	"",
	"/************************************************************************/",
	"/* FUNCTION PROTOTYPES                                                  */",
	"/************************************************************************/",
	g_prototypePoint,
	"",
	"/************************************************************************/",
	"/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */",
	"/************************************************************************/",
	g_implementationPoint
};

/*! @brief constructor of 'notification' descriptor
 *
 *  it initializes internal data structures, generates output file
 *  name and indicators m_ismq and m_issdl which will be used in
 *  code generation algorithm. Output file name is concatenated
 *  by strings 'nmFunc_', parameter fname and suffix '.c'. Indicators
 *  are m_ismq and m_issdl are generated following this rule. If
 *  parameter fname ends with '_mq.xml' or simply '_mq', m_ismq is
 *  set. m_issdl is set following the same rule interchanging mq
 *  with sdl.
 *
 */
XdbdNMGenNotification::XdbdNMGenNotification (const char* fname)
{
	m_ismq = false;
	m_issdl = false;
	m_tabinfo = 0;
	m_MQNotification = 0;
	m_SDLNotification = 0;
	m_outName += "nmFunc_";
	m_outName += fname;
	m_outName += ".c";

	char*	ptr = strrchr ((char*) fname, '.');
	if ((ptr == 0) || (strcasecmp (ptr, ".xml") != 0))
		ptr = (char*) fname + strlen ((char*) fname);
	if (((ptr - 3) > fname) && (strncasecmp (ptr - 3, "_mq", 3) == 0))
		m_ismq = true;
	if (((ptr - 4) > fname) && (strncasecmp (ptr - 4, "_sdl", 4) == 0))
		m_issdl = true;
}

/*! @brief destructor of 'notification' descriptor
 *
 *  deletes 'MQ_notification' and 'SDL_notification' descriptors
 *  and closes output file to flush its contents
 *
 */
XdbdNMGenNotification::~XdbdNMGenNotification ()
{
	delete m_MQNotification;
	delete m_SDLNotification;
	m_outFile.close ();
}

/*! @brief read attributes
 *
 *  'notification' node contains one valid attribute named
 *  'table'. It specifies DB table associated with trigges
 *  being defined by this node. Function associates DB table
 *  info with this attribute.
 *
 *  @param node reference to XML 'notification' node
 *
 *  @return 0 table name exists and can be associated with
 *  existing DB table
 *  @return -1 table name does not exist or it cannot be associated
 *  with an existing DB table
 *
 */
int XdbdNMGenNotification::ReadAttributes (xmlNodePtr node)
{
	for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
	{
		string name = (char*) prop->name;
		if (name == g_table)
		{
			m_table = (char*) prop->children->content;
			if ((m_tabinfo = XdbdDataBase::GetTableInfo (m_table)) == 0)
				return -1;
		}
	}
	return (m_tabinfo != 0) ? 0 : -1;
}

/*! @brief read sub-nodes
 *
 *  'notification' node should contain one instance of 'MQ_notification'
 *  node and one instance of ?SDL_notification' node. If there are many
 *  instances of any kind only the former are remembered, others are
 *  quietly destroyed. Attributes are fetched before reading sub-nodes.
 *
 *  @param node reference to XML 'notification' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenNotification::ReadSubNodes (xmlNodePtr node)
{
	ReadAttributes (node);

	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		string name = (char*) child->name;
		if (name == g_MQ_notification)
		{
			delete m_MQNotification;
			m_MQNotification = new XdbdNMGenMQNotification ();
			m_MQNotification->ReadSubNodes (child);
		}
		else if (name == g_SDL_notification)
		{
			delete m_SDLNotification;
			m_SDLNotification = new XdbdNMGenSDLNotification ();
			m_SDLNotification->ReadSubNodes (child);
		}
	}

	return 0;
}

/*! @brief generate code
 *
 *  function generates file containing code which implements
 *  trigger functionality for specific DB table. Code generation
 *  takes template strings contained within g_template[] array
 *  and generates code following this rule:
 *  - if template string does not match any special marker defined
 *  with g_includePoint, g_prototypePoint and g_implementationPoint
 *  it is simply emitted into output file
 *  - if template string matches include point marker then #include
 *  directives are generated by 'MQ_notification' and 'SDL_notification'
 *  descriptors calling their implementation of GenerateCode() using
 *  GenerateIncludes value for data[0] parameter.
 *  - if template string matches prototype point marker then three
 *  function prototypes are generated. DB table name taken from 'table'
 *  attribute value is used to generate part of these function names.
 *  Names are constructed in the following way: they begin with 'nmFunc_'
 *  followed by table name saved in property variable m_table and
 *  finished with one of these strings: '_load', '_fire' or '_unload'.
 *  Their prototypes are:
 *
 *  void* nmFunc_$(table-name)_load (int n, char* p[]);
 *  int nmFunc_$(table-name)_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
 *  void nmFunc_$(table-name)_unload (void *context);
 *
 *  - if template string matches implementation point marker, functions
 *  mentioned above are generated
 *
 */
int XdbdNMGenNotification::GenerateCode (int dataCount, void* data [])
{
	if (m_tabinfo == 0)
		return -1;

	switch ((GenerateFlags) (long) data [0])
	{
	case GenerateAll:
		{
			string pathName = (char*) data [1];
			pathName += "/";
			pathName += m_outName;

			m_outFile.open (pathName.c_str ());
			if (!m_outFile.is_open ())
				return -1;

			int count = sizeof g_template / sizeof g_template [0];
			for (int i = 0; i < count; ++i)
			{
				if (strcmp (g_template [i], g_includePoint) == 0)
				{
					void* data [] =
						{ (void*) GenerateIncludes, &m_outFile };
					if (m_MQNotification != 0)
						m_MQNotification->GenerateCode (2, data);
					else if (m_SDLNotification != 0)
						m_SDLNotification->GenerateCode (2, data);
					else
						;
				}
				else if (strcmp (g_template [i], g_prototypePoint) == 0)
				{
					m_outFile << "void* nmFunc_" << m_table
						<< "_load (int n, char* p[]);" << endl;
					m_outFile << "int nmFunc_" << m_table
						<< "_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);" << endl;
					m_outFile << "void nmFunc_" << m_table
						<< "_unload (void *context);" << endl;
				}
				else if (strcmp (g_template [i], g_implementationPoint) == 0)
				{
					m_outFile << "void* nmFunc_" << m_table
						<< "_load (int n, char* p[])" << endl;
					m_outFile << "{" << endl;
					m_outFile << "   return 0;" << endl;
					m_outFile << "}" << endl;
					m_outFile << endl;
					m_outFile << "int nmFunc_" << m_table
						<< "_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)" << endl;
					m_outFile << "{" << endl;
					m_outFile << endl;
					m_outFile << "   static const char*\t\tTableName = \"" << m_table << "\";" << endl;
					m_outFile << "   static const SQLSMALLINT\tUserInfo;" << endl;
					m_outFile << endl;
					{
						void* data [] =
							{ (void*) GenerateStructures, &m_outFile };
						if (m_MQNotification != 0)
							m_MQNotification->GenerateCode (2, data);
						else if (m_SDLNotification != 0)
							m_SDLNotification->GenerateCode (2, data);
						else
							;
					}
					m_outFile << endl;
					{
						nmlist mqlist;
						void* data [] =
							{ (void*) GenerateMQDefinitions, &m_outFile, &mqlist };
						if (m_MQNotification != 0)
							m_MQNotification->GenerateCode (2, data);
						else if (m_SDLNotification != 0)
							m_SDLNotification->GenerateCode (2, data);
						else
							;
					}
					m_outFile << endl;
					int index = 0;
					for (XdbdTableInfo::col_iterator cit = m_tabinfo->col_begin (); cit != m_tabinfo->col_end ();
						++cit, ++index)
					{
						XdbdColumnInfo* colinfo = *cit;
						string name = colinfo->name ();
						int position = colinfo->position ();
						int flagindex = 2 + (index >> 1);
						int flagvalue = ((index & 1) == 0) ? 1 : (1 << 4);

//						transform (name.begin (), name.end (), name.begin (), ::toupper);

						if (CheckColumnUsage(name.c_str()))
						{
							switch (colinfo->colType ())
							{
							case SQL_SMALLINT:
								m_outFile << "   SQLSMALLINT\tnew_" << name << " = *(SQLSMALLINT*)(newLine + " << position
									<< ");" << endl;
								m_outFile << "   SQLSMALLINT\told_" << name << " = *(SQLSMALLINT*)(oldLine + " << position
									<< ");" << endl;
								break;
							case SQL_INTEGER:
								if (sizeof(int) != sizeof(long))
								{
									switch (colinfo->maxLength ())
									{
									case sizeof(int):
										m_outFile << "   SQLINTEGER\tnew_" << name << " = *(SQLINTEGER*)(newLine + "
											<< position << ");" << endl;
										m_outFile << "   SQLINTEGER\told_" << name << " = *(SQLINTEGER*)(oldLine + "
											<< position << ");" << endl;
										break;
									case sizeof(long):
										m_outFile << "   SQLLEN\tnew_" << name << " = *(SQLLEN*)(newLine + " << position
											<< ");" << endl;
										m_outFile << "   SQLLEN\told_" << name << " = *(SQLLEN*)(oldLine + " << position
											<< ");" << endl;
										break;
									}
								}
								else
								{
									m_outFile << "   SQLINTEGER\tnew_" << name << " = *(SQLINTEGER*)(newLine + " << position
										<< ");" << endl;
									m_outFile << "   SQLINTEGER\told_" << name << " = *(SQLINTEGER*)(oldLine + " << position
										<< ");" << endl;
								}
								break;
							case SQL_VARCHAR:
								m_outFile << "   SQLVARCHAR*\tnew_" << name << " = (SQLVARCHAR*)(newLine + " << position
								<< ");" << endl;
								m_outFile << "   SQLVARCHAR*\told_" << name << " = (SQLVARCHAR*)(oldLine + " << position
									<< ");" << endl;
								break;
							}
							m_outFile << "   SQLINTEGER\tnew_" << name << "_null = (((*(SQLINTEGER*)(newLine + " << flagindex
								<< ")) & " << flagvalue << ") != 0) ? SQL_NULL_DATA : 0;" << endl;
							m_outFile << "   SQLINTEGER\told_" << name << "_null = (((*(SQLINTEGER*)(oldLine + " << flagindex
								<< ")) & " << flagvalue << ") != 0) ? SQL_NULL_DATA : 0;" << endl;
						}
					}
					m_outFile << endl;
					{
						nmlist mqlist;
						void* data [] =
							{ (void*) GenerateMQInitializations, &m_outFile, &mqlist };
						if (m_MQNotification != 0)
							m_MQNotification->GenerateCode (2, data);
						else if (m_SDLNotification != 0)
							m_SDLNotification->GenerateCode (2, data);
						else
							;
					}
					m_outFile << endl;
					{
						void* data [] =
							{ (void*) GenerateImplementation, &m_outFile, m_tabinfo };
						if (m_MQNotification != 0)
							m_MQNotification->GenerateCode (3, data);
						else if (m_SDLNotification != 0)
							m_SDLNotification->GenerateCode (3, data);
						else
							;
					}
					m_outFile << "   return 0;" << endl;
					m_outFile << "}" << endl;
					m_outFile << endl;
					m_outFile << "void nmFunc_" << m_table
						<< "_unload (void* context)" << endl;
					m_outFile << "{" << endl;
					m_outFile << "   // No code .." << endl;
					m_outFile << "}" << endl;
					m_outFile << endl;
				}
				else
				{
					m_outFile << g_template [i] << endl;
				}
			}
		}
		break;
	case GenerateTriggerRecord:
		{
			ofstream* xdrFile = (ofstream*) data [1];
			int index = (int) (long) data [2];

			*xdrFile << " <xdbd_trigger>" << endl;
			*xdrFile << "  <trigger_id>" << index << "</trigger_id>" << endl;
			*xdrFile << "  <trigger_name>trigger_" << m_table << "</trigger_name>" << endl;
			if (m_ismq)
				*xdrFile << "  <trigger_library>libxdbdmqtrigger.so</trigger_library>" << endl;
			else	if (m_issdl)
				*xdrFile << "  <trigger_library>libxdbdsdltrigger.so</trigger_library>" << endl;
			else
				*xdrFile << "  <trigger_library>libtrigger.so</trigger_library>" << endl;
			*xdrFile << "  <trigger_load>nmFunc_" << m_table << "_load</trigger_load>" << endl;
			*xdrFile << "  <trigger_fire>nmFunc_" << m_table << "_fire</trigger_fire>" << endl;
			*xdrFile << "  <trigger_unload>nmFunc_" << m_table << "_unload</trigger_unload>" << endl;
			*xdrFile << "  <trigger_table>" << m_table << "</trigger_table>" << endl;
			*xdrFile << "  <trigger_function>14</trigger_function>" << endl;
			*xdrFile << " </xdbd_trigger>" << endl;
		}
		break;
	case GenerateNull:
		cerr << __FILE__ << ": Enumeration value 'GenerateNull' not supported!" << endl;
		break;
	case GenerateIncludes:
		cerr << __FILE__ << ": Enumeration value 'GenerateIncludes' not supported!" << endl;
		break;
	case GenerateStructures:
		cerr << __FILE__ << ": Enumeration value 'GenerateStructures' not supported!" << endl;
		break;
	case GenerateMQDefinitions:
		cerr << __FILE__ << ": Enumeration value 'GenerateMQDefinitions' not supported!" << endl;
		break;
	case GenerateMQInitializations:
		cerr << __FILE__ << ": Enumeration value 'GenerateMQInitializations' not supported!" << endl;
		break;
	case GenerateImplementation:
		cerr << __FILE__ << ": Enumeration value 'GenerateImplementation' not supported!" << endl;
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
bool XdbdNMGenNotification::CheckColumnUsage (const char* name)
{
	if (m_MQNotification != 0)
		return	m_MQNotification->CheckColumnUsage(name);
	else if (m_SDLNotification != 0)
		return	m_SDLNotification->CheckColumnUsage(name);
	else
		return	false;
}

} /* namespace xml_db_daemon */

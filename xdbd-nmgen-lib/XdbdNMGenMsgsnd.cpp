/*
 * XdbdNMGenMsgsnd.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenMsgsnd.h"

namespace xml_db_daemon
{

/*! @brief 'msgsnd' descriptor constructor
 *
 */
XdbdNMGenMsgsnd::XdbdNMGenMsgsnd ()
{
}

/*! @brief 'msgsnd' descriptor destructor
 *
 */
XdbdNMGenMsgsnd::~XdbdNMGenMsgsnd ()
{
}

/*! @brief read sub-nodes
 *
 *  'msgsnd' node has no valid sub-nodes, but it has
 *  only valid attributes
 *
 *  @param node reference to XML 'msgsnd' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenMsgsnd::ReadSubNodes (xmlNodePtr node)
{
	ReadAttributes (node);
	return 0;
}

/*! @brief read attributes
 *
 *  'msgsnd' node has two attributes: 'data' and 'ipc_id'. Their
 *  values are saved into m_data and m_ipcId members of this class
 *  respectively.
 *
 *  @param node reference to XML 'msgsnd' node
 *
 *  @return 0 always
 *
 */
int XdbdNMGenMsgsnd::ReadAttributes (xmlNodePtr node)
{
	for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
	{
		string name = (char*) prop->name;
		if (name == g_data)
		{
			m_data = (char*) prop->children->content;
		}
		else if (name == g_ipc_id)
		{
			m_ipcId = (char*) prop->children->content;
		}
		else
		{

		}
	}
	return 0;
}

/*! @brief generate code
 *
 *  three cases should be considered when generating code:
 *
 *  - declaration of variables associated with message queue: message id,
 *  message creation flag and message key used to create message queue and
 *  to access its id. All these variables are defined as global variables
 *  with function scope. They are visible only within the function which
 *  fires trigger.
 *
 *  - initialization code creates message queue if it has not been created
 *  yet, using system call msgget() whose one of parameters is message key
 *  defined in declaration section, which returns message queue id for newly
 *  created message queue. If function fails, error is reported and generated
 *  code fails with return code -1. Otherwise message creation flag is set to
 *  prevent this code to be executed again in the next invocation of generated
 *  code
 *
 *  - implementation code sends data using function msgsnd(). Actual data to be
 *  sent is generated using 'data' attribute of this node. Error reporting
 *  code is generated, which will be executed if msgsnd() fails
 *
 *  @param dataCount size of data[] table - should be 3
 *  @param data code generation parameters - code generation flag
 *  and reference to output file object. Third parameter (data[2]) should be
 *  reference to list containing references to 'msgsnd' nodes whose code has
 *  already been generated. This prevents duplicated declaration and initialization
 *  code generation.
 *
 *  @return 0 always
 *
 */
int XdbdNMGenMsgsnd::GenerateCode (int dataCount, void* data [])
{
	ofstream* outFile = (ofstream*) data [1];
	switch ((GenerateFlags) (long) data [0])
	{
	case GenerateMQDefinitions:
		{
			nmlist* mqlist = (nmlist*) data [2];
			for (nmlist::iterator it = mqlist->begin (); it != mqlist->end (); ++it)
			{
				XdbdNMGenMsgsnd* mq = (XdbdNMGenMsgsnd*) *it;
				if (mq->ipcId () == m_ipcId)
					return 0;
			}
			mqlist->push_back (this);
			*outFile << "   static\tint\tmsg_id_" << m_ipcId << ";" << endl;
			*outFile << "   static\tint\tmsg_created_" << m_ipcId << " = 0;" << endl;
			*outFile << "   static\tint\tmsg_key_" << m_ipcId << " = " << m_ipcId << ";" << endl;
			*outFile << endl;
		}
		break;
	case GenerateMQInitializations:
		{
			nmlist* mqlist = (nmlist*) data [2];
			for (nmlist::iterator it = mqlist->begin (); it != mqlist->end (); ++it)
			{
				XdbdNMGenMsgsnd* mq = (XdbdNMGenMsgsnd*) *it;
				if (mq->ipcId () == m_ipcId)
					return 0;
			}
			mqlist->push_back (this);
			*outFile << "   if (msg_created_" << m_ipcId << " == 0)" << endl;
			*outFile << "   {" << endl;
			*outFile << "      msg_id_" << m_ipcId << " = msgget (msg_key_" << m_ipcId << ", 0666 | IPC_CREAT);" << endl;
			*outFile << "      if (msg_id_" << m_ipcId << " < 0)" << endl;
			*outFile << "      {" << endl;
			*outFile << "         sc_printf(SC_RTDB, SC_ERR," << endl;
			*outFile << "            \"msgget() for msg_key " << m_ipcId << "=%x failed with error: %d (%20s)\"," << endl;
			*outFile << "            msg_key_" << m_ipcId << ", errno, strerror(errno));" << endl;
			*outFile << "         return\t-1;" << endl;
			*outFile << "      }" << endl;
			*outFile << "      msg_created_" << m_ipcId << " = 1;" << endl;
			*outFile << "   }" << endl;
			*outFile << endl;
		}
		break;
	case GenerateImplementation:
		*outFile << "   if (msgsnd(msg_id_" << m_ipcId << ", " << m_data << ", sizeof *" << m_data
			<< " - sizeof (long), IPC_NOWAIT) == -1)" << endl;
		*outFile << "   {" << endl;
		*outFile << "      sc_printf(SC_RTDB, SC_ERR," << endl;
		*outFile << "         \"msgsnd() for msg_id " << m_ipcId << "=%x failed with error: %d (%20s)\"," << endl;
		*outFile << "         msg_id_" << m_ipcId << ", errno,       strerror(errno));" << endl;
		*outFile << "      return\t-1;" << endl;
		*outFile << "   }" << endl;
		break;
	default:
		break;
	}
	return 0;
}

/*! @brief check column usage
 *
 *  code generated by this class does not refer any DB
 *  column names
 *
 *  @param name DB column name
 *
 *  @return false always
 *
 */
bool XdbdNMGenMsgsnd::CheckColumnUsage (const char* name)
{
	return	false;
}

} /* namespace xml_db_daemon */

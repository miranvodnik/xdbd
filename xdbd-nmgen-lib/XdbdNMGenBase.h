/*
 * XdbdNMGenBase.h
 *
 *  Created on: 13. feb. 2016
 *      Author: miran
 */

#pragma once

#include "libxml/tree.h"
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief superclass for all descriptors
 *
 *  defines interface for derived classes and recursive
 *  list of descriptors. Members of this list are instances
 *  of this class's subclasses, thats why this list is
 *  considered recursive
 *
 */
class XdbdNMGenBase
{
public:
	typedef vector < XdbdNMGenBase* > nmlist;	//!< type for list of subclass instances
	typedef enum	//!< different cases for code generation
	{
		GenerateNull,
		GenerateAll,
		GenerateIncludes,
		GenerateStructures,
		GenerateMQDefinitions,
		GenerateMQInitializations,
		GenerateImplementation,
		GenerateTriggerRecord
	} GenerateFlags;
public:
	XdbdNMGenBase ();
	virtual ~XdbdNMGenBase ();

	/*! @brief read sub-nodes
	 *
	 *  this function should implement analyzer for XML node associated
	 *  with subclass instance. It should read its nodes allocate appropriate
	 *  subclass instances to them and analyze them recursively using
	 *  ReadSubNodes() function of allocated subclass instances
	 *
	 *  @param node XML node associated with subclass instance
	 *
	 *  @return 0 should indicate success
	 *  @return other should indicate failure
	 *
	 */
	virtual int ReadSubNodes (xmlNodePtr node) = 0;

	/*! @brief generate code
	 *
	 *  implementation should generate code for the associated XML node and it
	 *  should be prepared to be called many times. Code generation is controlled
	 *  by function parameters. It is supposed that:
	 *
	 *  - data[0] is one of GenerateFlags and dictates what kind of code should
	 *  be generated
	 *
	 *  - data[1] is reference to an instance of ofstream object (an output file)
	 *  and it dictates in which file should be saved generated code
	 *
	 *  @param dataCount length of data[] array (it should be 2)
	 *  @param data list of controlling parameters
	 *
	 *  @return 0 should indicate success: code generated correctly
	 *  @return other should indicate failure: problems with code generation
	 *
	 */
	virtual int GenerateCode (int dataCount, void* data []) = 0;

	/*! @brief check DB column usage
	 *
	 *  this function should recursively check if specific DB column
	 *  is specified in any 'field' node of given trigger specification
	 *  Function should recursively traverse all sub-nodes which
	 *  potentially contain 'field' nodes
	 *
	 *  @param name DB column name
	 *
	 *  @return 0 should indicate success: column found
	 *  @return other should indicate failure: column not found
	 *
	 */
	virtual bool CheckColumnUsage (const char* name) = 0;
protected:

	/*! @brief read attributes
	 *
	 *  function should read all attributes associated with given
	 *  XML node
	 *
	 *  @param node XML node reference
	 *
	 *  @return true should indicate success: attributes read
	 *  @return false should indicate failure: error reading attributes
	 *  or some of them are missing
	 *
	 */
	virtual int ReadAttributes (xmlNodePtr node) = 0;	//!< read attributes of some XML node
public:
	static const string g_notification;	//!< 'notification' node
	static const string g_MQ_notification;	//!< 'MQ_notification' node
	static const string g_SDL_notification;	//!< 'SDL_notification' node
	static const string g_events;	//!< 'events' node
	static const string g_includes;	//!< 'includes' node
	static const string g_unit;	//!< 'unit' node
	static const string g_table;	//!< 'table' attribute
	static const string g_struct_variables;	//!< 'struct_variables' node
	static const string g_struct;	//!< 'struct' node
	static const string g_subtype;	//!< 'subtype' attribute
	static const string g_type;	//! 'type' attribute
	static const string g_delete;	//!< 'delete' node
	static const string g_insert;	//!< 'insert' node
	static const string g_update;	//!< 'update' node
	static const string g_change_mask;	//!< 'change_mask' node
	static const string g_conditions;	//!< 'conditions' node
	static const string g_all_fields;	//!< 'all_fields' node
	static const string g_field;	//!< 'field' node
	static const string g_code_text;	//!< 'code_text' node
	static const string g_msgsnd;	//!< 'msgsnd' node
	static const string g_data;	//!< 'data' attribute
	static const string g_ipc_id;	//!< 'ipc_id' attribute
	static const string g_name;	//!< 'name' attribute
protected:
	nmlist m_nmlist;	//!< recursive list of subclass instances
};

} /* namespace xml_db_daemon */

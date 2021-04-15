/*
 * XdbdNMGenBase.cpp
 *
 *  Created on: 13. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief nodes and attribute names
 *
 *  list of all possible node and attribute names used
 *  in XML files which contain specifications for DB trigger
 *  event handlers
 *
 */
const string XdbdNMGenBase::g_notification = "notification";
const string XdbdNMGenBase::g_MQ_notification = "MQ_notification";
const string XdbdNMGenBase::g_SDL_notification = "SDL_notification";
const string XdbdNMGenBase::g_events = "events";
const string XdbdNMGenBase::g_includes = "includes";
const string XdbdNMGenBase::g_unit = "unit";
const string XdbdNMGenBase::g_table = "table";
const string XdbdNMGenBase::g_struct_variables = "struct_variables";
const string XdbdNMGenBase::g_struct = "struct";
const string XdbdNMGenBase::g_subtype = "subtype";
const string XdbdNMGenBase::g_type = "type";
const string XdbdNMGenBase::g_delete = "delete";
const string XdbdNMGenBase::g_insert = "insert";
const string XdbdNMGenBase::g_update = "update";
const string XdbdNMGenBase::g_change_mask = "change_mask";
const string XdbdNMGenBase::g_conditions = "conditions";
const string XdbdNMGenBase::g_all_fields = "all_fields";
const string XdbdNMGenBase::g_field = "field";
const string XdbdNMGenBase::g_code_text = "code_text";
const string XdbdNMGenBase::g_msgsnd = "msgsnd";
const string XdbdNMGenBase::g_data = "data";
const string XdbdNMGenBase::g_ipc_id = "ipc_id";
const string XdbdNMGenBase::g_name = "name";

/*! @brief superclass constructor
 *
 */
XdbdNMGenBase::XdbdNMGenBase ()
{
}

/*! @brief superclass destructor
 *
 *  recursively delete list of all descriptors
 *  collected by derived classes
 *
 */
XdbdNMGenBase::~XdbdNMGenBase ()
{
	for (nmlist::iterator it = m_nmlist.begin (); it != m_nmlist.end (); ++it)
		delete *it;
	m_nmlist.clear ();
}

} /* namespace xml_db_daemon */

/*
 * XdbdNMGenAllFields.cpp
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenAllFields.h"
#include "XdbdTableInfo.h"

namespace xml_db_daemon
{

/*! @brief constructor of 'all_fields' attribute descriptor
 *
 */
XdbdNMGenAllFields::XdbdNMGenAllFields ()
{
}

/*! @brief destructor of 'all_fields' attribute descriptor
 *
 */
XdbdNMGenAllFields::~XdbdNMGenAllFields ()
{
}

/*! @brief read sub-nodes of 'all_fields' attribute
 *
 *  since attributes have no sub-nodes this function
 *  does nothing. Function is required by implementation
 *  of interface XdbdNMGenBase
 *
 *  @return 0 always
 *
 */
int XdbdNMGenAllFields::ReadSubNodes (xmlNodePtr node)
{
	return 0;
}

/*! @brief read attributes of 'all_fields' attribute
 *
 *  since attributes have no attributes this function
 *  does nothing. Function is required by implementation
 *  of interface XdbdNMGenBase
 *
 *  @return 0 always
 *
 */
int XdbdNMGenAllFields::ReadAttributes (xmlNodePtr node)
{
	return 0;
}

} /* namespace xml_db_daemon */

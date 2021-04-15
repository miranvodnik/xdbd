/*
 * XdbdNMGenMQNotification.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenEvents.h"
#include "XdbdNMGenIncludes.h"
#include "XdbdNMGenStructVariables.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'MQ_notification' descriptor
 *
 *  represents MQ notification triggers. It can hold
 *  instances of 'events', 'includes' and 'variables'
 *  descriptors but at least one of each. All code
 *  generation is performed by these instances
 *
 */
class XdbdNMGenMQNotification : public XdbdNMGenBase
{
public:
	XdbdNMGenMQNotification ();
	virtual ~XdbdNMGenMQNotification ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	XdbdNMGenEvents* m_events;	//!< 'events' node
	XdbdNMGenIncludes* m_includes;	//!< 'includes' node
	XdbdNMGenStructVariables* m_variables;	//!< 'variables' node
};

} /* namespace xml_db_daemon */

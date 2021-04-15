/*
 * XdbdNMGenEvents.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenEventsDelete.h"
#include "XdbdNMGenEventsInsert.h"
#include "XdbdNMGenEventsUpdate.h"
#include "XdbdNMGenBase.h"
#include <string>
using namespace std;

namespace xml_db_daemon
{

/*! @brief 'events' descriptor
 *
 *  holds references to single instances of 'delete',
 *  'insert' and 'update' descriptors
 *
 */
class XdbdNMGenEvents : public XdbdNMGenBase
{
public:
	XdbdNMGenEvents ();
	virtual ~XdbdNMGenEvents ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	XdbdNMGenEventsDelete* m_delete;	//!< reference to single instance of 'delete' descriptor
	XdbdNMGenEventsInsert* m_insert;	//!< reference to single instance of 'insert' descriptor
	XdbdNMGenEventsUpdate* m_update;	//!< reference to single instance of 'update' descriptor
};

} /* namespace xml_db_daemon */

/*
 * XdbdNMGenEventsUpdate.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenChangeMask.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'delete' node descriptor
 *
 *  the main added value of this class is implementation of
 *  interface defined by its superclass XdbdNMGenBase. It
 *  can collect any number of 'change_mask' sub-nodes saved
 *  within superclass's m_nmlist
 *
 */
class XdbdNMGenEventsUpdate : public XdbdNMGenBase
{
public:
	XdbdNMGenEventsUpdate ();
	virtual ~XdbdNMGenEventsUpdate ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
};

} /* namespace xml_db_daemon */

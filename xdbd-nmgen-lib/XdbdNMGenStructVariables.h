/*
 * XdbdNMGenStructVariables.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenStruct.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'struct_variables' descriptor
 *
 *  it uses its superclass's m_nmlist to collect
 *  'struct' sub-nodes
 *
 */
class XdbdNMGenStructVariables : public XdbdNMGenBase
{
public:
	XdbdNMGenStructVariables ();
	virtual ~XdbdNMGenStructVariables ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
};

} /* namespace xml_db_daemon */

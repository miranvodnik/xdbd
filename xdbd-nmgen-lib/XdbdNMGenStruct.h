/*
 * XdbdNMGenStruct.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'struct' node descriptor
 *
 *  it is used to generate declarations of structure
 *  names and it's types
 *
 */
class XdbdNMGenStruct : public XdbdNMGenBase
{
public:
	XdbdNMGenStruct ();
	virtual ~XdbdNMGenStruct ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	string m_name;	//!< structure name
	string m_type;	//!< structure type
	string m_subtype;	//!< structure subtype
};

} /* namespace xml_db_daemon */

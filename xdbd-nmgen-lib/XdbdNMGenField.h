/*
 * XdbdNMGenField.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'field' node descriptor
 *
 *  it is used to hold 'name' attribute. This class does
 *  not generate any code by itself. Its 'name' attribute
 *  is used by class XdbdNMGenConditions to generate
 *  variable declarations associated with DB columns
 *
 */
class XdbdNMGenField : public XdbdNMGenBase
{
public:
	XdbdNMGenField ();
	virtual ~XdbdNMGenField ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []) { return 0; }	//!< does not generate any code
	virtual bool CheckColumnUsage (const char* name);
	inline string name () { return m_name; }
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	string m_name;	//!< 'name' attribute value
};

} /* namespace xml_db_daemon */

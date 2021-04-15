/*
 * XdbdNMGenUnit.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'unit' node descriptor
 *
 *  it contains name of file to be used in generation
 *  of #include directive
 *
 */
class XdbdNMGenUnit : public XdbdNMGenBase
{
public:
	XdbdNMGenUnit ();
	virtual ~XdbdNMGenUnit ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	string m_name;	//!< 'name' attribute - name of included file
};

} /* namespace xml_db_daemon */

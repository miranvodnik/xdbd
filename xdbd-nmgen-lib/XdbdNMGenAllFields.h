/*
 * XdbdNMGenAllFields.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'all_fields' attribute descriptor
 *
 *  it implements interface XdbdNMGenBase for attribute
 *  'all_fields'. Since XML attribute is an entity which
 *  does not contain XML nodes and other attributes, this
 *  implementation is trivial. This element is important
 *  because code generator takes into account the presence
 *  or absence of this element wherever it should be
 *  used by other descriptors (node 'conditions')
 *
 */
class XdbdNMGenAllFields : public XdbdNMGenBase
{
public:
	XdbdNMGenAllFields ();
	virtual ~XdbdNMGenAllFields ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []) { return 0; }
	virtual bool CheckColumnUsage (const char* name) { return false; }
protected:
	virtual int ReadAttributes (xmlNodePtr node);
};

} /* namespace xml_db_daemon */

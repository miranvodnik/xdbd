/*
 * XdbdNMGenConditions.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenAllFields.h"
#include "XdbdNMGenField.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'conditions' descriptor
 *
 *  superclass's m_nmlist contains 'field' descriptors,
 *  this class contains 'all_fields' if it is present
 *
 */
class XdbdNMGenConditions : public XdbdNMGenBase
{
public:
	XdbdNMGenConditions ();
	virtual ~XdbdNMGenConditions ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	XdbdNMGenAllFields* m_allFields;	//!< reference to 'all_fields' descriptor
};

} /* namespace xml_db_daemon */

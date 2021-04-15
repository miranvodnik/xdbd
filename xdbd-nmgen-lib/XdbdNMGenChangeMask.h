/*
 * XdbdNMGenChangeMask.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenConditions.h"
#include "XdbdNMGenCodeText.h"
#include "XdbdNMGenMsgsnd.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief descriptor associated with 'change_mask'
 *
 */
class XdbdNMGenChangeMask : public XdbdNMGenBase
{
public:
	XdbdNMGenChangeMask ();
	virtual ~XdbdNMGenChangeMask ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	XdbdNMGenConditions* m_conditions;	//!< 'conditions' node
};

} /* namespace xml_db_daemon */

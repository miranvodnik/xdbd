/*
 * XdbdNMGenAppl.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdDataBase.h"
#include "XdbdNMGenNotification.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief main descriptor
 *
 *  it analyzes XML trigger event descriptions at the
 *  higher level. Lower level job is provided by 'notification'
 *  XML nodes
 *
 */
class XdbdNMGenAppl: public XdbdNMGenBase
{
public:
	XdbdNMGenAppl (int n, char*p []);
	virtual ~XdbdNMGenAppl ();
	int GenerateTriggerSourceFiles ();
	int ReadXsdTableSpec (const char* dir, const char* fname);
	int	GenerateXsdTriggersSpec (string fileName);
	virtual int ReadSubNodes (xmlNodePtr node) { return 0; }
	virtual int GenerateCode (int dataCount, void* data []) { return 0; }
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node) { return 0; }
private:
	int m_n;	//!< number of program parameters
	char** m_p;	//!< program parameters
};

} /* namespace xml_db_daemon */

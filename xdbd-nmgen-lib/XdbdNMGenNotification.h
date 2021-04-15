/*
 * XdbdNMGenNotification.h
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdDataBase.h"
#include "XdbdNMGenMQNotification.h"
#include "XdbdNMGenSDLNotification.h"
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'notification' node descriptor
 *
 *  it represents root node for triggers XML specification file
 *  it defines template for all generated code and special string
 *  markers located within template which help in code generation
 *  process
 */
class XdbdNMGenNotification : public XdbdNMGenBase
{
public:
	XdbdNMGenNotification (const char* fname);
	virtual ~XdbdNMGenNotification ();
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	static const char* g_includePoint;	//!< include string marker
	static const char* g_prototypePoint;	//!< prototype string marker
	static const char* g_implementationPoint;	//!< implementation string marker
	static const char* g_template [];	//!< array of template strings
	bool	m_ismq;	//!< generate MQ trigger code
	bool	m_issdl;	//!< generate SDL trigger code
	ofstream m_outFile;	//!< output file object
	string m_outName;	//!< output file name
	string m_table;	//!< associated DB table name
	XdbdTableInfo* m_tabinfo;	//!< DB table info
	XdbdNMGenMQNotification* m_MQNotification;	//!< 'MQ-notification' node
	XdbdNMGenSDLNotification* m_SDLNotification;	//!< 'SDL-notification' node
};

} /* namespace xml_db_daemon */

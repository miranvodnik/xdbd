/*
 * XdbdNMGenMsgsnd.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'msgsnd' descriptor
 *
 *  'msgsnd' node is very simple node generating
 *  quite important pieces of code. It holds two
 *  attributes:
 *
 *  - 'data': reference to data which should be sent
 *  through message queue
 *  - 'ipc_id': IPC message queue id which should be
 *  used in generated code
 *
 */
class XdbdNMGenMsgsnd : public XdbdNMGenBase
{
public:
	XdbdNMGenMsgsnd ();
	virtual ~XdbdNMGenMsgsnd ();
	inline string data () { return m_data; }	//!< return 'data' attribute value
	inline string ipcId () { return m_ipcId; }	//!< return 'ipc_id' attribute value
	virtual int ReadSubNodes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
private:
	string m_data;	//!< 'data' attribute value
	string m_ipcId;	//!< 'ipc_id' attribute value
};

} /* namespace xml_db_daemon */

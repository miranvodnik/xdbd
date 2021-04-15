/*
 * XdbdConnection.h
 *
 *  Created on: 10. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sys/select.h>
#include "XdbdApi.h"
#include "XdbdCommon.h"
#include "XdbdMessages.h"
#include "XdbdStatement.h"
#include <map>
using namespace std;

namespace xml_db_daemon
{

/*! @brief client side connection object
 *
 */
class XdbdConnection
{
private:
	/*! @brief set of SQL statements
	 *
	 *  this type represents set of client side SQL statement structures accessed
	 *  by their handles
	 *
	 *  @param u_long SQL statement handle used as index with which can be
	 *  accessed associated SQL statement structure
	 *  @param XdbdStatement* reference to client side SQL statement structure
	 *
	 */
	typedef map < u_long, XdbdStatement* >	stmset;
public:
	XdbdConnection(xdbd_attributes_t* attributes);
	virtual ~XdbdConnection();
	int Connect();
	int Disconnect();
	XdbdStatement*	CreateStatement ();
	int CreateStatement (XdbdStatement* &stmt);
	int DeleteStatement(XdbdStatement* stmt);
	int	Prepare (XdbdStatement* stmt, const char* statement);
	int	Execute (XdbdStatement* stmt);
	int	ExecuteDirect (XdbdStatement* stmt, const char* statement);
	inline bool connected () { return m_localClientFd > 0; }

private:
	int	PostRequest (XdbdRequest*request);
	int	RecvReply (XdbdReply *reply);

private:
	xdbd_attributes_t	m_attributes;	//!< connection attributes
	int	m_localClientFd;	//!< local client connection file descriptor

	stmset	m_stmset;	//!< set of SQL statements allocated by this connection

	u_char*	m_inputBuffer;	//!< beginning of input buffer
	u_char*	m_inputPtr;	//!< input buffer read head
	u_char*	m_inputEnd;	//!< end of input buffer

	u_char*	m_outputBuffer;	//!< beginning of output buffer
	u_char*	m_outputPtr;	//!< output buffer write head
	u_char*	m_outputEnd;	//!< end of output buffer

	int	m_intSize;	//!< size of integer XDR transformation
};

} /* namespace xml_db_daemon */

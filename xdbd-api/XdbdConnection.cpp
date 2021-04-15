/*
 * XdbdConnection.cpp
 *
 *  Created on: 10. nov. 2015
 *      Author: miran
 */

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sqlxdbd.h>
#include <sqlext.h>
#include "XdbdConnection.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief client connection
 *
 *  initializes I/O buffers and I/O watch dog timer which
 *  is retrieved from associated attributes. If attributes
 *  are not present, watch dog timer is set to 10 seconds
 *
 *  @param attributes connection attributes reference (should be null)
 *
 */
XdbdConnection::XdbdConnection(xdbd_attributes_t* attributes)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdConnection %ld", this);
#endif
	if (attributes == 0)
	{
		m_attributes.wdTimer.tv_sec = 10;
		m_attributes.wdTimer.tv_usec = 0;
	}
	else
		m_attributes = *attributes;
	m_localClientFd = 0;

	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;

	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;

	int	dummy = 0;
	m_intSize = xdr_sizeof ((xdrproc_t)xdr_int, &dummy);
}

/*! @brief release connection object
 *
 *  releases all allocated resources:
 *
 *  - closes connection FD
 *  - deletes all allocated statement
 *  - releases I/O buffers
 *
 */
XdbdConnection::~XdbdConnection()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdConnection %ld", this);
#endif
	if (m_localClientFd > 0)
		close (m_localClientFd);
	m_localClientFd = 0;

	for (stmset::iterator it = m_stmset.begin(); it != m_stmset.end(); ++it)
	{
		delete	it->second;
	}
	m_stmset.clear();

	if (m_inputBuffer != 0)
		free (m_inputBuffer);
	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;

	if (m_outputBuffer != 0)
		free (m_outputBuffer);
	m_outputBuffer = 0;
	m_outputPtr = 0;
	m_outputEnd = 0;
}

/*! @brief connect to XML DB server
 *
 *  function establishes connection with XML DB server. It is
 *  blocking function. Calling process will block until connection
 *  is establish or until watcdog timer fires. It follows these steps:
 *
 *  - it creates nonblocking UNIX domain socket.
 *  - tries to connect to UNIX domain XML DB server listening point. Connection
 *  duration is guarded with connection watch-dog timer (reason for nonblocking
 *  mode)
 *  - next it prepares sql connection protocol message
 *  - it sends it to server
 *  - reads reply
 *  - analyzes reply
 *
 *  If all these steps succeeded and if server gives positive reply, connection is
 *  established
 *
 *  @return	0 connection established
 *  @return -1 connection refused
 */
int XdbdConnection::Connect()
{
	if (m_localClientFd > 0)
		return	0;

	struct	sockaddr_un	clnaddr;

	if ((m_localClientFd = socket (AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("socket() failed, errno = %d"), errno);
		return	-1;
	}

	int	gflags;

	if ((gflags = fcntl (m_localClientFd, F_GETFL, 0)) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_GETFL) failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	if (fcntl (m_localClientFd, F_SETFL, gflags | O_NONBLOCK) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("fcntl(F_SETFL) failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	bzero (&clnaddr, sizeof (clnaddr));
	clnaddr.sun_family = AF_LOCAL;
	strncpy (clnaddr.sun_path, g_localSocketPath.c_str(), sizeof (clnaddr.sun_path) - 1);
	if (connect (m_localClientFd, (sockaddr*) &clnaddr, sizeof (clnaddr)) < 0)
	{
		if (errno == EWOULDBLOCK)
		{
			struct	timeval	t;
			t = m_attributes.wdTimer;
			fd_set	rset, eset;
			FD_ZERO (&rset);
			FD_ZERO (&eset);
			FD_SET (m_localClientFd, &rset);
			FD_SET (m_localClientFd, &eset);
			if (select (m_localClientFd + 1, &rset, 0, &eset, &t) <= 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("select() failed, errno = %d, remaining time = (%d, %d)"), errno, t.tv_sec, t.tv_usec);
				close (m_localClientFd);
				m_localClientFd = -1;
				return	-1;
			}
			return	0;
		}
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("connect() failed, errno = %d"), errno);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlConnectWord;
	sqlRequest->m_parameters.m_parameters_len = 0;
	sqlRequest->m_parameters.m_parameters_val = 0;

	if (PostRequest (&request) < 0)
		return	-1;

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql connect failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}
	XdbdSqlReply*	sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if (sqlReply->m_header.m_error != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), sqlReply->m_header.m_error->m_errorCode, sqlReply->m_header.m_error->m_errorMessage);
		return	-1;
	}
	if (sqlReply->m_command != (int) XdbdSqlConnectWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), sqlReply->m_command, XdbdSqlConnectWord);
		return	-1;
	}

	return	0;
}

/*! @brief disconnect from XML DB server
 *
 *  if connection is not established function fails. For connected client
 *  function follows these steps:
 *  - sql disconnect protocol message is creted
 *  - disconnect message is sent to server
 *  - reply is received from server
 *  - reply is analyzed
 *
 *  @return 0 all steps succeed
 *  @return -1 some steps fail
 *
 */
int XdbdConnection::Disconnect()
{
	if (m_localClientFd <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql disconnect failed: not connected"));
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlDisconnectWord;
	sqlRequest->m_parameters.m_parameters_len = 0;
	sqlRequest->m_parameters.m_parameters_val = 0;

	if (PostRequest (&request) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql disconnect failed due to failed PostRequest()"));
		return	-1;
	}

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql disconnect failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}

	XdbdSqlReply*	p_sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if ((p_sqlReply->m_header.m_error != 0) && (p_sqlReply->m_header.m_error->m_errorCode != 0))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
		return	p_sqlReply->m_header.m_error->m_errorCode;
	}

	if (p_sqlReply->m_command != (int) XdbdSqlDisconnectWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlDisconnectWord);
		return	-1;
	}

	close (m_localClientFd);
	m_localClientFd = 0;
	return	0;
}

/*! @param create sql statement
 *
 *  function fails for disconnected client. For connected client it
 *  follows these steps:
 *
 *  - it creates create-statement protocol request
 *  - request is sent to server
 *  - reply is received from server
 *  - statement info is fetched from reply: shared memory ticket and shared
 *  memory name, allocated by server for this statement
 *  - statement handle is created using statement info and saved into
 *  internal statement handle.
 *
 *  @param reference to statement handle. After successful execution
 *  this reference addresses created statement handle
 *
 *  @return	0 all steps succeed
 *  @return -1 some step fail
 *
 */
int XdbdConnection::CreateStatement(XdbdStatement* &stmt)
{
	stmt = 0;
	if (m_localClientFd <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql create statement failed: not connected"));
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlCreateStatementWord;
	sqlRequest->m_parameters.m_parameters_len = 0;
	sqlRequest->m_parameters.m_parameters_val = 0;

	if (PostRequest (&request) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql create statement failed due to failed PostRequest()"));
		return	-1;
	}

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql create statement failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}

	XdbdSqlReply*	p_sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if (p_sqlReply->m_header.m_error != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
		return	p_sqlReply->m_header.m_error->m_errorCode;
	}

	if (p_sqlReply->m_command != (int) XdbdSqlCreateStatementWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlCreateStatementWord);
		return	-1;
	}

	if (p_sqlReply->m_parameters.m_parameters_len < 1)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("parameter count (%d) < 1"), p_sqlReply->m_parameters.m_parameters_len);
		return	-1;
	}

	XdbdParameter*	p_XdbdParameter = p_sqlReply->m_parameters.m_parameters_val;
	if ((p_XdbdParameter[0].m_flags & (INTVAL|STRVAL)) != (INTVAL|STRVAL))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("incorrect parameter flags = %d, expected = %d"), (p_XdbdParameter[0].m_flags & (INTVAL|STRVAL)), (INTVAL|STRVAL));
		return	-1;
	}

	m_stmset[p_XdbdParameter[0].m_longVal] = stmt = new XdbdStatement (this, p_XdbdParameter[0].m_longVal, p_XdbdParameter[0].m_stringVal);

	return	0;
}

/*! @param delete sql statement
 *
 *  function fails for disconnected client. For connected client it
 *  follows these steps:
 *
 *  - it creates delete-statement protocol request using statement ticket
 *  which will identify it with server data structures (SHM, ...)
 *  - request is sent to server
 *  - reply is received from server and analyzed
 *
 *  @param reference to statement handle.
 *
 *  @return	0 all steps succeed
 *  @return -1 some step fail
 *
 */
int XdbdConnection::DeleteStatement(XdbdStatement* stmt)
{
	if (m_localClientFd <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql delete statement failed: not connected"));
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlDeleteStatementWord;
	XdbdParameter*	parameters = new XdbdParameter[1];
	parameters[0].m_flags = INTVAL;
	parameters[0].m_longVal = stmt->ticket();
	parameters[0].m_stringVal = strdup ("");
	sqlRequest->m_parameters.m_parameters_len = 1;
	sqlRequest->m_parameters.m_parameters_val = parameters;

	if (PostRequest (&request) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql delete statement failed due to failed PostRequest()"));
		return	-1;
	}

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql delete statement failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}

	XdbdSqlReply*	p_sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if (p_sqlReply->m_header.m_error != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
		return	-1;
	}

	if (p_sqlReply->m_command != (int) XdbdSqlDeleteStatementWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlDeleteStatementWord);
		return	-1;
	}

	m_stmset.erase(stmt->ticket());
	delete	stmt;

	return	0;
}

/*! @param prepare sql statement
 *
 *  function fails for disconnected client. For connected client it
 *  follows these steps:
 *
 *  - it creates prepare protocol request using statement ticket
 *  which will identify it with server data structures (SHM, ...)
 *  and sql statement text
 *  - request is sent to server
 *  - reply is received from server and analyzed. Initial size
 *  of shared memory object (residing on server an visible by
 *  client) is reported in reply
 *
 *  @param reference to statement handle used to specify shared
 *  memory ticket to identify it with server
 *  @param statement sql statement text which will be used in
 *  preparation process
 *
 *  @return	0 all steps succeed
 *  @return -1 some step fail
 *
 */
int	XdbdConnection::Prepare (XdbdStatement* stmt, const char* statement)
{
	if (m_localClientFd <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql prepare failed: not connected"));
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlPrepareWord;
	XdbdParameter*	parameters = new XdbdParameter[1];
	parameters[0].m_flags = INTVAL|STRVAL;
	parameters[0].m_longVal = stmt->ticket();
	parameters[0].m_stringVal = strdup (statement);
	sqlRequest->m_parameters.m_parameters_len = 1;
	sqlRequest->m_parameters.m_parameters_val = parameters;

	if (PostRequest (&request) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql prepare failed due to failed PostRequest()"));
		return	-1;
	}

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql prepare failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}

	XdbdSqlReply*	p_sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if (p_sqlReply->m_header.m_error != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
		return	-1;
	}

	if (p_sqlReply->m_command != (int) XdbdSqlPrepareWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlPrepareWord);
		return	-1;
	}

	if (p_sqlReply->m_parameters.m_parameters_len < 1)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("parameter count (%d) < 1"), p_sqlReply->m_parameters.m_parameters_len);
		return	-1;
	}

	XdbdParameter*	p_XdbdParameter = p_sqlReply->m_parameters.m_parameters_val;
	if ((p_XdbdParameter[0].m_flags & INTVAL) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("incorrect parameter flags = %d, expected = %d"), (p_XdbdParameter[0].m_flags & INTVAL), INTVAL);
		return	-1;
	}

	return	stmt->StmtMap(p_XdbdParameter[0].m_longVal);
}

/*! @param execute sql statement
 *
 *  function fails for disconnected client. For connected client it
 *  follows these steps:
 *
 *  - it creates execute protocol request using statement ticket
 *  which will identify it with server data structures (SHM, ...)
 *  - request is sent to server
 *  - reply is received from server and analyzed. Size
 *  of shared memory object, holding result set) is reported in reply
 *
 *  @param reference to statement handle used to specify shared
 *  memory ticket to identify it with server
 *
 *  @return	0 all steps succeed
 *  @return -1 some step fail
 *
 */
int	XdbdConnection::Execute (XdbdStatement* stmt)
{
	int	status = SQL_ERROR;

	XdbdRequest	*request = 0;
	XdbdReply	*reply = 0;
	do
	{
		if (m_localClientFd <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute failed: not connected"));
			break;
		}

		if ((request = new XdbdRequest) == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc(%d) failed, errno = %d"), sizeof (XdbdReply), errno);
			break;
		}

		memset (request, 0, sizeof *request);
		request->m_requestType = SqlRequestCode;
		XdbdSqlRequest*	sqlRequest = &request->XdbdRequest_u.m_sqlRequest;
		sqlRequest->m_command = (int) XdbdSqlExecuteWord;
		XdbdParameter*	parameters = new XdbdParameter[1];
		sqlRequest->m_parameters.m_parameters_len = 1;
		sqlRequest->m_parameters.m_parameters_val = parameters;
		parameters[0].m_flags = INTVAL;
		parameters[0].m_longVal = stmt->ticket();
		parameters[0].m_stringVal = strdup ("");

		if (PostRequest (request) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute failed due to failed PostRequest()"));
			break;
		}

		if ((reply = new XdbdReply) == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc(%d) failed, errno = %d"), sizeof (XdbdReply), errno);
			break;
		}
		memset (reply, 0, sizeof *reply);
		if (RecvReply (reply) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute failed due to failed RecvReply()"));
			break;
		}

		if (reply->m_replyType != SqlReplyCode)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply->m_replyType, SqlReplyCode);
			break;
		}

		XdbdSqlReply*	p_sqlReply = &reply->XdbdReply_u.m_sqlReply;
		if (p_sqlReply->m_header.m_error != 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
			break;
		}

		if (p_sqlReply->m_command != (int) XdbdSqlExecuteWord)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlExecuteWord);
			break;
		}

		if (p_sqlReply->m_parameters.m_parameters_len < 1)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("parameter count (%d) < 1"), p_sqlReply->m_parameters.m_parameters_len);
			break;
		}

		XdbdParameter*	p_XdbdParameter = p_sqlReply->m_parameters.m_parameters_val;
		if ((p_XdbdParameter[0].m_flags & INTVAL) == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("incorrect parameter flags = %d, expected = %d"), (p_XdbdParameter[0].m_flags & INTVAL), INTVAL);
			break;
		}

		status = stmt->StmtSelect(p_XdbdParameter[0].m_longVal);
	}
	while (false);
	if (request != 0)
	{
		xdr_free ((xdrproc_t)xdr_XdbdRequest, (char*) request);
		delete	request;
	}
	if (reply != 0)
	{
		xdr_free ((xdrproc_t)xdr_XdbdReply, (char*) reply);
		delete	reply;
	}

	return	status;
}

/*! @param execute-direct sql statement
 *
 *  function fails for disconnected client. For connected client it
 *  follows these steps:
 *
 *  - it creates execute-direct protocol request using statement ticket
 *  which will identify it with server data structures (SHM, ...)
 *  and sql statement text
 *  - request is sent to server
 *  - reply is received from server and analyzed. Size
 *  of shared memory object, holding result set, is reported in reply
 *
 *  @param reference to statement handle used to specify shared
 *  memory ticket to identify it with server
 *  @param statement sql statement text which will be used to
 *  execute statement
 *
 *  @return	0 all steps succeed
 *  @return -1 some step fail
 *
 */
int	XdbdConnection::ExecuteDirect (XdbdStatement* stmt, const char* statement)
{
	int	status = Prepare (stmt, statement);
	if (status != SQL_SUCCESS)
		return	status;
	return	Execute (stmt);

	if (m_localClientFd <= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute direct failed: not connected"));
		return	-1;
	}

	XdbdRequest	request;
	memset (&request, 0, sizeof request);
	request.m_requestType = SqlRequestCode;
	XdbdSqlRequest*	sqlRequest = &request.XdbdRequest_u.m_sqlRequest;
	sqlRequest->m_command = (int) XdbdSqlExecuteDirectWord;
	XdbdParameter*	parameters = new XdbdParameter[1];
	parameters[0].m_flags = INTVAL|STRVAL;
	parameters[0].m_longVal = stmt->ticket();
	parameters[0].m_stringVal = strdup (statement);
	sqlRequest->m_parameters.m_parameters_len = 1;
	sqlRequest->m_parameters.m_parameters_val = parameters;

	if (PostRequest (&request) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute direct failed due to failed PostRequest()"));
		return	-1;
	}

	XdbdReply	reply;
	memset (&reply, 0, sizeof reply);
	if (RecvReply (&reply) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("sql execute direct failed due to failed RecvReply()"));
		return	-1;
	}

	if (reply.m_replyType != SqlReplyCode)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply type = %d, expected = %d"), reply.m_replyType, SqlReplyCode);
		return	-1;
	}

	XdbdSqlReply*	p_sqlReply = &reply.XdbdReply_u.m_sqlReply;
	if (p_sqlReply->m_header.m_error != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("reply error = %d, %s"), p_sqlReply->m_header.m_error->m_errorCode, p_sqlReply->m_header.m_error->m_errorMessage);
		return	-1;
	}

	if (p_sqlReply->m_command != (int) XdbdSqlExecuteDirectWord)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("illegal reply command = %d, expected = %d"), p_sqlReply->m_command, XdbdSqlExecuteDirectWord);
		return	-1;
	}

	if (p_sqlReply->m_parameters.m_parameters_len < 1)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("parameter count (%d) < 1"), p_sqlReply->m_parameters.m_parameters_len);
		return	-1;
	}

	XdbdParameter*	p_XdbdParameter = p_sqlReply->m_parameters.m_parameters_val;
	if ((p_XdbdParameter[0].m_flags & INTVAL) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("incorrect parameter flags = %d, expected = %d"), (p_XdbdParameter[0].m_flags & INTVAL), INTVAL);
		return	-1;
	}

	if (stmt->StmtMap(p_XdbdParameter[0].m_longVal) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("incorrect parameter value = %d, expected > 0"), p_XdbdParameter[0].m_longVal);
		return	-1;
	}
	else
		return	stmt->StmtSelect(p_XdbdParameter[0].m_longVal);
}

/*! @brief create common message header
 *
 *  creates common fields of all messages
 *
 *  @header message header
 *
 */
static	void	CreateMessageHeader (XdbdMessageHeader* header)
{
	static	int	g_messageIndex = 0;

	header->m_version.m_major = 1;
	header->m_version.m_minor = 0;
	header->m_index = ++g_messageIndex;
	uuid_generate (header->m_hash);
	header->m_error = 0;
	header->m_entity = XdbdClientEntity;
	header->m_pid = getpid();
}

/*! @brief send protocol message to server
 *
 *  Function follows these steps:
 *
 *  - allocates output buffer to accomodate message to be sent
 *  - serializes message into output buffer
 *  - tries to send message to server. Duration of staled send
 *  is guarded by watch-dog timer. If it expires, function fails.
 *  If message cannot be sent in one chunk, send operation is repeated
 *
 *  @param request reference to message to be sent
 *
 *  @return 0 all above steps succeed
 *  @return -1 some step fails
 *
 */
int	XdbdConnection::PostRequest (XdbdRequest*request)
{
	CreateMessageHeader(&request->XdbdRequest_u.m_header);

	u_int	msgSize = xdr_sizeof ((xdrproc_t)xdr_XdbdRequest, request);
	u_int	needSpace = msgSize + m_intSize;
	u_int	usedSpace = m_outputPtr - m_outputBuffer;
	u_int	freeSpace = m_outputEnd - m_outputPtr;

	if (freeSpace < needSpace)
	{
		needSpace += usedSpace;
		needSpace >>= 10;
		needSpace++;
		needSpace <<= 10;

		u_char*	buffer = (u_char*) malloc (needSpace);
		if (buffer == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc (%d) failed, errno = %d"), needSpace, errno);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		if (m_outputBuffer != 0)
		{
			memcpy (buffer, m_outputBuffer, usedSpace);
			free (m_outputBuffer);
		}
		m_outputBuffer = buffer;
		m_outputPtr = buffer + usedSpace;
		m_outputEnd = buffer + needSpace;
		freeSpace = needSpace - usedSpace;
	}

	XDR	xdr;

	xdrmem_create(&xdr, (char*) m_outputPtr, freeSpace, XDR_ENCODE);
	if (xdr_int(&xdr, (int*) &msgSize) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_int (%d) failed"), msgSize);
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	xdrmem_create(&xdr, (char*) (m_outputPtr + m_intSize), freeSpace - m_intSize, XDR_ENCODE);
	if (xdr_XdbdRequest (&xdr, request) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_XdbdRequest failed"));
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	u_int	pos = xdr_getpos (&xdr);
	m_outputPtr += pos + m_intSize;

	unsigned char*	ptr;
	size_t	len;
	ssize_t	size;

	for (ptr = m_outputBuffer, len = m_outputPtr - m_outputBuffer; len > 0; ptr += size, len -= size)
	{
		struct	timeval	t = m_attributes.wdTimer;
		fd_set	wset;
		FD_ZERO (&wset);
		FD_SET (m_localClientFd, &wset);
		if (select (m_localClientFd + 1, 0, &wset, 0, &t) <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("select() failed, errno = %d, remaining time = (%d, %d)"), errno, t.tv_sec, t.tv_usec);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}
		size = send (m_localClientFd, ptr, len, 0);
		if (size <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("send() failed, errno = %d"), errno);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}
	}
	m_outputPtr = m_outputBuffer;
	return	0;
}

/*! @brief receive protocol message from server
 *
 *  Function follows these steps:
 *
 *  - allocates input buffer to accomodate message to be received
 *  - tries to receive message from server. Duration of staled recv()
 *  is guarded by watch-dog timer. If it expires, function fails.
 *  If message cannot be received in one chunk, recv() operation is repeated
 *  - decode message from input buffer
 *
 *  @param reply reference to message to be received. After successful
 *  execution of this function this reference holds message sent by server
 *
 *  @return 0 all above steps succeed
 *  @return -1 some step fails
 *
 */
int	XdbdConnection::RecvReply (XdbdReply *reply)
{
	XDR	xdr;
	int	msgSize;

	while (true)
	{
		struct	timeval	t = m_attributes.wdTimer;
		fd_set	rset;
		FD_ZERO (&rset);
		FD_SET (m_localClientFd, &rset);

		if (select (m_localClientFd + 1, &rset, 0, 0, &t) <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("select() failed, errno = %d, remaining time = (%d, %d)"), errno, t.tv_sec, t.tv_usec);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		int	needSpace = 0;
		int	usedSpace = m_inputPtr - m_inputBuffer;
		int	freeSpace = m_inputEnd - m_inputPtr;

		if (ioctl (m_localClientFd, FIONREAD, &needSpace) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ioctl() failed, errno = %d"), errno);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		if (needSpace == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ioctl() failed, errno = %d"), errno);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		if (freeSpace < needSpace)
		{
			needSpace += usedSpace;
			needSpace >>= 10;
			needSpace++;
			needSpace <<= 10;
			u_char*	buffer = (u_char*) malloc (needSpace);
			if (buffer == 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc (%d) failed, errno = %d"), needSpace, errno);
				close (m_localClientFd);
				m_localClientFd = -1;
				return	-1;
			}
			if (m_inputBuffer != 0)
			{
				memcpy (buffer, m_inputBuffer, usedSpace);
				free (m_inputBuffer);
			}
			m_inputBuffer = buffer;
			m_inputPtr = buffer + usedSpace;
			m_inputEnd = buffer + needSpace;
			freeSpace = needSpace - usedSpace;
		}

		int	recvSize;
		if ((recvSize = recv (m_localClientFd, m_inputPtr, freeSpace, 0)) <= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("recv() failed, errno = %d"), errno);
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		usedSpace += recvSize;

		u_char*	ptr = m_inputBuffer;
		if (usedSpace < (int) m_intSize)
			continue;

		xdrmem_create(&xdr, (char*) ptr, m_intSize, XDR_DECODE);
		if (xdr_int (&xdr, &msgSize) != TRUE)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_int() failed"));
			close (m_localClientFd);
			m_localClientFd = -1;
			return	-1;
		}

		if (usedSpace >= msgSize + (int) m_intSize)
			break;
	}

	m_inputPtr = m_inputBuffer;
	xdrmem_create(&xdr, (char*) (m_inputBuffer + m_intSize), msgSize, XDR_DECODE);
	if (xdr_XdbdReply(&xdr, reply) != TRUE)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xdr_XdbdReply() failed"));
		close (m_localClientFd);
		m_localClientFd = -1;
		return	-1;
	}

	for (XdbdError* p_error = reply->XdbdReply_u.m_sqlReply.m_header.m_error; p_error != 0; p_error = p_error->m_next)
	{
		if (p_error->m_errorCode != 0)
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("Error (%d) : %s"), p_error->m_errorCode, p_error->m_errorMessage);
	}
	return	0;
}

} /* namespace xml_db_daemon */

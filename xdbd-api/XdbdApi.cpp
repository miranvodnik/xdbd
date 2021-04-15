/*
 * XdbdApi.cpp
 *
 *  Created on: 10. nov. 2015
 *      Author: miran
 */

#include "XdbdApi.h"
#include "XdbdConnection.h"
using namespace xml_db_daemon;

/*! @brief create connection api
 *
 *  function creates new instance of XdbdConnection object
 *  which represents XML DB client connection
 *
 *  @param attributes reference to attribute structure for new connection
 *
 *  @return xdbd_conn_t reference to new instance of XdbdConnection object
 *
 */
xdbd_conn_t	XdbdCreateConnection (xdbd_attributes_t* attributes)
{
	return	(xdbd_conn_t) new XdbdConnection (attributes);
}

/*! @brief delete connection api
 *
 *  delete an instance of XdbdConnection object
 *
 *  @param conn reference to an instance of XdbdConnection object
 *
 */
void	XdbdDeleteConnection (xdbd_conn_t conn)
{
	delete	(XdbdConnection*) conn;
}

/*! @brief post connection request
 *
 *  send connection request to XML DB server
 *
 *  @param conn connection object reference
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 */
xdbd_result_t	XdbdPostConnect (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx)
{
	return	((XdbdConnection*) conn)->Connect();
}

/*! @brief post disconnection request
 *
 *  send disconnection request to XML DB server
 *
 *  @param conn connection object reference
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 */
void	XdbdPostDisconnect (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx)
{
	((XdbdConnection*) conn)->Disconnect();
}

/*! @brief post create-statement request
 *
 *  send create-statement request to XML DB server
 *
 *  @param conn connection object reference
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 *  @return 0 create-statement request failed
 *  @return stm handle to created statement
 *
 */
xdbd_stmt_t	XdbdPostCreateStatement (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx)
{
	XdbdStatement*	stmt = 0;
	((XdbdConnection*) conn)->CreateStatement (stmt);
	return	stmt;
}

/*! @brief post delete-statement request
 *
 *  send delete-statement request to XML DB server
 *
 *  @param stmt sql statement handle
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 *  @return 0 delete-statement succeeded
 *  @return -1 delete-statement failed
 *
 */
xdbd_result_t	XdbdPostDeleteStatement (xdbd_stmt_t stmt, xdbd_callback_t cb, xdbd_context_t ctx)
{
	return	((XdbdConnection*)((XdbdStatement*) stmt)->conn())->DeleteStatement(((XdbdStatement*) stmt));
}

/*! @brief post sql prepare request
 *
 *  send sql prepare request to XML DB server
 *
 *  @param stmt sql statement handle
 *  @param statement sql statement string
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 *  @return 0 prepare statement succeeded
 *  @return -1 prepare statement failed
 *
 */
xdbd_result_t	XdbdPostPrepare (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx)
{
	return	((XdbdStatement*) stmt)->Prepare(statement);
}

/*! @brief post sql execute request
 *
 *  send sql execute request to XML DB server
 *
 *  @param stmt previously prepared sql statement handle
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 *  @return 0 execute statement succeeded
 *  @return -1 execute statement failed
 *
 */
xdbd_result_t	XdbdPostExecute (xdbd_stmt_t stmt, xdbd_callback_t cb, xdbd_context_t ctx)
{
	return	((XdbdStatement*) stmt)->Execute();
}

/*! @brief post sql execute-direct request
 *
 *  send sql execute-direct request to XML DB server
 *
 *  @param stmt sql statement handle
 *  @param statement sql statement string
 *  @param cb unused callback function (should be null)
 *  @param ctx unused context parameter for callback function (should be null)
 *
 *  @return 0 execute-direct statement succeeded
 *  @return -1 execute-direct statement failed
 *
 */
xdbd_result_t	XdbdPostExecuteDirect (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx)
{
	return	((XdbdStatement*) stmt)->ExecuteDirect (statement);
}

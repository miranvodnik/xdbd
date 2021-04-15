/*
 * XdbdApi.h
 *
 *  Created on: 10. nov. 2015
 *      Author: miran
 */

#pragma once

#include <time.h>
#include "XdbdMessages.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef	void*	xdbd_conn_t;	//!< sql connection type
typedef	void*	xdbd_stmt_t;	//!< sql statement type
typedef	short	xdbd_short_result_t;	//!< short result type
typedef	int	xdbd_result_t;	//!< result type
typedef	int	xdbd_length_result_t;	//!< length result type
typedef	int	xdbd_int_result_t;	//!< integer result type
typedef	long	xdbd_long_result_t;	//!< long result type
typedef	char*	xdbd_string_result_t;	//!< string result type
typedef	void*	xdbd_any_result_t;	//!< general reference result type
typedef	unsigned int	xdbd_flags_t;	//!< flags type
typedef	const char*	xdbd_parname_t;	//!< parameter name references

typedef	void*	xdbd_context_t;	//!< context data reference type
typedef	void	(*xdbd_callback_t) (xdbd_conn_t conn, xdbd_context_t ctx);	//!< call-back function prototype

/*! @brief connection attributes
 *
 */
typedef	struct	_xdbd_attributes_t
{
	struct timeval	wdTimer;	//!< connection watch dog timer
}	xdbd_attributes_t;

typedef	XdbdParameter	xdbd_parameter_t;	//!< parameter type

xdbd_conn_t	XdbdCreateConnection (xdbd_attributes_t* attributes = 0);
void	XdbdDeleteConnection (xdbd_conn_t conn);
xdbd_result_t	XdbdPostConnect (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx);
void	XdbdPostDisconnect (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_stmt_t	XdbdPostCreateStatement (xdbd_conn_t conn, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostDeleteStatement (xdbd_stmt_t stmt, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostPrepare (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostExecute (xdbd_stmt_t stmt, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostExecuteDirect (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostSelect (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostUpdate (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostInsert (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdPostDelete (xdbd_stmt_t stmt, const char* statement, xdbd_callback_t cb, xdbd_context_t ctx);
xdbd_result_t	XdbdFetch (xdbd_stmt_t stmt);

#define	XdbdConnect()	XdbdPostConnect(0,0)
#define	XdbdDisconnect(conn)	XdbdPostDisconnect (conn,0,0)
#define	XdbdCreateStatement(conn)	XdbdPostCreateStatement(conn,0,0)
#define	XdbdDeleteStatement(stmt)	XdbdPostDeleteStatement(stmt,0,0)
#define	XdbdPrepare(stmt,statement)	XdbdPostPrepare(stmt,statement,0,0)
#define	XdbdExecute (stmt)	XdbdPostExecute (stmt,0,0)
#define	XdbdExecuteDirect (stmt,statement)	XdbdPostExecuteDirect (stmt,statement,0,0)
#define	XdbdSelect(stmt,parcnt,parameters)	XdbdPostSelect(stmt,parcnt,parameters,0,0)
#define	XdbdUpdate(stmt,parcnt,parameters)	XdbdPostUpdate(stmt,parcnt,parameters,0,0)
#define	XdbdInsert(stmt,parcnt,parameters)	XdbdPostInsert(stmt,parcnt,parameters,0,0)
#define	XdbdDelete(stmt,parcnt,parameters)	XdbdPostDelete(stmt,parcnt,parameters,0,0)

#if defined(__cplusplus)
}
#endif

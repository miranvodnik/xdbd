/*
 * XdbdStatement.h
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#pragma once

#include <sqlxdbd.h>
#include <sys/types.h>
#include <string>
#include <vector>
using namespace std;

namespace xml_db_daemon
{

/*! @brief client side SQL statement structure
 *
 */
class XdbdStatement
{
public:
	XdbdStatement(xdbd_conn_t conn, u_long ticket, const char* shmName);
	virtual ~XdbdStatement();
	int	Prepare (const char* statement);
	int	Execute ();
	int	ExecuteDirect (const char* statement);
	int	Select (const char* statement);
	int	StmtSelect (u_long shmSize);
	int	StmtMap (u_long shmSize);
	inline xdbd_conn_t conn () { return m_conn; }	//!< get associated connection
	inline u_long ticket () { return m_ticket; }	//!< get ticket - server side statement handle
	inline unsigned char* shmAddr () { return m_shmAddr; }	//!< get virtual address of shared memory segment
	inline int shmSize () { return m_shmSize; }	//!< get shared memory segment size

private:
	void	Reset ();
private:
	static const int	g_maxParNr = 128;	//!< maximal number of statement parameters

	xdbd_conn_t	m_conn;	//!< associated connection
	u_long	m_ticket;	//!< ticket - server side statement handle
	string	m_shmName;	//!< name of associated shared memory segment

	unsigned char*	m_shmAddr;	//!< virtual address of shared memory segment
	int	m_shmSize;	//!< shared memory segment size
};

} /* namespace xml_db_daemon */

/*
 * XdbdStatement.cpp
 *
 *  Created on: 24. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <XdbdConnection.h>

namespace xml_db_daemon
{

/*! @brief create client side SQL statement structure
 *
 *  this structure is allocated as soon as client successfully executes
 *  SQL create-statement. Newly created statement is provided with sufficient
 *  information to establish virtual connection with SQL statement structure
 *  residing on the server side of connection
 *
 *  @param conn connection handle which 'owns' this statement
 *  @param ticket server side handle used to associate client and server statements
 *  @param shmName name of shared memory allocated by server
 *
 */
XdbdStatement::XdbdStatement(xdbd_conn_t conn, u_long ticket, const char* shmName)
{
	Reset ();
	m_conn = conn;
	m_ticket = ticket;
	m_shmName = shmName;
}

/*! @brief release client side SQL statement structure
 *
 *  it disconnects from shared memory segment used to transfer
 *  data between server and client side statements
 *
 */
XdbdStatement::~XdbdStatement()
{
	if (m_shmAddr != MAP_FAILED)
		munmap (m_shmAddr, m_shmSize);
	Reset ();
}

/*! @brief initialize client side SQL statement structure
 *
 */
void	XdbdStatement::Reset ()
{
	m_shmAddr = (unsigned char*) MAP_FAILED;
	m_shmSize = 0;
}

/*! @brief prepare SQL statement
 *
 *  wrapper for XdbdConnection Prepare() function
 *
 *  @see XdbdConnection
 *
 */
int	XdbdStatement::Prepare (const char* statement)
{
	return	((XdbdConnection*)m_conn)->Prepare (this, statement);
}

/*! @brief execute prepared SQL statement
 *
 *  wrapper for XdbdConnection Execute() function
 *
 *  @see XdbdConnection
 *
 */
int	XdbdStatement::Execute ()
{
	return	((XdbdConnection*)m_conn)->Execute (this);
}

/*! @brief execute not prepared SQL statement
 *
 *  wrapper for XdbdConnection ExecuteDirect ()
 *
 *  @see XdbdConnection
 */
int	XdbdStatement::ExecuteDirect (const char* statement)
{
	return	((XdbdConnection*)m_conn)->ExecuteDirect (this, statement);
}

/*! @brief map shared memory
 *
 *  this function makes client side mapping of statement's shared memory
 *  into it's virtual address space. If mapping succeeds it is possible to
 *  transfer data between client and server side statements. This mapping
 *  is made in the last phase of client side statement preparation, after
 *  client side has received confirmation of successful execution of SQL
 *  prepare statement. After prepare statement succeeds, associated shared
 *  memory is filled with information which will enable client side to
 *  read data after executing SQL statements
 *
 *  Function follows these steps:
 *
 *  - if shared memory is already mapped and it has sufficient size it is
 *  leaved intact
 *
 *  - if it is mapped with insufficient side it is reopened and reallocated
 *
 *  -if it is not mapped it is opened, allocated and mapped into virtual
 *  address space of calling process
 *
 *  - after successful mapping, function check structure of shared memory
 *  segment:
 *
 *  - shared memory must contain six integer numbers:
 *  - shared memory size
 *  - number of statement parameters
 *  - size of statement parameters
 *  - number of column descriptors
 *  - size of column descriptors
 *  - number of affected rows
 *
 *  - size of shared memory must be sufficiently large to accomodate
 *  parameter and column descriptors
 *
 *  @param shmSize size of shared memory segment
 *
 */
int	XdbdStatement::StmtMap (u_long shmSize)
{
	if (m_shmAddr != MAP_FAILED)
	{
		if (shmSize > (u_long) m_shmSize)
		{
			int	shmFd;
			if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR, 0666)) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
				return	SQL_ERROR;
			}
			munmap (m_shmAddr, m_shmSize);
			m_shmSize = shmSize;
			if (ftruncate (shmFd, m_shmSize) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, m_shmSize, errno);
				return	SQL_ERROR;
			}
			m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0);
			close (shmFd);
		}
	}
	else
	{
		int	shmFd;
		if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR, 0666)) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
			return	SQL_ERROR;
		}
		m_shmSize = shmSize;
		if (ftruncate (shmFd, shmSize) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, shmSize, errno);
			return	SQL_ERROR;
		}
		m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0);
		close (shmFd);
	}

	unsigned char*	addr = m_shmAddr;

	if (addr == MAP_FAILED)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_shmSize, errno);
		return	SQL_ERROR;
	}

	if (m_shmSize < (int) (6 * sizeof (int)))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("SHM size too short = %d, expected = %d"), m_shmSize, 6 * sizeof (int));
		return	SQL_ERROR;
	}

	int	hdrSize  = ((((int*)addr) [1] + ((int*)addr) [3]) * 2 + 6) * sizeof (int) + ((int*)addr) [2];
	if ((u_long) hdrSize > shmSize)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("SHM size too short = %d, expected = %d"), m_shmSize, hdrSize);
		return	SQL_ERROR;
	}

	return	SQL_SUCCESS;
}

/*! @brief map shared memory
 *
 *  this function makes client side mapping of statement's shared memory
 *  into it's virtual address space. If mapping succeeds it is possible to
 *  transfer data between client and server side statements. This mapping
 *  is made in the last phase of client side statement preparation, after
 *  client side has received confirmation of successful execution of SQL
 *  prepare statement. After prepare statement succeeds, associated shared
 *  memory is filled with information which will enable client side to
 *  read data after executing SQL statements
 *
 *  Function follows these steps:
 *
 *  - if shared memory is already mapped and it has sufficient size it is
 *  leaved intact
 *
 *  - if it is mapped with insufficient side it is reopened and reallocated
 *
 *  -if it is not mapped it is opened, allocated and mapped into virtual
 *  address space of calling process
 *
 *  - after successful mapping, function check structure of shared memory
 *  segment:
 *
 *  - shared memory must contain six integer numbers:
 *  - shared memory size
 *  - number of statement parameters
 *  - size of statement parameters
 *  - number of column descriptors
 *  - size of column descriptors
 *  - number of affected rows
 *
 *  - size of shared memory must be sufficiently large to accomodate
 *  parameter and column descriptors
 *
 *  @param shmSize size of shared memory segment
 *
 */
int	XdbdStatement::StmtSelect (u_long shmSize)
{
	if (m_shmAddr != MAP_FAILED)
	{
		if (shmSize > (u_long) m_shmSize)
		{
			int	shmFd;
			if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR, 0666)) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
				return	SQL_ERROR;
			}
			munmap (m_shmAddr, m_shmSize);
			m_shmSize = shmSize;
			if (ftruncate (shmFd, m_shmSize) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, m_shmSize, errno);
				return	SQL_ERROR;
			}
			m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0);
			close (shmFd);
		}
	}
	else
	{
		int	shmFd;
		if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR, 0666)) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
			return	SQL_ERROR;
		}
		m_shmSize = shmSize;
		if (ftruncate (shmFd, shmSize) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, shmSize, errno);
			return	SQL_ERROR;
		}
		m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0);
		close (shmFd);
	}

	unsigned char*	addr = m_shmAddr;

	if (addr == MAP_FAILED)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%d) failed, errno = %d"), m_shmSize, errno);
		return	SQL_ERROR;
	}

	if (m_shmSize < (int) (6 * sizeof (int)))
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("SHM size too short = %d, expected = %d"), m_shmSize, 6 * sizeof (int));
		return	SQL_ERROR;
	}

	return	0;
}

} /* namespace xml_db_daemon */

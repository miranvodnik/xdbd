/*
 * XdbdShmSegment.cpp
 *
 *  Created on: 25. nov. 2015
 *      Author: miran
 */

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "XdbdShmSegment.h"
#include "XdbdSqlParser.h"
#include <iostream>
#include <sstream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief create new instance of shared memory description
 *
 *  initializes components: shared memory address is invalid,
 *  SQL statement and associated parser are unknown, shared memory
 *  name is initialized
 *
 *  @param shmName shared memory name
 *
 */
XdbdShmSegment::XdbdShmSegment (string shmName)
{
	m_shmName = shmName;
	m_shmSize = -1;
	m_hdrSize = -1;
	m_shmAddr = MAP_FAILED;
	m_sqlParser = 0;
}

/*! @brief destroy an instance of shared memory description
 *
 *  unlinks shared memory segment associated with SQL statements and
 *  deletes its parser
 *
 */
XdbdShmSegment::~XdbdShmSegment ()
{
	if (m_shmAddr != MAP_FAILED)
		if (munmap (m_shmAddr, m_shmSize) < 0)
			xdbdErrReport (SC_XDBD, SC_ERR, err_info("munmap (%ld, %d) failed, errno = %d"), m_shmAddr, m_shmSize,
				errno);
	m_shmAddr = MAP_FAILED;
	m_shmSize = 0;
	m_hdrSize = 0;
	shm_unlink (m_shmName.c_str ());
	m_shmName = "";
	delete (XdbdSqlParser*) m_sqlParser;
	m_sqlParser = 0;
}

void XdbdShmSegment::sqlParser (void* sqlParser) { delete (XdbdSqlParser*) m_sqlParser; m_sqlParser = sqlParser; }	//!< set sql parser object reference

/*! @brief resize shared memory segment
 *
 *  function resizes shared memory segmen in the following manner:
 *
 *  if there is no shared memory segment or there is one with insufficient size
 *  then create new one, otherwise do nothing
 *
 *  @param shmSize desired size of shared memory segment
 *
 *  @return MAP_FAILED shared memory cannot be created or resized
 *  @return other shared memory created
 *
 */
void*	XdbdShmSegment::Resize (int shmSize)
{
	int	shmFd;

	if (m_shmAddr != MAP_FAILED)
	{
		if (shmSize > m_shmSize)
		{
			if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR|O_CREAT, 0666)) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
				return	MAP_FAILED;
			}
			munmap (m_shmAddr, m_shmSize);
			if (ftruncate (shmFd, shmSize) < 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, shmSize, errno);
				close (shmFd);
				return	MAP_FAILED;
			}
			m_shmSize = shmSize;
			if ((m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0)) == MAP_FAILED)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%s) failed, errno = %d"), m_shmName.c_str(), errno);
				close (shmFd);
				return	MAP_FAILED;
			}
			close (shmFd);
		}
	}
	else
	{
		if ((shmFd = shm_open (m_shmName.c_str(), O_RDWR|O_CREAT, 0666)) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("shm_open (%s) failed, errno = %d"), m_shmName.c_str(), errno);
			return	MAP_FAILED;
		}
		if (ftruncate (shmFd, shmSize) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ftruncate (%d, %d) failed, errno = %d"), shmFd, shmSize, errno);
			close (shmFd);
			return	MAP_FAILED;
		}
		m_shmSize = shmSize;
		if ((m_shmAddr = (unsigned char*) mmap (0, m_shmSize, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd, 0)) == MAP_FAILED)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("mmap (%s) failed, errno = %d"), m_shmName.c_str(), errno);
			close (shmFd);
			return	MAP_FAILED;
		}
		close (shmFd);
	}
	return	m_shmAddr;
}

/*! @ brief display SQL statement parameter
 *
 *  @param index parameter index
 *  @param par parameter field reference
 *  @param plen length of parameter field
 *  @param parType parameter type
 *
 */
void XdbdShmSegment::DisplayParameter (int index, unsigned char* par, int plen, int parType)
{
	if (g_debug == false)
		return;

	stringstream str;
	str << "PAR " << index << ": ";
	switch (parType)
	{
	case SQL_SMALLINT:
		str << *((SQLSMALLINT*) par);
		break;
	case SQL_INTEGER:
		if (sizeof(int) != sizeof(long))
		{
			switch (plen)
			{
			case sizeof(int):
				str << *((SQLINTEGER*) par);
				break;
			case sizeof(long):
				str << *((SQLLEN*) par);
				break;
			default:
				break;
			}
		}
		else
			str << *((SQLINTEGER*) par);
		break;
	case SQL_VARCHAR:
		{
			str << "'" << par << "'";
		}
		break;
	default:
		str << "NULL";
		break;
	}
	str << endl;
	str << ends;

	const char* msg = str.str ().c_str();
	xdbdErrReport (SC_XDBD, SC_ERR, msg);
}

} /* namespace xml_db_daemon */

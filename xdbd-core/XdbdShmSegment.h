/*
 * XdbdShmSegment.h
 *
 *  Created on: 25. nov. 2015
 *      Author: miran
 */

#pragma once

#include <string>
using namespace std;

namespace xml_db_daemon
{

/*! @brief class representing shared memory description
 *
 *  this is actualy description of SQL statement since it contains
 *  data which fully describe SQL statement: SHM used to communicate parameters
 *  and SQL results between SQL client and server, SQL statement string and its parser
 *
 */
class XdbdShmSegment
{
public:
	XdbdShmSegment(string shmName);
	virtual ~XdbdShmSegment();
	inline string shmName () { return m_shmName; }	//!< get shared memory name
	inline string sqlStmt () { return m_sqlStmt; }	//!< get sql statement
	inline void sqlStmt (const char* sqlStmt) { m_sqlStmt = sqlStmt; }	//!< set sql statement
	inline int shmSize () { return m_shmSize; }	//!< get shared memory size
	inline int hdrSize () { return m_hdrSize; }	//!< get shared memory header size
	inline void hdrSize (int hdrSize) { m_hdrSize = hdrSize; }	//!< set shared memory header size
	inline void* shmAddr () { return m_shmAddr; }	//!< get shared memory address
	inline void* sqlParser () { return m_sqlParser; }	//!< get sql parser object reference
	void sqlParser (void* sqlParser);
	void*	Resize (int shmSize);
	void DisplayParameter (int index, unsigned char* par, int plen, int parType);
private:
	string	m_shmName;	//!< shared memory name
	string	m_sqlStmt;	//!< SQL statement string
	int	m_shmSize;	//!< shared memory size
	int	m_hdrSize;	//!< size of shared memory header
	void*	m_shmAddr;	//! shared memory address
	void*	m_sqlParser;	// SQL statement parser
};

} /* namespace xml_db_daemon */

/*
 * XdbdCommon.h
 *
 *  Created on: 5. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdErrApi.h"
#include "XdbdMessages.h"
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <map>
using namespace std;

namespace xml_db_daemon
{

#define	xdbdalign(A,T)	(T) ((((long) (A)) + (sizeof (T) - 1)) & ~(sizeof (T) - 1))
#define	setcolflag(A,O,I,F)	((*((A) + (O) + ((I) >> 1))) |= ((((I) & 1) == 0) ? (F) : ((F) << 4)))
#define	chkcolflag(A,O,I,F)	((*((A) + (O) + ((I) >> 1))) & ((((I) & 1) == 0) ? (F) : ((F) << 4)))
#define	clrcolflag(A,O,I,F)	((*((A) + (O) + ((I) >> 1))) &= ~((((I) & 1) == 0) ? (F) : ((F) << 4)))

#define	COLPRIMARY	(1<<0)
#define	COLNULLABLE	(1<<1)
#define	COLNULLVALUE	(1<<2)
#define	COLPARVALUE	(1<<3)

#define	INTVAL	(1 << 0)
#define	STRVAL	(1 << 1)
#define	SHMNAME	(1 << 2)
#define	TABNAME	(1 << 3)
#define	COLNAME	(1 << 4)

#define	XDBD_SEM_KEY	0x00001000

const	string	g_localSocketPath = "/var/run/xdbd.socket";
const	string	g_localMqPath = "/xdbd.mq";
const	string	g_workerMqPath = "/xdbd-worker.mq";
const	string	g_workingQueueKeyPath = "/var/run/xdbd.mq";
const	unsigned int	g_tcpSocketNumber = 12345;	// ux-domain listen socket
const	unsigned int	g_localListener = 3;	// worker's unix domain listener
const	unsigned int	g_networkListener = 4;	// worker's network listener
const	unsigned int	g_mqListener = 5;	// worker's posix queue listener
const	unsigned int	g_localCtlWriter = 6;	// worker's unix domain master client
const	unsigned int	g_localWrkReader = 6;	// worker's unix domain master client
const	unsigned int	g_localCtlReader = 7;	// worker's unix domain master client
const	unsigned int	g_localWrkWriter = 7;	// worker's unix domain master client
const	bool	g_debug = false;	// display debug info?
const	unsigned int	g_threadCount = 4;	// minimal number of working threads

const int	g_shmSizeIndex = 0;
const int	g_parCountIndex = 1;
const int	g_parSizeIndex = 2;
const int	g_colCountIndex = 3;
const int	g_colSizeIndex = 4;
const int	g_rowCountIndex = 5;

#define	_STR2(line)	#line
#define	_STR(line)	_STR2(line)
#define	err_info(fmt)	__FILE__ ":" _STR(__LINE__) ": " fmt

typedef	enum
{
	XdbdFirstWord,
	XdbdAdminWord,
	XdbdAdminStartWord,
	XdbdAdminRestartWord,
	XdbdAdminStopWord,
	XdbdAdminStatusWord,
	XdbdWorkerWord,
	XdbdSqlWord,
	XdbdSqlConnectWord,
	XdbdSqlDisconnectWord,
	XdbdSqlCreateStatementWord,
	XdbdSqlDeleteStatementWord,
	XdbdSqlPrepareWord,
	XdbdSqlExecuteWord,
	XdbdSqlExecuteDirectWord,
	XdbdLastWord,
}	XdbdWordCode;

class	wordcmp : public binary_function < const char*, const char*, bool >
{
public:
	bool operator () (const first_argument_type& x, const first_argument_type& y) const
	{
		return	strcasecmp (x, y) < 0;
	}
};

class	XdbdCommon
{
private:
	typedef map < const char*, XdbdWordCode, wordcmp >	wordmap;
	XdbdCommon ()
	{
		m_wordmap["admin"] = XdbdAdminWord;
		m_wordmap["start"] = XdbdAdminStartWord;
		m_wordmap["restart"] = XdbdAdminRestartWord;
		m_wordmap["stop"] = XdbdAdminStopWord;
		m_wordmap["status"] = XdbdAdminStatusWord;
		m_wordmap["sql"] = XdbdSqlWord;
		m_wordmap["connect"] = XdbdSqlConnectWord;
		m_wordmap["disconnect"] = XdbdSqlDisconnectWord;
		m_wordmap["stmtcreate"] = XdbdSqlCreateStatementWord;
		m_wordmap["stmtdelete"] = XdbdSqlDeleteStatementWord;
		m_wordmap["worker"] = XdbdWorkerWord;

		int	nrcpus = get_nprocs_conf();
		if (nrcpus < 1)
			nrcpus = 1;
		m_cpuCount = nrcpus;
		m_wthreadCount = (nrcpus < (int)g_threadCount) ? g_threadCount : nrcpus;
		char*	env = getenv ("XDBD_INDEX_ALL");
		m_indexAll = ((env != 0) && (strcasecmp (env, "true") == 0));
		m_entity = XdbdNoEntity;
		m_pid = getpid ();
	}
	~XdbdCommon () {}
public:
	inline static XdbdWordCode	findWord (const char* word) { return g_commonInitializer->_findWord (word); }
	inline static int	threadCount () { return g_commonInitializer->_threadCount (); }
	inline static int	cpuCount () { return g_commonInitializer->_cpuCount(); }
	inline static bool	indexAll () { return g_commonInitializer->_indexAll(); }
	inline static XdbdEntity	entity () { return g_commonInitializer->_entity(); }
	inline static void entity (XdbdEntity entity) { g_commonInitializer->_entity (entity); }
	inline static pid_t	pid () { return g_commonInitializer->_pid(); }
private:
	inline	XdbdWordCode	_findWord (const char* word)
	{
		return (m_wordmap.find (word) == m_wordmap.end ()) ? XdbdFirstWord : m_wordmap [word];
	}
	inline int	_threadCount () { return m_wthreadCount; }
	inline int	_cpuCount () { return m_cpuCount; }
	inline bool	_indexAll () { return m_indexAll; }
	inline XdbdEntity	_entity () { return m_entity; }
	inline void _entity (XdbdEntity entity) { m_entity = entity; }
	inline pid_t	_pid () { return m_pid; }
private:
	static	XdbdCommon*	g_commonInitializer;
	wordmap	m_wordmap;
	int	m_wthreadCount;
	int	m_cpuCount;
	bool	m_indexAll;
	XdbdEntity	m_entity;
	pid_t	m_pid;
};

}

/*
 * XdbdXmlTableInfo.h
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "XdbdXmlColumnInfo.h"
#include "XdbdTrigger.h"

#include <vector>
#include <map>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

class XdbdXmlTableInfo
{
public:
	typedef struct
	{
		string	name;
		size_t	size;
		void*	addr;
	}	shmdescr;
	typedef vector < shmdescr >	shmset;
	typedef shmset::iterator	shm_iterator;
	typedef vector < XdbdXmlColumnInfo* >	collist;
	typedef collist::iterator	col_iterator;
	typedef vector < XdbdTrigger* >	trglist;
	typedef trglist::iterator	trg_iterator;
	XdbdXmlTableInfo (const char* name);
	virtual ~XdbdXmlTableInfo();
	int	IndexXmlTable (xmlXPathContextPtr xmlCtx, string name);
	int	ReloadXmlTable (xmlXPathContextPtr xmlCtx, string name);
	void	AlignColPositions ();
	inline void add (XdbdXmlColumnInfo* col) { m_collist.push_back(col); }
	inline void add_trigger (XdbdTrigger* trg) { m_trglist.push_back(trg); }
	XdbdXmlColumnInfo* find (string name);
	inline string name () { return m_name; }
	inline shm_iterator shm_begin () { return m_shmset.begin(); }
	inline shm_iterator shm_end () { return m_shmset.end(); }
	inline shm_iterator shm_next (shm_iterator it) { return ++it; }
	inline col_iterator col_begin () { return m_collist.begin(); }
	inline col_iterator col_end () { return m_collist.end(); }
	inline col_iterator col_next (col_iterator it) { return ++it; }
	inline trg_iterator trg_begin () { return m_trglist.begin(); }
	inline trg_iterator trg_end () { return m_trglist.end(); }
	inline trg_iterator trg_next (trg_iterator it) { return ++it; }
	inline int	resCount () { return m_collist.size(); }
	inline int	resSize () { return m_resSize; }
	inline int	colhdrSize () { return m_colhdrSize; }
	inline void	freeCol (unsigned char* addr) { *((unsigned char**)addr) = m_free; m_free = addr; }
	unsigned char*	getFreeCol ();
	int	allocShmChunk ();
	void	restoreShmData ();

	void	LockTable ();
	void	UnlockTable ();
private:
	void	Reset ();
	char*	findNodeContent (xmlNodePtr node, const char*path[]);
	char*	findAttrContent (xmlNodePtr node, const char*attr);
	void	generateSelectStatement ();
	void	generateUpdateStatement ();
	void	generateInsertStatement ();
	void	generateDeleteStatement ();
	void	connectDB ();
	void	connectSelectStatement ();
	void	connectUpdateStatement ();
	void	connectInsertStatement ();
	void	connectDeleteStatement ();
	int	bindSelectParameters (xmlNodePtr node);
	int	checkSelectResults (xmlNodePtr node);
	void	bindUpdateParameters (xmlNodePtr node);
	void	bindInsertParameters (xmlNodePtr node);
	void	bindDeleteParameters (unsigned char* rowData);
	void	saveShmData ();
private:
	string	m_name;
	shmset	m_shmset;
	collist	m_collist;
	trglist	m_trglist;
	u_int	m_colhdrSize;
	u_int	m_resSize;
	unsigned char*	m_free;
	static	int	g_refCount;
	static	const u_char	g_deadPtr[];

	string	m_selectStm;
	string	m_updateStm;
	string	m_insertStm;
	string	m_deleteStm;

	static	SQLHENV	g_henv;
	static	SQLHDBC g_hdbc;
	SQLHSTMT m_hselectStm;
	SQLHSTMT m_hupdateStm;
	SQLHSTMT m_hinsertStm;
	SQLHSTMT m_hdeleteStm;

	static	bool g_connected;
	bool m_selectPrepared;
	bool m_updatePrepared;
	bool m_insertPrepared;
	bool m_deletePrepared;
	char* m_rowData;
	char* m_colData;
	char* m_parData;
	SQLLEN* m_nullInd;

	sem_t	m_sem;
};

} /* namespace xml_db_daemon */

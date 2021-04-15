/*
 * XdbdTrigger.h
 *
 *  Created on: 9. dec. 2015
 *      Author: miran
 */

#pragma once

#include <sys/types.h>
#include "XdbdTriggerParameter.h"
#include <vector>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

typedef	vector < XdbdTriggerParameter* >	parlist;
typedef void* (*loadf) (int n, char*p[]);
typedef void (*firef) (void* ctx, short function, unsigned char* oldData, unsigned char* newData, size_t size);
typedef void (*unloadf) (void* ctx);

class XdbdTrigger
{
public:
	XdbdTrigger (short id, u_char* name, u_char* library, u_char* load, u_char* fire, u_char* unload, u_char* table, short function);
	virtual ~XdbdTrigger();
	bool activate ();
	inline void add (XdbdTriggerParameter* par) { m_parlist.push_back (par); }
	inline void	fire (short function, unsigned char* oldData, unsigned char* newData, size_t size)
		{ cout << "trigger fired, type = " << function << ", table = " << m_table << ", trigger = " << m_name << ", function = " << m_fire << endl; (*((firef)m_hfire)) (m_ctx, function, oldData, newData, size); }
	inline short	id () { return m_id; }
	inline u_char*	triggerName () { return m_name; }
	inline u_char*	libraryName () { return m_library; }
	inline u_char*	loadName () { return m_load; }
	inline u_char*	fireName () { return m_fire; }
	inline u_char*	unloadName () { return m_unload; }
	inline u_char*	tableName () { return m_table; }
	inline short	function () { return m_function; }
public:
	static const short	g_selectFunction = (1 << 0);
	static const short	g_insertFunction = (1 << 1);
	static const short	g_updateFunction = (1 << 2);
	static const short	g_deleteFunction = (1 << 3);
private:
	short	m_id;
	u_char*	m_name;
	u_char*	m_library;
	u_char*	m_load;
	u_char*	m_fire;
	u_char*	m_unload;
	u_char*	m_table;
	short	m_function;
	bool	m_activated;
	void*	m_handle;
	void*	m_hload;
	void*	m_hfire;
	void*	m_hunload;
	void*	m_ctx;
	parlist	m_parlist;
};

} /* namespace xml_db_daemon */

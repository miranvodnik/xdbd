/*
 * XdbdWorkingThread.h
 *
 *  Created on: 3. nov. 2015
 *      Author: miran
 */

#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "XdbdRunnable.h"
#include "XdbdCommon.h"
#include "XdbdBaseJob.h"
#include "XdbdMessageQueue.h"

#include <list>

using namespace std;

namespace xml_db_daemon
{

/*! @brief working thread class
 *
 *  it is an implementation of XdbdRunnable and as such it must
 *  implement Run(), InitInstance() and ExitInstance() functions
 *
 */
class	XdbdWorkingThread: public XdbdRunnable
{
private:
	typedef	list < XdbdWorkingThread* >	wtlist;	//!< type representing working thread list

public:
	XdbdWorkingThread (u_int index, bool internal);
	virtual ~XdbdWorkingThread();
	static void	StartWorkingThreads ();
	static void	StopWorkingThreads ();
	static bool	ReplaceWorkingThread (XdbdWorkingThread* wth);
	inline u_int	index () { return m_index; }	//!< get index of CPU thread affinity
	inline bool internal () { return m_internal; }	//!< internal thread
private:
	virtual int Run (void);
	virtual int InitInstance (void);
	virtual int ExitInstance (void);
	int	StartSentinel (u_long tvalue);
	int	StopSentinel (void);
	int	RestartSentinel (u_long tvalue);
	static	void	g_ptf (sigval_t val);
	void	_g_ptf ();
	inline static void ThreadCleanup (void* data) { ((XdbdWorkingThread*)data)->ThreadCleanup(); }
	void ThreadCleanup ();
	static void JobCleanup (void* data);
private:
	static wtlist	g_wtlist;	//!< global list of working threads
	static bool	g_started;	//!< indicator: true - working threads started
	u_int	m_index;	//!< CPU thread affinity for this working thread
	bool	m_internal; //!< internal thread
	timer_t		m_tid;	//!< watch dog timer for this working thread
};

} /* namespace xml_db_daemon */

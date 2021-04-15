/*
 * XdbdChkXsdDir.h
 *
 *  Created on: 1. mar. 2016
 *      Author: miran
 */

#pragma once

#include <time.h>
#include <sys/inotify.h>
#include <XdbdBaseJob.h>
#include <string>
using namespace std;

namespace xml_db_daemon
{

class XdbdChkXsdDir : public XdbdBaseJob
{
public:
	typedef struct
	{
		string	m_dirname;
	}	XdbdChkContext;
private:
	XdbdChkXsdDir ();
public:
	XdbdChkXsdDir (int fd, void* data, struct inotify_event* nevent);
	virtual ~XdbdChkXsdDir ();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
	virtual int	Invoke (XdbdRunningContext* ctx) { return Execute ((XdbdWorkingThread*)ctx); }
private:
	static XdbdChkXsdDir	*g_XdbdChkXsdDir;
	static struct timespec	g_mtime;
	int	m_fd;
	XdbdChkContext*	m_ctx;
	int	m_wd;
	uint32_t	m_mask;
	uint32_t	m_cookie;
	uint32_t	m_len;
	string	m_fname;
};

} /* namespace xml_db_daemon */

/*
 * XdbdChkStatDir.h
 *
 *  Created on: Jan 29, 2019
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

class XdbdChkStatDir: public XdbdBaseJob
{
public:
	typedef struct
	{
		string	m_dirname;
	}	XdbdChkContext;
public:
	XdbdChkStatDir (int fd, void* data, struct inotify_event* nevent);
	virtual ~XdbdChkStatDir();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
	virtual int	Invoke (XdbdRunningContext* ctx) { return Execute ((XdbdWorkingThread*)ctx); }
private:
	int	m_fd;
	void*	m_ctx;
	int	m_wd;
	uint32_t	m_mask;
	uint32_t	m_cookie;
	uint32_t	m_len;
	string	m_fname;
};

}

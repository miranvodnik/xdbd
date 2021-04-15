/*
 * XdbdChkStatDir.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: miran
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <XdbdChkStatDir.h>
#include <XdbdXmlDataBase.h>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

XdbdChkStatDir::XdbdChkStatDir (int fd, void* data, struct inotify_event* nevent) : XdbdBaseJob (XdbdBaseJob::Infinite)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdChkStatDir (%s)", nevent->name);
#endif
	m_fd = fd;
	m_ctx = data;
	m_wd = nevent->wd;
	m_mask = nevent->mask;
	m_cookie = nevent->cookie;
	m_len = nevent->len;
	m_fname = nevent->name;
}

XdbdChkStatDir::~XdbdChkStatDir()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdChkStatDir (%s)", m_fname.c_str());
#endif
}

int XdbdChkStatDir::Execute (XdbdWorkingThread* wt)
{
	if ((m_mask & (IN_CLOSE_WRITE | IN_MOVED_TO)) == 0)
		return	0;

#if defined (XDBD_PERFTEST)
	struct timeval	tbegin, tend;

	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST

	xdbdErrReport (SC_XDBD, SC_ERR, "Load statistical data, file = '%s'", m_fname.c_str());
	XdbdXmlDataBase::ReloadStatFile (m_ctx, (char*) m_fname.c_str());

#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: loading statistical file '%s/%s', total time = %d.%06d"), m_ctx->m_dirname.c_str(), m_fname.c_str(), tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST
	return	0;
}

int XdbdChkStatDir::Cleanup ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("Statistical job killed, file = %s"), m_fname.c_str());
	delete	this;
	return	0;
}

void	XdbdChkStatDir::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - checking statistical dir: file = %s"), m_fname.c_str());
}

}

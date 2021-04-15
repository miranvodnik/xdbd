/*
 * XdbdChkXmlDir.cpp
 *
 *  Created on: 1. mar. 2016
 *      Author: miran
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <XdbdChkXmlDir.h>
#include <XdbdXmlDataBase.h>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

XdbdChkXmlDir*	XdbdChkXmlDir::g_XdbdChkXmlDir = new XdbdChkXmlDir ();
struct timespec XdbdChkXmlDir::g_mtime;

XdbdChkXmlDir::XdbdChkXmlDir () : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
	clock_gettime (CLOCK_REALTIME, &g_mtime);
}

XdbdChkXmlDir::XdbdChkXmlDir (int fd, void* data, struct inotify_event* nevent) : XdbdBaseJob (XdbdBaseJob::Infinite)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdChkXmlDir (%s)", nevent->name);
#endif
	m_fd = fd;
	m_ctx = (XdbdChkContext*) data;
	m_wd = nevent->wd;
	m_mask = nevent->mask;
	m_cookie = nevent->cookie;
	m_len = nevent->len;
	m_fname = nevent->name;
}

XdbdChkXmlDir::~XdbdChkXmlDir ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdChkXmlDir (%s)", m_fname.c_str());
#endif
}

int XdbdChkXmlDir::Execute (XdbdWorkingThread* wt)
{
	if ((m_mask & (IN_CLOSE_WRITE | IN_MOVED_TO)) == 0)
		return	0;

#if defined (XDBD_PERFTEST)
	struct timeval	tbegin, tend;

	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST

	xdbdErrReport (SC_XDBD, SC_ERR, "Reload xml db data, file = '%s', dir = '%s'", m_fname.c_str(), m_ctx->m_dirname.c_str());
	XdbdXmlDataBase::ReloadXmlFile ((char*) m_fname.c_str(), (char*) m_ctx->m_dirname.c_str());

#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reloading xml file '%s/%s', total time = %d.%06d"), m_ctx->m_dirname.c_str(), m_fname.c_str(), tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST
	return	0;
}

int XdbdChkXmlDir::Cleanup ()
{
	delete	this;
	return	0;
}

void	XdbdChkXmlDir::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - checking xml dir: file = %s"), m_fname.c_str());
}

} /* namespace xml_db_daemon */

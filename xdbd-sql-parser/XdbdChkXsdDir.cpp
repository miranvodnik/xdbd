/*
* XdbdChkXsdDir.cpp
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
#include <XdbdChkXsdDir.h>
#include <XdbdXmlDataBase.h>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

XdbdChkXsdDir*	XdbdChkXsdDir::g_XdbdChkXsdDir = new XdbdChkXsdDir ();

XdbdChkXsdDir::XdbdChkXsdDir () : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
}

XdbdChkXsdDir::XdbdChkXsdDir (int fd, void* data, struct inotify_event* nevent) : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdChkXsdDir (%s)", nevent->name);
#endif
	m_fd = fd;
	m_ctx = (XdbdChkContext*) data;
	m_wd = nevent->wd;
	m_mask = nevent->mask;
	m_cookie = nevent->cookie;
	m_len = nevent->len;
	m_fname = nevent->name;
}

XdbdChkXsdDir::~XdbdChkXsdDir ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdChkXsdDir (%s)", m_fname.c_str());
#endif
}

int XdbdChkXsdDir::Execute (XdbdWorkingThread* wt)
{
	if ((m_mask & (IN_CLOSE_WRITE | IN_MOVED_TO)) == 0)
		return	0;

	xdbdErrReport (SC_XDBD, SC_ERR, "Reload xsd db schema, file = '%s', dir = '%s'", m_fname.c_str(), m_ctx->m_dirname.c_str());
	return	0;
}

int XdbdChkXsdDir::Cleanup ()
{
	delete	this;
	return	0;
}

void	XdbdChkXsdDir::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - checking xsd dir: file = %s"), m_fname.c_str());
}

} /* namespace xml_db_daemon */

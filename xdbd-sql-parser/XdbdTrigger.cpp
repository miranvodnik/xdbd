/*
 * XdbdTrigger.cpp
 *
 *  Created on: 9. dec. 2015
 *      Author: miran
 */

#include <errno.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include "XdbdTrigger.h"
#include "XdbdCommon.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

XdbdTrigger::XdbdTrigger(short id, u_char* name, u_char* library, u_char* load, u_char* fire, u_char* unload, u_char* table, short function)
{
	m_id = id;
	m_name = (u_char*) strdup ((char*)name);
	m_library = (u_char*) strdup ((char*)library);
	m_load = (u_char*) strdup ((char*)load);
	m_fire = (u_char*) strdup ((char*)fire);
	m_unload = (u_char*) strdup ((char*)unload);
	m_table = (u_char*) strdup ((char*)table);
	m_function = function;
	m_activated = false;
	m_handle = 0;
	m_hload = 0;
	m_hfire = 0;
	m_hunload = 0;
	m_ctx = 0;
}

XdbdTrigger::~XdbdTrigger()
{
	free (m_name);
	free (m_library);
	free (m_load);
	free (m_fire);
	free (m_unload);
	free (m_table);
	if (m_handle != 0)
	{
		(*((unloadf) m_hunload)) (m_ctx);
		dlclose (m_handle);
	}
}

bool	XdbdTrigger::activate ()
{
	if (m_activated)
		return	true;
	void*	handle;
	if ((handle = dlopen ((char*) m_library, RTLD_LAZY)) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("dlopen (%s) failed, errno = %d"), m_library, errno);
		return	false;
	}
	if ((m_hload = dlsym (handle, (char*) m_load)) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("dlsym (%s) failed, errno = %d"), m_load, errno);
		dlclose (handle);
		return	false;
	}
	if ((m_hfire = dlsym (handle, (char*) m_fire)) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("dlsym (%s) failed, errno = %d"), m_fire, errno);
		dlclose (handle);
		return	false;
	}
	if ((m_hunload = dlsym (handle, (char*) m_unload)) == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("dlsym (%s) failed, errno = %d"), m_unload, errno);
		dlclose (handle);
		return	false;
	}
	m_handle = handle;

	int	n = m_parlist.size();
	char**	p = (char**) malloc ((2 * n + 1) * sizeof (char*));
	if (p == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc (%d) failed, errno = %d"), (2 * n + 1) * sizeof (char*), errno);
	}

	for (int i = 0; i < n; ++i)
	{
		p [2 * i] = (char*) m_parlist[i]->name();
		p [2 * i + 1] = (char*) m_parlist[i]->value();
	}
	p [2 * n] = 0;
	m_ctx = (*((loadf) m_hload)) (2 * n, p);
	free (p);

	xdbdErrReport (SC_XDBD, SC_APL, err_info ("trigger '%s.%s' loaded"), m_library, m_name);

	return	m_activated = true;
}

} /* namespace xml_db_daemon */

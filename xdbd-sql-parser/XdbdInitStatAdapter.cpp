/*
 * XdbdInitStatAdapter.cpp
 *
 *  Created on: Feb 6, 2019
 *      Author: miran
 */

#include "XdbdInitStatAdapter.h"
#include <XdbdXmlDataBase.h>

namespace xml_db_daemon
{

XdbdInitStatAdapter::XdbdInitStatAdapter() : XdbdBaseJob (XdbdBaseJob::Infinite)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdInitStatAdapter");
#endif
}

XdbdInitStatAdapter::~XdbdInitStatAdapter()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdInitStatAdapter");
#endif
}

int XdbdInitStatAdapter::Execute (XdbdWorkingThread* wt)
{
	XdbdXmlDataBase::LoadStatFiles ();
}

int XdbdInitStatAdapter::Cleanup ()
{
	delete	this;
	return	0;
}

void XdbdInitStatAdapter::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - initialize statistical adapter"));
}

} /* namespace xml_db_daemon */

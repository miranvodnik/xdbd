/*
 * XdbdDeleteTriggerJob.cpp
 *
 *  Created on: 19. feb. 2016
 *      Author: miran
 */

#include <XdbdDeleteTriggerJob.h>

namespace xml_db_daemon
{

XdbdDeleteTriggerJob::XdbdDeleteTriggerJob (XdbdXmlTableInfo* tabinfo, unsigned char* data, size_t size) : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdDeleteTriggerJob (%s)", m_tabinfo->name().c_str());
#endif
	m_tabinfo = tabinfo;
	m_data = data;
	m_size = size;
}

XdbdDeleteTriggerJob::~XdbdDeleteTriggerJob ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdDeleteTriggerJob (%s)", m_tabinfo->name().c_str());
#endif
	if (m_data != 0)
		free (m_data);
	m_data = 0;
}

int XdbdDeleteTriggerJob::Execute (XdbdWorkingThread* wt)
{
	for (XdbdXmlTableInfo::trg_iterator trgi = m_tabinfo->trg_begin(); trgi != m_tabinfo->trg_end(); ++trgi)
	{
		XdbdTrigger*	trg = *trgi;
		if ((trg->function() & XdbdTrigger::g_deleteFunction) != 0)
			trg->fire(XdbdTrigger::g_deleteFunction, m_data, m_data, m_size);
	}
	return	0;
}

int XdbdDeleteTriggerJob::Cleanup ()
{
	delete	this;
	return	0;
}

void	XdbdDeleteTriggerJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - delete trigger fired: table = %s"), m_tabinfo->name().c_str());
}

} /* namespace xml_db_daemon */

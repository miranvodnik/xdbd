/*
 * XdbdInsertTriggerJob.cpp
 *
 *  Created on: 19. feb. 2016
 *      Author: miran
 */

#include <XdbdInsertTriggerJob.h>

namespace xml_db_daemon
{

XdbdInsertTriggerJob::XdbdInsertTriggerJob (XdbdXmlTableInfo* tabinfo, unsigned char* data, size_t size) : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "CREATE XdbdInsertTriggerJob (%s)", m_tabinfo->name().c_str());
#endif
	m_tabinfo = tabinfo;
	m_data = data;
	m_size = size;
}

XdbdInsertTriggerJob::~XdbdInsertTriggerJob ()
{
#if defined(DEBUG)
	if (g_debug) xdbdErrReport (SC_XDBD, SC_ERR, "DELETE XdbdInsertTriggerJob (%s)", m_tabinfo->name().c_str());
#endif
	if (m_data != 0)
		free (m_data);
	m_data = 0;
}

int XdbdInsertTriggerJob::Execute (XdbdWorkingThread* wt)
{
	for (XdbdXmlTableInfo::trg_iterator trgi = m_tabinfo->trg_begin(); trgi != m_tabinfo->trg_end(); ++trgi)
	{
		XdbdTrigger*	trg = *trgi;
		if ((trg->function() & XdbdTrigger::g_insertFunction) != 0)
			trg->fire(XdbdTrigger::g_insertFunction, m_data, m_data, m_size);
	}
	return	0;
}

int XdbdInsertTriggerJob::Cleanup ()
{
	delete	this;
	return	0;
}

void	XdbdInsertTriggerJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - insert trigger fired: table = %s"), m_tabinfo->name().c_str());
}

} /* namespace xml_db_daemon */

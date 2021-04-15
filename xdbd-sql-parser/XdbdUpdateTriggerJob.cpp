/*
 * XdbdUpdateTriggerJob.cpp
 *
 *  Created on: 19. feb. 2016
 *      Author: miran
 */

#include <XdbdUpdateTriggerJob.h>

namespace xml_db_daemon
{

XdbdUpdateTriggerJob::XdbdUpdateTriggerJob (XdbdXmlTableInfo* tabinfo, unsigned char* data, size_t size) : XdbdBaseJob (10 * XdbdBaseJob::NsecPerSec)
{
	m_tabinfo = tabinfo;
	m_data = data;
	m_size = size;
}

XdbdUpdateTriggerJob::~XdbdUpdateTriggerJob ()
{
	if (m_data != 0)
		free (m_data);
	m_data = 0;
}

int XdbdUpdateTriggerJob::Execute (XdbdWorkingThread* wt)
{
	for (XdbdXmlTableInfo::trg_iterator trgi = m_tabinfo->trg_begin(); trgi != m_tabinfo->trg_end(); ++trgi)
	{
		XdbdTrigger*	trg = *trgi;
		if ((trg->function() & XdbdTrigger::g_updateFunction) != 0)
			trg->fire(XdbdTrigger::g_updateFunction, m_data, m_data + m_size, m_size);
	}
	return	0;
}

int XdbdUpdateTriggerJob::Cleanup ()
{
	delete	this;
	return	0;
}

void	XdbdUpdateTriggerJob::Report ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("job - update trigger fired: table = %s"), m_tabinfo->name().c_str());
}

} /* namespace xml_db_daemon */

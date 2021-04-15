/*
 * XdbdInsertTriggerJob.h
 *
 *  Created on: 19. feb. 2016
 *      Author: miran
 */

#pragma once

#include <stdlib.h>
#include "XdbdBaseJob.h"
#include "XdbdXmlTableInfo.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

class XdbdInsertTriggerJob : public XdbdBaseJob
{
public:
	XdbdInsertTriggerJob (XdbdXmlTableInfo* tabinfo, unsigned char* data, size_t size);
	virtual ~XdbdInsertTriggerJob ();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
	virtual int	Invoke (XdbdRunningContext* ctx) { return Execute ((XdbdWorkingThread*)ctx); }
private:
	XdbdXmlTableInfo*	m_tabinfo;
	unsigned char*	m_data;
	size_t	m_size;
};

} /* namespace xml_db_daemon */

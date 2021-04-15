/*
 * XdbdInitStatAdapter.h
 *
 *  Created on: Feb 6, 2019
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

class XdbdInitStatAdapter: public XdbdBaseJob
{
public:
	XdbdInitStatAdapter();
	virtual ~XdbdInitStatAdapter();
	virtual int Execute (XdbdWorkingThread* wt);
	virtual int Cleanup ();
	virtual void Report ();
	virtual int	Invoke (XdbdRunningContext* ctx) { return Execute ((XdbdWorkingThread*)ctx); }
};

} /* namespace xml_db_daemon */

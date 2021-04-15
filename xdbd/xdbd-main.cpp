/*
 * xdbd-main.cpp
 *
 *  Created on: 28. okt. 2015
 *      Author: miran
 */

#include "XdbdController.h"
#include "XdbdWorker.h"
using namespace xml_db_daemon;

/*! @brief main function
 *
 *  main function will start either XDBD controlling entity,
 *  XDBD working entity or ordinary XDBD client.
 *
 *  Working entity cannot run alone. It can be started only by
 *  controlling entity, because only controlling entity can provide
 *  all necessary conditions to start it correctly. These conditions
 *  can be difficult to provide by other working environments.
 *
 *  If there is any running controller, it can be only one. It is
 *  impossible to run many controllers at the same time, since they
 *  use singleton system objects like listening sockets. Controller
 *  which cannot obtain all these singletons simply ceases.
 *
 *  There can be any number of ordinary clients running simultaneously,
 *  but they can do only very simple jobs: start, stop and restart
 *  working entity.
 *
 *  Ordinary client which executes 'admin start' command and which can
 *  gain control over singleton system resources, will become controlling
 *  entity
 *
 *  Every working entity is child process of controlling entity
 *
 *  Following is the list of all meaningful commands which can be executed
 *  by this function:
 *
 *  xdbd admin start - if there is no running controller (and consequently
 *  also no runnig worker) this command will start new pair in background
 *
 *  xdbd admin stop - if there is runing controller this command will
 *  stop it, together with associated worker
 *
 *  xdbd admin restart - if there ir running worker it will be terminated
 *  and started new instance
 *
 *  xdbd worker - this command can be executed only by controlling entity
 *  otherwise it will generate exception and cease.
 *
 *  @param n number of program parameters
 *  @param p program parameters
 *
 *  @return 0 process executed successfully
 *  @return other process failed
 *
 */
int	main (int n, char*p[])
{
	int	status = 1;
	XdbdRunningContext*	xdbd =
			((n > 1) && (strcmp (p[1], "worker") == 0)) ?
					(XdbdRunningContext*) new XdbdWorker (n, p) :
					(XdbdRunningContext*) new XdbdController (n, p);

	try
	{
		status = xdbd->MainLoop();
	}
	catch (int e)
	{
		cout << "exception " << e << endl;
	}
	delete	xdbd;
	return	status;
 }

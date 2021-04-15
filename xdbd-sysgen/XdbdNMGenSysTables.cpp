/*
 * XdbdNMGenSysTables.cpp
 *
 *  Created on: 25. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenSystemTables.h"
using namespace xml_db_daemon;

int	main (int n, char*p[])
{
	XdbdNMGenSystemTables*	sysgen = new XdbdNMGenSystemTables (n, p);
	sysgen->GenerateSystemTables();
	delete	sysgen;
	return	0;
}

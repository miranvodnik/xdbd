/*
 * XdbdNMGenMain.cpp
 *
 *  Created on: 11. feb. 2016
 *      Author: miran
 */

#include "XdbdNMGenAppl.h"
using namespace xml_db_daemon;

/*! @brief generate DB trigger handlers
 *
 *  main program for XML data-base trigger source
 *  handler generation
 *
 */
int	main (int n, char*p[])
{
	XdbdNMGenAppl*	nmgen = new XdbdNMGenAppl (n, p);
	nmgen->GenerateTriggerSourceFiles();
	delete	nmgen;
	return	0;
}

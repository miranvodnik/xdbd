/*
 * XdbdHamTest.cpp
 *
 *  Created on: 27. okt. 2016
 *      Author: miran
 */

#include "XdbdHamThread.h"
using namespace genxdbddata;

int	main (int n, char*p[])
{
	XdbdHamThread*	hamThread = new XdbdHamThread ();

	hamThread->InitInstance();
	hamThread->Run();
	hamThread->Cleanup();
	return	0;
}

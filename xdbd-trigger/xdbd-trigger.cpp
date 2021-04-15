
#include "xdbd-trigger.h"
#include <iostream>
using namespace std;

extern	"C"
{

static	int	cnt = 1;

extern	void*	triggerLoad (int n, char* p[])
{
	cout << "TRIGGER LOADED" << endl;
	cout << "PARAMETERS:" << endl;
	for (int i = 0; i < n; i += 2)
		cout << p[i] << " = " << p[i+1] << endl;
	return	(void*) cnt++;
}

extern	void triggerFire (void* ctx, short function, unsigned char* oldData, unsigned char* newData, size_t size)
{
	switch (function)
	{
	case	1:
		cout << "SELECT TRIGGER FIRED: " << ctx << endl;
		break;
	case	2:
		cout << "INSERT TRIGGER FIRED: " << ctx << endl;
		break;
	case	4:
		cout << "UPDATE TRIGGER FIRED: " << ctx << endl;
		break;
	case	8:
		cout << "DELETE TRIGGER FIRED: " << ctx << endl;
		break;
	}
}

extern	void triggerUnload (void* ctx)
{
	cout << "TRIGGER UNLOADED " << ctx << endl;
}

}

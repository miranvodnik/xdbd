
#include <iostream>
using namespace std;

int main (int n, char*p[])
{
	cout << "Integer sizes:" << endl;
	cout << "sizeof (short) = " << sizeof (short) << endl;
	cout << "sizeof (int)   = " << sizeof (int) << endl;
	cout << "sizeof (long)  = " << sizeof (long) << endl;

	cout << endl << "Integer offsets and conversions:" << endl;
	{
		short s1 = 1;
		short s2 = 1;
		cout << endl << "short - short  = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of short = " << *((short*)&s1) << endl;
		cout << "short of short = " << *((short*)&s2) << endl;
		cout << "int   of short = " << *((int*)&s1) << endl;
		cout << "int   of short = " << *((int*)&s2) << endl;
		cout << "long  of short = " << *((long*)&s1) << endl;
		cout << "long  of short = " << *((long*)&s2) << endl;
	}

	{
		int s1 = 1;
		short s2 = 1;
		cout << endl << "short - int    = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of int   = " << *((short*)&s1) << endl;
		cout << "short of short = " << *((short*)&s2) << endl;
		cout << "int   of int   = " << *((int*)&s1) << endl;
		cout << "int   of short = " << *((int*)&s2) << endl;
		cout << "long  of int   = " << *((long*)&s1) << endl;
		cout << "long  of short = " << *((long*)&s2) << endl;
	}

	{
		long s1 = 1;
		short s2 = 1;
		cout << endl << "short - long   = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of long  = " << *((short*)&s1) << endl;
		cout << "short of short = " << *((short*)&s2) << endl;
		cout << "int   of long  = " << *((int*)&s1) << endl;
		cout << "int   of short = " << *((int*)&s2) << endl;
		cout << "long  of long  = " << *((long*)&s1) << endl;
		cout << "long  of short = " << *((long*)&s2) << endl;
	}

	{
		short s1 = 1;
		int s2 = 1;
		cout << endl << "int   - short  = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of short = " << *((short*)&s1) << endl;
		cout << "short of int   = " << *((short*)&s2) << endl;
		cout << "int   of short = " << *((int*)&s1) << endl;
		cout << "int   of int   = " << *((int*)&s2) << endl;
		cout << "long  of short = " << *((long*)&s1) << endl;
		cout << "long  of int   = " << *((long*)&s2) << endl;
	}

	{
		int s1 = 1;
		int s2 = 1;
		cout << endl << "int   - int    = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of int   = " << *((short*)&s1) << endl;
		cout << "short of int   = " << *((short*)&s2) << endl;
		cout << "int   of int   = " << *((int*)&s1) << endl;
		cout << "int   of int   = " << *((int*)&s2) << endl;
		cout << "long  of int   = " << *((long*)&s1) << endl;
		cout << "long  of int   = " << *((long*)&s2) << endl;
	}

	{
		long s1 = 1;
		int s2 = 1;
		cout << endl << "int   - long   = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of long  = " << *((short*)&s1) << endl;
		cout << "short of int   = " << *((short*)&s2) << endl;
		cout << "int   of long  = " << *((int*)&s1) << endl;
		cout << "int   of int   = " << *((int*)&s2) << endl;
		cout << "long  of long  = " << *((long*)&s1) << endl;
		cout << "long  of int   = " << *((long*)&s2) << endl;
	}

	{
		short s1 = 1;
		long s2 = 1;
		cout << endl << "long  - short  = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of short = " << *((short*)&s1) << endl;
		cout << "short of long  = " << *((short*)&s2) << endl;
		cout << "int   of short = " << *((int*)&s1) << endl;
		cout << "int   of long  = " << *((int*)&s2) << endl;
		cout << "long  of short = " << *((long*)&s1) << endl;
		cout << "long  of long  = " << *((long*)&s2) << endl;
	}

	{
		int s1 = 1;
		long s2 = 1;
		cout << endl << "long  - int    = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of int   = " << *((short*)&s1) << endl;
		cout << "short of long  = " << *((short*)&s2) << endl;
		cout << "int   of int   = " << *((int*)&s1) << endl;
		cout << "int   of long  = " << *((int*)&s2) << endl;
		cout << "long  of int   = " << *((long*)&s1) << endl;
		cout << "long  of long  = " << *((long*)&s2) << endl;
	}

	{
		long s1 = 1;
		long s2 = 1;
		cout << endl << "long  - long   = " << ((long)&s2 - (long)&s1) << endl;
		cout << "short of long  = " << *((short*)&s1) << endl;
		cout << "short of long  = " << *((short*)&s2) << endl;
		cout << "int   of long  = " << *((int*)&s1) << endl;
		cout << "int   of long  = " << *((int*)&s2) << endl;
		cout << "long  of long  = " << *((long*)&s1) << endl;
		cout << "long  of long  = " << *((long*)&s2) << endl;
	}

	return 0;
}


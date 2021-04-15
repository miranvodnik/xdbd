
#include <sys/vfs.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;


int	main (int n, char*q[])
{
	FILE*	f;

	if ((f = fopen ("/proc/mounts", "r")) == 0)
		return	0;

	char	line[1028];
	while (fgets (line, 1024, f))
	{
		char	*p;
		if ((p = strtok (line, " \t")) == 0)
			continue;
		if ((p = strtok (0, " \t")) == 0)
			continue;
		struct	statfs	sf;
		if (statfs (p, &sf) < 0)
			continue;
		double	dfree = (sf.f_blocks == 0) ? 100.0 : (double) sf.f_bfree / (double) sf.f_blocks * 100.0;
		double	davail = (sf.f_blocks == 0) ? 100.0 : (double) sf.f_bavail / (double) sf.f_blocks * 100.0;
		cout << p << ", all = " << sf.f_blocks << ", free = " << sf.f_bfree << " (" << (int) dfree << "%), available = " << sf.f_bavail  << " (" << (int) davail << "%)" << endl;
	}
	fclose (f);

	return	0;
}


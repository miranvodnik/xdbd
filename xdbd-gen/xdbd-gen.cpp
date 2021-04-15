
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

int	main (int n, char* p[])
{
	int	counter = (n < 4) ? 10000 : atoi (p[3]);
	int	id =      (n < 5) ? 10000 : atoi (p[4]);
	int	name =    (n < 6) ? 10000 : atoi (p[5]);
	int	module =  (n < 7) ? 10000 : atoi (p[6]);
	int	port =    (n < 8) ? 10000 : atoi (p[7]);
	int	oosi =    (n < 9) ? 10000 : atoi (p[8]);
	int	state =   (n < 10) ? 10000 : atoi (p[9]);
	int	group =   (n < 11) ? 10000 : atoi (p[10]);
	int	itype =   (n < 12) ? 10000 : atoi (p[11]);

	if (n < 3)
	{
		cerr << "Usage: xdbd-gen file-name count ..." << endl;
		return	0;
	}

	int	count = atoi(p[2]);
	for (int i = 0; i < count; ++i)
	{
		FILE*	f;
		char	fileName[64];
		sprintf (fileName, "%s-%03d.xml", p[1], i);

		if ((f = fopen (fileName, "w")) == 0)
		{
			continue;
		}

		fprintf (f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf (f, "<eth_interface_root xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='eth_interface.xsd'>\n");

		for (int j = 0; j < counter; ++j)
		{
			fprintf (f, " <eth_interface>\n");
			fprintf (f, "  <nodeid>2000</nodeid>\n");
			fprintf (f, "  <eth_intf_id>%d</eth_intf_id>\n", i * counter + j);
			fprintf (f, "  <eth_intf_name>eth_%d</eth_intf_name>\n", j % name);
			fprintf (f, "  <modulenr>%d</modulenr>\n", j % module);
			fprintf (f, "  <portnr>%d</portnr>\n", j % port);
			fprintf (f, "  <oosi>%d</oosi>\n", j % oosi);
			fprintf (f, "  <admin_state>%d</admin_state>\n", j % state);
			fprintf (f, "  <log_stat_group_id>%d</log_stat_group_id>\n", j % group);
			fprintf (f, "  <interface_type>%d</interface_type>\n", j % itype);
			fprintf (f, "  </eth_interface>\n");
		}

		fprintf (f, "</eth_interface_root>\n");
		fclose (f);
	}

	return	0;
}

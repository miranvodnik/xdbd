
char*	stmt = "xdbd-test -p 1 \"insert into filter (nodeid, filter_id, filter_name) values (?, ?, 'FILTER')\" %d %d\n";

int	main (int n, char*p[])
{
	int	i;
	for (i = 0; i < 10000; ++i)
		printf (stmt, 2000, 200 + i);
}


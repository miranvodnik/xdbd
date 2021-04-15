
#include <sys/select.h>		// types declarations
#include <sys/ioctl.h>
#include <sys/types.h>		// types declarations
#include <sys/stat.h>		// file modes
#include <sys/mman.h>		// file mapping API
#include <sys/inotify.h>	// file system notifications
#include <dirent.h>		// directory entries
#include <regex.h>		// regular expressions
#include <grp.h>		// group info
#include <pwd.h>		// user info
#include <sqlxdbd.h>
#include <sqlext.h>
#include <unistd.h>		// standard library
#include <stddef.h>		// standard library
#include <stdlib.h>		// standard library
#include <string.h>		// standard library
#include <stdio.h>		// standard library
#include <errno.h>		// standard library

#include "XdbdErrApi.h"
#include <iostream>
using namespace std;

int main (int n, char* p[])
{
	static const char* query = "select nodeid, adapter_id, stat_dir_name, dest_dir_name, owner_name, group_name, dir_permissions, file_permissions from stat_adapter where nodeid=?";

	SQLHENV	env = SQL_NULL_HENV;
	SQLHDBC	dbc = SQL_NULL_HDBC;
	SQLHSTMT	stm = SQL_NULL_HSTMT;

	unsigned int nodeid = 0;
	unsigned short node_id;
	unsigned short adapter_id;
	char	stat_dir_name[256];
	char	dest_dir_name[256];
	char	owner_name[64];
	char	group_name[64];
	char	dir_permissions[64];
	char	file_permissions[64];

	bool done = false;

	{
		{
			const char*	cntrFname = p[1];

			if (cntrFname == 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "Cannot read statistical counters info, ENV variable XDBD_STAT_CNTR_INFO not found");
				return	-1;
			}

			FILE*	cntrFd = fopen (cntrFname, "r");

			if (cntrFd == 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "Cannot open statistical counters info file '%s', errno = %d", cntrFname, errno);
				return	-1;
			}

			int lineCount = 0;
			int	statGroup = -1;
			char	line[4100];
			if (fgets (line, 4096, cntrFd) != 0)
			while (fgets (line, 4096, cntrFd) != 0)
			{
				int	statCounter = -1;
				int	dataCounter = 0;
				int	fieldCounter = 0;
				char*	fields[6];

				++lineCount;
				fields[fieldCounter] = line;
				line[4096] = 0;
				for (char* readPtr = line; (*readPtr != 0) && (*readPtr != '\n'); ++readPtr)
				{
					if (*readPtr == '\t')
					{
						*readPtr = 0;
						if (*fields[fieldCounter] != 0)
							++dataCounter;
						if (++fieldCounter < 6)
							fields[fieldCounter] = readPtr + 1;
						else
						{
							xdbdErrReport (SC_XDBD, SC_ERR, "line %d: field counter > 6", lineCount);
							--fieldCounter;
						}
					}
				}
				if (dataCounter == 0)
				{
					statGroup = -1;
					continue;
				}
				if ((*fields[0] == 0) && (*fields[3] == 0))
					continue;
				if (*fields[0] != 0)
				{
					char*	ptr;
					if ((ptr = strstr (fields[0], " = ")) != 0)
						statGroup = atoi (ptr + 3);
					else if ((ptr = strstr (fields[0], " =")) != 0)
						statGroup = atoi (ptr + 2);
					else if ((ptr = strstr (fields[0], "= ")) != 0)
						statGroup = atoi (ptr + 2);
					else if ((ptr = strstr (fields[0], "=")) != 0)
						statGroup = atoi (ptr + 1);
					cout << "GROUP - " << statGroup << ": " << fields[1] << endl;
				}
				if (*fields[3] != 0)
				{
					char*	ptr;
					if ((ptr = strstr (fields[2], " = ")) != 0)
						statCounter = atoi (ptr + 3);
					else if ((ptr = strstr (fields[2], " =")) != 0)
						statCounter = atoi (ptr + 2);
					else if ((ptr = strstr (fields[2], "= ")) != 0)
						statCounter = atoi (ptr + 2);
					else if ((ptr = strstr (fields[2], "=")) != 0)
						statCounter = atoi (ptr + 1);
				}

				if ((statGroup < 0) || (statCounter < 0))
					continue;
				cout << "\tCOUNTER " << statCounter << ": " << fields[1] << ", " << fields[3] << ", " << fields[4] << ", " << fields[5] << endl;
			}

			fclose (cntrFd);
			return	0;

		}
	}
	while (!done)
	{
		const char* nodeidStr = getenv ("NODEID");
		if (nodeidStr == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot get NODEID");
			break;
		}
		nodeid = atoi (nodeidStr);

		if (SQLAllocHandle(SQL_HANDLE_ENV, 0, &env) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot allocate ENV");
			break;
		}

		if (SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot allocate DBC");
			break;
		}

		if (SQLConnect (dbc, (SQLCHAR*) "localhost", SQL_NTS, (SQLCHAR*) "user", SQL_NTS, (SQLCHAR*) "passwd", SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot connect to data base");
			break;
		}

		if (SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stm) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot allocate STMT");
			break;
		}

		if (SQLPrepare(stm, (SQLCHAR*) query, SQL_NTS) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot prepare '%s'", query);
			break;
		}

		if (SQLBindParameter(stm, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nodeid, 0, NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind parameter NODEID");
			break;
		}

		node_id = -1;
		if (SQLBindCol (stm, 1, SQL_C_SLONG, &node_id, sizeof(node_id), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column NODEID");
			break;
		}

		adapter_id = -1;
		if (SQLBindCol (stm, 2, SQL_C_SLONG, &adapter_id, sizeof(adapter_id), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column ADAPTER_ID");
			break;
		}

		memset ((void*)stat_dir_name, 0, sizeof (stat_dir_name));
		if (SQLBindCol (stm, 3, SQL_C_CHAR, (SQLPOINTER)stat_dir_name, sizeof(stat_dir_name), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column STAT_DIR_NAME");
			break;
		}

		memset ((void*)dest_dir_name, 0, sizeof (dest_dir_name));
		if (SQLBindCol (stm, 4, SQL_C_CHAR, (SQLPOINTER)dest_dir_name, sizeof(dest_dir_name), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column DEST_DIR_NAME");
			break;
		}

		memset ((void*)owner_name, 0, sizeof (owner_name));
		if (SQLBindCol (stm, 5, SQL_C_CHAR, (SQLPOINTER)owner_name, sizeof(owner_name), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column OWNER_NAME");
			break;
		}

		memset ((void*)group_name, 0, sizeof (group_name));
		if (SQLBindCol (stm, 6, SQL_C_CHAR, (SQLPOINTER)group_name, sizeof(group_name), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column GROUP_NAME");
			break;
		}

		memset ((void*)dir_permissions, 0, sizeof (dir_permissions));
		if (SQLBindCol (stm, 7, SQL_C_CHAR, (SQLPOINTER)dir_permissions, sizeof(dir_permissions), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column DIR_PERMISSIONS");
			break;
		}

		memset ((void*)file_permissions, 0, sizeof (file_permissions));
		if (SQLBindCol (stm, 8, SQL_C_CHAR, (SQLPOINTER)file_permissions, sizeof(file_permissions), NULL) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot bind column FILE_PERMISSIONS");
			break;
		}

		done = true;
		break;
	}

	while (done)
	{
		if (SQLExecute (stm) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot execute '%s'", query);
			break;
		}

		while (SQLFetch(stm) == SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter data OK, id = %d", adapter_id);
		}

		sleep (1);
	}

	if (stm != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, stm);
	if (dbc != SQL_NULL_HDBC)
		SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	if (env != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, env);

	return done ? 0 : -1;
}


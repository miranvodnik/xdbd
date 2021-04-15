
/****************************************************************/
/*      Header files                                            */
/****************************************************************/

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

#include "StatisticalAdapter.h"	// general adapter
#include "StatisticalAdapterThread.h"	// general adapter
#include "XdbdErrApi.h"

/****************************************************************/
/*      Private data                                            */
/****************************************************************/

StatisticalAdapterThread* StatisticalAdapterThread::g_StatisticalAdapterThread = new StatisticalAdapterThread ();

void StatisticalAdapterThread::_Init ()
{
	ReadDBSettings();
	ReadStatCountersInfo();
}

void StatisticalAdapterThread::_Release ()
{
	for (cntrset::iterator cit = m_cntrset.begin(); cit != m_cntrset.end(); ++cit)
		delete cit->second;
	for (salist::iterator sit = m_salist.begin(); sit != m_salist.end(); ++sit)
		delete sit->second;
}

int StatisticalAdapterThread::ReadDBSettings ()
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

		if (SQLExecute (stm) != SQL_SUCCESS)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter: Cannot execute '%s'", query);
			break;
		}

		while (SQLFetch(stm) == SQL_SUCCESS)
		{
			salist::iterator sit = m_salist.find (adapter_id);
			if (sit == m_salist.end())
			{
				m_salist[adapter_id] = new StatisticalAdapter
					(
						adapter_id,
						(const char*) stat_dir_name,
						(const char*) dest_dir_name,
						(const char*) owner_name,
						(const char*) group_name,
						(const char*) dir_permissions,
						(const char*) file_permissions
					);
			}
			else
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "Statistical adapter already exists, id = %d", adapter_id);
			}
		}
		done = true;
	}

	if (stm != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, stm);
	if (dbc != SQL_NULL_HDBC)
		SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	if (env != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, env);

	return done ? 0 : -1;
}

int	StatisticalAdapterThread::ReadStatCountersInfo ()
{
	char*	cntrFname = getenv ("XDBD_STAT_CNTR_INFO");

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

	int	statGroup = -1;
	char	line[4100];
	if (fgets (line, 4096, cntrFd) != 0)
	while (fgets (line, 4096, cntrFd) != 0)
	{
		int	statCounter = -1;
		int	dataCounter = 0;
		int	fieldCounter = 0;
		char*	fields[6];
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
					--fieldCounter;
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
			m_cntrset[cntrkey (statGroup, (u_int) -1)] = new StatisticalCounterInfo (statGroup, (u_int) -1, (char*) "", fields[1], (char*) "", (char*) "");
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
		if (m_cntrset.find (cntrkey (statGroup, statCounter)) != m_cntrset.end())
			continue;
		m_cntrset[cntrkey (statGroup, statCounter)] = new StatisticalCounterInfo (statGroup, statCounter, fields[1], fields[3], fields[4], fields[5]);
	}

	fclose (cntrFd);
	return	0;

}

const char* StatisticalAdapterThread::_FetchCounterName (cntrkey key)
{
	cntrset::iterator	it = m_cntrset.find(key);
	return	(it == m_cntrset.end()) ? 0 : it->second->statCounterName ();
}

const char* StatisticalAdapterThread::_FetchCounterDescription (cntrkey key)
{
	cntrset::iterator	it = m_cntrset.find(key);
	return	(it == m_cntrset.end()) ? 0 : it->second->description ();
}

const char* StatisticalAdapterThread::_FetchCounterIms (cntrkey key)
{
	cntrset::iterator	it = m_cntrset.find(key);
	return	(it == m_cntrset.end()) ? 0 : it->second->imsElement ();
}

const char* StatisticalAdapterThread::_FetchCounterComment (cntrkey key)
{
	cntrset::iterator	it = m_cntrset.find(key);
	return	(it == m_cntrset.end()) ? 0 : it->second->comment ();
}

int StatisticalAdapterThread::equal (SQLVARCHAR* newLine, SQLVARCHAR* oldLine, SQLVARCHAR* mask, size_t lineSize)
{
	for (; (lineSize > 0) && ((*mask == 0) || (((*newLine ^ *oldLine) & *mask) == 0));
		++newLine, ++oldLine, ++mask, --lineSize)
		;
	return lineSize;
}

void StatisticalAdapterThread::_InsertAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
{
	xdbdErrReport (SC_XDBD, SC_ERR, "insert statistical adapter %d", adapter_id);
	salist::iterator sit = m_salist.find(adapter_id);
	if (sit != m_salist.end())
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "cannot add statistical adapter: adapter with that id (%d) already exists", adapter_id);
		return;
	}
	m_salist[adapter_id] = new StatisticalAdapter
	(
		adapter_id,
		stat_dir_name,
		dest_dir_name,
		owner_name,
		group_name,
		dir_permissions,
		file_permissions
	);
}

void StatisticalAdapterThread::_UpdateAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
{
	xdbdErrReport (SC_XDBD, SC_ERR, "update statistical adapter %d", adapter_id);
	salist::iterator sit = m_salist.find(adapter_id);
	if (sit == m_salist.end())
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "cannot update statistical adapter: adapter with that id (%d) does not exists", adapter_id);
		return;
	}
	delete sit->second;
	m_salist.erase (sit);
	m_salist[adapter_id] = new StatisticalAdapter
	(
		adapter_id,
		stat_dir_name,
		dest_dir_name,
		owner_name,
		group_name,
		dir_permissions,
		file_permissions
	);
}

void StatisticalAdapterThread::_DeleteAdapter (SQLSMALLINT adapter_id)
{
	xdbdErrReport (SC_XDBD, SC_ERR, "delete statistical adapter %d", adapter_id);
	salist::iterator sit = m_salist.find(adapter_id);
	if (sit == m_salist.end())
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "cannot delete statistical adapter: adapter with that id (%d) does not exists", adapter_id);
		return;
	}
	delete sit->second;
	m_salist.erase (sit);
}

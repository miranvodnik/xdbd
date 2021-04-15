//============================================================================
// Name        : xdbd-test.cpp
// Author      : miran
// Version     :
// Copyright   : cr
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sys/types.h>
#include <unistd.h>
#include <sqlxdbd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
// #include "XdbdCommon.h"
using namespace std;

#include "XdbdRunningContext.h"
#include "XdbdXmlDataBase.h"
using namespace xml_db_daemon;

static bool test_db_load = false;

static	bool	dcmstr (const char* str)
{
	if ((*str == '-') || (*str == '+'))
		++str;
	for (; isdigit (*str) != 0; ++str);
	return	*str == 0;
}

int main(int n, char*p[])
{
	if (test_db_load)
	{
		XdbdRunningContext* ctx = new XdbdRunningContext (0, 0, 0, 0);
		XdbdXmlDataBase::LoadXmlDataBase (ctx);
	}

	int	stmIndex = 1;

	//
	//	Check input parameters
	//

	if (n < 2)
	{
		cerr << getpid() << ": " << "Usage: " << p[0] << " [-c <repetition-counter>] <SQL statement> [parameters]" << endl;
		return	0;
	}

	int	count = 1;
	if (strcmp (p[1], "-c") == 0)
	{
		count = atoi (p[2]);
		stmIndex = 3;

		if (n < stmIndex)
		{
			cerr << getpid() << ": " << "Usage: " << p[0] << " [-c <repetition-counter>] <SQL statement> [parameters]" << endl;
			return	0;
		}
	}

	if (count <= 0)
	{
		cerr << getpid() << ": " << "repetition counter must be greater than 0" << endl;
		cerr << getpid() << ": " << "Usage: " << p[0] << " [-c <repetition-counter>] <SQL statement> [parameters]" << endl;
		return	0;
	}

	const char*	stmStr = p[stmIndex++];

	//
	//	Allocate ENV, connection and statement handles
	//

	SQLHENV	env = SQL_NULL_HENV;
	SQLHDBC	dbc = SQL_NULL_HDBC;
	SQLHSTMT	stm = SQL_NULL_HSTMT;

	if (SQLAllocHandle(SQL_HANDLE_ENV, 0, &env) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot allocate ENV" << endl;
		return	0;
	}

	if (SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot allocate ENV" << endl;
		return	0;
	}

	if (SQLConnect (dbc, (SQLCHAR*) "localhost", SQL_NTS, (SQLCHAR*) "user", SQL_NTS, (SQLCHAR*) "passwd", SQL_NTS) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot allocate DBC" << endl;
		return	0;
	}

	if (SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stm) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot allocate STM" << endl;
		return	0;
	}

	//
	//	Prepare statement
	//

	if (SQLPrepare(stm, (SQLCHAR*) stmStr, SQL_NTS) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot prepare '" << stmStr << "'" << endl;
		return	0;
	}

	//
	//	Inspect implementation row descriptor and prepare to fetch column info
	//

	typedef	struct
	{
		SQLULEN	colOffset;
		SQLCHAR	colName[128];
		SQLSMALLINT	nameLen;
		SQLSMALLINT	dataType;
		SQLULEN	colSize;
		SQLSMALLINT	dcmDigits;
		SQLSMALLINT	nullable;
		SQLPOINTER	dataPointer;
	}	colstruct;

	SQLHDESC	impRowDesc;
	SQLSMALLINT	colcount = 0;
	colstruct*	colinfo = 0;
	if (SQLGetStmtAttr (stm, SQL_ATTR_IMP_ROW_DESC, (SQLPOINTER) &impRowDesc, 0, 0) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot allocate implementation row descriptor" << endl;
		return	0;
	}
	if (SQLGetDescField (impRowDesc, -1, SQL_DESC_COUNT, (SQLPOINTER) &colcount, 0, 0) != SQL_SUCCESS)
	{
		cerr << getpid() << ": " << "Cannot get column count" << endl;
		return	0;
	}
	if ((colinfo = (colstruct*) malloc (colcount * sizeof (colstruct))) == 0)
	{
		cerr << getpid() << ": " << "Cannot allocate column info, malloc() failed, colcount = " << colcount << ", errno = " << errno << endl;
		return	0;
	}

	//
	//	Fetch column info
	//

	int	dataSize = 0;
	for (int index = 1; index <= colcount; ++index)
	{
		if (SQLDescribeCol (stm, index,
			(colinfo + index - 1)->colName, 128,
			&(colinfo + index - 1)->nameLen,
			&(colinfo + index - 1)->dataType,
			&(colinfo + index - 1)->colSize,
			&(colinfo + index - 1)->dcmDigits,
			&(colinfo + index - 1)->nullable) != SQL_SUCCESS)
			break;

		dataSize += ((colinfo + index -1)->colOffset = (((colinfo + index -1)->colSize / sizeof (long)) + 1) * sizeof (long));
	}

	//
	//	Allocate space and bind columns to it. This space will hold fetched data of bound columns
	//

	SQLLEN*	nullInd = (SQLLEN*) malloc (colcount * sizeof (SQLLEN));
	if (nullInd == 0)
	{
		cerr << getpid() << ": " << "Cannot allocate NULL index table, colcount = " << colcount << ", errno = " << errno << endl;
		return	0;
	}
	char*	dataRow = (char*) malloc (dataSize);
	if (dataRow == 0)
	{
		cerr << getpid() << ": " << "Cannot allocate data field for returned data, colcount = " << colcount << ", errno = " << errno << endl;
		return	0;
	}

	char*	colData = dataRow;
	for (int index = 1; index <= colcount; ++index)
	{
		SQLBindCol (stm, (SQLUSMALLINT) index, (SQLSMALLINT) (colinfo + index - 1)->dataType, (colinfo + index - 1)->dataPointer = colData, (colinfo + index - 1)->colSize, nullInd + index - 1);
		colData += (colinfo + index - 1)->colOffset;
	}

	//
	//	Allocate space for statement parameters and bind these parameters to allocated space
	//

	char**	params = 0;
	if (n > stmIndex)
	{
		params = new char*[n - stmIndex];
		if (params == 0)
		{
			cerr << getpid() << ": " << "Cannot allocate space to bind input parameters, errno = " << errno << endl;
			return	0;
		}
		for (int i = stmIndex; i < n; ++i)
		{
			int	parIndex = i - stmIndex;
			if (strcasecmp (p[i], "null") == 0)
			{
				params[parIndex] = (char*) SQL_NULL_DATA;
				SQLBindParam(stm, parIndex + 1, SQL_UNKNOWN_TYPE, SQL_UNKNOWN_TYPE, 0, 0, (SQLPOINTER) (long) (parIndex), (SQLLEN*) SQL_NULL_DATA);
			}
			else	if (dcmstr (p[i]))
			{
				if ((params[parIndex] = (char*) new long) == 0)
				{
					cerr << getpid() << ": " << "Cannot allocate space to bind input parameter: " << p[i] << ", errno = " << errno << endl;
					return	0;
				}
				*((long*)params[parIndex]) = atol (p[i]);
				SQLBindParam(stm, parIndex + 1, SQL_INTEGER, SQL_INTEGER, 0, 0, (SQLPOINTER) (long) (parIndex), (SQLLEN*) SQL_DATA_AT_EXEC);
			}
			else
			{
				if ((params[parIndex] = new char [((strlen (p[i]) / sizeof (int)) + 1) * sizeof (int)]) == 0)
				{
					cerr << getpid() << ": " << "Cannot allocate space to bind input parameter: " << p[i] << ", errno = " << errno << endl;
					return	0;
				}
				strcpy (params[parIndex], p[i]);
				SQLBindParam(stm, parIndex + 1, SQL_VARCHAR, SQL_VARCHAR, 0, 0, (SQLPOINTER) (long) (parIndex), (SQLLEN*) SQL_DATA_AT_EXEC);
			}
		}
	}

	//
	//	Execute statement as many times as desired
	//

	for (int j = 0; j < count; ++j)
	{
		SQLRETURN	result;
		if ((result = SQLExecute (stm)) == SQL_NEED_DATA)
		//
		//	Load all bound parameters
		//
		while (result != SQL_SUCCESS)
		{
			SQLPOINTER	data = 0;
			if ((result = SQLParamData(stm, &data)) != SQL_NEED_DATA)
				break;
			if ((result = SQLPutData (stm, params [(int)(long)data], 0)) == SQL_ERROR)
				break;
		}
		if (result != SQL_SUCCESS)
		{
			cerr << getpid() << ": (" << j << ") " << "Cannot execute '" << stmStr << "'" << endl;
			return	0;
		}
		//
		//	Fetch rows and display results
		//
		while (true)
		{
			memset (nullInd, 0, colcount * sizeof (SQLLEN));
			if (SQLFetch (stm) == SQL_NO_DATA)
				break;
			for (int index = 1; index <= colcount; ++index)
			{
				if (nullInd[index - 1] == SQL_NULL_DATA)
				{
					cout << "NULL, ";
					continue;
				}
				SQLPOINTER	dataAddr = (colinfo + index - 1)->dataPointer;
				switch ((colinfo + index - 1)->dataType)
				{
				case SQL_SMALLINT:
					cout << *((SQLSMALLINT*) dataAddr) << ", ";
					break;
				case SQL_INTEGER:
					if (sizeof(int) != sizeof(long))
					{
						switch ((colinfo + index - 1)->colSize)
						{
						case sizeof(int):
							cout << *((SQLINTEGER*) dataAddr) << ", ";
							break;
						case sizeof(long):
							cout << *((SQLLEN*) dataAddr) << ", ";
							break;
						}
					}
					else
						cout << *((SQLINTEGER*) dataAddr) << ", ";
					break;
				case SQL_VARCHAR:
					((SQLVARCHAR*)dataAddr) [(colinfo + index - 1)->colSize] = 0;
					cout << "'" << ((SQLVARCHAR*)dataAddr) << "', ";
					break;
				}
			}
			cout << endl;
		}
	}

	//
	//	Release allocated resources
	//

	for (int i = stmIndex; i < n; ++i)
		if (params[i - stmIndex] != (char*) SQL_NULL_DATA)
			delete	params[i - stmIndex];
	delete [] params;

	free (dataRow);
	free (nullInd);
	free (colinfo);

	SQLFreeHandle(SQL_HANDLE_STMT, stm);
	SQLDisconnect(dbc);
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);

	return 0;
}

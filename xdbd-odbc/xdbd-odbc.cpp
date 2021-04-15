/*
 * xdbd-odbc.cpp
 *
 *  Created on: 13. dec. 2015
 *      Author: miran
 */

#define	ODBCVER	0x0300

#define	RC_INVOKED
#define	__xdbdimplementation
#include <sqlxdbd.h>
#include "XdbdOdbcEnvironment.h"
using namespace xml_db_daemon;

extern	"C"
{

SQLRETURN SQL_API	SQLPrepare
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	StatementText,
		SQLINTEGER	TextLength
);

SQLRETURN SQL_API	SQLExecute
(
		SQLHSTMT	StatementHandle
);

SQLRETURN SQL_API	SQLGetEnvAttr
(
		SQLHENV	EnvironmentHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength,
		SQLINTEGER*	StringLength
);

/*! @brief allocate a handle
 *
 *  @param HandleType A handle type identifier that describes the handle
 *  type of OutputHandle. It must contain one of the following values:
 *  - SQL_HANDLE_ENV - creates an environment and returns a handle to it
 *  in OutputHandle. InputHandle should be SQL_NULL_HANDLE since an
 *  environment handle does not exist in the context of any other handle.
 *  - SQL_HANDLE_DBC - allocates a connection within the environment
 *  specified by the environment handle InputHandle and returns a handle
 *  to the connection in OutputHandle.
 *  - SQL_HANDLE_STMT - allocates a statement within the connection
 *  specified by the connection handle InputHandle and returns a handle
 *  to the statement in OutputHandle.
 *  - SQL_HANDLE_DESC - allocates a descriptor, associates it with the
 *  connection handle InputHandle and returns a handle to the descriptor
 *  in OutputHandle.
 *  @param InputHandle The handle that describes the data structure in
 *  whose context the new data structure is to be allocated; except that,
 *  if HandleType is SQL_HANDLE_ENV, this is SQL_NULL_HANDLE.
 *  @param OutputHandle The handle for the newly allocated data structure.
 *
 *  @return SQL_SUCCESS handle created
 *  @return SQL_ERROR invalid HandleType or null reference to OutputHandle
 *  @return SQL_INVALID_HANDLE InputHandle refers to null handle when it
 *  should not
 *
 */
SQLRETURN SQL_API	SQLAllocHandle
(
		SQLSMALLINT	HandleType,
		SQLHANDLE	InputHandle,
		SQLHANDLE*	OutputHandle
)
{
	if (OutputHandle == 0)
		return	SQL_ERROR;
	switch (HandleType)
	{
	case	SQL_HANDLE_ENV:
		*OutputHandle = (SQLHANDLE*) new XdbdOdbcEnvironment (0);
		break;
	case	SQL_HANDLE_DBC:
		if (InputHandle == 0)
			return	SQL_INVALID_HANDLE;
		*OutputHandle = (SQLHANDLE*) ((XdbdOdbcEnvironment*)InputHandle)->createConnection();
		break;
	case	SQL_HANDLE_STMT:
		if (InputHandle == 0)
			return	SQL_INVALID_HANDLE;
		*OutputHandle = (SQLHANDLE*) ((XdbdOdbcConnection*)InputHandle)->createStatement();
		break;
	case	SQL_HANDLE_DESC:
		if (InputHandle == 0)
			return	SQL_INVALID_HANDLE;
		*OutputHandle = (SQLHANDLE*) ((XdbdOdbcConnection*)InputHandle)->createDescriptor();
		break;
	default:
		return	SQL_ERROR;
	}
	if (*OutputHandle == 0)
		return	SQL_INVALID_HANDLE;
	return	SQL_SUCCESS;
}

/*! @brief Bind a column in a result set
 *
 *  The BindCol( ) function binds a column of a result set to an application
 *  variable, enabling implicit data transfer and optional data conversion
 *  when a row of the result set is fetched
 *
 *  @param StatementHandle Statement handle.
 *  @param ColumnNumber Column number. Columns are numbered sequentially
 *  from left to right and the leftmost is number 1.
 *  @param TargetType The application data type of TargetValue. Specifying
 *  SQL_DEFAULT provides that a buffer type from Table 4-9 on page 61 will
 *  be selected at fetch time based on the SQL data type of the column being
 *  bound. Specifying SQL_CHAR, if the database value is a date/time or numeric
 *  value, results in string data conversion
 *  @param TargetValue A variable that the implementation loads with column
 *  data when the fetch occurs.
 *  @param BufferLength If TargetType specifies a character-string data type
 *  (or is SQL_DEFAULT and this implies a character-string data type), then
 *  this is the maximum number of octets to store in TargetValue. Otherwise,
 *  it is ignored.
 *  @param StrLen_or_Ind A variable the implementation sets, when the fetch
 *  occurs, to one of the following:
 *  - the value SQL_NULL_DATA if the data value for the column is null
 *  - the length in octets of the data value for the column (even if it does
 *  not entirely fit in TargetValue), excluding any null terminator, if
 *  TargetType is SQL_CHAR (or TargetType is SQL_DEFAULT and the database
 *  column is of type CHAR or VARCHAR)
 *  - an undefined value (but not SQL_NULL_DATA) in all other cases.
 *
 *  @return SQL_SUCCESS column bound
 *  @return SQL_INVALID_HANDLE null StatementHandle
 *  @return SQL_ERROR column cannot be bound: ColumnNumber out of range
 *
 */
SQLRETURN SQL_API	SQLBindCol
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ColumnNumber,
		SQLSMALLINT	TargetType,
		SQLPOINTER	TargetValue,
		SQLLEN	BufferLength,
		SQLLEN*	StrLen_or_Ind
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;

	if (stmt->BindCol(ColumnNumber, TargetType, TargetValue, BufferLength, StrLen_or_Ind) != 0)
		return	SQL_ERROR;

	XdbdOdbcDescriptor*	appDesc = (XdbdOdbcDescriptor*) stmt->attrAppRowDesc();
	if (appDesc->descCount() < ColumnNumber)
		appDesc->descCount(ColumnNumber);

	appDesc->descType (ColumnNumber, TargetType);
	appDesc->descDataPtr (ColumnNumber, TargetValue);
	appDesc->descOctetLength(ColumnNumber, BufferLength);
	appDesc->descOctetLengthPtr(ColumnNumber, StrLen_or_Ind);

	return	SQL_SUCCESS;
}

/*! @brief Bind a dynamic parameter
 *
 *  binds a parameter in an SQL statement to an application variable,
 *  enabling implicit data transfer and optional data conversion when
 *  the statement is executed.
 *
 *  @param StatementHandle Statement handle.
 *  @param ParameterNumber Parameter number. Parameters are numbered
 *  sequentially from left to right and the leftmost is number 1.
 *  @param ValueType The application data type of ParameterValue.
 *  @param ParameterType The SQL data type of the parameter. For the
 *  date/time data types, the application can specify SQL_TYPE_DATE,
 *  SQL_TYPE_TIME, or SQL_TYPE_TIMESTAMP.
 *  @param LengthPrecision Maximum size of the parameter. Its
 *  interpretation depends on the value of ParameterType
 *  @param ParameterScale If ParameterType is SQL_DECIMAL or SQL_NUMERIC,
 *  this is the scale of the parameter (the total number of digits to
 *  the right of the decimal point). Otherwise, ParameterScale is ignored.
 *  @param ParameterValue A variable whose value is used as the parameter
 *  data when StatementHandle is executed.
 *  @param StrLen_or_Ind A variable whose value is interpreted when
 *  StatementHandle is executed.
 *
 *  @return SQL_SUCCESS parameter bound
 *  @return SQL_INVALID_HANDLE null StatementHandle value
 *
 */
SQLRETURN SQL_API	SQLBindParam
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ParameterNumber,
		SQLSMALLINT	ValueType,
		SQLSMALLINT	ParameterType,
		SQLULEN	LengthPrecision,
		SQLSMALLINT	ParameterScale,
		SQLPOINTER	ParameterValue,
		SQLLEN*	StrLen_or_Ind
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;

	stmt->BindParam(ParameterNumber, ValueType, ParameterType, LengthPrecision, ParameterScale, ParameterValue, (SQLLEN) StrLen_or_Ind);

	XdbdOdbcDescriptor*	appDesc = (XdbdOdbcDescriptor*) stmt->attrAppParamDesc();
	XdbdOdbcDescriptor*	impDesc = (XdbdOdbcDescriptor*) stmt->attrImpParamDesc();
	if (appDesc->descCount() < ParameterNumber)
		appDesc->descCount(ParameterNumber);
	if (impDesc->descCount() < ParameterNumber)
		impDesc->descCount(ParameterNumber);

	appDesc->descType(ParameterNumber, ValueType);
	appDesc->descDataPtr(ParameterNumber, ParameterValue);
	appDesc->descOctetLengthPtr(ParameterNumber, StrLen_or_Ind);
	appDesc->descIndicatorPtr(ParameterNumber, StrLen_or_Ind);

	switch (ParameterType)
	{
	case	SQL_TYPE_DATE:
		impDesc->descType(ParameterNumber, SQL_DATETIME);
		impDesc->descDatetimeIntervalCode(ParameterNumber, SQL_CODE_DATE);
		impDesc->descLength(ParameterNumber, 8);
		impDesc->descPrecision(ParameterNumber, 0);
		break;
	case	SQL_TYPE_TIME:
		impDesc->descType(ParameterNumber, SQL_DATETIME);
		impDesc->descDatetimeIntervalCode(ParameterNumber, SQL_CODE_TIME);
		impDesc->descLength(ParameterNumber, 8);
		impDesc->descPrecision(ParameterNumber, 0);
		break;
	case	SQL_TYPE_TIMESTAMP:
		impDesc->descType(ParameterNumber, SQL_DATETIME);
		impDesc->descDatetimeIntervalCode(ParameterNumber, SQL_CODE_TIMESTAMP);
		if (LengthPrecision > 0)
			impDesc->descLength(ParameterNumber, 20 + LengthPrecision);
		else
			impDesc->descLength(ParameterNumber, 19);
		impDesc->descPrecision(ParameterNumber, LengthPrecision);
		break;
	default:
		impDesc->descType(ParameterNumber, ParameterType);
		impDesc->descLength(ParameterNumber, LengthPrecision);
		impDesc->descPrecision(ParameterNumber, LengthPrecision);
		impDesc->descScale(ParameterNumber, ParameterScale);
		break;
	}

	return	SQL_SUCCESS;
}

/*! @brief Attempt to cancel SQL statement execution
 *
 *  this function is not implemented.
 *
 *  @param The statement handle representing the work to be canceled.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLCancel
(
		SQLHSTMT	StatementHandle
)
{
	return	SQL_SUCCESS;
}

/*! @brief close a cursor
 *
 *  @param StatementHandle statement handle
 *
 *  @return SQL_SUCCESS cursor closed
 *  @return SQL_INVALID_HANDLE null statement handle
 *
 */
SQLRETURN SQL_API	SQLCloseCursor
(
		SQLHSTMT	StatementHandle
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	return	(stmt->closeCursor () < 0) ? SQL_ERROR : SQL_SUCCESS;
}

/*! @brief Get an implementation row descriptor field
 *
 *  function is not implemented
 *
 *  @param ConnectionHandle Statement handle.
 *  @param ColumnNumber The record number from which the specified field in
 *  the descriptor is to be retrieved. The first record is number 1.
 *  @param FieldIdentifier The identifier of the field to be retrieved
 *  @param CharacterAttribute The value of FieldIdentifier of ColumnNumber,
 *  if the field is a character string; otherwise, unused.
 *  @param BufferLength The maximum number of octets to store in
 *  CharacterAttribute, if the field is a character string; otherwise, ignored.
 *  @param StringLength Length in octets of the output data (even if it does
 *  not entirely fit in CharacterAttribute), if the field is a character string;
 *  otherwise, unused.
 *  @param NumericAttribute The value of FieldIdentifier of ColumnNumber, if
 *  the field is an integer; otherwise unused
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLColAttribute
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ColumnNumber,
		SQLUSMALLINT	FieldIdentifier,
		SQLPOINTER	CharacterAttribute,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	StringLength,
		SQLLEN*	NumericAttribute
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get column information
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param CatalogName Buffer containing catalog name to qualify the tables
 *  of the result set
 *  @param NameLength1 Length in octets of CatalogName
 *  @param SchemaName Buffer that may qualify the result set by schema name
 *  @param NameLength2 Length in octets of SchemaName.
 *  @param TableName Buffer that may qualify the result set by table name
 *  @param NameLength3 Length in octets of TableName.
 *  @param ColumnName Buffer that may qualify the result set by column name
 *  @param NameLength4 Length in octets of ColumnName
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLColumns
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	CatalogName,
		SQLSMALLINT	NameLength1,
		SQLCHAR*	SchemaName,
		SQLSMALLINT	NameLength2,
		SQLCHAR*	TableName,
		SQLSMALLINT	NameLength3,
		SQLCHAR*	ColumnName,
		SQLSMALLINT	NameLength4
)
{
	return	SQL_SUCCESS;
}

/*! @brief Open a connection to a server
 *
 *  function tries to connect to DB server using low
 *  level function Connect()
 *
 *  @param ConnectionHandle Connection handle.
 *  @param ServerName Buffer containing server name
 *  @param NameLength1 Length in octets of ServerName.
 *  @param UserName Buffer containing user identifier
 *  @param NameLength2 Length in octets of UserName
 *  @param Authentication Buffer containing authentication string
 *  @param NameLength3 Length in octets of Authentication
 *
 *  @return SQL_SUCCESS connected to DB server
 *  @return SQL_ERROR not connected to DB server
 *
 */
SQLRETURN SQL_API	SQLConnect
(
		SQLHDBC	ConnectionHandle,
		SQLCHAR*	ServerName,
		SQLSMALLINT	NameLength1,
		SQLCHAR*	UserName,
		SQLSMALLINT	NameLength2,
		SQLCHAR*	Authentication,
		SQLSMALLINT	NameLength3
)
{
	XdbdOdbcConnection*	conn = (XdbdOdbcConnection*) ConnectionHandle;
	return	conn->Connect (ServerName, NameLength1, UserName, NameLength2, Authentication, NameLength3);
}

/*! @brief copy a descriptor
 *
 *  copies the fields of the data structure associated with SourceDescHandle to
 *  the data structure associated with TargetDescHandle
 *
 *  @param SourceDescHandle Source descriptor handle.
 *  @param TargetDescHandle Target descriptor handle
 *
 *  @return SQL_SUCCESS descriptor copied
 *  @return SQL_ERROR TargetDescHandle refers implementation descriptor
 *
 */
SQLRETURN SQL_API	SQLCopyDesc
(
		SQLHDESC	SourceDescHandle,
		SQLHDESC	TargetDescHandle
)
{
	XdbdOdbcDescriptor*	src = (XdbdOdbcDescriptor*) SourceDescHandle;
	XdbdOdbcDescriptor*	dst = (XdbdOdbcDescriptor*) TargetDescHandle;
	XdbdOdbcStatement*	parent = (XdbdOdbcStatement*) dst->parent();
	if ((parent != 0) && (parent->attrImpRowDesc() == dst))
		return	SQL_ERROR;
	dst->copy(src);
	return	SQL_SUCCESS;
}

/*! @brief Get a list of server names
 *
 *  function is not implemented
 *
 *  @brief EnvironmentHandle Environment handle
 *  @brief Direction Used by the application to control the manner in
 *  which it gains access to the list of servers.
 *  @param ServerName The server name.
 *  @param BufferLength1 Maximum number of octets to store in ServerName.
 *  @param NameLength1 Length in octets of the output server name (even
 *  if it does not entirely fit in ServerName)
 *  @param Description The description associated with ServerName. If no
 *  information is available for the current server, then this is a
 *  zero-length string
 *  @param BufferLength2 Maximum number of octets to store in Description.
 *  @param NameLength2 Length in octets of the output description (even
 *  if it does not entirely fit in Description).
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLDataSources
(
		SQLHENV	EnvironmentHandle,
		SQLUSMALLINT	Direction,
		SQLCHAR*	ServerName,
		SQLSMALLINT	BufferLength1,
		SQLSMALLINT*	NameLength1,
		SQLCHAR*	Description,
		SQLSMALLINT	BufferLength2,
		SQLSMALLINT*	NameLength2
)
{
	return	SQL_SUCCESS;
}

/*! @brief Describe a column of a result set
 *
 *  @param StatementHandle Statement handle
 *  @param ColumnNumber Column number. Columns are numbered sequentially
 *  from left to right and the leftmost is number 1.
 *  @param ColumnName Column name.
 *  @param BufferLength Length in octets of ColumnName buffer.
 *  @param NameLength Actual number of octets in ColumnName.
 *  @param DataType The SQL data type of the column
 *  @param ColumnSize Length or precision for column’s data type in the
 *  database
 *  @param DecimalDigits For SQL_DECIMAL and SQL_NUMERIC, DecimalDigits
 *  is the total number of digits to the right of the decimal point. For
 *  SQL_INTEGER and SQL_SMALLINT, DecimalDigits is 0. Otherwise
 *  DecimalDigits is undefined.
 *  @param Nullable SQL_NULLABLE for nullable columns otherwise SQL_NO_NULLS
 *
 *  @return SQL_SUCCESS column described
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return SQL_ERROR column number out of range
 *
 */
SQLRETURN SQL_API	SQLDescribeCol
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ColumnNumber,
		SQLCHAR*	ColumnName,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	NameLength,
		SQLSMALLINT*	DataType,
		SQLULEN*	ColumnSize,
		SQLSMALLINT*	DecimalDigits,
		SQLSMALLINT*	Nullable
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;

	XdbdOdbcDescriptor*	impDesc = (XdbdOdbcDescriptor*) stmt->attrImpRowDesc();
	if (impDesc->descCount() <= ColumnNumber)
		return	SQL_ERROR;

	SQLCHAR*	colname = impDesc->descName (ColumnNumber);
	if (colname != 0)
	{
		if (BufferLength < 128)
			strncpy ((char*) ColumnName, (char*) colname, BufferLength);
		else
			strncpy ((char*) ColumnName, (char*) colname, 128);
	}
	if (NameLength != 0)
		*NameLength = strnlen ((char*) colname, 128);
	if (DataType != 0)
		*DataType = impDesc->descType (ColumnNumber);
	if (ColumnSize != 0)
		*ColumnSize = impDesc->descOctetLength (ColumnNumber);
	if (DecimalDigits != 0)
		*DecimalDigits = impDesc->descScale (ColumnNumber);
	if (Nullable != 0)
		*Nullable = impDesc->descNullable (ColumnNumber);
	return	SQL_SUCCESS;
}

/*! @brief Close a connection to a server
 *
 *  The function closes the connection associated with ConnectionHandle
 *
 *  @param ConnectionHandle Connection handle
 *
 *  @return SQL_SUCCESS connection closed
 *  @return SQL_ERROR not connected
 *
 */
SQLRETURN SQL_API	SQLDisconnect
(
		SQLHDBC	ConnectionHandle
)
{
	XdbdOdbcConnection*	conn = (XdbdOdbcConnection*) ConnectionHandle;
	return	conn->Disconnect();
}

/*! @brief Commit or roll back a transaction
 *
 *  @brief HandleType A handle type identifier. It must contain SQL_HANDLE_ENV
 *  if Handle is an environment handle, or SQL_HANDLE_DBC if Handle is a
 *  connection handle.
 *  @param Handle The handle indicating the scope of work to be completed.
 *  @param CompletionType The desired completion outcome (SQL_COMMIT or
 *  SQL_ROLLBACK).
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLEndTran
(
		SQLSMALLINT	HandleType,
		SQLHANDLE	Handle,
		SQLSMALLINT	CompletionType
)
{
	return	SQL_SUCCESS;
}

/*! @brief Return error information associated with a handle
 *
 *  function is not implemented
 *
 *  @param EnvironmentHandle Environment handle.
 *  @param ConnectionHandle Connection handle.
 *  @param StatementHandle Statement handle.
 *  @param Sqlstate A string of 5 characters. The first 2
 *  indicate error class; the next 3 indicate subclass
 *  @param NativeError An implementation-defined error code
 *  @param MessageText Implementation-defined message text.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLError
(
		SQLHENV	EnvironmentHandle,
		SQLHDBC	ConnectionHandle,
		SQLHSTMT	StatementHandle,
		SQLCHAR*	Sqlstate,
		SQLINTEGER*	NativeError,
		SQLCHAR*	MessageText,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	TextLength
)
{
	*Sqlstate = 0;
	*NativeError = 0;
	*MessageText = 0;
	*TextLength = 0;
	return	SQL_SUCCESS;
}

/*! @brief Execute an SQL statement directly
 *
 *  The ExecDirect( ) function effects a one-time execution of an
 *  SQL statement
 *
 *  @param StatementHandle Statement handle
 *  @param StatementText SQL text string, using question-mark (?)
 *  characters as dynamic parameter markers.
 *  @param TextLength Length in octets of StatementText
 *
 *  @return status success of SQLPrepare() or SQLExecute()
 *
 */
SQLRETURN SQL_API	SQLExecDirect
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	StatementText,
		SQLINTEGER	TextLength
)
{
	SQLRETURN	status = SQLPrepare (StatementHandle, StatementText, TextLength);
	if (status != SQL_SUCCESS)
		return	status;
	return	SQLExecute (StatementHandle);
}

/*! @brief Execute a prepared SQL statement
 *
 *  function checks statement handle against null value. Non null handle
 *  is then checked if it is prepared. Prepared statement is executed. If
 *  it executed successfully, fetching is initialized
 *
 *  @param StatementHandle Statement handle
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return SQL_ERROR statement not prepared
 *  @return status success of low level functions Execute() or InitFetch()
 *
 */
SQLRETURN SQL_API	SQLExecute
(
		SQLHSTMT	StatementHandle
)
{
	SQLRETURN	status;
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	if (stmt->currentRecordSet() == 0)
		return	SQL_ERROR;
	if ((status = stmt->Execute()) != SQL_SUCCESS)
		return	status;
	return	stmt->InitFetch();
}

/*! @brief Fetch the next row of a result set
 *
 *  @param StatementHandle Statement handle
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return SQL_NO_DATA no more data in result set
 *  @return SQL_SUCCESS next row fetched
 *
 */
SQLRETURN SQL_API	SQLFetch
(
		SQLHSTMT	StatementHandle
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	return	stmt->FetchNextRow();
}

/*! @brief Move the cursor to, and fetch, a specified row of a result set
 *
 *  function retrieves row of a result set in a specified direction
 *  using low level function FetchRow() if it is possible to do it.
 *
 *  @param StatementHandle Statement handle.
 *  @param FetchOrientation The cursor positioning operation to be
 *  performed before fetching a row of the result set.
 *  @param FetchOffset When FetchOrientation is SQL_FETCH_ABSOLUTE,
 *  this is an absolute row number of the result set, as described
 *  above. When FetchOrientation is SQL_FETCH_RELATIVE, this is an
 *  offset to the current cursor position, as described above.
 *  Otherwise, FetchOffset is ignored
 *
 *  @return SQL_INVALID_HANDLE in case of null statement handle
 *  @return SQL_NO_DATA no data in specified direction
 *  @return SQL_SUCCESS fetched succeeded
 *
 */
SQLRETURN SQL_API	SQLFetchScroll
(
		SQLHSTMT	StatementHandle,
		SQLSMALLINT	FetchOrientation,
		SQLLEN	FetchOffset
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	return	stmt->FetchRow (FetchOrientation, FetchOffset);
}

/*! @brief free a handle
 *
 *  function deletes all objects associated with given handle
 *
 *  @param HandleType A handle type identifier that describes the
 *  handle type of Handle
 *  @param Handle The handle to be freed.
 *
 *  @return SQL_INVALID_HANDLE null handle
 *  @return SQL_ERROR unknown HandleType
 *  @return SQL_SUCCESS handle released
 *
 */
SQLRETURN SQL_API	SQLFreeHandle
(
		SQLSMALLINT	HandleType,
		SQLHANDLE	Handle
)
{
	if (Handle == 0)
		return	SQL_INVALID_HANDLE;
	switch (HandleType)
	{
	case	SQL_HANDLE_ENV:
		delete (XdbdOdbcEnvironment*) Handle;
		break;
	case	SQL_HANDLE_DBC:
		((XdbdOdbcEnvironment*) ((XdbdOdbcHandle*) Handle)->parent())->deleteConnection ((XdbdOdbcConnection*) Handle);
		break;
	case	SQL_HANDLE_STMT:
		((XdbdOdbcConnection*) ((XdbdOdbcHandle*) Handle)->parent())->deleteStatement ((XdbdOdbcStatement*) Handle);
		break;
	case	SQL_HANDLE_DESC:
		((XdbdOdbcConnection*) ((XdbdOdbcHandle*) Handle)->parent())->deleteDescriptor ((XdbdOdbcDescriptor*) Handle);
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Get the value of a connection attribute
 *
 *  @param ConnectionHandle Connection handle.
 *  @param Attribute The desired connection attribute
 *  @param Value The current value of the attribute specified by
 *  Attribute. The type of the value returned depends on Attribute.
 *  @param BufferLength Maximum number of octets to store in Value,
 *  if the attribute value is a character string; otherwise, unused.
 *  @param StringLength Length in octets of the output data (even
 *  if it does not entirely fit in Value), if the attribute value
 *  is a character string; otherwise, unused.
 *
 *  @return SQL_INVALID_HANDLE null connection handle
 *  @return SQL_ERROR unknown attribute
 *  @return SQL_SUCCESS attribute value acquired
 *
 */
SQLRETURN SQL_API	SQLGetConnectAttr
(
		SQLHDBC	ConnectionHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength,
		SQLINTEGER*	StringLength
)
{
	if (ConnectionHandle == 0)
		return	SQL_INVALID_HANDLE;

	switch (Attribute)
	{
	case	SQL_ATTR_AUTO_IPD:
		*((SQLINTEGER*)Value) = ((XdbdOdbcConnection*)ConnectionHandle)->attrAutoIpd ();
		break;
	case	SQL_ATTR_METADATA_ID:
		*((SQLINTEGER*)Value) = ((XdbdOdbcConnection*)ConnectionHandle)->attrMetadataId ();
		break;
	default:
		return	SQL_ERROR;
	}

	if (StringLength != 0)
		*StringLength = 0;

	return	SQL_SUCCESS;
}

/*! @brief Get the name of a cursor
 *
 *  returns a cursor name associated with a statement handle
 *
 *  @param StatementHandle Statement handle.
 *  @param CursorName Cursor name.
 *  @param BufferLength Maximum number of octets to store in CursorName.
 *  @param NameLength Length in octets of the output data (even if it
 *  does not entirely fit in CursorName).
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetCursorName
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	CursorName,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	NameLength
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	stmt->getCursorName(CursorName, BufferLength);
	if (NameLength != 0)
		*NameLength = stmt->getCursorLength();
	return	SQL_SUCCESS;
}

/*! @brief Retrieve one column of a row of the result set
 *
 *  After fetching a new row of a result set, the application
 *  can call GetData( ) to obtain result data for part or all
 *  of a single unbound column. Function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param ColumnNumber Column number. The first column is number 1.
 *  @param TargetType The data type of TargetValue in the application
 *  row buffer
 *  @param TargetValue Destination for the output data.
 *  @param BufferLength Maximum number of octets to store in TargetValue,
 *  if the application row descriptor specifies a character-string data
 *  type; otherwise, ignored.
 *  @param StrLen_or_Ind A variable whose value is interpreted as follows:
 *  - If the output data is null, this is set to SQL_NULL_DATA.
 *  - Otherwise, if the data type of the application buffer is SQL_CHAR,
 *  this is set to the length in octets of the output data (even if it
 *  does not entirely fit in TargetValue). This value excludes any data
 *  from this column that the application has already obtained from
 *  previous calls to GetData( ).
 *  - Otherwise, the value is undefined.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetData
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ColumnNumber,
		SQLSMALLINT	TargetType,
		SQLPOINTER	TargetValue,
		SQLLEN	BufferLength,
		SQLLEN*	StrLen_or_Ind
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get a descriptor field
 *
 *  @param DescriptorHandle Descriptor handle.
 *  @param RecNumber The record number from which the specified
 *  field in the descriptor is to be retrieved. The first record
 *  is number 1.
 *  @param FieldIdentifier The identifier of the field to be retrieved
 *  @param Value The value for FieldIdentifier of RecNumber.
 *  @param BufferLength Maximum number of octets to store in Value,
 *  if the field is a character string; otherwise, unused.
 *  @param StringLength Length in octets of the output data (even
 *  if it does not entirely fit in Value), if the field is a
 *  character string; otherwise, unused
 *
 *  @return SQL_ERROR unknown FieldIdentifier
 *  @return SQL_SUCCESS field retrieved
 *
 */
SQLRETURN SQL_API	SQLGetDescField
(
		SQLHDESC	DescriptorHandle,
		SQLSMALLINT	RecNumber,
		SQLSMALLINT	FieldIdentifier,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength,
		SQLINTEGER*	StringLength
)
{
	XdbdOdbcDescriptor*	desc = (XdbdOdbcDescriptor*) DescriptorHandle;

	switch (FieldIdentifier)
	{
	case	SQL_DESC_COUNT:
		*((SQLSMALLINT*)Value) = desc->descCount() - 1;
		break;
	case	SQL_DESC_TYPE:
		*((SQLSMALLINT*)Value) = desc->descType(RecNumber);
		break;
	case	SQL_DESC_LENGTH:
		*((SQLINTEGER*)Value) = desc->descLength(RecNumber);
		break;
	case	SQL_DESC_OCTET_LENGTH_PTR:
		*((SQLPOINTER*)Value) = desc->descOctetLengthPtr(RecNumber);
		break;
	case	SQL_DESC_PRECISION:
		*((SQLSMALLINT*)Value) = desc->descPrecision(RecNumber);
		break;
	case	SQL_DESC_SCALE:
		*((SQLSMALLINT*)Value) = desc->descScale(RecNumber);
		break;
	case	SQL_DESC_DATETIME_INTERVAL_CODE:
		*((SQLSMALLINT*)Value) = desc->descDatetimeIntervalCode(RecNumber);
		break;
	case	SQL_DESC_NULLABLE:
		*((SQLSMALLINT*)Value) = desc->descNullable(RecNumber);
		break;
	case	SQL_DESC_INDICATOR_PTR:
		*((SQLPOINTER*)Value) = desc->descIndicatorPtr(RecNumber);
		break;
	case	SQL_DESC_DATA_PTR:
		*((SQLPOINTER*)Value) = desc->descDataPtr(RecNumber);
		break;
	case	SQL_DESC_NAME:
		*((SQLCHAR**)Value) = desc->descName(RecNumber);
		break;
	case	SQL_DESC_UNNAMED:
		*((SQLSMALLINT*)Value) = desc->descType(RecNumber);
		break;
	case	SQL_DESC_OCTET_LENGTH:
		*((SQLINTEGER*)Value) = desc->descOctetLength(RecNumber);
		break;
	case	SQL_DESC_ALLOC_TYPE:
		*((SQLSMALLINT*)Value) = desc->descAllocType();
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Get a descriptor record
 *
 *  @param DescriptorHandle Descriptor handle.
 *  @param RecNumber The record number from which the description
 *  in the descriptor is to be retrieved. The first record is number 1.
 *  @param Name The NAME field for the record.
 *  @param BufferLength Maximum number of octets to store in Name.
 *  @param NameLength Length in octets of the output data (even if
 *  it does not entirely fit in Name).
 *  @param Type The TYPE field for the record
 *  @param SubType The DATETIME_INTERVAL_CODE field, for records
 *  whose TYPE is SQL_DATETIME.
 *  @param Length The OCTET_LENGTH field for the record.
 *  @param Precision The PRECISION field for the record.
 *  @param Scale The SCALE field for the record.
 *  @param Nullable The NULLABLE field for the record.
 *
 *  @return SQL_INVALID_HANDLE null description handle
 *  @return SQL_NO_DATA record number out of range
 *  @return SQL_SUCCESS record retrieved
 *
 */
SQLRETURN SQL_API	SQLGetDescRec
(
		SQLHDESC	DescriptorHandle,
		SQLSMALLINT	RecNumber,
		SQLCHAR*	Name,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	StringLength,
		SQLSMALLINT*	Type,
		SQLSMALLINT*	SubType,
		SQLLEN*	Length,
		SQLSMALLINT*	Precision,
		SQLSMALLINT*	Scale,
		SQLSMALLINT*	Nullable
)
{
	XdbdOdbcDescriptor*	desc = (XdbdOdbcDescriptor*) DescriptorHandle;
	if (desc == 0)
		return	SQL_INVALID_HANDLE;
	if ((RecNumber < 1) || (RecNumber >= desc->descCount()))
		return	SQL_NO_DATA;

	XdbdOdbcEnvironment*	env = (XdbdOdbcEnvironment*) desc->parent()->parent();
	SQLINTEGER	attrOutputNts;
	SQLGetEnvAttr(env, SQL_ATTR_OUTPUT_NTS, &attrOutputNts, 0, 0);

	if (Name != 0)
	{
		SQLCHAR*	name = desc->descName(RecNumber);
		if (StringLength != 0)
		{
			if (attrOutputNts == SQL_TRUE)
				*StringLength = SQL_NTS;
			else
				*StringLength = strlen ((char*) name);
		}
		strncpy ((char*) Name, (char*) name, BufferLength);
		Name [BufferLength - 1] = 0;
	}
	if (Type != 0)
		*((SQLSMALLINT*)Type) = desc->descType(RecNumber);
	if ((SubType != 0) && (desc->descType(RecNumber) == SQL_DATETIME))
		*((SQLSMALLINT*)SubType) = desc->descDatetimeIntervalCode(RecNumber);
	if (Length != 0)
		*((SQLLEN*)Length) = desc->descOctetLength(RecNumber);
	if (Precision != 0)
		*((SQLSMALLINT*)Precision) = desc->descPrecision(RecNumber);
	if (Scale != 0)
		*((SQLSMALLINT*)Scale) = desc->descScale(RecNumber);
	if (Nullable != 0)
		*((SQLSMALLINT*)Nullable) = desc->descNullable(RecNumber);
	return	SQL_SUCCESS;
}

/*! @brief Return diagnostic information
 *
 *  function is not implemented
 *
 *  @param HandleType A handle type identifier that describes the handle
 *  type of Handle
 *  @param Handle A handle for which diagnostic information is desired.
 *  @param RecNumber Indicates the status record from which the application
 *  seeks information.
 *  @param DiagIdentifier Indicates the desired piece of diagnostic
 *  information, and implicitly indicates whether the information comes from
 *  the header or from record RecNumber
 *  @param DiagInfo Buffer for diagnostic information.
 *  @param BufferLength Length in octets of DiagInfo , if the requested
 *  diagnostic data is a character string; otherwise, unused.
 *  @param StringLength Length in octets of complete diagnostic information
 *  (even if it does not entirely fit in DiagInfo ), if the requested
 *  diagnostic data is a character string; otherwise, unused.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetDiagField
(
		SQLSMALLINT	HandleType,
		SQLHANDLE	Handle,
		SQLSMALLINT	RecNumber,
		SQLSMALLINT	DiagIdentifier,
		SQLPOINTER	DiagInfo,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	StringLength
)
{
	return	SQL_SUCCESS;
}

/*! @brief Return diagnostic information
 *
 *  function is not implemented
 *
 *  @param HandleType A handle type identifier that describes the handle type
 *  of Handle
 *  @param Handle A handle for which diagnostic information is desired.
 *  @param RecNumber Indicates the status record from which the application
 *  seeks information. The first record is number 1.
 *  @param Sqlstate A 5-character SQLSTATE code pertaining to diagnostic record
 *  RecNumber. The first 2 characters indicate class; the next 3 indicate
 *  subclass. The SQLSTATE code provides a portable diagnostic indication.
 *  @param NativeError An implementation-defined error code pertaining to
 *  diagnostic record RecNumber.
 *  @param MessageText Implementation-defined message text pertaining to
 *  diagnostic record RecNumber.
 *  @param BufferLength Maximum number of octets to store in MessageText.
 *  @param TextLength Length in octets of the output data (even if it does not
 *  entirely fit in MessageText).
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetDiagRec
(
		SQLSMALLINT	HandleType,
		SQLHANDLE	Handle,
		SQLSMALLINT	RecNumber,
		SQLCHAR*	Sqlstate,
		SQLINTEGER*	NativeError,
		SQLCHAR*	MessageText,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	TextLength
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get the value of an environment attribute
 *
 *  @param EnvironmentHandle Environment handle.
 *  @param Attribute The desired environment attribute
 *  @param Value The current value of the attribute specified by Attribute.
 *  The type of the value returned depends on Attribute.
 *  @param BufferLength Maximum number of octets to store in Value, if the
 *  attribute value is a character string; otherwise, unused.
 *  @param StringLength Length in octets of the output data (even if it does
 *  not entirely fit in Value), if the attribute value is a character string;
 *  otherwise, unused.
 *
 *  @return SQL_INVALID_HANDLE null environment handle
 *  @return SQL_ERROR invalid attribute
 *  @return SQL_SUCCESS attribute fetched
 *
 */
SQLRETURN SQL_API	SQLGetEnvAttr
(
		SQLHENV	EnvironmentHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength,
		SQLINTEGER*	StringLength
)
{
	if (EnvironmentHandle == 0)
		return	SQL_INVALID_HANDLE;
	switch (Attribute)
	{
	case	SQL_ATTR_OUTPUT_NTS:
		*((SQLINTEGER*)Value) = ((XdbdOdbcEnvironment*)EnvironmentHandle)->attrOutputNts();
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Determine whether a function is supported
 *
 *  function is not implemented
 *
 *  @param ConnectionHandle Connection handle.
 *  @param FunctionId A number that represents function in question
 *  @param Supported The value SQL_TRUE if the CLI implementation supports
 *  FunctionId; otherwise, the value SQL_FALSE
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetFunctions
(
		SQLHDBC	ConnectionHandle,
		SQLUSMALLINT 	FunctionId,
		SQLUSMALLINT*	Supported
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get information about the CLI implementation and the currently
 *  connected server
 *
 *  function is not implemented
 *
 *  @param ConnectionHandle Connection handle.
 *  @param InfoType The type of information being requested
 *  @param InfoValue The current value of the characteristic specified by
 *  InfoType. The type of the value returned depends on InfoType.
 *  @param BufferLength The maximum number of octets to store in InfoValue, if
 *  Value is a character string; otherwise, unused.
 *  @param StringLength The length in octets of the output data (even if it
 *  does not entirely fit into InfoValue) if InfoValue is a character string;
 *  otherwise, unused.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetInfo
(
		SQLHDBC	ConnectionHandle,
		SQLUSMALLINT	InfoType,
		SQLPOINTER	InfoValue,
		SQLSMALLINT	BufferLength,
		SQLSMALLINT*	StringLength
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get the value of a statement attribute
 *
 *  @param StatementHandle Statement handle.
 *  @param Attribute The desired statement attribute
 *  @param Value The current value of the attribute specified by Attribute. The
 *  type of the value returned depends on Attribute.
 *  @param BufferLength Maximum number of octets to store in Value, if the
 *  attribute value is a character string; otherwise, unused.
 *  @param StringLength Length in octets of the output data (even if it does
 *  not entirely fit in Value), if the attribute value is a character string;
 *  otherwise, unused
 *
 *  @return SQL_ERROR invalid Attribute
 *  @return SQL_SUCCESS attribute acquired
 *
 */
SQLRETURN SQL_API	SQLGetStmtAttr
(
		SQLHSTMT	StatementHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength,
		SQLINTEGER*	StringLength
)
{
	switch (Attribute)
	{
	case	SQL_ATTR_APP_ROW_DESC:
		*((SQLHANDLE*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrAppRowDesc();
		break;
	case	SQL_ATTR_APP_PARAM_DESC:
		*((SQLHANDLE*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrAppParamDesc();
		break;
	case	SQL_ATTR_CURSOR_SCROLLABLE:
		*((SQLINTEGER*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrCursorScrollable();
		break;
	case	SQL_ATTR_CURSOR_SENSITIVITY:
		*((SQLINTEGER*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrCursorSensitivity();
		break;
	case	SQL_ATTR_IMP_ROW_DESC:
		*((SQLHANDLE*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrImpRowDesc();
		break;
	case	SQL_ATTR_IMP_PARAM_DESC:
		*((SQLHANDLE*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrImpParamDesc();
		break;
	case	SQL_ATTR_METADATA_ID:
		*((SQLINTEGER*)Value) = ((XdbdOdbcStatement*)StatementHandle)->attrMetadataId();
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Get information on server data types
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param DataType The SQL data type being queried
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLGetTypeInfo
(
		SQLHSTMT	StatementHandle,
		SQLSMALLINT	DataType
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get the number of columns in a result set
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param ColumnCount Number of columns in the result set.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLNumResultCols
(
		SQLHSTMT	StatementHandle,
		SQLSMALLINT*	ColumnCount
)
{
	return	SQL_SUCCESS;
}

/*! @brief Address the next dynamic parameter requiring data at execution time
 *
 *  @param StatementHandle Statement handle.
 *  @param Value If the function returns [SQL_NEED_DATA], this returns the
 *  contents of the DATA_PTR field of the record in the application parameter
 *  descriptor that relates to the dynamic parameter for which the
 *  implementation requires information. For any other return value, Value is
 *  undefined
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return status of low level function ParamData(), Execute() or InitFetch(),
 *  in that order, whichever failed first, if they not succeed.
 *
 */
SQLRETURN SQL_API	SQLParamData
(
		SQLHSTMT	StatementHandle,
		SQLPOINTER*	Value
)
{
	SQLRETURN	status;
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	if ((status = stmt->ParamData(Value)) != SQL_SUCCESS)
		return	status;
	if ((status = stmt->Execute()) != SQL_SUCCESS)
		return	status;
	return	stmt->InitFetch();
}

/*! @brief Prepare an SQL statement for execution
 *
 *  @param StatementHandle Statement handle.
 *  @param StatementText SQL text string, using question-mark (?) characters as
 *  dynamic parameter markers.
 *  @param TextLength Length in octets of StatementText.
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return SQL_ERROR some internal step failed
 *  @return SQL_SUCCESS statement is prepared for execution
 *
 */
SQLRETURN SQL_API	SQLPrepare
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	StatementText,
		SQLINTEGER	TextLength
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	if (XdbdPostPrepare(stmt->statement(), (char*) StatementText, 0, 0) != 0)
		return	SQL_ERROR;
	if (stmt->LoadDescriptors() < 0)
		return	SQL_ERROR;
	return	SQL_SUCCESS;
}

/*! @brief Send part or all of a dynamic argument at execute time
 *
 *  @param StatementHandle Statement handle.
 *  @param Data All or part of a dynamic argument to be used in execution of a
 *  statement on StatementHandle.
 *  @param StrLen_or_Ind A variable whose value is interpreted as follows:
 *  - If a null value is to be used as the parameter, StrLen_or_Ind must
 *  contain the value SQL_NULL_DATA.
 *  - If the ValueType specified when binding the parameter is SQL_CHAR:
 *  - If the data in Data contains a null-terminated string, StrLen_or_Ind must
 *  either contain the length in octets of the string in Data or contain the
 *  value SQL_NTS.
 *  - If the data in Data is not null-terminated, StrLen_or_Ind must contain
 *  the length in octets of the string in Data.
 *  - Otherwise, StrLen_or_Ind is ignored
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return status of internal function call PutData()
 *
 */
SQLRETURN SQL_API	SQLPutData
(
		SQLHSTMT	StatementHandle,
		SQLPOINTER	Data,
		SQLLEN	StrLen_or_Ind
)
{
	int status;
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	if (stmt == 0)
		return	SQL_INVALID_HANDLE;
	if ((status = stmt->PutData (Data, StrLen_or_Ind)) != SQL_SUCCESS)
		return status;
	if ((status = stmt->Execute()) != SQL_SUCCESS)
		return	status;
	return	stmt->InitFetch();
}

/*! @brief Get the number of rows affected by an SQL statement
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param RowCount The count of rows affected
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLRowCount
(
		SQLHSTMT	StatementHandle,
		SQLLEN*	RowCount
)
{
	return	SQL_SUCCESS;
}

/*! @brief Set a connection attribute
 *
 *  @param ConnectionHandle Connection handle.
 *  @param Attribute The attribute whose value is to be set
 *  @param Value The desired value for Attribute
 *  @param StringLength Length in octets of Value, if the attribute value is a
 *  character string; otherwise ignored.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLSetConnectAttr
(
		SQLHDBC	ConnectionHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	StringLength
)
{
	switch (Attribute)
	{
	case	SQL_ATTR_AUTO_IPD:
		((XdbdOdbcConnection*)ConnectionHandle)->attrAutoIpd (*((SQLINTEGER*)Value));
		break;
	case	SQL_ATTR_METADATA_ID:
		((XdbdOdbcConnection*)ConnectionHandle)->attrMetadataId (*((SQLINTEGER*)Value));
		break;
	default:
		return	SQL_SUCCESS;	// SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Set the name of a cursor
 *
 *  The application can call SetCursorName( ) to assign a name to a cursor.
 *  This is an optional step; the implementation implicitly gives each cursor a
 *  name (starting with ’SQLCUR’ or ’SQL_CUR’) when it creates the cursor.
 *
 *  @param StatementHandle Statement handle.
 *  @param CursorName Cursor name
 *  @param NameLength Length in octets of CursorName.
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLSetCursorName
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	CursorName,
		SQLSMALLINT	NameLength
)
{
	XdbdOdbcStatement*	stmt = (XdbdOdbcStatement*) StatementHandle;
	stmt->setCursorName(CursorName, NameLength);
	return	SQL_SUCCESS;
}

/*! @brief Set a descriptor field
 *
 *  sets the value of one field of a specified descriptor record associated
 *  with DescriptorHandle
 *
 *  @param DescriptorHandle Descriptor handle.
 *  @param RecNumber The record number for which the specified field in the
 *  descriptor is to be set. The first record is number 1.
 *  @param FieldIdentifier The identifier of the field to be set.
 *  @param Value The desired value for FieldIdentifier. The application must
 *  provide an argument of an appropriate type for FieldIdentifier.
 *  @param BufferLength Length in octets of Value.
 *
 *  @return SQL_INVALID_HANDLE null descriptor handle
 *  @return SQL_ERROR unknown field identifier
 *  @return SQL_SUCCESS field value set
 *
 */
SQLRETURN SQL_API	SQLSetDescField
(
		SQLHDESC	DescriptorHandle,
		SQLSMALLINT	RecNumber,
		SQLSMALLINT	FieldIdentifier,
		SQLPOINTER	Value,
		SQLINTEGER	BufferLength
)
{
	XdbdOdbcDescriptor*	desc = (XdbdOdbcDescriptor*) DescriptorHandle;
	if (desc == 0)
		return	SQL_INVALID_HANDLE;
	switch (FieldIdentifier)
	{
	case	SQL_DESC_COUNT:
		desc->descCount(*((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_TYPE:
		desc->descType(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_LENGTH:
		desc->descLength(RecNumber, *((SQLINTEGER*)Value));
		break;
	case	SQL_DESC_OCTET_LENGTH_PTR:
		desc->descOctetLengthPtr(RecNumber, *((SQLPOINTER*)Value));
		break;
	case	SQL_DESC_PRECISION:
		desc->descPrecision(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_SCALE:
		desc->descScale(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_DATETIME_INTERVAL_CODE:
		desc->descDatetimeIntervalCode(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_NULLABLE:
		desc->descNullable(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_INDICATOR_PTR:
		desc->descIndicatorPtr(RecNumber, *((SQLPOINTER*)Value));
		break;
	case	SQL_DESC_DATA_PTR:
		desc->descDataPtr(RecNumber, *((SQLPOINTER*)Value));
		break;
	case	SQL_DESC_NAME:
		desc->descName(RecNumber, *((SQLCHAR**)Value));
		break;
	case	SQL_DESC_UNNAMED:
		desc->descType(RecNumber, *((SQLSMALLINT*)Value));
		break;
	case	SQL_DESC_OCTET_LENGTH:
		desc->descOctetLength(RecNumber, *((SQLINTEGER*)Value));
		break;
	case	SQL_DESC_ALLOC_TYPE:
		return	SQL_ERROR;
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Set a descriptor record
 *
 *  initializes a descriptor record associated with DescriptorHandle
 *
 *  @param DescriptorHandle Descriptor handle.
 *  @param RecNumber The record number from which the description in the
 *  descriptor is to be set. The first record is number 1.
 *  @param Type The TYPE field for the record.
 *  @param SubType The DATETIME_INTERVAL_CODE field, for records whose TYPE is
 *  SQL_DATETIME.
 *  @param Length The OCTET_LENGTH field for the record.
 *  @param Precision The PRECISION field for the record.
 *  @param Scale The SCALE field for the record.
 *  @param Data The DATA_PTR field for the record.
 *  @param StringLength The OCTET_LENGTH_PTR field for the record.
 *  @param Indicator The INDICATOR_PTR field for the record.
 *
 *  @return SQL_INVALID_HANDLE null descriptor handle
 *  @return SQL_SUCCESS descriptor record set
 *
 */
SQLRETURN SQL_API	SQLSetDescRec
(
		SQLHDESC	DescriptorHandle,
		SQLSMALLINT	RecNumber,
		SQLSMALLINT	Type,
		SQLSMALLINT	SubType,
		SQLLEN	Length,
		SQLSMALLINT	Precision,
		SQLSMALLINT	Scale,
		SQLPOINTER	Data,
		SQLLEN*	StringLength,
		SQLLEN*	Indicator
)
{
	XdbdOdbcDescriptor*	desc = (XdbdOdbcDescriptor*) DescriptorHandle;
	if (desc == 0)
		return	SQL_INVALID_HANDLE;
	desc->descType(RecNumber, Type);
	desc->descDatetimeIntervalCode(RecNumber, SubType);
	desc->descOctetLength(RecNumber, Length);
	desc->descPrecision(RecNumber, Precision);
	desc->descScale(RecNumber, Scale);
	desc->descDataPtr(RecNumber, Data);
	desc->descOctetLengthPtr(RecNumber, StringLength);
	desc->descIndicatorPtr(RecNumber, Indicator);
	return	SQL_SUCCESS;
}

/*! @brief Set an environment attribute
 *
 *  @param EnvironmentHandle Environment handle.
 *  @param Attribute The attribute whose value is to be set
 *  @param Value The desired value for Attribute
 *  @param StringLength Length in octets of Value, if the attribute value is a
 *  character string; otherwise, unused
 *
 *  @return SQL_INVALID_HANDLE null environment handle
 *  @return SQL_ERROR unknown attribute
 *  @return SQL_SUCCESS attribute value set
 *
 */
SQLRETURN SQL_API	SQLSetEnvAttr
(
		SQLHENV	EnvironmentHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	StringLength
)
{
	if (EnvironmentHandle == 0)
		return	SQL_INVALID_HANDLE;
	switch (Attribute)
	{
	case	SQL_ATTR_OUTPUT_NTS:
		((XdbdOdbcEnvironment*)EnvironmentHandle)->attrOutputNts(*((SQLINTEGER*)Value));
		break;
	case	SQL_ATTR_ODBC_VERSION:
		return SQL_SUCCESS;
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Set a statement attribute
 *
 *  @param StatementHandle Statement handle.
 *  @param Attribute The attribute whose value is to be set
 *  @param Value The desired value for Attribute
 *  @param StringLength Length in octets of Value, if the attribute value is a
 *  character string; otherwise, unused
 *
 *  @return SQL_INVALID_HANDLE null statement handle
 *  @return SQL_ERROR invalid attribute
 *  @return SQL_SUCCESS attribute set
 *
 */
SQLRETURN SQL_API	SQLSetStmtAttr
(
		SQLHSTMT	StatementHandle,
		SQLINTEGER	Attribute,
		SQLPOINTER	Value,
		SQLINTEGER	StringLength
)
{
	if (StatementHandle == 0)
		return	SQL_INVALID_HANDLE;
	switch (Attribute)
	{
	case	SQL_ATTR_APP_ROW_DESC:
		((XdbdOdbcStatement*)StatementHandle)->attrAppRowDesc(*((SQLHANDLE*)Value));
		break;
	case	SQL_ATTR_APP_PARAM_DESC:
		((XdbdOdbcStatement*)StatementHandle)->attrAppParamDesc(*((SQLHANDLE*)Value));
		break;
	case	SQL_ATTR_CURSOR_SCROLLABLE:
		((XdbdOdbcStatement*)StatementHandle)->attrCursorScrollable(*((SQLINTEGER*)Value));
		break;
	case	SQL_ATTR_CURSOR_SENSITIVITY:
		((XdbdOdbcStatement*)StatementHandle)->attrCursorSensitivity(*((SQLINTEGER*)Value));
		break;
	case	SQL_ATTR_IMP_ROW_DESC:
		return	SQL_ERROR;
		break;
	case	SQL_ATTR_IMP_PARAM_DESC:
		return	SQL_ERROR;
		break;
	case	SQL_ATTR_METADATA_ID:
		((XdbdOdbcStatement*)StatementHandle)->attrMetadataId(*((SQLINTEGER*)Value));
		break;
	default:
		return	SQL_ERROR;
	}
	return	SQL_SUCCESS;
}

/*! @brief Get a unique row identifier for a table
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param IdentifierType Reserved for future definition by X/Open in order to
 *  support additional types of special columns. Must be set to
 *  SQL_ROW_IDENTIFIER.
 *  @param CatalogName Buffer containing catalog name of table
 *  @param NameLength1 Length in octets of CatalogName
 *  @param SchemaName Buffer containing schema name of table
 *  @param NameLength2 Length in octets of SchemaName.
 *  @param TableName Buffer containing name of table
 *  @param NameLength3 Length in octets of TableName.
 *  @param Scope The application sets this argument to specify the period of
 *  time for which it requires thatthe name returned in COLUMN_NAME be valid
 *  @param Nullable Set to SQL_NO_NULLS to restrict the result set of
 *  SpecialColumns () to describe only those columns of TableName that are not
 *  nullable; or to SQL_NULLABLE to not thus restrict the result set
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLSpecialColumns
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	IdentifierType,
		SQLCHAR*	CatalogName,
		SQLSMALLINT	NameLength1,
		SQLCHAR*	SchemaName,
		SQLSMALLINT	NameLength2,
		SQLCHAR*	TableName,
		SQLSMALLINT	NameLength3,
		SQLUSMALLINT	Scope,
		SQLUSMALLINT	Nullable
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get index information for a base table
 *
 *  function is not implemented
 *
 *  @param StatementHandle Statement handle.
 *  @param CatalogName Buffer containing catalog name of table
 *  @param NameLength1 Length in octets of CatalogName
 *  @param SchemaName Buffer containing schema name of table
 *  @param NameLength2 Length in octets of SchemaName.
 *  @param TableName Buffer containing name of table
 *  @param NameLength3 Length in octets of TableName.
 *  @param Unique Set to SQL_INDEX_UNIQUE to direct Statistics () to return
 *  only unique indexes, or set to SQL_INDEX_ALL to obtain all indexes.
 *  @param Reserved  Reserved for future definition by X/Open. Must be set to 0
 *
 *  @return SQL_SUCCESS always
 *
 */
SQLRETURN SQL_API	SQLStatistics
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	CatalogName,
		SQLSMALLINT	NameLength1,
		SQLCHAR*	SchemaName,
		SQLSMALLINT	NameLength2,
		SQLCHAR*	TableName,
		SQLSMALLINT	NameLength3,
		SQLUSMALLINT	Unique,
		SQLUSMALLINT	Reserved
)
{
	return	SQL_SUCCESS;
}

/*! @brief Get table information
 *
 *  function is not implemented
 *
 *
 */
SQLRETURN SQL_API	SQLTables
(
		SQLHSTMT	StatementHandle,
		SQLCHAR*	CatalogName,
		SQLSMALLINT	NameLength1,
		SQLCHAR*	SchemaName,
		SQLSMALLINT	NameLength2,
		SQLCHAR*	TableName,
		SQLSMALLINT	NameLength3,
		SQLCHAR*	TableType,
		SQLSMALLINT	NameLength4
)
{
	return	SQL_SUCCESS;
}

SQLRETURN SQL_API	SQLTransact
(
		SQLHENV	EnvironmentHandle,
		SQLHDBC	ConnectionHandle,
		SQLUSMALLINT	CompletionType
)
{
	return	SQL_SUCCESS;
}

#if defined(__xdbdimplementation)

SQLRETURN SQL_API	SQLAllocConnect
(
	SQLHANDLE	EnvironmentHandle,
	SQLHANDLE*	ConnectionHandle
)
{
	return	SQLAllocHandle(SQL_HANDLE_DBC,EnvironmentHandle,ConnectionHandle);
}

SQLRETURN SQL_API	SQLAllocEnv
(
	SQLHANDLE*	EnvironmentHandle
)
{
	return	SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,EnvironmentHandle);
}

SQLRETURN SQL_API	SQLAllocStmt
(
	SQLHANDLE	ConnectionHandle,
	SQLHANDLE*	StatementHandle
)
{
	return	SQLAllocHandle(SQL_HANDLE_STMT,ConnectionHandle,StatementHandle);
}

SQLRETURN SQL_API	SQLBindParameter
(
	SQLHSTMT	StatementHandle,
	SQLUSMALLINT	ParameterNumber,
	SQLSMALLINT	fParamType,
	SQLSMALLINT	ValueType,
	SQLSMALLINT	ParameterType,
	SQLULEN	LengthPrecision,
	SQLSMALLINT	ParameterScale,
	SQLPOINTER	ParameterValue,
	SQLSMALLINT	cbValueMax,
	SQLLEN*	StrLen_or_Ind
)
{
	return	SQLBindParam (StatementHandle,ParameterNumber,ValueType,ParameterType,LengthPrecision,ParameterScale,ParameterValue,StrLen_or_Ind);
}

SQLRETURN SQL_API	SQLFreeConnect
(
	SQLHANDLE	ConnectionHandle
)
{
	return	SQLFreeHandle(SQL_HANDLE_DBC,ConnectionHandle);
}

SQLRETURN SQL_API	SQLFreeEnv
(
	SQLHANDLE	EnvironmentHandle
)
{
	return	SQLFreeHandle(SQL_HANDLE_ENV,EnvironmentHandle);
}

SQLRETURN SQL_API	SQLFreeStmt
(
	SQLHANDLE	StatementHandle,
	SQLSMALLINT	Option
)
{
	return	SQLFreeHandle(SQL_HANDLE_STMT,StatementHandle);
}

SQLRETURN SQL_API	SQLGetConnectOption
(
	SQLHDBC	ConnectionHandle,
	SQLINTEGER	Option,
	SQLPOINTER	Value
)
{
	return	SQLGetConnectAttr(ConnectionHandle,Option,Value,0,0);
}

SQLRETURN SQL_API	SQLGetStmtOption
(
	SQLHSTMT	StatementHandle,
	SQLINTEGER	Option,
	SQLPOINTER	Value
)
{
	return	SQLGetStmtAttr(StatementHandle,Option,Value,0,0);
}

SQLRETURN SQL_API	SQLSetConnectOption
(
	SQLHDBC	ConnectionHandle,
	SQLINTEGER	Option,
	SQLPOINTER	Value
)
{
	return	SQLSetConnectAttr(ConnectionHandle,Option,Value,0);
}

SQLRETURN SQL_API	SQLSetParam
(
		SQLHSTMT	StatementHandle,
		SQLUSMALLINT	ParameterNumber,
		SQLSMALLINT	ValueType,
		SQLSMALLINT	ParameterType,
		SQLULEN	LengthPrecision,
		SQLSMALLINT	ParameterScale,
		SQLPOINTER	ParameterValue,
		SQLLEN*	StrLen_or_Ind
)
{
	return	SQLBindParam(StatementHandle,ParameterNumber,ValueType,ParameterType,LengthPrecision,ParameterScale,ParameterValue,StrLen_or_Ind);
}

SQLRETURN SQL_API	SQLSetStmtOption
(
		SQLHSTMT	StatementHandle,
		SQLINTEGER	Option,
		SQLPOINTER	Value
)
{
	return	SQLSetStmtAttr(StatementHandle,Option,Value,0);
}

#endif

}

/*
 * XdbdXmlDataBase.h
 *
 *  Created on: 14. nov. 2015
 *      Author: miran
 */

#pragma once

#include <signal.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "sql_statement.h"
#include "XdbdShmSegment.h"
#include "XdbdSqlComparison.h"
#include "XdbdTrigger.h"
#include "XdbdTriggerParameter.h"
#include "XdbdChkXmlDir.h"
#include "XdbdChkXsdDir.h"
#include "XdbdChkStatDir.h"
#include "XdbdInitStatAdapter.h"
#include "XdbdMessageQueue.h"
#include "XdbdRunningContext.h"

#include <map>
#include <vector>
#include <deque>
#include <string>
using namespace std;

namespace xml_db_daemon
{

class	XdbdSqlSelectFunction;
class	XdbdSqlUpdateFunction;
class	XdbdSqlInsertFunction;
class	XdbdSqlDeleteFunction;

typedef map < string, XdbdXmlTableInfo* >	docmap;	// one to one (table name) <--> (table) mapping
typedef multimap < string, string >	tabmap;	// one to many (xsd file name) <--> (table name) mapping
typedef	pair < tabmap::iterator, tabmap::iterator >	tabrng;	// range of tables defined in given xsd file
typedef map < short, XdbdTriggerParameter* >	parset;
typedef map < short, XdbdTrigger* >	trigset;
typedef deque < XdbdBaseJob* >	joblist;

class XdbdXmlDataBase
{
	typedef	void (*chgcb) (int, void*, struct inotify_event*);
	typedef struct _chgstruct
	{
		chgcb m_f;
		void* m_data;
		struct _chgstruct* m_next;
	}	chgstruct;
	typedef pair < chgcb, void* >	chgtpr;
	typedef	map < int, chgstruct* >	chgmap;
private:
	XdbdXmlDataBase();
	virtual ~XdbdXmlDataBase();
public:
	inline static int	LoadXmlDataBase (XdbdRunningContext* ctx) { return (g_XdbdXmlDataBase != 0) ? -1 : (g_XdbdXmlDataBase = new XdbdXmlDataBase())->_LoadXmlDataBase(ctx); }
	inline static int	UnloadXmlDataBase () { return g_XdbdXmlDataBase->_UnloadXmlDataBase(); }
	inline static int	LoadXsdFiles (char* xsdPath = 0) { return g_XdbdXmlDataBase->_LoadXsdFiles (xsdPath); }
	inline static int	LoadXmlFiles (char* xmlPath = 0) { return g_XdbdXmlDataBase->_LoadXmlFiles (xmlPath); }
	inline static int	LoadStatFiles (char* statPath = 0) { return g_XdbdXmlDataBase->_LoadStatFiles (statPath); }
	inline static int	RestoreXdbFiles (char* xmlPath = 0) { return g_XdbdXmlDataBase->_RestoreXdbFiles (); }
	inline static int	ReloadXmlFile (char* xmlFname, char* xmlPath = 0) { g_XdbdXmlDataBase->DisableTriggers(); int status = g_XdbdXmlDataBase->_ReloadXmlFile (xmlFname, xmlPath); g_XdbdXmlDataBase->EnableTriggers(); return status; }
	inline static int	ReloadStatFile (void* p_adapter, char* statFname) { return g_XdbdXmlDataBase->_ReloadStatFile (p_adapter, statFname); }
	inline static int	LoadTriggers () { return g_XdbdXmlDataBase->_LoadTriggers (); }
	inline static XdbdXmlColumnInfo::colrng	PrepareComparison (tablist& tables, sql_comp_expression* p_sql_comp_expression, XdbdXmlTableInfo* &tabinfo, XdbdXmlColumnInfo* &colinfo, unsigned char* &data, ssize_t &size, SqlSyntaxContext& sqlSyntaxContext, bool& invert)
		{ return g_XdbdXmlDataBase->_PrepareComparison (tables, p_sql_comp_expression, tabinfo, colinfo, data, size, sqlSyntaxContext, invert); }
	inline static XdbdXmlColumnInfo::colrng	PrepareComparison (XdbdXmlTableInfo* tabinfo, sql_comp_expression* p_sql_comp_expression, XdbdXmlColumnInfo* &colinfo, unsigned char* &data, ssize_t &size, SqlSyntaxContext& sqlSyntaxContext, bool& invert)
		{ return g_XdbdXmlDataBase->_PrepareComparison (tabinfo, p_sql_comp_expression, colinfo, data, size, sqlSyntaxContext, invert); }
	inline static XdbdXmlColumnInfo::colrng	ExecComparison (XdbdXmlColumnInfo* colinfo, unsigned char* data, unsigned char* srcData, size_t size, SqlSyntaxContext sqlSyntaxContext)
		{ return g_XdbdXmlDataBase->_ExecComparison (colinfo, data, srcData, size, sqlSyntaxContext, true); }
	inline static XdbdXmlTableInfo*	GetTableInfo (string name) { return g_XdbdXmlDataBase->_GetTableInfo(name); }
	inline static bool	IgnoreNodeid () { return g_XdbdIgnoreNodeid; }
	static inline string	XdbdXsdDataBasePath() { return g_XdbdXsdDataBasePath; }
	static inline string	XdbdXmlDataBasePath() { return g_XdbdXmlDataBasePath; }
	static inline string	XdbdDatDataBasePath() { return g_XdbdDatDataBasePath; }
	static inline void	EnableTriggers () { g_XdbdXmlDataBase->_EnableTriggers(); }
	static inline void	DisableTriggers () { g_XdbdXmlDataBase->_DisableTriggers(); }
	static inline bool	TriggerStatus () { return g_XdbdXmlDataBase->_TriggerStatus(); }
	static inline void	SaveTrgJob (XdbdBaseJob* job) { g_XdbdXmlDataBase->_SaveTrgJob(job); }
	inline static void InitStatAdapter () { XdbdMessageQueue::PutInt (new XdbdInitStatAdapter()); }
	inline static void ReleaseStatAdapter () { g_XdbdXmlDataBase->_ReleaseStatAdapter(); }
	inline static void InsertAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
		{ g_XdbdXmlDataBase->_InsertAdapter (adapter_id, stat_dir_name, dest_dir_name, owner_name, group_name, dir_permissions, file_permissions); }
	inline static void UpdateAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
		{ g_XdbdXmlDataBase->_UpdateAdapter( adapter_id, stat_dir_name, dest_dir_name, owner_name, group_name, dir_permissions, file_permissions); }
	inline static void DeleteAdapter (SQLSMALLINT adapter_id) { g_XdbdXmlDataBase->_DeleteAdapter (adapter_id); }
	static int equal (SQLVARCHAR* newLine, SQLVARCHAR* oldLine, SQLVARCHAR* mask, size_t lineSize);
private:
	void lock ();
	void unlock ();
	int RegChgTracker (char *dir, chgcb f, void *data);
	int	UnregChgTracker (int wd, void* data);
	inline static void HandleXmlDirChanges (int fd, void* data, struct inotify_event* nevent) { XdbdMessageQueue::PutInt (new XdbdChkXmlDir (fd, data, nevent)); }
	inline static void HandleXsdDirChanges (int fd, void* data, struct inotify_event* nevent) { XdbdMessageQueue::PutInt (new XdbdChkXsdDir (fd, data, nevent));}
	inline static void HandleStatDirChanges (int fd, void* data, struct inotify_event* nevent) { XdbdMessageQueue::PutInt (new XdbdChkStatDir (fd, data, nevent));}
	inline static chgstruct* GetChgInfo (int fd) { return g_XdbdXmlDataBase->_GetChgInfo(fd); }
	inline chgstruct* _GetChgInfo (int fd) { return m_chgmap[fd]; }

	int	_LoadXmlDataBase (XdbdRunningContext* ctx);
	int	_InitXmlDataBase ();
	int	_LoadXsdFiles (char* xsdPath = 0);
	int	_LoadXsdFile (char* xsdFname, char* xsdPath = 0);
	int	_LoadXmlFiles (char* xmlPath = 0);
	int	_LoadXmlFile (char* xmlFname, char* xmlPath = 0);
	int _LoadStatFiles (char* statPath = 0);
	int	_RestoreXdbFiles ();
	int	_ReloadXmlFiles (char* xmlPath = 0);
	int	_ReloadXmlFile (char* xmlFname, char* xmlPath = 0);
	int	_ReloadStatFile (void* p_adapter, char* statFname);
	void _InsertAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions);
	void _UpdateAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions);
	void _DeleteAdapter (SQLSMALLINT adapter_id);
	void _ReleaseStatAdapter ();
	int	_LoadTriggers ();
	int	_UnloadXmlDataBase ();
	XdbdXmlColumnInfo::colrng	_PrepareComparison (tablist& tables, sql_comp_expression* p_sql_comp_expression, XdbdXmlTableInfo* &tabinfo, XdbdXmlColumnInfo* &colinfo, unsigned char* &data, ssize_t &size, SqlSyntaxContext& sqlSyntaxContext, bool& invert);
	XdbdXmlColumnInfo::colrng	_PrepareComparison (XdbdXmlTableInfo* tabinfo, sql_comp_expression* p_sql_comp_expression, XdbdXmlColumnInfo* &colinfo, unsigned char* &data, ssize_t &size, SqlSyntaxContext& sqlSyntaxContext, bool& invert);
	XdbdXmlColumnInfo::colrng	_ExecComparison (XdbdXmlColumnInfo* colinfo, unsigned char* data, unsigned char* srcData, ssize_t size, SqlSyntaxContext sqlSyntaxContext, bool binary);
	XdbdXmlTableInfo* _GetTableInfo (string  name);
	xmlAttrPtr	findAttribute (xmlNodePtr node, const char*path[], const char*name, int index);

	fd_handler (ReadNotifyChanges, XdbdXmlDataBase)
	sig_handler (HandleSigPipe, XdbdXmlDataBase)

	inline void	_EnableTriggers () { m_enableTriggers = true; }
	inline void	_DisableTriggers () { m_enableTriggers = false; }
	inline bool	_TriggerStatus () { return m_enableTriggers; }
	inline void	_SaveTrgJob (XdbdBaseJob* job) { m_trgJobList.push_back(job); }
	void	_ExecuteTriggers ();
private:
	static const char*	g_trigger_id;
	static const char*	g_trigger_name;
	static const char*	g_trigger_library;
	static const char*	g_trigger_load;
	static const char*	g_trigger_fire;
	static const char*	g_trigger_unload;
	static const char*	g_trigger_table;
	static const char*	g_trigger_function;

	static const char*	g_trigger_par_id;
	static const char*	g_trigger_par_rid;
	static const char*	g_trigger_par_name;
	static const char*	g_trigger_par_value;

	static string	g_XdbdXsdDataBasePath;
	static string	g_XdbdXmlDataBasePath;
	static string	g_XdbdDatDataBasePath;
	static string	g_XdbdNsPrefix;
	static string	g_XdbdNsUri;
	static XdbdXmlDataBase*	g_XdbdXmlDataBase;
	static bool	g_XdbdIgnoreNodeid;

	docmap	m_docmap;
	tabmap	m_tabmap;
	XdbdXmlTableInfo*	m_currentTableInfo;
	XdbdXmlColumnInfo*	m_currentColumnInfo;
	parset	m_parset;
	trigset	m_trigset;
	chgmap	m_chgmap;

	XdbdRunningContext*	m_ctx;
	int m_sync;
	int	m_notify;
	ctx_fddes_t	m_notifyHandler;

	u_char*	m_inputBuffer;
	u_char*	m_inputPtr;
	u_char*	m_inputEnd;
	bool	m_enableTriggers;
	joblist	m_trgJobList;
};

} /* namespace xml_db_daemon */

extern	"C"
{
void* nmFunc_stat_adapter_load (int n, char* p[]);
int nmFunc_stat_adapter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_stat_adapter_unload (void *context);
}

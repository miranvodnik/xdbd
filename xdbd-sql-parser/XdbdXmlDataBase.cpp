/*
 * XdbdXmlDataBase.cpp
 *
 *  Created on: 14. nov. 2015
 *      Author: miran
 */

#include <sqlxdbd.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "XdbdCommon.h"
#include "XdbdXmlDataBase.h"
#include "XdbdSqlSelectFunction.h"
#include "XdbdSqlUpdateFunction.h"
#include "XdbdSqlInsertFunction.h"
#include "StatisticalAdapterThread.h"
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

string XdbdXmlDataBase::g_XdbdXsdDataBasePath = "/opt/si2000/neroot/xsddb";
string XdbdXmlDataBase::g_XdbdXmlDataBasePath = "/opt/si2000/neroot/xmldb";
string XdbdXmlDataBase::g_XdbdDatDataBasePath = "/opt/si2000/neroot/datdb";
string XdbdXmlDataBase::g_XdbdNsPrefix = "xs";
string XdbdXmlDataBase::g_XdbdNsUri = "http://www.w3.org/2001/XMLSchema";
XdbdXmlDataBase* XdbdXmlDataBase::g_XdbdXmlDataBase = 0;
bool XdbdXmlDataBase::g_XdbdIgnoreNodeid = true;

const char*	XdbdXmlDataBase::g_trigger_id = "trigger_id";
const char*	XdbdXmlDataBase::g_trigger_name = "trigger_name";
const char*	XdbdXmlDataBase::g_trigger_library = "trigger_library";
const char*	XdbdXmlDataBase::g_trigger_load = "trigger_load";
const char*	XdbdXmlDataBase::g_trigger_fire = "trigger_fire";
const char*	XdbdXmlDataBase::g_trigger_unload = "trigger_unload";
const char*	XdbdXmlDataBase::g_trigger_table = "trigger_table";
const char*	XdbdXmlDataBase::g_trigger_function= "trigger_function";

const char*	XdbdXmlDataBase::g_trigger_par_id = "trigger_par_id";
const char*	XdbdXmlDataBase::g_trigger_par_rid = "trigger_id";
const char*	XdbdXmlDataBase::g_trigger_par_name = "trigger_par_name";
const char*	XdbdXmlDataBase::g_trigger_par_value = "trigger_par_value";

XdbdXmlDataBase::XdbdXmlDataBase ()
{
	char* path;

	path = getenv ("XDBD_XSD_DATABASE_PATH");
	if (path != 0)
		g_XdbdXsdDataBasePath = path;
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot get env variable XDBD_XSD_DATABASE_PATH, using default value: %s"), g_XdbdXsdDataBasePath.c_str());

	path = getenv ("XDBD_XML_DATABASE_PATH");
	if (path != 0)
		g_XdbdXmlDataBasePath = path;
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot get env variable XDBD_XML_DATABASE_PATH, using default value: %s"), g_XdbdXmlDataBasePath.c_str());

	path = getenv ("XDBD_DAT_DATABASE_PATH");
	if (path != 0)
		g_XdbdDatDataBasePath = path;
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot get env variable XDBD_DAT_DATABASE_PATH, using default value: %s"), g_XdbdDatDataBasePath.c_str());

	path = getenv ("XDBD_IGNORE_NODEID");
	if (path != 0)
		g_XdbdIgnoreNodeid = (strcasecmp (path, "true") == 0);
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot get env variable XDBD_IGNORE_NODEID, using default value: true"));

	m_sync = semget (IPC_PRIVATE, 1, IPC_CREAT);
	semctl (m_sync, 0, SETVAL, 1);

	m_currentTableInfo = 0;
	m_currentColumnInfo = 0;
	m_ctx = 0;
	m_notify = -1;
	m_notifyHandler = 0;
	m_inputBuffer = 0;
	m_inputPtr = 0;
	m_inputEnd = 0;
	m_enableTriggers = true;
}

XdbdXmlDataBase::~XdbdXmlDataBase ()
{
	_UnloadXmlDataBase ();
}

void XdbdXmlDataBase::lock ()
{
	struct	sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	semop (m_sync, &sb, 1);
}

void XdbdXmlDataBase::unlock ()
{
	struct	sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	semop (m_sync, &sb, 1);
}

int XdbdXmlDataBase::RegChgTracker (char *dir, chgcb f, void *data)
{
	int fd;

	lock ();
	if ((fd = inotify_add_watch (m_notify, dir, IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO)) > 0)
	{
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("register changes for: %s"), dir);
		chgstruct* p_chgstruct = new chgstruct;
		p_chgstruct->m_f = f;
		p_chgstruct->m_data = data;
		chgmap::iterator cit = m_chgmap.find(fd);
		if (cit != m_chgmap.end())
		{
			p_chgstruct->m_next = cit->second;
			cit->second = p_chgstruct;
		}
		else
			(m_chgmap [fd] = p_chgstruct)->m_next = 0;
		unlock ();
		return fd;
	}
	else
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot register changes for: %s, errno = %d"), dir, errno);

	unlock ();
	return -1;
}

int	XdbdXmlDataBase::UnregChgTracker (int wd, void*data)
{
	lock ();
	chgmap::iterator cit = m_chgmap.find(wd);
	if (cit == m_chgmap.end())
		return -1;
	for (chgstruct** p_chgstruct = &(cit->second); *p_chgstruct != 0; )
	{
		if ((*p_chgstruct)->m_data != data)
		{
			p_chgstruct = &((*p_chgstruct)->m_next);
			continue;
		}
		chgstruct* q_chgstruct = *p_chgstruct;
		*p_chgstruct = q_chgstruct->m_next;
		delete q_chgstruct;
		if (cit->second == 0)
		{
			m_chgmap.erase (cit);
			inotify_rm_watch (m_notify, cit->first);
		}
		unlock ();
		return 0;
	}
	unlock ();
	return -1;
}

void XdbdXmlDataBase::HandleSigPipe (XdbdRunningContext *ctx, ctx_sig_t handler, siginfo_t* info)
{
}

void XdbdXmlDataBase::ReadNotifyChanges (XdbdRunningContext *ctx, uint flags, ctx_fddes_t handler, int fd)
{
	if ((flags & EPOLLIN) == 0)
		return;

	int needSpace = 0;
	int usedSpace = m_inputPtr - m_inputBuffer;
	int freeSpace = m_inputEnd - m_inputPtr;

	if (ioctl (fd, FIONREAD, &needSpace) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ioctl() failed, errno = %d"), errno);
		return;
	}

	if (needSpace == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("ioctl() failed, 0 bytes read"));
		return;
	}

	if (freeSpace < needSpace)
	{
		needSpace += usedSpace;
		needSpace >>= 10;
		needSpace++;
		needSpace <<= 10;
		u_char* buffer = (u_char*) malloc (needSpace);
		if (buffer == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("malloc(%d) failed, errno = %d"), needSpace, errno);
			return;
		}
		if (m_inputBuffer != 0)
		{
			memcpy (buffer, m_inputBuffer, usedSpace);
			free (m_inputBuffer);
		}
		m_inputBuffer = buffer;
		m_inputPtr = buffer + usedSpace;
		m_inputEnd = buffer + needSpace;
		freeSpace = needSpace - usedSpace;
	}

	int recvSize;
	if ((recvSize = read (fd, m_inputPtr, freeSpace)) <= 0)
	{
		if (errno == EWOULDBLOCK)
		{
			return;
		}
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("read() failed, errno = %d"), errno);
		return;
	}

	usedSpace += recvSize;

	u_char* ptr = m_inputBuffer;
	while (true)
	{
		if (usedSpace < (int) sizeof(struct inotify_event))
			break;

		struct inotify_event* nevent = (struct inotify_event*) ptr;
		size_t notifySize = offsetof (struct inotify_event, name) + nevent->len;
		ptr += notifySize;
		usedSpace -= notifySize;

#if _DEBUG
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("wd     = %d"), nevent->wd);
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("mask   = %d"), nevent->mask);
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("cookie = %d"), nevent->cookie);
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("len    = %d"), nevent->len);
		xdbdErrReport (SC_XDBD, SC_APL, err_info ("name   = %d"), nevent->name);
#endif

		if ((nevent->mask & (IN_CLOSE_WRITE | IN_MOVED_TO)) == 0)
			continue;

		for (chgstruct* chg = XdbdXmlDataBase::GetChgInfo (nevent->wd); chg != 0; chg = chg->m_next)
			chg->m_f (nevent->wd, chg->m_data, nevent);
	}
}

XdbdXmlTableInfo* XdbdXmlDataBase::_GetTableInfo (string name)
{
	docmap::iterator it = m_docmap.find (name);
	return (it != m_docmap.end ()) ? it->second : 0;
}

int XdbdXmlDataBase::_InitXmlDataBase ()
{
	if ((m_notify = inotify_init ()) < 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("inotify_init() failed, errno = %d"), errno);
		return -1;
	}

	int gflags;
	gflags = fcntl (m_notify, F_GETFL, 0);
	fcntl (m_notify, F_SETFL, gflags | O_NONBLOCK);
	xdbdErrReport (SC_XDBD, SC_APL, err_info ("NOTIFY = %d"), m_notify);
	m_ctx->RegisterSignal(SIGPIPE, HandleSigPipe, this);
	m_notifyHandler = m_ctx->RegisterDescriptor (EPOLLIN, m_notify, ReadNotifyChanges, this, ctx_info);
	return	0;
}

int XdbdXmlDataBase::_LoadXmlDataBase (XdbdRunningContext* ctx)
{
	m_ctx = ctx;
	xmlInitParser ();
	_InitXmlDataBase ();
	_LoadXsdFiles ();
	_LoadXmlFiles ();
	_LoadTriggers ();
	return 0;
}

xmlAttrPtr XdbdXmlDataBase::findAttribute (xmlNodePtr node, const char*path [], const char*name, int index)
{
	if (*path == 0)
	{
		for (xmlAttrPtr prop = node->properties; prop != 0; prop = prop->next)
		{
			if (strcmp (name, (char*) prop->name) != 0)
				continue;
			return prop;
		}
		return 0;
	}
	int i = 0;
	for (xmlNodePtr child = node->children; child != 0; child = child->next)
	{
		xmlAttrPtr attr;
		if (strcmp (*path, (char*) child->name) != 0)
			continue;
		if ((attr = findAttribute (child, path + 1, name, index)) == 0)
			continue;
		if (++i < index)
			continue;
		return attr;
	}
	return 0;
}

int XdbdXmlDataBase::_LoadXsdFiles (char* xsdPath)
{
	if (xsdPath == 0)
		xsdPath = (char*) g_XdbdXsdDataBasePath.c_str ();

	DIR* dir = opendir (xsdPath);
	if (dir == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("opendir (%s) failed, errno = %d"), xsdPath, errno);
		return -1;
	}
	struct dirent dir_entry;
	struct dirent* dir_result = 0;

	XdbdChkXsdDir::XdbdChkContext* ctx = new XdbdChkXsdDir::XdbdChkContext;
	ctx->m_dirname = xsdPath;
	RegChgTracker (xsdPath, HandleXsdDirChanges, ctx);

	while (true)
	{
		memset (&dir_entry, 0, sizeof dir_entry);
		dir_entry.d_reclen = sizeof dir_entry;
		if (readdir_r (dir, &dir_entry, &dir_result) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("readdir (%s) failed, errno = %d"), xsdPath, errno);
			break;
		}
		if (dir_result == 0)
			break;
		if ((dir_entry.d_type & DT_REG) == 0)
			continue;
		char* ptr = strrchr (dir_entry.d_name, '.');
		if (ptr == 0)
			continue;
		if (strcmp (ptr, ".xsd") != 0)
			continue;

		_LoadXsdFile (dir_entry.d_name, xsdPath);
	}
	closedir (dir);

	for (docmap::iterator it = m_docmap.begin (); it != m_docmap.end (); ++it)
		it->second->AlignColPositions ();
	return 0;
}

int XdbdXmlDataBase::_LoadXsdFile (char* xsdFname, char* xsdPath)
{
	if (xsdPath == 0)
		xsdPath = (char*) g_XdbdXsdDataBasePath.c_str ();

	string xsdfname = xsdPath;
	xsdfname += '/';
	xsdfname += xsdFname;

	xmlDocPtr xsdDoc = 0;
	xmlXPathContextPtr xsdCtx = 0;
	xmlXPathObjectPtr obj = 0;

	do
	{
		xsdDoc = xmlReadFile (xsdfname.c_str (), 0, 0);
		if (xsdDoc == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xmlReadFile (%s) failed"), xsdfname.c_str());
			break;
		}

		xsdCtx = xmlXPathNewContext (xsdDoc);
		if (xsdCtx == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xmlXPathNewContext (%s) failed"), xsdfname.c_str());
			break;
		}

		if (xmlXPathRegisterNs (xsdCtx, (const xmlChar*) g_XdbdNsPrefix.c_str (), (const xmlChar*) g_XdbdNsUri.c_str ())
			!= 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xmlXPathRegisterNs (%s, %s, %s) failed"), xsdfname.c_str(), g_XdbdNsPrefix.c_str (), g_XdbdNsUri.c_str ());
			break;
		}

		obj = xmlXPathEvalExpression ((xmlChar*) "//xs:element", xsdCtx);
		if (obj == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xmlXPathEvalExpression (%s, %s) failed"), xsdfname.c_str(), "//xs:element");
			break;
		}

		xmlNodeSetPtr nodes = obj->nodesetval;
		if (nodes == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s: empty set of xs:element(s)"), xsdfname.c_str());
			break;
		}

		int columnIndex = 0;
		int currentDepth = 0;
		int elementDepth = 0;
		int columnPosition = 0;
		for (int i = 0; i < nodes->nodeNr; ++i)
		{
			xmlAttrPtr nameAttr;
			xmlAttrPtr defaultAttr;
			xmlAttrPtr minOccursAttr;
			xmlAttrPtr maxOccursAttr;
			xmlAttrPtr baseAttr;
			xmlAttrPtr minValAttr;
			xmlAttrPtr maxValAttr;
			xmlAttrPtr minInclusiveAttr;
			xmlAttrPtr maxInclusiveAttr;

			xmlNodePtr node = nodes->nodeTab [i];
			if (node == 0)
				continue;

			int depth;
			xmlNodePtr parent;
			for (depth = 0, parent = node->parent; parent != 0; parent = parent->parent, ++depth)
				;
			if (depth > currentDepth)
				++elementDepth, columnPosition = 0;
			else if (depth < currentDepth)
				--elementDepth;
			currentDepth = depth;

			switch (elementDepth)
			{
			case 1:
				//	root name
				break;
			case 2:
				//	table name
				{
					xmlXPathObjectPtr namePtr = 0;
					do
					{
						columnIndex = 0;
						m_currentTableInfo = 0;
						{
							const char* path [] =
								{ 0 };
							nameAttr = findAttribute (node, path, "name", 1);
						}
						if (nameAttr == 0)
							break;

						docmap::iterator dit;
						if ((dit = m_docmap.find ((char*) nameAttr->children->content)) != m_docmap.end ())
						{
							delete dit->second;
							m_docmap.erase (dit);
							xdbdErrReport (SC_XDBD, SC_ERR, err_info ("redefinition of table '%s' in file '%s'"), nameAttr->children->content, xsdfname.c_str());
						}
						else
							m_tabmap.insert (make_pair (xsdFname, (char*) nameAttr->children->content));
						m_docmap [(char*) nameAttr->children->content] = m_currentTableInfo = new XdbdXmlTableInfo (
							(char*) nameAttr->children->content);
						xdbdErrReport (SC_XDBD, SC_APL, "TABLE: %s, FILE: %s", nameAttr->children->content, xsdfname.c_str());
					}
					while (false);
					if (namePtr != 0)
						xmlXPathFreeObject (namePtr);
				}
				break;
			case 3:
				//	column name
				{
					do
					{
						m_currentColumnInfo = 0;
						//	@name attribute must be present
						{
							const char* path [] =
								{ 0 };
							nameAttr = findAttribute (node, path, "name", 1);
						}
						if (nameAttr == 0)
							break;

						//	@minOccurs attribute need not be present
						{
							const char* path [] =
								{ 0 };
							minOccursAttr = findAttribute (node, path, "minOccurs", 1);
						}

						//	@default attribute need not be present
						{
							const char* path [] =
								{ 0 };
							defaultAttr = findAttribute (node, path, "default", 1);
						}

						//	@maxOccurs attribute need not be present
						{
							const char* path [] =
								{ 0 };
							maxOccursAttr = findAttribute (node, path, "maxOccurs", 1);
						}

						//	@base attribute must be present
						{
							const char* path [] =
								{ "simpleType", "restriction", 0 };
							baseAttr = findAttribute (node, path, "base", 1);
						}
						if (baseAttr == 0)
							break;

						char* minlValue = 0;
						//	minLength need not be present
						{
							const char* path [] =
								{ "simpleType", "restriction", "minLength", 0 };
							minValAttr = findAttribute (node, path, "value", 1);
						}
						if (minValAttr != 0)
							minlValue = (char*) minValAttr->children->content;

						char* maxlValue = 0;
						//	maxLength need not be present
						{
							const char* path [] =
								{ "simpleType", "restriction", "maxLength", 0 };
							maxValAttr = findAttribute (node, path, "value", 1);
						}
						if (maxValAttr != 0)
							maxlValue = (char*) maxValAttr->children->content;

						char* minlInclusive = 0;
						//	minLength need not be present
						{
							const char* path [] =
								{ "simpleType", "restriction", "minInclusive", 0 };
							minInclusiveAttr = findAttribute (node, path, "value", 1);
						}
						if (minInclusiveAttr != 0)
							minlInclusive = (char*) minInclusiveAttr->children->content;

						char* maxInclusive = 0;
						//	maxLength need not be present
						{
							const char* path [] =
								{ "simpleType", "restriction", "maxInclusive", 0 };
							maxInclusiveAttr = findAttribute (node, path, "value", 1);
						}
						if (maxInclusiveAttr != 0)
							maxInclusive = (char*) maxInclusiveAttr->children->content;

						m_currentColumnInfo = new XdbdXmlColumnInfo ((char*) nameAttr->children->content,
							(char*) baseAttr->children->content,
							(defaultAttr != 0) ? (char*) defaultAttr->children->content : 0,
							(minOccursAttr != 0) ? atoi ((char*) minOccursAttr->children->content) : 1,
							(maxOccursAttr != 0) ? atoi ((char*) maxOccursAttr->children->content) : 1, columnPosition,
							(minlValue != 0) ? atoi (minlValue) : -1, (maxlValue != 0) ? atoi (maxlValue) : -1,
							(minlInclusive != 0) ? atoi (minlInclusive) : INT_MIN, (maxInclusive != 0) ? atoi (maxInclusive) : INT_MAX,
							columnIndex++);
						columnPosition += m_currentColumnInfo->realLength ();
						if (m_currentTableInfo != 0)
							m_currentTableInfo->add (m_currentColumnInfo);
					}
					while (false);
				}
				break;
			}
		}

		obj = xmlXPathEvalExpression ((xmlChar*) "//xs:key", xsdCtx);
		if (obj == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s: cannot locate xs:key(s)"), xsdfname.c_str());
			break;
		}

		nodes = obj->nodesetval;
		if (nodes == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s: empty set of xs:key(s)"), xsdfname.c_str());
			break;
		}

		for (int i = 0; i < nodes->nodeNr; ++i)
		{
			xmlAttrPtr xpathAttr;

			xmlNodePtr node = nodes->nodeTab [i];
			if (node == 0)
				continue;

			{
				const char* path [] =
					{ "selector", 0 };
				xpathAttr = findAttribute (node, path, "xpath", 1);
			}
			if (xpathAttr == 0)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s, node %s: missing attribute 'selector'"), xsdfname.c_str(), node->name);
				continue;
			}

			char* ptr;
			for (ptr = (char*) xpathAttr->children->content; (*ptr != 0) && (isalpha (*ptr) == 0); ++ptr)
				;

			docmap::iterator dit = m_docmap.find (ptr);
			if (dit == m_docmap.end ())
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s, table %s not registered"), xsdfname.c_str(), ptr);
				continue;
			}

			XdbdXmlTableInfo* tabinfo = dit->second;
			for (int j = 0;; ++j)
			{
				{
					const char* path [] =
						{ "field", 0 };
					xpathAttr = findAttribute (node, path, "xpath", j + 1);
				}
				if (xpathAttr == 0)
					break;

				XdbdXmlColumnInfo* colinfo = tabinfo->find ((char*) xpathAttr->children->content);
				if (colinfo == 0)
				{
					xdbdErrReport (SC_XDBD, SC_ERR, err_info ("%s, column %s not registered"), xsdfname.c_str(), xpathAttr->children->content);
					continue;
				}

				colinfo->primary (true);
			}
		}
	}
	while (false);	// only one iteration of do loop

	if (xsdDoc != 0)
		xmlFreeDoc (xsdDoc);
	if (xsdCtx != 0)
		xmlXPathFreeContext (xsdCtx);
	if (obj != 0)
		xmlXPathFreeObject (obj);
	return 0;
}

int XdbdXmlDataBase::_LoadXmlFiles (char* xmlPath)
{
	if (xmlPath == 0)
		xmlPath = (char*) g_XdbdXmlDataBasePath.c_str ();

	DIR*	dir = opendir (xmlPath);
	if (dir == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("opendir (%s) failed, errno = %d"), xmlPath, errno);
		return -1;
	}
	struct	dirent	dir_entry;
	struct	dirent*	dir_result = 0;

	XdbdChkXsdDir::XdbdChkContext* ctx = new XdbdChkXsdDir::XdbdChkContext;
	ctx->m_dirname = xmlPath;
	RegChgTracker (xmlPath, HandleXmlDirChanges, ctx);

	while (true)
	{
		memset (&dir_entry, 0, sizeof dir_entry);
		dir_entry.d_reclen = sizeof dir_entry;
		if (readdir_r (dir, &dir_entry, &dir_result) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("readdir (%s) failed, errno = %d"), xmlPath, errno);
			break;
		}
		if (dir_result == 0)
			break;
		if ((dir_entry.d_type & DT_REG) == 0)
			continue;
		char*	ptr = strrchr (dir_entry.d_name, '.');
		if (ptr == 0)
			continue;
		if (strcmp (ptr, ".xml") != 0)
			continue;

		_LoadXmlFile (dir_entry.d_name, xmlPath);
	}
	closedir (dir);

	return	0;
}

int XdbdXmlDataBase::_LoadXmlFile (char* xmlFname, char* xmlPath)
{
	if (xmlPath == 0)
		xmlPath = (char*) g_XdbdXmlDataBasePath.c_str ();

	string xmlfname = xmlPath;
	xmlfname += '/';
	xmlfname += xmlFname;

	xmlDocPtr xmlDoc = 0;
	xmlXPathContextPtr xmlCtx = 0;

#if defined (XDBD_PERFTEST)
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: loading xml file '%s/%s'"), xmlPath, xmlFname);

	struct timeval	tbegin, tend;

	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST
	xmlDoc = xmlReadFile (xmlfname.c_str (), 0, 0);
#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
#endif	// XDBD_PERFTEST
	if (xmlDoc == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot parse file '%s': xmlReadFile () failed"), xmlFname);
		return	-1;
	}
#if defined (XDBD_PERFTEST)
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: xml dom created for file '%s/%s', time = %d.%06d"), xmlPath, xmlFname, tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST

	xmlCtx = xmlXPathNewContext (xmlDoc);
	if (xmlCtx == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xml context for file '%s': xmlXPathNewContext () failed"), xmlFname);
		return	-1;
	}

	if (xmlXPathRegisterNs (xmlCtx, (const xmlChar*) g_XdbdNsPrefix.c_str (), (const xmlChar*) g_XdbdNsUri.c_str ())
		!= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot set namespace for file '%s': xmlXPathRegisterNs () failed"), xmlFname);
		return	-1;
	}

	xmlNode*	children;
	xmlAttrPtr attrs;
	for (children = xmlDoc->children; children != 0; children = children->next)
	{
		for (attrs = children->properties; attrs != 0; attrs = attrs->next)
		{
			if (strcmp ((char*) attrs->name, "noNamespaceSchemaLocation") == 0)
				break;
		}
		if (attrs != 0)
			break;
	}

	if (children == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("no root element, file '%s': xmlXPathRegisterNs () failed"), xmlFname);
		return	-1;
	}

	if (attrs == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing root element attributes, file '%s'"), xmlFname);
		return	-1;
	}

	xmlNodePtr value = attrs->children;
	if (value == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing root element attributes, file '%s'"), xmlFname);
		return	-1;
	}

#if defined (XDBD_PERFTEST)
	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST
	tabrng rng = m_tabmap.equal_range ((char*) value->content);
	for (tabmap::iterator it = rng.first; it != rng.second; ++it)
	{
		docmap::iterator dit = m_docmap.find (it->second);
		if (dit == m_docmap.end ())
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("undeclared table name '%s', file '%s'"), it->second.c_str(), it->first.c_str());
			continue;
		}
		if (dit->second == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("undeclared table '%s', file '%s'"), it->second.c_str(), it->first.c_str());
			continue;
		}

		dit->second->IndexXmlTable (xmlCtx, it->second);
	}
#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: db loaded for file '%s/%s', time = %d.%06d"), xmlPath, xmlFname, tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST

	if (xmlDoc != 0)
		xmlFreeDoc (xmlDoc);
	if (xmlCtx != 0)
		xmlXPathFreeContext (xmlCtx);
	return 0;
}

int XdbdXmlDataBase::_LoadStatFiles (char* statPath)
{
	StatisticalAdapterThread::Init();

	salist p_salist = StatisticalAdapterThread::get_salist();
	for (salist::iterator sit = p_salist.begin(); sit != p_salist.end(); ++sit)
	{
		StatisticalAdapter* p_adapter = sit->second;
		char* statPath = (char*) p_adapter->statDirName();
		p_adapter->notifyFD(RegChgTracker (statPath, HandleStatDirChanges, p_adapter));
	}
	return 0;
}

int	XdbdXmlDataBase::_RestoreXdbFiles ()
{
	for (docmap::iterator it = m_docmap.begin(); it != m_docmap.end(); ++it)
		it->second->restoreShmData();
	return	0;
}

int	XdbdXmlDataBase::_ReloadXmlFiles (char* xmlPath)
{
	if (xmlPath == 0)
		xmlPath = (char*) g_XdbdXmlDataBasePath.c_str ();

	DIR*	dir = opendir (xmlPath);
	if (dir == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot open dir '%s', errno = %d"), xmlPath, errno);
		return -1;
	}
	struct	dirent	dir_entry;
	struct	dirent*	dir_result = 0;

	XdbdChkXsdDir::XdbdChkContext* ctx = new XdbdChkXsdDir::XdbdChkContext;
	ctx->m_dirname = xmlPath;
	RegChgTracker (xmlPath, HandleXmlDirChanges, ctx);

	while (true)
	{
		memset (&dir_entry, 0, sizeof dir_entry);
		dir_entry.d_reclen = sizeof dir_entry;
		if (readdir_r (dir, &dir_entry, &dir_result) < 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot read dir '%s', errno = %d"), xmlPath, errno);
			break;
		}
		if (dir_result == 0)
			break;
		if ((dir_entry.d_type & DT_REG) == 0)
			continue;
		char*	ptr = strrchr (dir_entry.d_name, '.');
		if (ptr == 0)
			continue;
		if (strcmp (ptr, ".xml") != 0)
			continue;

		_ReloadXmlFile (dir_entry.d_name, xmlPath);
	}
	closedir (dir);

	return	0;
}

int XdbdXmlDataBase::_ReloadXmlFile (char* xmlFname, char* xmlPath)
{
	char* ptr = strrchr (xmlFname, '.');
	if (ptr == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("file name with .xml suffix expected '%s/%s'"), xmlPath, xmlFname, errno);
		return -1;
	}
	if (strcmp (ptr, ".xml") != 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("file name with .xml suffix expected '%s/%s'"), xmlPath, xmlFname, errno);
		return -1;
	}

	if (xmlPath == 0)
		xmlPath = (char*) g_XdbdXmlDataBasePath.c_str ();

	string xmlfname = xmlPath;
	xmlfname += '/';
	xmlfname += xmlFname;

	xmlDocPtr xmlDoc = 0;
	xmlXPathContextPtr xmlCtx = 0;

#if defined (XDBD_PERFTEST)
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: loading xml file '%s/%s'"), xmlPath, xmlFname);

	struct timeval	tbegin, tend;

	gettimeofday (&tbegin, 0);
#endif	// XDBD_PERFTEST

	xmlDoc = xmlReadFile (xmlfname.c_str (), 0, 0);
	if (xmlDoc == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot parse file, xmlReadFile (%s/%s) failed"), xmlPath, xmlFname);
		return	-1;
	}

#if defined (XDBD_PERFTEST)
	gettimeofday (&tend, 0);
	tend.tv_sec -= tbegin.tv_sec;
	if ((tend.tv_usec -= tbegin.tv_usec) < 0)
	{
		tend.tv_usec += 1000 * 1000;
		tend.tv_sec--;
	}
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("PERFTEST: reloading xml dom for file '%s/%s', time = %d.%06d"), xmlPath, xmlFname, tend.tv_sec, tend.tv_usec);
#endif	// XDBD_PERFTEST

	xmlCtx = xmlXPathNewContext (xmlDoc);
	if (xmlCtx == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot create xml context for '%s/%s', xmlXPathNewContext () failed"), xmlPath, xmlFname);
		return	-1;
	}

	if (xmlXPathRegisterNs (xmlCtx, (const xmlChar*) g_XdbdNsPrefix.c_str (), (const xmlChar*) g_XdbdNsUri.c_str ())
		!= 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot register xml namespace for '%s/%s', xmlXPathRegisterNs () failed"), xmlPath, xmlFname);
		return	-1;
	}

	xmlNode*	children;
	xmlAttrPtr attrs;
	for (children = xmlDoc->children; children != 0; children = children->next)
	{
		for (attrs = children->properties; attrs != 0; attrs = attrs->next)
		{
			if (strcmp ((char*) attrs->name, "noNamespaceSchemaLocation") == 0)
				break;
		}
		if (attrs != 0)
			break;
	}

	if (children == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xml file without root element: '%s/%s'"), xmlPath, xmlFname);
		return	-1;
	}

	if (attrs == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xml file without root element attributes: '%s/%s'"), xmlPath, xmlFname);
		return	-1;
	}

	xmlNodePtr value = attrs->children;
	if (value == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("xml file without root element attributes: '%s/%s'"), xmlPath, xmlFname);
		return	-1;
	}

	tabrng rng = m_tabmap.equal_range ((char*) value->content);
	for (tabmap::iterator it = rng.first; it != rng.second; ++it)
	{
		docmap::iterator dit = m_docmap.find (it->second);
		if (dit == m_docmap.end ())
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("undeclared table name: '%s', file: '%s/%s'"), it->second.c_str(), xmlPath, xmlFname);
			continue;
		}
		if (dit->second == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("undeclared table: '%s', file: '%s/%s'"), it->second.c_str(), xmlPath, xmlFname);
			continue;
		}

#ifdef	_DEBUG
		xdbdErrReport (SC_XDBD, SC_APP, err_info ("reload: '%s/%s'"), xmlPath, xmlFname);
#endif

		dit->second->ReloadXmlTable (xmlCtx, it->second);
	}

	_ExecuteTriggers();

	if (xmlDoc != 0)
		xmlFreeDoc (xmlDoc);
	if (xmlCtx != 0)
		xmlXPathFreeContext (xmlCtx);
	return 0;
}

int XdbdXmlDataBase::_ReloadStatFile (void* p_adapter, char* statFname)
{
	xdbdErrReport (SC_XDBD, SC_ERR, err_info ("STAT FILE: %s/%s"), ((StatisticalAdapter*) p_adapter)->statDirName(), statFname);
	((StatisticalAdapter*) p_adapter)->DoCollect (statFname);
	return 0;
}

void	XdbdXmlDataBase::_ExecuteTriggers ()
{
	while (!m_trgJobList.empty())
	{
		XdbdBaseJob*	job = m_trgJobList.front();
		XdbdMessageQueue::Put (job);
		m_trgJobList.pop_front();
	}
}

int XdbdXmlDataBase::_LoadTriggers ()
{
	short stat_id = -1;

	do
	{
		// check existence of xdbd_trigger_par table
		docmap::iterator dit = m_docmap.find ("xdbd_trigger_par");
		if (dit == m_docmap.end ())
		{
			xdbdErrReport (SC_XDBD, SC_WRN, err_info ("no trigger parameters registered, table xdbd-trigger_par does not exist"));
			break;
		}

		XdbdXmlTableInfo* parameters = dit->second;
		if (parameters == 0)
		{
			xdbdErrReport (SC_XDBD, SC_WRN, err_info ("no trigger parameters registered, table xdbd-trigger_par does not exist"));
			break;
		}

		// check structure of xdbd_trigger_par table
		XdbdXmlColumnInfo* idinfo = parameters->find (g_trigger_par_id);
		if (idinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger_par column trigger_par_id"));
			break;
		}

		XdbdXmlColumnInfo* trigidinfo = parameters->find (g_trigger_par_rid);
		if (trigidinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger_par column trigger_id"));
			break;
		}

		XdbdXmlColumnInfo* nameinfo = parameters->find (g_trigger_par_name);
		if (nameinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger_par column trigger_par_name"));
			break;
		}

		XdbdXmlColumnInfo* valueinfo = parameters->find (g_trigger_par_value);
		if (valueinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger_par column trigger_par_value"));
			break;
		}

		// iterate through id column to load trigger parameters
		for (XdbdXmlColumnInfo::colidx idx = idinfo->begin(); idx != idinfo->end(); ++idx)
		{
			unsigned char*	rowaddr = idx->second;
			short id = *((short*) idinfo->recordToColumn (rowaddr));
			if (chkcolflag (rowaddr, 2, trigidinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger parameter without trigger id"));
				continue;
			}
			short trigid = *((short*) trigidinfo->recordToColumn (rowaddr));
			if (chkcolflag (rowaddr, 2, nameinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger parameter without name"));
				continue;
			}
			unsigned char* name = nameinfo->recordToColumn (rowaddr);
			if (chkcolflag (rowaddr, 2, valueinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger parameter without value"));
				continue;
			}
			unsigned char* value = valueinfo->recordToColumn (rowaddr);

			XdbdTriggerParameter* par = new XdbdTriggerParameter (id, trigid, name, value);
			parset::iterator pit = m_parset.find (id);
			if (pit != m_parset.end ())
			{
				delete par;
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("duplicated trigger parameter"));
				continue;
			}

			m_parset [id] = par;
		}
	}
	while (false);

	do
	{
		// check existence of xdbd_trigger table
		docmap::iterator dit = m_docmap.find ("xdbd_trigger");
		if (dit == m_docmap.end ())
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("no triggers registered, table xdbd_trigger does not exist"));
			break;
		}

		XdbdXmlTableInfo* triggers = dit->second;
		if (triggers == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("no triggers registered, table xdbd_trigger does not exist"));
			break;
		}

		// check structure of xdbd_trigger table
		XdbdXmlColumnInfo* idinfo = triggers->find (g_trigger_id);
		if (idinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_id"));
			break;
		}

		XdbdXmlColumnInfo* nameinfo = triggers->find (g_trigger_name);
		if (nameinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_name"));
			break;
		}

		XdbdXmlColumnInfo* libinfo = triggers->find (g_trigger_library);
		if (libinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_library"));
			break;
		}

		XdbdXmlColumnInfo* loadinfo = triggers->find (g_trigger_load);
		if (loadinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_load"));
			break;
		}

		XdbdXmlColumnInfo* fireinfo = triggers->find (g_trigger_fire);
		if (fireinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_fire"));
			break;
		}

		XdbdXmlColumnInfo* unloadinfo = triggers->find (g_trigger_unload);
		if (unloadinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_unload"));
			break;
		}

		XdbdXmlColumnInfo* tableinfo = triggers->find (g_trigger_table);
		if (tableinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_table"));
			break;
		}

		XdbdXmlColumnInfo* funcinfo = triggers->find (g_trigger_function);
		if (funcinfo == 0)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("missing xdbd_trigger column trigger_function"));
			break;
		}

		// iterate through id column to load triggers
		for (XdbdXmlColumnInfo::colidx idx = idinfo->begin(); idx != idinfo->end(); ++idx)
		{
			unsigned char*	rowaddr = idx->second;
			short id = *((short*) idinfo->recordToColumn (rowaddr));
			if (chkcolflag (rowaddr, 2, nameinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger without name"));
				continue;
			}
			unsigned char* name = nameinfo->recordToColumn (rowaddr);
			if (chkcolflag (rowaddr, 2, libinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger '%s' without library"), name);
				continue;
			}
			unsigned char* library = libinfo->recordToColumn (rowaddr);
			unsigned char* load = (unsigned char*) "Load";
			if (chkcolflag (rowaddr, 2, loadinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_WRN, err_info ("trigger '%s' with null load function"), name);
			}
			else
				load = loadinfo->recordToColumn (rowaddr);
			unsigned char* fire = (unsigned char*) "Fire";
			if (chkcolflag (rowaddr, 2, fireinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_WRN, err_info ("trigger '%s' with null fire function"), name);
			}
			else
				fire = fireinfo->recordToColumn (rowaddr);
			unsigned char* unload = (unsigned char*) "Unload";
			if (chkcolflag (rowaddr, 2, unloadinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_WRN, err_info ("trigger '%s' with null unload function"), name);
			}
			else
				unload = unloadinfo->recordToColumn (rowaddr);
			if (chkcolflag (rowaddr, 2, tableinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger '%s' without table"), name);
				continue;
			}
			unsigned char* table = tableinfo->recordToColumn (rowaddr);
			if (chkcolflag (rowaddr, 2, funcinfo->index(), COLNULLVALUE))
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger '%s' without function selector"), name);
				continue;
			}
			short function = *((short*) funcinfo->recordToColumn (rowaddr));

			if ((dit = m_docmap.find ((char*) table)) == m_docmap.end ())
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger '%s' with unknown table '%s'"), name, table);
				continue;
			}

			XdbdTrigger* trigger = new XdbdTrigger (id, name, library, load, fire, unload, table, function);

			for (parset::iterator pit = m_parset.begin (); pit != m_parset.end (); ++pit)
			{
				XdbdTriggerParameter* par = pit->second;
				if (par->trigid () != id)
					continue;
				trigger->add (par);
			}

			if (!trigger->activate ())
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot activate trigger '%s'"), name);
				delete trigger;
				continue;
			}

			if (id > stat_id)
				stat_id = id + 1;
			m_trigset [id] = trigger;
			dit->second->add_trigger (trigger);
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger (%s, %s, %s, %s, %s, %s, %d) created"), name, library, load, fire, unload, table, function);
		}

		{
			if (stat_id <= 0)
				stat_id = 1;

			unsigned char* name = (unsigned char*) "stat_adapter_trigger";
			unsigned char* library = (unsigned char*) "libxdbdsqlparser.so";
			unsigned char* load = (unsigned char*) "nmFunc_stat_adapter_load";
			unsigned char* fire = (unsigned char*) "nmFunc_stat_adapter_fire";
			unsigned char* unload = (unsigned char*) "nmFunc_stat_adapter_unload";
			unsigned char* table = (unsigned char*) "stat_adapter";
			short function = XdbdTrigger::g_insertFunction|XdbdTrigger::g_updateFunction|XdbdTrigger::g_deleteFunction;

			if ((dit = m_docmap.find ((char*) table)) == m_docmap.end ())
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger '%s' with unknown table '%s'"), name, table);
				continue;
			}

			XdbdTrigger* trigger = new XdbdTrigger (stat_id, name, library, load, fire, unload, table, function);

			if (!trigger->activate ())
			{
				xdbdErrReport (SC_XDBD, SC_ERR, err_info ("cannot activate trigger '%s'"), name);
				delete trigger;
				continue;
			}

			m_trigset [stat_id] = trigger;
			dit->second->add_trigger (trigger);
			xdbdErrReport (SC_XDBD, SC_ERR, err_info ("trigger (%s, %s, %s, %s, %s, %s, %d) created"), name, library, load, fire, unload, table, function);
		}
	}
	while (false);

	return 0;
}

int XdbdXmlDataBase::_UnloadXmlDataBase ()
{
	for (docmap::iterator it = m_docmap.begin (); it != m_docmap.end (); ++it)
		delete it->second;
	m_docmap.clear ();
	for (trigset::iterator it = m_trigset.begin (); it != m_trigset.end (); ++it)
		delete it->second;
	m_trigset.clear ();
	for (parset::iterator it = m_parset.begin (); it != m_parset.end (); ++it)
		delete it->second;
	m_parset.clear ();
	if (m_notifyHandler != 0)
		m_ctx->RemoveDescriptor (m_notifyHandler);
	m_notifyHandler = 0;
	semctl (m_sync, 0, IPC_RMID, 0);
	return	0;
}

XdbdXmlColumnInfo::colrng XdbdXmlDataBase::_PrepareComparison (tablist& tables,
	sql_comp_expression* p_sql_comp_expression, XdbdXmlTableInfo* &tabinfo, XdbdXmlColumnInfo* &colinfo,
	unsigned char* &data, ssize_t &size, SqlSyntaxContext &sqlSyntaxContext, bool& invert)
{
	sql_primary_expression* p_sql_primary_expression_1 = 0;
	sql_primary_expression* p_sql_primary_expression_2 = 0;

	tabinfo = 0;
	colinfo = 0;
	data = 0;
	size = 0;
	sqlSyntaxContext = SqlFirstSyntaxContext;
	invert = false;

	switch (p_sql_comp_expression->kind ())
	{
	case sql_comp_expression::g_sql_comp_expression_1:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_eq_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_eq_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_eq_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_2:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_ne_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_ne_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_ne_expression ()->context ();
		invert = true;
		break;
	case sql_comp_expression::g_sql_comp_expression_3:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_lt_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_lt_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_lt_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_4:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_le_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_le_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_le_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_5:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_gt_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_gt_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_gt_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_6:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_ge_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_ge_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_ge_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_7:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_like_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_like_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_like_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_9:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_is_eq_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_is_eq_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_is_eq_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_10:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_is_ne_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_is_ne_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_is_ne_expression ()->context ();
		invert = true;
		break;
	}

	if (p_sql_primary_expression_1->kind () != sql_primary_expression::g_sql_primary_expression_1)
		return XdbdXmlColumnInfo::colrng ();
	if (p_sql_primary_expression_2->kind () != sql_primary_expression::g_sql_primary_expression_2)
		return XdbdXmlColumnInfo::colrng ();

	sql_column* p_sql_column = p_sql_primary_expression_1->get_sql_column ();
	sql_column_name* p_sql_column_name = p_sql_column->get_sql_column_name ();
	string colname = p_sql_column_name->get_char ();
	sql_table* p_sql_table = p_sql_column->get_sql_table ();
	docmap::iterator dit = m_docmap.end ();

	if (p_sql_table != 0)
	{
		dit = m_docmap.find (p_sql_table->get_char ());
		if (dit != m_docmap.end ())
		{
			tabinfo = dit->second;
			colinfo = tabinfo->find (colname);
		}
	}
	else
	{
		for (tablist::iterator it = tables.begin (); it != tables.end (); ++it)
		{
			tabinfo = *it;
			colinfo = tabinfo->find (colname);
			if (colinfo != 0)
				break;
		}
	}
	if (colinfo == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("unknown column name '%s'"), colname.c_str());
		return XdbdXmlColumnInfo::colrng ();
	}

	size = colinfo->maxLength ();

	sql_constant* p_sql_constant = p_sql_primary_expression_2->get_sql_constant ();

	data = new unsigned char [tabinfo->resSize()];
	memset (data, 0, tabinfo->resSize());
	switch (p_sql_constant->kind ())
	{
	case	sql_constant::g_sql_constant_3:	// NULL value
		setcolflag((data), 2, colinfo->index(), COLNULLVALUE);
		break;
	case	sql_constant::g_sql_constant_4:	// ? value
		setcolflag(data, 2, colinfo->index(), COLPARVALUE);
		size = -((ssize_t) p_sql_constant->kind ());
		break;
	default:	// numbers and strings
		{
			string dataStr = p_sql_constant->get_char ();
			unsigned char*	srcData = (unsigned char*) dataStr.c_str ();
			unsigned char*	dstData = data + colinfo->position ();
			switch (colinfo->colType ())
			{
			case SQL_SMALLINT:
				*((SQLSMALLINT*) dstData) = (SQLSMALLINT) atoi ((char*) srcData);
				break;
			case SQL_INTEGER:
				if (sizeof(int) != sizeof(long))
				{
					switch (size)
					{
					case sizeof(int):
						*((SQLINTEGER*) dstData) = (SQLINTEGER) atoi ((char*) srcData);
						break;
					case sizeof(long):
						*((SQLLEN*) dstData) = (SQLLEN) atoi ((char*) srcData);
						break;
					default:
						break;
					}
				}
				else
					*((SQLINTEGER*) dstData) = (SQLINTEGER) atoi ((char*) srcData);
				break;
			case SQL_VARCHAR:
				{
					memcpy (dstData, srcData, size);
				}
				break;
			default:
				return XdbdXmlColumnInfo::colrng ();
			}
		}
		break;
	}
	return XdbdXmlColumnInfo::colrng ();
}

XdbdXmlColumnInfo::colrng XdbdXmlDataBase::_ExecComparison (XdbdXmlColumnInfo* colinfo, unsigned char* data,
	unsigned char* srcData, ssize_t size, SqlSyntaxContext sqlSyntaxContext, bool binary)
{
	XdbdXmlColumnInfo::colrng rng;
	if (colinfo->isTrue())
	{
		rng.first = colinfo->begin();
		rng.second = colinfo->end();
		return	rng;
	}

	if ((srcData != 0) && (size != SQL_NULL_DATA))
		switch (colinfo->colType ())
		{
		case SQL_SMALLINT:
			*((SQLSMALLINT*) data) = binary ? *((SQLSMALLINT*) srcData) : (SQLSMALLINT) atoi ((char*) srcData);
			break;
		case SQL_INTEGER:
			if (sizeof(int) != sizeof(long))
			{
				switch (size)
				{
				case sizeof(int):
					*((SQLINTEGER*) data) = binary ? *((SQLINTEGER*) srcData) : (SQLINTEGER) atoi ((char*) srcData);
					break;
				case sizeof(long):
					*((SQLLEN*) data) = binary ? *((SQLLEN*) srcData) : (SQLLEN) atoi ((char*) srcData);
					break;
				default:
					break;
				}
			}
			else
				*((SQLINTEGER*) data) = binary ? *((SQLINTEGER*) srcData) : (SQLINTEGER) atoi ((char*) srcData);
			break;
		case SQL_VARCHAR:
			{
				memcpy (data, srcData, size);
			}
			break;
		default:
			return XdbdXmlColumnInfo::colrng ();
		}
	else
		setcolflag(colinfo->columnToRecord(data), 2, colinfo->index(), COLNULLVALUE);

	switch (sqlSyntaxContext)
	{
	case sql_eq_expression_Context:
		{
			rng = colinfo->eq_range (data);
		}
		break;
	case sql_ne_expression_Context:
		{
			rng = colinfo->eq_range (data);
		}
		break;
	case sql_lt_expression_Context:
		{
			rng = colinfo->lt_range (data);
		}
		break;
	case sql_le_expression_Context:
		{
			rng = colinfo->le_range (data);
		}
		break;
	case sql_gt_expression_Context:
		{
			rng = colinfo->gt_range (data);
		}
		break;
	case sql_ge_expression_Context:
		{
			rng = colinfo->ge_range (data);
		}
		break;
	case sql_like_expression_Context:
		{
			rng = colinfo->like_range (data);
		}
		break;
	case sql_is_eq_expression_Context:
		{
			rng = colinfo->eq_range (data);
		}
		break;
	case sql_is_ne_expression_Context:
		{
			rng = colinfo->eq_range (data);
		}
		break;
	default:
		{
			rng = colinfo->no_range (data);
		}
		break;
	}
	return rng;
}

XdbdXmlColumnInfo::colrng XdbdXmlDataBase::_PrepareComparison (XdbdXmlTableInfo* tabinfo,
	sql_comp_expression* p_sql_comp_expression, XdbdXmlColumnInfo* &colinfo, unsigned char* &data, ssize_t &size,
	SqlSyntaxContext &sqlSyntaxContext, bool& invert)
{
	sql_primary_expression* p_sql_primary_expression_1 = 0;
	sql_primary_expression* p_sql_primary_expression_2 = 0;

	colinfo = 0;
	data = 0;
	size = 0;
	invert = false;

	switch (p_sql_comp_expression->kind ())
	{
	case sql_comp_expression::g_sql_comp_expression_1:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_eq_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_eq_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_eq_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_2:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_ne_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_ne_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_ne_expression ()->context ();
		invert = true;
		break;
	case sql_comp_expression::g_sql_comp_expression_3:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_lt_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_lt_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_lt_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_4:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_le_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_le_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_le_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_5:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_gt_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_gt_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_gt_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_6:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_ge_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_ge_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_ge_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_7:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_like_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_like_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_like_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_9:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_is_eq_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_is_eq_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_is_eq_expression ()->context ();
		break;
	case sql_comp_expression::g_sql_comp_expression_10:
		p_sql_primary_expression_1 = p_sql_comp_expression->get_sql_is_ne_expression ()->get_sql_primary_expression_1 ();
		p_sql_primary_expression_2 = p_sql_comp_expression->get_sql_is_ne_expression ()->get_sql_primary_expression_2 ();
		sqlSyntaxContext = p_sql_comp_expression->get_sql_is_ne_expression ()->context ();
		invert = true;
		break;
	}

	if (p_sql_primary_expression_1->kind () != sql_primary_expression::g_sql_primary_expression_1)
		return XdbdXmlColumnInfo::colrng ();
	if (p_sql_primary_expression_2->kind () != sql_primary_expression::g_sql_primary_expression_2)
		return XdbdXmlColumnInfo::colrng ();

	sql_column* p_sql_column = p_sql_primary_expression_1->get_sql_column ();
	sql_column_name* p_sql_column_name = p_sql_column->get_sql_column_name ();
	string colname = p_sql_column_name->get_char ();
	sql_table* p_sql_table = p_sql_column->get_sql_table ();
	docmap::iterator dit = m_docmap.end ();

	if (p_sql_table != 0)
	{
		dit = m_docmap.find (p_sql_table->get_char ());
		if (dit != m_docmap.end ())
		{
			if (dit->second->name () != tabinfo->name ())
				return XdbdXmlColumnInfo::colrng ();
		}
	}
	colinfo = tabinfo->find (colname);
	if (colinfo == 0)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, err_info ("unknown column name '%s'"), colname.c_str());
		return XdbdXmlColumnInfo::colrng ();
	}

	size = colinfo->maxLength ();

	sql_constant* p_sql_constant = p_sql_primary_expression_2->get_sql_constant ();

	data = new unsigned char [tabinfo->resSize()];
	memset (data, 0, tabinfo->resSize());
	switch (p_sql_constant->kind ())
	{
	case	sql_constant::g_sql_constant_3:	// NULL value
		setcolflag((data), 2, colinfo->index(), COLNULLVALUE);
		break;
	case	sql_constant::g_sql_constant_4:	// ? value
		setcolflag(data, 2, colinfo->index(), COLPARVALUE);
		size = -((ssize_t) p_sql_constant->kind ());
		break;
	default:	// numbers and strings
		{
			string dataStr = p_sql_constant->get_char ();
			unsigned char*	srcData = (unsigned char*) dataStr.c_str ();
			unsigned char*	dstData = data + colinfo->position ();
			switch (colinfo->colType ())
			{
			case SQL_SMALLINT:
				*((SQLSMALLINT*) dstData) = (SQLSMALLINT) atoi ((char*) srcData);
				break;
			case SQL_INTEGER:
				if (sizeof(int) != sizeof(long))
				{
					switch (size)
					{
					case sizeof(int):
						*((SQLINTEGER*) dstData) = (SQLINTEGER) atoi ((char*) srcData);
						break;
					case sizeof(long):
						*((SQLLEN*) dstData) = (SQLLEN) atoi ((char*) srcData);
						break;
					default:
						break;
					}
				}
				else
					*((SQLINTEGER*) dstData) = (SQLINTEGER) atoi ((char*) srcData);
				break;
			case SQL_VARCHAR:
				{
					memcpy (dstData, srcData, size);
				}
				break;
			default:
				return XdbdXmlColumnInfo::colrng ();
			}
		}
		break;
	}
	return XdbdXmlColumnInfo::colrng ();
}

int XdbdXmlDataBase::equal (SQLVARCHAR* newLine, SQLVARCHAR* oldLine, SQLVARCHAR* mask, size_t lineSize)
{
	for (; (lineSize > 0) && ((*mask == 0) || (((*newLine ^ *oldLine) & *mask) == 0));
		++newLine, ++oldLine, ++mask, --lineSize)
		;
	return lineSize;
}

void XdbdXmlDataBase::_InsertAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
{
	StatisticalAdapterThread::InsertAdapter(adapter_id, stat_dir_name, dest_dir_name, owner_name, group_name, dir_permissions, file_permissions);

	StatisticalAdapter* p_StatisticalAdapter = StatisticalAdapterThread::getAdapter(adapter_id);
	if (p_StatisticalAdapter == 0)
		return;
	char* statPath = (char*) p_StatisticalAdapter->statDirName();
	p_StatisticalAdapter->notifyFD(RegChgTracker (statPath, HandleStatDirChanges, p_StatisticalAdapter));
}

void XdbdXmlDataBase::_UpdateAdapter (SQLSMALLINT adapter_id, SQLVARCHAR* stat_dir_name, SQLVARCHAR* dest_dir_name, SQLVARCHAR* owner_name, SQLVARCHAR* group_name, SQLVARCHAR* dir_permissions, SQLVARCHAR* file_permissions)
{
	StatisticalAdapter* p_StatisticalAdapter = StatisticalAdapterThread::getAdapter(adapter_id);
	if (p_StatisticalAdapter != 0)
		UnregChgTracker(p_StatisticalAdapter->notifyFD(), p_StatisticalAdapter);

	StatisticalAdapterThread::UpdateAdapter(adapter_id, stat_dir_name, dest_dir_name, owner_name, group_name, dir_permissions, file_permissions);

	p_StatisticalAdapter = StatisticalAdapterThread::getAdapter(adapter_id);
	if (p_StatisticalAdapter == 0)
		return;
	char* statPath = (char*) p_StatisticalAdapter->statDirName();
	p_StatisticalAdapter->notifyFD(RegChgTracker (statPath, HandleStatDirChanges, p_StatisticalAdapter));
}

void XdbdXmlDataBase::_DeleteAdapter (SQLSMALLINT adapter_id)
{
	StatisticalAdapter* p_StatisticalAdapter = StatisticalAdapterThread::getAdapter(adapter_id);
	if (p_StatisticalAdapter != 0)
		UnregChgTracker(p_StatisticalAdapter->notifyFD(), p_StatisticalAdapter);

	StatisticalAdapterThread::DeleteAdapter(adapter_id);
}

void XdbdXmlDataBase::_ReleaseStatAdapter ()
{
	StatisticalAdapterThread::Release();
}

extern "C"
{

/************************************************************************/
/* trigger functions for table stat_adapter                             */
/************************************************************************/

void* nmFunc_stat_adapter_load (int n, char* p[])
{
	return 0;
}

int nmFunc_stat_adapter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{
	SQLSMALLINT	new_adapter_id = *(SQLSMALLINT*)(newLine + 16);
	SQLSMALLINT	old_adapter_id = *(SQLSMALLINT*)(oldLine + 16);
	SQLVARCHAR*	new_stat_dir_name = (SQLVARCHAR*)(newLine + 24);
	SQLVARCHAR*	new_dest_dir_name = (SQLVARCHAR*)(newLine + 280);
	SQLVARCHAR*	new_owner_name = (SQLVARCHAR*)(newLine + 536);
	SQLVARCHAR*	new_group_name = (SQLVARCHAR*)(newLine + 600);
	SQLVARCHAR*	new_dir_permissions = (SQLVARCHAR*)(newLine + 664);
	SQLVARCHAR*	new_file_permissions = (SQLVARCHAR*)(newLine + 680);

	switch (Data_Op_Type)
	{
	case 2:
		{
			/*	INSERT CODE	*/
			XdbdXmlDataBase::InsertAdapter(new_adapter_id, new_stat_dir_name, new_dest_dir_name, new_owner_name, new_group_name, new_dir_permissions, new_file_permissions);
		}
		break;
	case 4:
		{
			static unsigned char	cond_mask[] =
			{
				0, 0, 17, 17, 17, 17, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0,
				255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
				255, 255, 255, 255, 255, 255, 255, 255,
			};

            if (XdbdXmlDataBase::equal (newLine, oldLine, (SQLVARCHAR*) cond_mask, sizeof cond_mask) != 0)
			{
				/*	CHANGE MASK CODE	*/
            	XdbdXmlDataBase::UpdateAdapter(new_adapter_id, new_stat_dir_name, new_dest_dir_name, new_owner_name, new_group_name, new_dir_permissions, new_file_permissions);
			}
		}
		break;
	case 8:
		{
			/*	DELETE CODE	*/
			XdbdXmlDataBase::DeleteAdapter(old_adapter_id);
		}
		break;
	default:
		break;
	}
	return 0;
}

void nmFunc_stat_adapter_unload (void* context)
{
	// No code ..
}

}

} /* namespace xml_db_daemon */

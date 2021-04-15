/*
 * XdbdSqlResultInfo.cpp
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#include "XdbdSqlResultInfo.h"

namespace xml_db_daemon
{

XdbdSqlResultInfo::funcset	XdbdSqlResultInfo::g_functions;
XdbdSqlResultInfo*	XdbdSqlResultInfo::g_dummy = new XdbdSqlResultInfo (0);

XdbdSqlResultInfo::XdbdSqlResultInfo(int dummy)
{
	g_functions.insert (funcset::value_type ("count", XdbdSqlCountFunction));
}

XdbdSqlResultInfo::XdbdSqlResultInfo(XdbdXmlTableInfo* tabinfo, XdbdXmlColumnInfo* colinfo, const char* function)
{
	m_tabinfo = tabinfo;
	m_colinfo = colinfo;
	m_function = defineFunction (function);
}

XdbdSqlResultInfo::~XdbdSqlResultInfo()
{
}

XdbdSqlResultInfo::function	XdbdSqlResultInfo::defineFunction (const char* function)
{
	funcset::iterator	it;

	if ((it = g_functions.find(function)) == g_functions.end())
		return	XdbdSqlNoFunction;
	return	it->second;
}

} /* namespace xml_db_daemon */

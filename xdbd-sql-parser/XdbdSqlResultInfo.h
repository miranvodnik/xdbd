/*
 * XdbdSqlResultInfo.h
 *
 *  Created on: 18. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdXmlTableInfo.h"
#include <string.h>
#include <map>
using namespace std;

namespace xml_db_daemon
{

class XdbdSqlResultInfo
{
public:
	typedef	enum
	{
		XdbdSqlNoFunction,
		XdbdSqlCountFunction,
		XdbdSqlMinFunction,
		XdbdSqlMaxFunction
	}	function;
	static function	defineFunction (const char* function);
	XdbdSqlResultInfo(XdbdXmlTableInfo* tabinfo, XdbdXmlColumnInfo* colinfo, const char* function);
	virtual ~XdbdSqlResultInfo();
	inline XdbdXmlTableInfo*	tabinfo() { return m_tabinfo; }
	inline XdbdXmlColumnInfo*	colinfo() { return m_colinfo; }
	inline function	getFunction () { return m_function; }
private:
	class funccmp : public less < string >
	{
	public:
	    inline bool operator()(const string& __x, const string& __y) const
	    {
	    	return strcasecmp (__x.c_str(), __y.c_str()) < 0;
	    }
	};
	typedef	map < string, function, funccmp >	funcset;
	XdbdSqlResultInfo(int dummy);
private:
	static	XdbdSqlResultInfo*	g_dummy;
	static	funcset	g_functions;
	XdbdXmlTableInfo*	m_tabinfo;
	XdbdXmlColumnInfo*	m_colinfo;
	function	m_function;
};

} /* namespace xml_db_daemon */

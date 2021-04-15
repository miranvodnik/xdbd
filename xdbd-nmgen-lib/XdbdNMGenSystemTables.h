/*
 * XdbdNMGenSystemTables.h
 *
 *  Created on: 25. feb. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdDataBase.h"
#include "libxml/tree.h"
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

namespace xml_db_daemon
{

/*! @brief system table generator
 *
 */
class XdbdNMGenSystemTables
{
public:
	XdbdNMGenSystemTables (int n, char*p []);
	virtual ~XdbdNMGenSystemTables ();
	int	GenerateSystemTables ();
private:
	int m_n;	//!< number of controlling parameters
	char** m_p;	//!< controlling parameters
};

} /* namespace xml_db_daemon */

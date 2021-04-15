/*
 * XdbdNMGenCodeText.h
 *
 *  Created on: 12. feb. 2016
 *      Author: miran
 */

#pragma once

#include <sys/types.h>
#include <regex.h>
#include "XdbdNMGenBase.h"

namespace xml_db_daemon
{

/*! @brief 'code_text' descriptor
 *
 *  implements XdbdNMGenBase interface. Every instance
 *  contains original text of 'code_text' node and code
 *  generated from it.
 *
 *  class defines regular expression used to search for
 *  variable names in original text and an initializer
 *  for it implemented as private singleton instance of
 *  this class, which is instantiated by running environment
 *  at application startup when this library is loaded
 *
 */
class XdbdNMGenCodeText : public XdbdNMGenBase
{
private:
	XdbdNMGenCodeText (bool doIt);
public:
	XdbdNMGenCodeText ();
	virtual ~XdbdNMGenCodeText ();
	inline string text ()	//!< get generated code
	{
		return m_outText;
	}
	virtual int ReadSubNodes (xmlNodePtr node);
protected:
	virtual int ReadAttributes (xmlNodePtr node);
	virtual int GenerateCode (int dataCount, void* data []);
	virtual bool CheckColumnUsage (const char* name);
private:
	static const char* g_regexpr;	//!< regular expression text
	static regex_t g_reg;	//!< regular expression object reference
	static XdbdNMGenCodeText* g_initializer;	//!< regular expression initializer
	string m_text;	//!< original text
	string m_outText;	//!< generated code
};

} /* namespace xml_db_daemon */

/*
 * XdbdClientHandler.h
 *
 *  Created on: 11. nov. 2016
 *      Author: miran
 */

#pragma once

#include "XdbdRunningContext.h"

namespace xml_db_daemon
{

/*! @brief client driver multiplexer
 *
 *  This class implements no new functionality. All of its functionality
 *  is implemented by its superclass XdbdRunningContext which implements
 *  general mechanisms for I/O multiplexing. This class is merely a 'bag'
 *  holding theoretically unlimited number of clients which implement
 *  their own I/O logic in such a way that it can be multiplexed with
 *  other objects of this kind. In general: these objects are any open
 *  file descriptors obtained by open(), socket(), mq_open() and other
 *  system calls which return file descriptors.
 *
 *  @see XdbdRunningContext
 *
 */
class XdbdClientHandler : public XdbdRunningContext
{
public:
	/*! @brief XdbdClientHandler constructor
	 *
	 *  it provides start, stop and time hook routines to its superclass
	 *
	 */
	XdbdClientHandler () : XdbdRunningContext (HandlerStart, HandlerStop, HandlerTimeHook, this, "client-handler") {}
	/*! @brief XdbdClientHandler destructor
	 *
	 *  enables 'chaining' of destructor calls
	 *
	 */
	virtual ~XdbdClientHandler () {}

	ctx_starter (HandlerStart, XdbdClientHandler)	//!< declaration of start routine
	ctx_finisher (HandlerStop, XdbdClientHandler)	//!< declaration of stop routine
	ctx_timehook (HandlerTimeHook, XdbdClientHandler)	//!< declaration of time hook routine
};

} /* namespace xml_db_daemon */

/*
 * XdbdClientHandler.cpp
 *
 *  Created on: 11. nov. 2016
 *      Author: miran
 */

#include "XdbdClientHandler.h"

namespace xml_db_daemon
{

/*! @brief starting routine for client driver multiplexer
 *
 *  implementation of starting routine required by XdbdRunningContext
 *  interface. It does nothing
 *
 *  @see XdbdRunningContext
 *
 *  @param ctx refinement of this object referring to its superclass object
 *
 */
void	XdbdClientHandler::HandlerStart (XdbdRunningContext *ctx) {}

/*! @brief ending routine for client driver multiplexer
 *
 *  implementation of ending routine required by XdbdRunningContext
 *  interface. It does nothing
 *
 *  @see XdbdRunningContext
 *
 *  @param ctx refinement of this object referring to its superclass object
 *
 */
void	XdbdClientHandler::HandlerStop (XdbdRunningContext *ctx) {}

/*! @brief time hook routine for client driver multiplexer
 *
 *  implementation of time hook routine required by XdbdRunningContext
 *  interface. It does nothing
 *
 *  @see XdbdRunningContext
 *
 *  @param ctx refinement of this object referring to its superclass object
 *  @param oldTime time stamp before time change
 *  @param new Time time stamp after time change
 *  @param timeDiff difference between time stamps expressed in nanoseconds
 *
 */
void	XdbdClientHandler::HandlerTimeHook (XdbdRunningContext *ctx, timespec oldTime, timespec newTime, long long int timeDiff) {}

} /* namespace xml_db_daemon */

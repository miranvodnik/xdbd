/*
 * XdbdErrApi.cpp
 *
 *  Created on: 7. sep. 2016
 *      Author: miran
 */

#include <stdarg.h>
#include "XdbdErrInfo.h"
#include "XdbdErrApi.h"

/*! @brief error report routine

 * function disables cancellation mechanisms during invocation of sc_printf
 *
 *  @param module software module id
 *  @param severity error severity
 *  @param format print format
 *  @param ... printf arguments
 */
void	xdbdErrReport (int module, int severity, const char* format, ...)
{
	XdbdErrInfo*	log = XdbdErrInfo::xdbdErrInfo();
	int	cancelstate, dummy;
	va_list	pArg;

	pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &cancelstate);
	va_start (pArg, format);
	log->ErrReport (module, severity, format, pArg);
	va_end (pArg);
	pthread_setcancelstate (cancelstate, &dummy);
	pthread_testcancel ();
}

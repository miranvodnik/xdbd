/*
 * XdbdMessageQueue.h
 *
 *  Created on: 3. nov. 2015
 *      Author: miran
 */

#pragma once

#include "XdbdBaseJob.h"
#include <string>
using namespace std;

namespace xml_db_daemon
{

typedef	void*	msg_addr;

/*! @brief message queue object
 *
 *  this object contains identifier of System V message queue
 *  governed by it. It provides two basic operations on that
 *  queue: one can put and get job references into or from the
 *  queue. All job references can also be removed from message
 *  queue using single function call. Queue should be initialized
 *  using Initialize(), before job references can be put or get
 *  from underlying message queue. This should be done early at
 *  the process startup.
 *
 *  All public functions are static, because we cannot directly
 *  access this object since it has no public access and is thus
 *  accessed indirectly from these functions.
 *
 */
class XdbdMessageQueue
{
private:
	XdbdMessageQueue (key_t key);
public:
	virtual ~XdbdMessageQueue ();
public:
	/*! @brief one shot creation and initialization of this object
	 *
	 */
	inline static void	Initialize () { ((XdbdMessageQueue*)0)->_Initialize(); }
	/*! @brief put job reference into message queue
	 *
	 *  @param job job reference to be saved in message queue
	 *
	 *  @return true job reference successfully saved
	 *  @return false job reference not saved
	 */
	inline static bool	Put (XdbdBaseJob* job) { return g_msgQueue->_Put (job); }
	/*! @brief put job reference into message queue
	 *
	 *  @param job job reference to be saved in message queue
	 *
	 *  @return true job reference successfully saved
	 *  @return false job reference not saved
	 */
	inline static bool	PutInt (XdbdBaseJob* job) { return g_intQueue->_Put (job); }
	/*! @brief retrieve job reference from message queue
	 *
	 *  @return 0 job not retrieved
	 *  @return other job reference
	 *
	 */
	inline static XdbdBaseJob*	Get () { return g_msgQueue->_Get (); }
	/*! @brief retrieve job reference from message queue
	 *
	 *  @return 0 job not retrieved
	 *  @return other job reference
	 *
	 */
	inline static XdbdBaseJob*	GetInt () { return g_intQueue->_Get (); }
private:
	void	_Initialize ();
	bool	_Put (XdbdBaseJob* job);
	XdbdBaseJob*	_Get ();
	void	_Clear (bool destructive);
private:
	static bool	g_initialized;	//!< indicates one shot initialization
	static XdbdMessageQueue*	g_msgQueue;	//!< single instance of this class
	static XdbdMessageQueue*	g_intQueue;	//!< internal message queue singleton
	long	m_mtype;	//!< message types for messages holding job references
	int	m_reqQue;	//!< System V message queue ID
};

} /* namespace xml_db_daemon */

/*
 * XdbdHamThread.h
 *
 *  Created on: 27. okt. 2016
 *      Author: miran
 */

#pragma once

#if defined (XDBD_HAM)

extern	"C"			// C style headers
{
#include <ham/hamcfg.h>		// HAM
#include <ham/hamipc.h>		// HAM
#include <itcommon/ipcs_keys.h>	// DB notify mechanism
}

#endif	// XDBD_HAM

#include "XdbdRunnable.h"

#if defined (XDBD_HAM)

#define	HAM_ERROR	((unsigned int) HA_ERROR)	// make compiler happy

#endif	// XDBD_HAM

namespace xml_db_daemon
{

/*! @brief HAM thread
 *
 */
class XdbdHamThread : public XdbdRunnable
{

	static const char	*g_hamMsgStr[];
	static const char	*g_hamErrStr[];

	/*! @brief HAM thread exceptions
	 *
	 *  these exception codes can be thrown from erroneous HAM connection
	 */
	typedef	enum
	{
		HamNoError,	// disable 0 error codes
		HamDontUseHam,
		HamOpenCommPointError,
		HamConnectError,
		HamSendRegistrationError,
		HamReceiveError,
		HamReceiveRegOkError,
		HamReceiveRegFailError,
		HamReceiveToActiveError,
		HamReceiveToStandbyError,
		HamSendAppStateError,
		HamReceiveToBusyError,
		HamReceiveToIdleError,
		HamSendAppUstateError,
		HamReceiveAhbQueryError,
		HamSendAhbAckError,
	}	HamErrorCode;

	/*! @brief message codes reported by HAM thread
	 *
	 */
	typedef	enum
	{
		HamNoCode,	// disable 0 message codes
		HamOpenCommPoint,
		HamOpenPeerPoint,
		HamSendRegReq,
		HamRecvRegOk,
		HamRecvRegFail,
		HamRecvAppToActive,
		HamRecvAppToMounted,
		HamRecvAppToUmounted,
		HamRecvAppToStandby,
		HamRecvAppToBusy,
		HamRecvAppToIdle,
		HamRecvAhbQuery,
		HamRecvAhbAck,
		HamRecvUnknown,
		HamExceptionCode,
	}	HamMsgCode;

public:
	XdbdHamThread ();
	virtual ~XdbdHamThread ();
	int SendHamMsg (HamMsgCode code, HamErrorCode err);
	void	HAMLoop ();
	void Cleanup ();
	virtual int InitInstance (void);
	virtual int Run (void);
	inline static void ThreadCleanup (void* data) { ((XdbdHamThread*)data)->ThreadCleanup(); }
	void ThreadCleanup ();
#if defined (XDBD_HAM)
	inline int	Registration () { return m_registration; }
	inline void	WorkerReady (bool ready) { m_workerFailed = !ready; m_workerReady = true; }
#endif	// XDBD_HAM

	inline bool	useHam () { return m_useHam; }
private:
	bool		m_useHam;	// use HMA or not

#if defined (XDBD_HAM)

	KKIPC_T_CommPointId	m_clnConn;	//!< client HAM connection
	KKIPC_T_CommPointId	m_hamConn;	//!< peer HAM connection
	HA_T_ApplId		m_appId;	//!< application ID registered with HAM
	union
	{
		HA_T_RegistrationReq	regReq;
		HA_T_RegistrationOk	regOk;
		HA_T_RegistrationFail	regFail;
		HA_T_AppToActive	toActive;
		HA_T_AppToStandby	toStandby;
		HA_T_AppStateOk		stateOk;
		HA_T_AppStateFail	stateFail;
		HA_T_AppToBusy		toBusy;
		HA_T_AppToIdle		toIdle;
		HA_T_AppUstateOk	ustateOk;
		HA_T_AppUstateFail	ustateFail;
		HA_T_AhbQuery		ahbQuery;
		HA_T_AhbAck		ahbAck;
		HA_T_Error		error;
	}	*m_udata;	//!< union of HAM messages

	int	m_registration;
	bool	m_workerReady;
	bool	m_workerFailed;

#endif	// XDBD_HAM
};

} /* namespace xml_db_daemon */

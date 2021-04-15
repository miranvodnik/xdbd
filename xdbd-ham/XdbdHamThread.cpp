/*
 * XdbdHamThread.cpp
 *
 *  Created on: 27. okt. 2016
 *      Author: miran
 */

#include <stdlib.h>
#include <strings.h>
#include "XdbdHamThread.h"
#include "XdbdErrApi.h"

namespace xml_db_daemon
{

/*! @brief create HAM thread object
 *
 */
XdbdHamThread::XdbdHamThread ()
{
#if defined(XDBD_HAM)
	m_clnConn = KKIPC_CommPointIdDummy;
	m_hamConn = KKIPC_CommPointIdDummy;
	m_udata = NULL;
	m_registration = EV_START;
	m_workerReady = false;
	m_workerFailed = false;
#endif	// XDBD_HAM
	m_useHam = true;
}

/*! @brief release HAM thread object
 *
 */
XdbdHamThread::~XdbdHamThread ()
{
	Cleanup ();
}

int XdbdHamThread::SendHamMsg (HamMsgCode code, HamErrorCode err)
{
	return	0;
}

/*! @brief HAM communication loop
 *
 */
void	XdbdHamThread::HAMLoop ()
{
	char*	appName = (char*) "XDBD";
	int pSrvId = 0;

	if (!m_useHam)
		throw	HamDontUseHam;

#if defined(XDBD_HAM)
	int	status;

	xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: kkipcOpenCommPoint");
	if ((m_clnConn = kkipcOpenCommPoint ((char *)"APPL_TO_HAM", (char *)"XDBD", KKIPC_Client, (void**) &m_udata)) == KKIPC_CommPointIdDummy)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: kkipcOpenCommPoint() failed");
		throw	HamOpenCommPointError;
	}
	SendHamMsg (HamOpenCommPoint, HamNoError);

	xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: kkipcConnectTo");
	if ((m_hamConn = kkipcConnectTo ((char *)"APPL_TO_HAM", m_clnConn)) == KKIPC_CommPointIdDummy)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: kkipcConnectTo() failed");
		throw	HamConnectError;
	}
	SendHamMsg (HamOpenPeerPoint, HamNoError);

	xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: haSendRegistrationReq");
	if (haSendRegistrationReq (m_clnConn, m_hamConn, &m_udata->regReq, appName) == HAM_ERROR)
	{
		xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haSendRegistrationReq() failed");
		throw	HamSendRegistrationError;
	}
	SendHamMsg (HamSendRegReq, HamNoError);

	while (1)
	{
		KKIPC_T_CommPointId	conn;
		KKIPC_T_CommType	ctype;
		unsigned long		len;

		xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: kkipcReceive");
		if (kkipcReceive (m_clnConn, &conn, &ctype, (void**) &m_udata, &len) != KKIPC_OK)
		{
			xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: kkipcReceive() failed");
			continue;
		}

		if (ctype != KKIPC_COMMTYPE_HA)
			continue;
		HA_T_IpcMessage	*msg = (HA_T_IpcMessage*) m_udata;
		switch (msg->fHeader.fEventName)
		{
		case EV_REG_OK:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_REG_OK");
			m_registration = EV_REG_OK;
			if (haReceiveRegistrationOk (&m_udata->regOk, len, &m_appId) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveRegistrationOk() failed");
				throw	HamReceiveRegOkError;
			}
			SendHamMsg (HamRecvRegOk, HamNoError);
			break;
		case EV_REG_FAIL:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_REG_FAIL");
			m_registration = EV_REG_FAIL;
			if (haReceiveRegistrationFail (&m_udata->regFail, len) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveRegistrationFail() failed");
				throw	HamReceiveRegFailError;
			}
			SendHamMsg (HamRecvRegFail, HamNoError);
			break;
		case EV_APP_TO_ACTIVE:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_APP_TO_ACTIVE");
			if (haReceiveAppToActive (&m_udata->toActive, len) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveAppToActive() failed");
				throw	HamReceiveToActiveError;
			}
			while (!m_workerReady)
			{
				xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: waiting xdbd worker to become ready, active state");
				sleep (1);
			}
			if (m_workerFailed)
				status = haSendAppStateFail (m_clnConn, conn, &m_udata->stateFail, m_appId);
			else
				status = haSendAppStateOk (m_clnConn, conn, &m_udata->stateOk, m_appId);
			if (status == (int) HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haSendAppStateOk() failed");
				throw	HamSendAppStateError;
			}
			SendHamMsg (HamRecvAppToActive, HamNoError);
			break;
		case EV_APP_TO_SB:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_APP_TO_SB");
			if (haReceiveAppToStandby (&m_udata->toStandby, len) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveAppToStandby() failed");
				throw	HamReceiveToStandbyError;
			}
			while (!m_workerReady)
			{
				xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: waiting xdbd worker to become ready, stand-by state");
				sleep (1);
			}
			if (m_workerFailed)
				status = haSendAppStateFail (m_clnConn, conn, &m_udata->stateFail, m_appId);
			else
				status = haSendAppStateOk (m_clnConn, conn, &m_udata->stateOk, m_appId);
			if (status == (int) HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haSendAppStateOk() failed");
				throw	HamSendAppStateError;
			}
			SendHamMsg (HamRecvAppToStandby, HamNoError);
			break;
		case EV_APP_TO_BUSY:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_APP_TO_BUSY");
			if (haReceiveAppToBusy (&m_udata->toBusy, len, &pSrvId) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveAppToBusy() failed");
				throw	HamReceiveToBusyError;
			}
			while (!m_workerReady)
			{
				xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: waiting xdbd worker to become ready, busy state");
				sleep (1);
			}
			if (m_workerFailed)
				status = haSendAppUstateFail (m_clnConn, conn, &m_udata->ustateFail, m_appId);
			else
				status = haSendAppUstateOk (m_clnConn, conn, &m_udata->ustateOk, m_appId);
			if (status == (int) HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haSendAppUstateOk() failed");
				throw	HamSendAppUstateError;
			}
			SendHamMsg (HamRecvAppToBusy, HamNoError);
			break;
		case EV_APP_TO_IDLE:
			xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: EV_APP_TO_IDLE");
			if (haReceiveAppToIdle (&m_udata->toIdle, len, &pSrvId) == HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haReceiveAppToIdle() failed");
				throw	HamReceiveToIdleError;
			}
			while (!m_workerReady)
			{
				xdbdErrReport (SC_XDBD, SC_APL, "XDBD HAM thread: waiting xdbd worker to become ready, idle state");
				sleep (1);
			}
			if (m_workerFailed)
				status = haSendAppUstateFail (m_clnConn, conn, &m_udata->ustateFail, m_appId);
			else
				status = haSendAppUstateOk (m_clnConn, conn, &m_udata->ustateOk, m_appId);
			if (status == (int) HAM_ERROR)
			{
				xdbdErrReport (SC_XDBD, SC_ERR, "XDBD HAM thread: haSendAppUstateOk() failed");
				throw	HamSendAppUstateError;
			}
			SendHamMsg (HamRecvAppToIdle, HamNoError);
			break;
		default:
			xdbdErrReport (SC_XDBD, SC_WRN, "XDBD HAM thread: unknown message type = %d",
				msg->fHeader.fEventName);
			SendHamMsg (HamRecvUnknown, HamNoError);
			break;
		}
	}
#endif	// XDBD_HAM
}

/*! @brief Function closes all connections established with HAM
 *
 */

void	XdbdHamThread::Cleanup ()
{
#if defined(XDBD_HAM)
	if (m_hamConn != KKIPC_CommPointIdDummy)
		kkipcCloseConnection (m_clnConn, m_hamConn);
	if (m_clnConn != KKIPC_CommPointIdDummy)
		kkipcCloseMyCommunicationPoint (m_clnConn);
	m_clnConn = KKIPC_CommPointIdDummy;
	m_hamConn = KKIPC_CommPointIdDummy;
#endif	// XDBD_HAM
}

/*! @brief This function performs initialization of controlling
 *  thread Initialization is successful if controlling message
 *  can be attached.
 *
 *  @return 0 always
 *
 */

int	XdbdHamThread::InitInstance (void)
{
	char	*env;

	if ((env = getenv ("XDBD_USE_HAM")) != NULL)
	{
		if (strcasecmp (env, "true") != 0)
			m_useHam = false;
	}

	return	0;
}

/*! @brief working function for HAM thread
 *
 *  It connects to HAM, performs registration and handles
 *  other messages reported by HAM. In case of error it terminates
 *  telling to the controlling thread that it should continue
 *  without HAM support
 *
 *  @return 0 always
 */

int	XdbdHamThread::Run (void)
{

	xdbdErrReport (SC_XDBD, SC_ERR, "XDBD-controller: HAM thread started, TID = %d", getTid());

	pthread_cleanup_push (ThreadCleanup, this);
	while (1)
	{
		try
		{
			HAMLoop ();
		}
		catch (HamErrorCode err)
		{
			Cleanup ();
			SendHamMsg (HamExceptionCode, err);
		}
		break;
	}
	pthread_cleanup_pop (0);

	xdbdErrReport (SC_XDBD, SC_ERR, "XDBD-controller: HAM thread terminated, TID = %d", getTid());

	return	0;
}

/*! @brief HAM thread cancellation routine
 *
 */
void XdbdHamThread::ThreadCleanup ()
{
	xdbdErrReport (SC_XDBD, SC_ERR, "HAM thread canceled, TID = %d", getTid ());
}

} /* namespace xml_db_daemon */

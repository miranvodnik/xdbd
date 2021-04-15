
#ifdef RPC_HDR
%#include <uuid/uuid.h>
#define XDBDPROG          0x20004050
#define XDBDFUNCTUN          1			/* union of all CDCP administrative requests */
#define	XDBDVERSION          XDBDVERSION_1	/* current version */

#if	0
	static	int	iencode = 0;
	static	int	idecode = 0;
	static	int	ifree = 0;

	switch (xdrs->x_op)
	{
	case	XDR_ENCODE:
		if (++iencode > 1000)
		{
			printf ("ENCODE %016x\n", objp);
			iencode = 0;
		}
		break;
	case	XDR_DECODE:
		if (++idecode > 1000)
		{
			printf ("DECODE %016x\n", objp);
			idecode = 0;
		}
		break;
	case	XDR_FREE:
		if (++ifree > 1000)
		{
			printf ("FREE %016x\n", objp);
			ifree = 0;
		}
		break;
	}
#endif

#endif

struct	XdbdVersion
{
	u_int	m_major;
	u_int	m_minor;
};

enum	XdbdEntity
{
	XdbdNoEntity,
	XdbdClientEntity,
	XdbdControllerEntity,
	XdbdWorkerEntity
};

enum	XdbdErrorType
{
	XdbdAppErrorType,
	XdbdSysErrorType
};

enum	XdbdErrorCode
{
	NoSqlError,
	SqlNotConnected,
	SqlAlreadyConnected,
	SqlUnknownRequest
};

struct	XdbdError
{
	XdbdErrorType	m_type;
	int	m_errorCode;
	string	m_errorMessage<>;
	struct XdbdError*	m_next;
};

struct	XdbdMessageHeader
{
	XdbdVersion	m_version;
	XdbdEntity	m_entity;
	u_int	m_pid;
	u_int	m_index;
	u_char	m_hash[16];
	struct XdbdError*	m_error;
};

enum	XdbdRequestType
{
	FirstRequestCode = 0,
	XdbdUnknownRequestCode = -1,
	AdminRequestCode = 1,
	SqlRequestCode = 2
};

enum	XdbdReplyType
{
	FirstReplyCode = 0,
	XdbdUnknownReplyCode = -1,
	AdminReplyCode = 1,
	SqlReplyCode = 2
};

struct	XdbdParameter
{
	u_int	m_flags;
	uint64_t	m_longVal;
	string	m_stringVal<>;
};

struct	XdbdAdminRequest
{
	XdbdMessageHeader	m_header;
	u_int	m_command;
	XdbdParameter	m_parameters<>;
};

struct	XdbdSqlRequest
{
	XdbdMessageHeader	m_header;
	u_int	m_command;
	XdbdParameter	m_parameters<>;
};

union	XdbdRequest
switch (XdbdRequestType m_requestType)
{
	case	XdbdUnknownRequestCode:
		XdbdMessageHeader	m_header;
	case	AdminRequestCode:
		XdbdAdminRequest	m_adminRequest;
	case	SqlRequestCode:
		XdbdSqlRequest	m_sqlRequest;
};

struct	XdbdSqlReply
{
	XdbdMessageHeader	m_header;
	u_int	m_command;
	XdbdParameter	m_parameters<>;
};

union	XdbdReply
switch (XdbdReplyType m_replyType)
{
	case	XdbdUnknownReplyCode:
		XdbdMessageHeader	m_header;
	case	SqlReplyCode:
		XdbdSqlReply	m_sqlReply;
};

program	XDBD_RPC
{
	version	XDBDVERSION_1
	{
		XdbdReply	XdbdExecRequest (XdbdRequest) = XDBDFUNCTUN;
	} = 1;
} = XDBDPROG;

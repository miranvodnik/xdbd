
#include <string>
using namespace std;

/*! @brief error info object
 *
 */
class	XdbdErrInfo
{
private:
	XdbdErrInfo ();

	int	Connect (void);
	void	Disconnect (void);

public:

	static	inline	XdbdErrInfo	*xdbdErrInfo (void) { return g_errInfo; }
	int	ErrReport (int module, int severity, const char* format, va_list msg);
	bool	disable (bool disable);

private:
	static	XdbdErrInfo*	g_errInfo;
	int	m_logLevel;	//!< log level
	string	m_syslogSocketPath;	//!< UNIX domain socket path to access syslog-ng
	int	m_syslogSocket;	//!< UNIX domain socket to access syslog-ng
	bool	m_disabled;	//!< disable error reporting when set to true
};

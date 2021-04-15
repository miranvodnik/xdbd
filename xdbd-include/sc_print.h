/*#######################################################################*/
/*#                                                                     #*/
/*#                                                                     #*/
/*#                  Copyright (c) 2001 Iskra Tel                       #*/
/*#                                                                     #*/
/*#                                                                     #*/
/*# Name        : sc_print.h                                            #*/
/*#                                                                     #*/
/*# Description : System console print - header file                    #*/
/*#                                                                     #*/
/*# Code        : KPQF - XAG9064                                        #*/
/*#                                                                     #*/
/*# Date        : Jun 2001                                              #*/
/*#                                                                     #*/
/*# Author      : Tomo Jarc, RDSP5                                      #*/
/*#                                                                     #*/
/*# Remarks     :                                                       #*/
/*#                                                                     #*/
/*# Revision    :                                                       #*/
/*#                                                                     #*/
/*#######################################################################*/

#ifndef _SC_PRINT_H
#define _SC_PRINT_H

/****************************************************************/
/*          UNIX - SCCS  VERSION DESCRIPTION                    */
/****************************************************************/
/*static char unixid_sc_print[] = "@(#)KPQF.z	1.9.2.10	11/03/03	sc_print.h";*/


#include	<stdarg.h>


/****************************************************************/
/*         SYMBOLIC CONSTANTS                                   */
/****************************************************************/

/* modules */
enum {
   SC_RTDB	= 0,
   SC_DSP,
   SC_SYSTEM,
   SC_SDL,
   SC_TRACER,
   SC_PPP,
   SC_DIAG,
   SC_V52,
   SC_VVS,
   SC_SSN7,
   SC_LEVEL1,
   SC_FILE_SYS,
   SC_MOTSW,
   SC_ATM,
   SC_H323,
   SC_BRIDGE,
   SC_PIPC,
   SC_VXML,
   SC_RTCP,
   SC_SIP,
   SC_DRH,
   SC_CDCP,
   SC_CHARGE,
   SC_REPLMGR,
   SC_KKIPC,
   SC_HATA,
   SC_DS,
   SC_C_PROT,
   SC_SCTP,
   SC_SYSUP,
   SC_CAS,
   SC_LIM,
   SC_DBC,
   SC_GAD,
   SC_ZL_DIAG,
   SC_HW_INIT,
   SC_RCM,
   SC_SHMC,
   SC_SFTP,
   SC_XDBD,
   		/* <<-- here is place for new modules	*/
   /*********************************************************************/
   /* new modules must be inserted BEFORE SC_EXC line (bellow) !!!	*/
   /*********************************************************************/
   SC_EXC,
   MAX_MODULES
};

/* sc_print module names */
#define SC_MODUL_LIST	{ \
   "RTDB",	\
   "DSP",	\
   "SYSTEM",	\
   "SDL",	\
   "TRACER",	\
   "PPP",	\
   "DIAG",	\
   "V52",	\
   "VVS",	\
   "SS7",	\
   "LEVEL1",	\
   "FILE_SYS",	\
   "MOTSW",	\
   "ATM",	\
   "H323",	\
   "BRIDGE",	\
   "PIPC",	\
   "VXML",	\
   "RTCP",	\
   "SIP",	\
   "DRH",	\
   "CDCP",	\
   "CHARGE",	\
   "REPLMGR",	\
   "KKIPC",	\
   "HATA",	\
   "DS",	\
   "C_PROT",	\
   "SCTP",	\
   "SYSUP",	\
   "CAS",	\
   "LIM",	\
   "DBC",	\
   "GAD",	\
   "ZL_DIAG",	\
   "HW_INIT",	\
   "RCM",	\
   "SHMC",	\
   "SFTP",	\
   "XDBD",	\
   		   		/* <<-- here is place for new modules	*/ \
   "EXC",       \
   (char *) NULL /* null terminated */ \
}

#define SCP_MAKE_MASK(module) (MODULE_T) 1 << (module)  /* Make module "module" usable (un hide) - prem */


/* message types */
#if defined MG_PCU_NODE && !defined (LINUX)

#define SC_ERR		0x01
#define SC_WRN		0x02
#define SC_INIT		0x04
#define SC_APL		0x08
#define SC_APL1		0x10
#define SC_APL2		0x20

#else

enum {
/*  --------		   -----------	---------	-------------	*/
/*  sc_print		   syslog.h	text mark	syslog config	*/
/*  --------		   -----------	---------	-------------	*/
    SC_UNKN      = 0,	/* LOG_EMERG	   UNK		emerg		*/
    SC_INIT,		/* LOG_ALERT	   INI		alert		*/
    SC_CRIT,		/* LOG_CRIT	   CRT		crit		*/
    SC_ERR,		/* LOG_ERR	   ERR		err		*/
    SC_WRN,		/* LOG_WARNING	   WRN		warning		*/
    SC_NOTE,		/* LOG_NOTICE	   NOT		notice		*/
    SC_INFO,		/* LOG_INFO	   INF		info		*/
    SC_APL,		/* LOG_DEBUG	   APL		debug		*/
    MAX_TYPES
};

#define SC_APL1 SC_APL
#define SC_APL2 SC_APL

#endif

#define SC_MSG_LEN	512

/****************************************************************/
/* TYPE DECLARATIONS                                            */
/****************************************************************/
typedef unsigned short MSGTYPE_T;
typedef unsigned long MODULE_T;

/****************************************************************/
/*          E X T E R N A L S                                   */
/****************************************************************/
#ifndef __STDC__
extern void sc_printf( );
extern void sc_printf_pcu( );
extern void sc_printf_remote( );
extern void sc_sdl_printf( );
extern void sc_perror( );
extern void scp_CmdLine();
#else	/* ! __STDC__ */
extern void sc_printf(int module, MSGTYPE_T msgType, const char *fmt, ...);
extern void sc_printf_va(int module, MSGTYPE_T msgType,
			 const char *fmt, va_list args);
extern void sc_printf_pcu(int module, MSGTYPE_T msgType,
                          const char *fmt, ...);
extern void sc_printf_remote(int pid, char *nodeName, char *processName,
                          int module, MSGTYPE_T msgType,
                          const char *fmt, ...);
extern void sc_sdl_printf(const char *fmt, ...);
extern void sc_perror(int modul, int pr_level, unsigned char *str);
extern void scp_CmdLine(int argc, char *argv[], char *env[]);
extern MODULE_T scp_used_modules;
extern int scp_start(void);
#endif	/* __STDC__ */

#endif	/* _SC_PRINT_H */

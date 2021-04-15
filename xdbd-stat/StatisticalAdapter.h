
#pragma once

/****************************************************************/
/*          UNIX - SCCS  VERSION DESCRIPTION                   	*/
/****************************************************************/
/*static char  unixid_QGNJ[] = "%W%	%D%	GeneralAdapter.h";*/

/****************************************************************/
/*      Header files                                            */
/****************************************************************/

//typedef	unsigned long	ULONG;

#include <stdlib.h>
#include "sqlxdbd.h"
#include "sadapter.h"

#define syn_cv_cscf_type_p  1

#define syn_cv_cscf_type_i  2

#define syn_cv_cscf_type_s  4

#define syn_cv_cscf_type_as  8

#define syn_cv_cscf_type_mrc  16

#define syn_cv_cscf_type_agcf  32

#define syn_cv_cscf_type_mgcf  64

#define syn_cv_cscf_type_bgcf  128

#define syn_cv_cscf_type_nonimsagcf  256

#define syn_cv_cscf_type_e  512

#define syn_cv_cscf_type_ims_iagcf  1024

#define syn_cv_cscf_type_imsiad  2048

#define syn_cv_cscf_type_sccas  4096

#define syn_cv_cscf_type_imobgw  8192

#define syn_cv_cscf_type_rgcf  16384

#define syn_cv_cscf_type_tagcf  32768

/****************************************************************/
/*      External functions                                      */
/****************************************************************/

class StatisticalAdapter
{
public:
	StatisticalAdapter(unsigned short adapterId, const char* statDirName, const char* destDirName, const char* owner, const char* group, const char* dperm, const char* fperm);
	virtual ~StatisticalAdapter();

private:
	void _Init (void);
	void _Collect (bool, time_t);
	int	GetImsCscfType (int nodeid);
	void CollectStatData (string srcFileName, string dstFileName);
	void _DoCollect (const char* fileName, time_t t = 0);
	void lock ();
	void unlock ();

public:
	inline const char* statDirName () { return m_statDirName.c_str(); }
	inline void DoCollect (const char* fileName, time_t t = 0) { lock(); _DoCollect (fileName, t); unlock(); }
	inline void notifyFD (int notifyFD) { m_notifyFD = notifyFD; }
	inline int notifyFD () { return m_notifyFD; }
private:
	unsigned long	m_fileIndex;
	int m_sync;
	int m_notifyFD;

	uid_t	m_owid;
	gid_t	m_grid;
	mode_t	m_dperms;
	mode_t	m_fperms;

	unsigned short m_adapterId;
	string	m_owner;
	string	m_group;
	string	m_dperm;
	string	m_fperm;
	string	m_statDirName;
	string	m_dstDirName;
	bool m_preserveHistory;
	bool m_generateXml;
};

/*########################################################################*/
/*#                                                                      #*/
/*#                                                                      #*/
/*#               Copyright (c) 2006 Iskratel                            #*/
/*#                                                                      #*/
/*#                                                                      #*/
/*# Name:         nmStructureDefinition.c                                #*/
/*#                                                                      #*/
/*# Description:  Functions for filling stuctures for notification       #*/
/*#                                                                      #*/
/*# Code:         BNYB - XAB2781                                         #*/
/*#                                                                      #*/
/*# Date:         May 2006                                               #*/
/*#                                                                      #*/
/*# Author:       Ales Borsic, I-ROSE                                    #*/
/*#                                                                      #*/
/*# Remarks:                                                             #*/
/*#                                                                      #*/
/*# Revisions:    Matej Susnik                                           #*/
/*#                                                                      #*/
/*########################################################################*/

/*************************************************************************/
/*                   UNIX - SCCS   VERSION DESCRIPTION                   */
/*************************************************************************/
/* static char unixid[] = "@(#)BNYB.z	1.1.4.13	15/01/22	nmStructureDefinition.c -O- "; */

/*************/
/* LIBRARIES */
/*************/

#include <string.h>

#include "nmStructureDefinition.h"

/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

void nmGetGU_T_DPDATAStruct (GU_T_DPDATA * dpdata, short fd_f_dpnr, short fd_f_dpsrvname, short fd_f_dptype,
short seqnr, short fd_f_dparming, short fd_f_dpcriteriaid, short fd_f_dppropertyid, short dpsrvgroup, short dpsrvgroupseqnr);

void nmGetGU_T_PFXDATAStruct(GU_T_PFXDATA *pPfxData, short ndcid, char *dstpfx, long bgid, short cgid,
short dstctg, short dstid, short ststgrp, short nplanid);

void nmGetGU_T_QDATAStruct(GU_T_QDATA * pQdata, short cgido, short cgidt, char *qpfx, short crgcode);

void nmGetnmng_msg_tsf_settingsStruct (nmng_msg_tsf_settings *p_nmng_msg_tsf_settings, long int mtype, char *TableName,
short Data_Op_Type, short UserInfo, long old_nodeid, short old_tsf_settings_id, char *old_ftp_settings, long new_nodeid,
short new_tsf_settings_id, char *new_ftp_settings);

void nmGetGU_T_TARIFFPFXDATAStruct (GU_T_TARIFFPFXDATA *pTariffPfxData, short ndcid, char *dstpfx, long bgid,
short cid, short dstchcode, short ststgrp, short nplanid);

void nmGetGU_T_TRUNKStruct (GU_T_TRUNK *ptrunk, short module, short port, short ci);

void nmGetIua_access_data (GU_T_IUA_ACCESSDATA *pIua_access_data, long sysintfid, long sysaccessid,
long intiid, char *textiid, short equipped, short block, short state);

void nmGetArp_data (lnxadm_msg_data *pArp_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_ip_addr, char *old_mac_addr, short old_arp_proxy, char *new_ip_addr, char *new_mac_addr, short new_arp_proxy);

void nmGetBasic_filter_data (lnxadm_msg_data *pBasic_filter_data, long int mtype, char *TableName, short Data_Op_Type,
short old_filter_id, short old_basic_filt_order, short old_basic_filt_action, char *old_src_mac_addr,
char *old_dst_mac_addr, long old_ll_prot, char *old_src_ip_addr, char *old_src_ip_mask, char *old_dst_ip_addr, char *old_dst_ip_mask,
short old_ip_prot, long old_src_porthigh, long old_src_portlow, long old_dst_porthigh , long old_dst_portlow, short old_icmp_type, 
short old_tos, short old_precedense, short new_filter_id, short new_basic_filt_order, short new_basic_filt_action,
char *new_src_mac_addr, char *new_dst_mac_addr, long new_ll_prot, char *new_src_ip_addr, char *new_src_ip_mask,
char *new_dst_ip_addr, char *new_dst_ip_mask, short new_ip_prot, long new_src_porthigh, long new_src_portlow, 
long new_dst_porthigh, long new_dst_portlow, short new_icmp_type, short new_tos, short new_precedense);

void nmGetEth_interface_data (lnxadm_msg_data *pEth_interface_data, long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_eth_intf_name, short  old_modulenr, short  old_portnr, short  old_portnr2, short  old_oosi,
short  old_admin_state, short  old_log_stat_group_id, short  old_interface_type, short  new_eth_intf_id, char *new_eth_intf_name,
short  new_modulenr, short  new_portnr, short  new_portnr2, short  new_oosi, short  new_admin_state, short  new_log_stat_group_id, short new_interface_type);

void nmGetEth_profile_data (lnxadm_msg_data *pEth_profile_data, long int mtype, char *TableName, short Data_Op_Type,
short old_eth_profile_id, char *old_profile_name, short old_bandwidth, long old_mtu, short old_eth_mode, short new_eth_profile_id,
char *new_profile_name, short new_bandwidth, long new_mtu, short new_eth_mode);

void nmGetEth_tag_rule_data (lnxadm_msg_data *pEth_tag_rule_data, long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, short  old_vlan_id, short  old_acc_untag_frms, short  old_snd_tag_frms, short  old_ins_prior, short  old_prior_value,
short  new_eth_intf_id, short  new_vlan_id, short  new_acc_untag_frms, short  new_snd_tag_frms, short  new_ins_prior, short new_prior_value);

void nmGetFilter_data (lnxadm_msg_data *pFilter_data, long int mtype, char *TableName, short Data_Op_Type,
short old_filter_id, char *old_filter_name, short new_filter_id, char *new_filter_name);

void nmGetFilter_grp_member_data (lnxadm_msg_data *pFilter_grp_member_data, long int mtype, char *TableName, short Data_Op_Type,
short old_intf_id, short old_sub_type, short old_filter_direction, short old_member_order, short old_filter_id,
short new_intf_id, short new_sub_type, short new_filter_direction, short new_member_order, short new_filter_id);

void nmGetIp_device_data (lnxadm_msg_data *pIp_device_data, long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_ip_addr, char *old_ip_mask, short old_function, short old_metric,
short  new_eth_intf_id, char *new_ip_addr, char *new_ip_mask, short new_function, short new_metric);

void nmGetIp_route_data (lnxadm_msg_data *pIp_route_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_dest_ip_addr, char *old_gw_ip_addr, char *old_subnet_mask, short old_route_metric, short old_pmtu,
char *new_dest_ip_addr, char *new_gw_ip_addr, char *new_subnet_mask, short new_route_metric, short new_pmtu);

void	nmGetNe_hostname_data (lnxadm_msg_data * pNe_hostname_data, long int mtype, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id);

void nmGetNe_hosts_data (lnxadm_msg_data *pNe_hosts_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_host_srv_ip_addr, char *old_ip_domain_name, char *old_ip_host_name,
char *new_host_srv_ip_addr, char *new_ip_domain_name, char *new_ip_host_name);

void nmGetNe_srv_security_data (lnxadm_msg_data *pNe_srv_security_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_srv_acc_ip_addr, short old_srv_scrt_name, short old_scrt_rule_action,
char *new_srv_acc_ip_addr, short new_srv_scrt_name, short new_scrt_rule_action);

void nmGetNe_user_data (lnxadm_msg_data *pNe_user_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_username, char *old_user_password, char *old_calling_station_id, char *new_username, char *new_user_password, char *new_calling_station_id);

void	nmGetNodeData (lnxadm_msg_data * pNodeData, long int mtype, char * TableName, short Data_Op_Type,
char old_prefered_unit_id, char old_geo_mode, char old_geoswtch_timeout, short old_pref_eth_port_id,
char new_prefered_unit_id, char new_geo_mode, char new_geoswtch_timeout, short new_pref_eth_port_id);

void nmGetOs_user_group_data (lnxadm_msg_data *pOs_user_group_data, long int mtype, char *TableName, short Data_Op_Type,
short old_os_user_group_id, char *old_os_group_name, short new_os_user_group_id, char *new_os_group_name);

void nmGetPort_data (lnxadm_msg_data *pPort_data, long int mtype, char *TableName, short Data_Op_Type, short  old_portnr,
short  old_port_id, short  old_boardnr, short  old_port_equip, short  old_port_oosi, short old_sigtrace_req, short  old_port_mlb,
short  old_log_stat_group_id, short  old_port_profile_type, short  old_port_profile_id, char *old_port_sys_name, short  new_portnr, short  new_port_id, short  new_boardnr,
short  new_port_equip, short  new_port_oosi, short  new_sigtrace_req, short  new_port_mlb, short  new_log_stat_group_id, short  new_port_profile_type,
short new_port_profile_id, char *new_port_sys_name);

void	nmGetProtocolQos (lnxadm_msg_data * pProtocolQos, long int mtype, char * TableName, short Data_Op_Type, char new_protocol_type, char new_dscp);

void nmGetRlt_user_group_data (lnxadm_msg_data *pRlt_user_group_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_username, short old_os_user_group_id, char *new_username, short new_os_user_group_id);

void nmGetSquid_ne_port_data (lnxadm_msg_data *pSquid_ne_port_data, long int mtype, char *TableName, short Data_Op_Type,
long old_squid_ne_port_nr, long new_squid_ne_port_nr);

void nmGetSyslog_client_data (lnxadm_msg_data *pSyslog_client_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_log_srv_ip, long old_log_srv_udp_port, short old_syslog_state, short old_syslog_severity,
char *new_log_srv_ip, long new_log_srv_udp_port, short new_syslog_state, short new_syslog_severity);

void nmGetpdatanmng_msg_port (nmng_msg_port *pdatanmng_msg_port, long int mtype, char *TableName, short Data_Op_Type,
short UserInfo, long old_nodeid, short old_portnr, short old_port_id, short old_boardnr, short old_port_equip, short old_port_oosi,
short old_sigtrace_req, short old_port_mlb, short old_log_stat_group_id, short old_port_profile_type, short old_port_profile_id, char *old_port_sys_name,
long new_nodeid, short new_portnr, short new_port_id, short new_boardnr, short new_port_equip, short new_port_oosi, short new_sigtrace_req,
short new_port_mlb, short new_log_stat_group_id, short new_port_profile_type, short new_port_profile_id, char *new_port_sys_name);

void	nmGetDnsConfigVoip (GU_T_DNS_CONFIG * pDnsConfig, char * domain_name, char * primary_server, char * secondary_server,
long dns_max_queries, long dns_cache_size, long dns_cache_ttl_max, char * tertiary_server, long srvr_fail_time);

void	nmGetMng_msg_board (mng_msg_board * pMng_msg_board, long int mtype, short Data_Op_Type, short boardnr, short parent_boardnr,
short board_pos, short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id);

void	nmGetBoard_data (lnxadm_msg_data * pBoard_data, long int mtype, char * TableName, short Data_Op_Type,
short boardnr, short parent_boardnr, short old_board_pos, short board_pos,
short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id);

void	nmGetAgcfGlobalData (GU_T_AGCFGLBDATA * pAgcfGlobal, short modulenr, short portnr, short cookie_method,
long agcf_dflt_expires, short agcf_tm_frst_rgstr, short agcf_tm_next_rgstr, short agcf_tm_fail_rgstr, char * agcf_pswd_dflt,
char * cnf_user, char * cnf_host, long max_nr_of_agcf_reg, char impi_add_domain, char * s3pty_user, char * s3pty_host,
char search_unreg_sts, long search_unreg_cycle, short search_unreg_tm, short reg_per_sec_tm, char sys_startup_reg,
long agcf_param1, long agcf_param2, char check_rgstr_subs, char scscf_assgn_type, char mon_nr_of_pings,
char pani_access_type, char * hot_user_code, char hotd_tm, short tm_check_rgstr_sts, char standalone_mode, char cnf_procedure, char pani_hndl);

void	nmGetInterCarrTraff (GU_T_CARRINOUTTRAFFDATA * pCarrTraff, short cid, long cid_overplus,
long cid_inc_traffic, long cid_out_traffic, long cid_traffic_diff, short cid_dyndistprio);

void	nmGetIMS_GlobalLicence (GU_T_IMS_GLOBALLICENCE * GlbLicence, char * ims_bsts, char * ims_asts, long ims_tm_valid, char * ims_ssts);

void	nmGetIMS_Global (GU_T_IMS_GLOBAL * ImsGlobal, short ims_cscf_type, short ims_diam_variant, short ims_nr_of_impu, char * ims_homedomain,
long ims_param1, long ims_param2, short ims_saporttype, long ims_as_mode, short ims_ndc_id, short tariff_dir, char siptoteluri, char sh_activation, 
char handle_uri_num, char survive_mode, short spec_func, char user_delete);

void	nmGetIMS_GlobalREG (GU_T_IMS_GLOBALREG * ImsGlobal, long nodeid, char * ims_ownnetworkid,
long ims_nr_of_acc, short ims_auth_alg, short ims_ipsec, short pcscf_subscription, short ims_min_expires, long ims_max_expires,
short sip_feature_id, char ims_3gpprls, short typesc, char ims_magcf, char * ims_bsts, char * ims_asts,
long ims_tm_valid, char replica_synch, char impi_add_domain, char store_srv_prf, char ims_chrg_mode, char alarm_sts, 
char * ims_ssts, short user_properties);

void	nmGetIMS_NeConfig (GU_T_IMS_GLOBAL * ImsGlobal, long nodeid, short ims_ne_type,
short ims_ne_useownip, long ims_ne_port, char * ims_ne_uri, short ims_ne_transport, short stat_group_id);

void	nmGetSigtraceNumbers (GU_T_STRCNRDATA * StrcNrData, short trnkgrpid, char sigtrace_nr_type, char * sigtrace_nr, short sigtrace_req);

void	nmGetPingTestData (nmng_ping_test * pPingTest, long int my_type, char * TableName, short Data_Op_Type,
long old_rtt_min_threshold, long old_rtt_max_threshold, short old_test_period,
long new_rtt_min_threshold, long new_rtt_max_threshold, short new_test_period);

void	nmGetIMS_Net (GU_T_IMS_ELEMENT * ImsElement, char include, short ims_dns_type, char * ims_ip_addr, long ims_ip_port,
short ims_ip_transport, char * ims_uri, short d_attr_id, short tm_retransmit, short stat_group_id, short ims_rmt_device_id, long cpb_set_id, char * realm_domain);

void	nmGetCRSrvSts (GU_T_CRSRVSTS * CRSrvSts, char crcg_state, char crcb_state, char crr_state);

void	nmGetCRFeature (GU_T_CRDATADB * CRFeature, short cr_grp_id, char * cr_grp_name, long crcg_interval, char crcb_percent, char crr_max_conn);

void	nmGetGlobalTrnkDataConfig (GU_T_GLOBALTRNKDATACONFIG * TrnkConfig, short cbact_list_id, char cbact_call_rej, char sldr_mode, char li_prefix_act);

void	nmGetSIP_Transaction (GU_T_SIPTRANSACTION * SIP_Trans, long nodeid, long ims_max_nr,
short alr_report_val, short alr_remove_val, short alr_avg_time, char * resp_ims_max_nr, long ims_tm_valid);

void	nmGetSIP_SubsData (GU_T_SIP_SUBSDATA * SubsData, long nodeid, short modulenr, short portnr,
char * acc_ip_addr, char * acc_sec_ip_addr, char * acc_eth_addr, char * acc_sec_eth_addr, short acc_eth_ident_mode,
short sip_feature_id, short sigtrace_req, short mad_id, char * sip_subs_alias, short sip_alias_type, short sip_acc_param1, long sip_acc_param2,
char * sip_acc_param3, short reg_mode, char * sip_key, long expires, short sip_authent_mode, short sip_display, short local_tone,
long acc_ip_port, char * sip_domain_name, short ims_routeid, long subscribe_expires, char * md5_nonce,
char acc_ip_transport, short pri_ccf_id, short sec_ccf_id, char ims_alarm_sts, char * p_gruu,
short refer_notify, char refer_handling, char ims_reg_entity, char sip_acc_alrind, char * acc_cntct, char integrity_protected, char rem_auth_req);

void	nmGetFMCCommonData (GU_T_FMCCOMMONDATA * FMC, char fmc_crosspath, char fmc_mobile_acc, char fmc_universal_dialing);

void	nmGetSCP_Global (GU_T_SCPGLOBAL * SCPGlobal, short loop_trnk_dstid, char * cntr_code, short tm_msrnvalidity, short tm_msrncheckvalidity,
char project_spec_id, short tm_loopvalidity, short tm_loopcheckvalidity, char np_foreign_nr_mode, short np_dflt_conv_id);

void	nmGetIP_TrunkSession (GU_T_IPTRUNKSESSION * IpTrunkSession, long nodeid, long max_ip_trunk_sess,
short alr_report_val, short alr_remove_val, char * resp_ip_trunk_sess, long ip_trunk_tm_valid, long max_fmc_sess, char * resp_fmc_sess);

void	nmGetRegSysParamArp (nmng_reg_sys_param_data * Arp_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_ip_addr, char * old_mac_addr, short old_arp_proxy, char * new_ip_addr, char * new_mac_addr, short new_arp_proxy);

void	nmGetRegSysParamIpDevice (nmng_reg_sys_param_data * Ip_device_data, long int my_type, char * TableName, short Data_Op_Type,
short old_eth_intf_id, char * old_ip_addr, char * old_ip_mask, short old_function, short old_metric,
short new_eth_intf_id, char * new_ip_addr, char * new_ip_mask, short new_function, short new_metric);

void	nmGetRegSysParamIpRoute (nmng_reg_sys_param_data * Ip_route_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_dest_ip_addr, char * old_gw_ip_addr, char * old_subnet_mask, short old_route_metric, short old_pmtu,
char * new_dest_ip_addr, char * new_gw_ip_addr, char * new_subnet_mask, short new_route_metric, short new_pmtu);

void	nmGetRegSysParamNeHostname (nmng_reg_sys_param_data * Ne_hostname_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id);

void	nmGetRegSysParamNeHosts (nmng_reg_sys_param_data * Ne_hosts_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_host_srv_ip_addr, char * old_ip_domain_name, char * old_ip_host_name,
char * new_host_srv_ip_addr, char * new_ip_domain_name, char * new_ip_host_name);

void	nmGetGU_T_RSTRRANGEDATAStruct (GU_T_RSTRRANGEDATA *pRstrRangeData, long ip_geo_group_id, char *start_rng_ip_addr, char *end_rng_ip_addr);

/*************/
/* FUNCTIONS */
/*************/

/*
 * Signature of function nmGetGU_T_DPDATAStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# IN par      : dpdata     Detection Point Data Structure             #*/
/*#                                                                     #*/
/*# OUT par     : dpdata     Detection Point Data Structure             #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (dpdata) with elements             #*/
/*#               from table: detect_point.                             #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_DPDATAStruct (GU_T_DPDATA * dpdata, short fd_f_dpnr, short fd_f_dpsrvname, short fd_f_dptype,
short seqnr, short fd_f_dparming, short fd_f_dpcriteriaid, short fd_f_dppropertyid, short dpsrvgroup, short dpsrvgroupseqnr)
{
	if (dpdata == NULL)
	{
		return;
	}

	dpdata->fd_f_dpnr = fd_f_dpnr;
	dpdata->fd_f_dpsrvname = fd_f_dpsrvname;
	dpdata->fd_f_dptype = fd_f_dptype;
	dpdata->fd_f_seqnr = seqnr;
	dpdata->fd_f_dparming = fd_f_dparming;
	dpdata->fd_f_dpcriteriaid = fd_f_dpcriteriaid;
	dpdata->fd_f_dppropertyid = fd_f_dppropertyid;
	dpdata->fd_f_dpsrvgroup.fd_f_dpsrvgroupnr = dpsrvgroup;
	dpdata->fd_f_dpsrvgroup.fd_f_dpsrvgroupseqnr = dpsrvgroupseqnr;

	return;
}


/*
 * Signature of function nmGetGU_T_PFXDATAStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pPfxData     Prefix Data Structure                    #*/
/*#                                                                     #*/
/*# OUT par     : pPfxData     Prefix Data Structure                    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pPfxData) with elements           #*/
/*#               from table: prefix.                                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_PFXDATAStruct (GU_T_PFXDATA *pPfxData, short ndcid, char *dstpfx, long bgid, short cgid,
short dstctg, short dstid, short ststgrp, short nplanid)
{
	if ((pPfxData == NULL) || (dstpfx == NULL))
	{
		return;
	}

	pPfxData->fd_f_ndcid = ndcid;
	strcpy (pPfxData->fd_f_pfx, dstpfx);
	pPfxData->fd_f_cgid = cgid;
	pPfxData->fd_f_bgid = bgid;
	pPfxData->fd_f_dstctg = dstctg;
	pPfxData->fd_f_dstid = dstid;
	pPfxData->fd_f_ststgrp = ststgrp;
	pPfxData->fd_f_nplan = nplanid;

	return;
}


/*
 * Signature of function nmGetGU_T_QDATAStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pQdata     Quasi Data Structure                       #*/
/*#                                                                     #*/
/*# OUT par     : pQdata     Quasi Data Structure                       #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pQdata) with elements             #*/
/*#               from table: quasi_inter_call.                         #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_QDATAStruct (GU_T_QDATA *pQdata, short cgido, short cgidt, char *qpfx, short crgcode)
{
	if ((pQdata == NULL) || (qpfx == NULL))
	{
		return;
	}

	pQdata->fd_f_cgido = cgido;
	pQdata->fd_f_cgidt = cgidt;
	strcpy (pQdata->fd_f_qpfx, qpfx);
	pQdata->fd_f_crgcode = crgcode;

	return;
}


/*
 * Signature of function nmGetnmng_msg_tsf_settingsStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : p_nmng_msg_tsf_settings     TSF Settings Structure    #*/
/*#                                                                     #*/
/*# OUT par     : p_nmng_msg_tsf_settings     TSF Settings Structure    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (p_nmng_msg_tsf_settings) with     #*/
/*#               elements from table: tsf_settings.                    #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetnmng_msg_tsf_settingsStruct (nmng_msg_tsf_settings *p_nmng_msg_tsf_settings, long int mtype, char *TableName,
short Data_Op_Type, short UserInfo, long old_nodeid, short old_tsf_settings_id, char *old_ftp_settings,
long new_nodeid, short new_tsf_settings_id, char *new_ftp_settings)
{
	if ((p_nmng_msg_tsf_settings == NULL) || (TableName == NULL) || (old_ftp_settings == NULL) || (new_ftp_settings == NULL))
	{
		return;
	}

	p_nmng_msg_tsf_settings->mtype = NMNG_MESSAGE_TYPE;
	strcpy (p_nmng_msg_tsf_settings->tsf_settings_type.TableName, TableName);
	p_nmng_msg_tsf_settings->tsf_settings_type.Data_Op_Type = Data_Op_Type;
	p_nmng_msg_tsf_settings->tsf_settings_type.UserInfo = UserInfo;

	p_nmng_msg_tsf_settings->tsf_settings_type.new_nodeid = new_nodeid;
	p_nmng_msg_tsf_settings->tsf_settings_type.new_tsf_settings_id = new_tsf_settings_id;
	strcpy (p_nmng_msg_tsf_settings->tsf_settings_type.new_ftp_settings, new_ftp_settings);

	p_nmng_msg_tsf_settings->tsf_settings_type.old_nodeid = old_nodeid;
	p_nmng_msg_tsf_settings->tsf_settings_type.old_tsf_settings_id = old_tsf_settings_id;
	strcpy (p_nmng_msg_tsf_settings->tsf_settings_type.old_ftp_settings, old_ftp_settings);

	return;
}


/*
 * Signature of function nmGetGU_T_TARIFFPFXDATAStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pTariffPfxData     Tariff Prefix Data Structure       #*/
/*#                                                                     #*/
/*# OUT par     : pTariffPfxData     Tariff Prefix Data Structure       #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pTariffPfxData) with elements     #*/
/*#               from table: tariff_prefix.                            #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_TARIFFPFXDATAStruct (GU_T_TARIFFPFXDATA *pTariffPfxData, short ndcid, char *dstpfx, long bgid,
short cid, short dstchcode, short ststgrp, short nplanid)
{
	if ((pTariffPfxData == NULL) || (dstpfx == NULL))
	{
		return;
	}

	pTariffPfxData->fd_f_ndcid = ndcid;
	strcpy (pTariffPfxData->fd_f_pfx, dstpfx);
	pTariffPfxData->fd_f_bgid = bgid;
	pTariffPfxData->fd_f_cid = cid;
	pTariffPfxData->fd_f_dstchcode = dstchcode;
	pTariffPfxData->fd_f_ststgrp = ststgrp;
	pTariffPfxData->fd_f_nplan = nplanid;

	return;
}


/*
 * Signature of function nmGetGU_T_TRUNKStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : ptrunk     Trunk Data Structure                       #*/
/*#                                                                     #*/
/*# OUT par     : ptrunk     Trunk Data Structure                       #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (ptrunk) with elements             #*/
/*#               from table: trunk.                                    #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_TRUNKStruct (GU_T_TRUNK *ptrunk, short module, short port, short ci)
{
	if (ptrunk == NULL)
		return;

	ptrunk->fd_f_module = module;
	ptrunk->fd_f_port = port;
	ptrunk->fd_f_ci = ci;

	return;
}


/*
 * Signature of function nmGetIua_access_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pIua_access_data     IUA Access Data Structure        #*/
/*#                                                                     #*/
/*# OUT par     : pIua_access_data     IUA Access Data Structure        #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pIua_access_data) with elements   #*/
/*#               from table: iua_access.                               #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetIua_access_data (GU_T_IUA_ACCESSDATA *pIua_access_data, long sysintfid, long sysaccessid,
long intiid, char *textiid, short equipped, short block, short state)
{
	if ((pIua_access_data == NULL) || (textiid == NULL))
	{
		return;
	}

	pIua_access_data->fd_f_sysintfid = sysintfid;
	pIua_access_data->fd_f_sysaccessid = sysaccessid;
	pIua_access_data->fd_f_intiid = intiid;
	strcpy(pIua_access_data->fd_f_textiid, textiid);
	pIua_access_data->fd_f_equipped = equipped;
	pIua_access_data->fd_f_block = block;
	pIua_access_data->fd_f_state = state;

	return;
}


/*
 * Signature of function nmGetArp_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pArp_data     Arp Data Structure                      #*/
/*#                                                                     #*/
/*# OUT par     : pArp_data     Arp Data Structure                      #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pArp_data) with elements          #*/
/*#               from table: arp.                                      #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetArp_data (lnxadm_msg_data *pArp_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_ip_addr, char *old_mac_addr, short old_arp_proxy, char *new_ip_addr, char *new_mac_addr, short new_arp_proxy)
{
	if ((pArp_data == NULL) || (TableName == NULL) || (old_ip_addr == NULL) || (old_mac_addr == NULL) ||
		(new_ip_addr == NULL) || (new_mac_addr == NULL))
	{
		return;
	}

	pArp_data->mtype = mtype;
	strcpy (pArp_data->msgData.TableName, TableName);
	pArp_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pArp_data->msgData.lnxadmData.arpData.old_ip_addr, old_ip_addr);
	strcpy (pArp_data->msgData.lnxadmData.arpData.old_mac_addr, old_mac_addr);
	pArp_data->msgData.lnxadmData.arpData.old_arp_proxy = old_arp_proxy;

	strcpy (pArp_data->msgData.lnxadmData.arpData.new_ip_addr, new_ip_addr);
	strcpy (pArp_data->msgData.lnxadmData.arpData.new_mac_addr, new_mac_addr);
	pArp_data->msgData.lnxadmData.arpData.new_arp_proxy = new_arp_proxy;

	return;
}


/*
 * Signature of function nmGetBasic_filter_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# IN par      : pBasic_filter_data     Basic Filter Data Structure    #*/
/*#                                                                     #*/
/*# OUT par     : pBasic_filter_data     Basic Filter Data Structure    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pBasic_filter_data) with elements #*/
/*#               from table: basic_filter.                             #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetBasic_filter_data (lnxadm_msg_data *pBasic_filter_data,long int mtype, char *TableName, short Data_Op_Type,
short old_filter_id, short old_basic_filt_order, short old_basic_filt_action, char *old_src_mac_addr,
char *old_dst_mac_addr, long old_ll_prot, char *old_src_ip_addr, char *old_src_ip_mask, char *old_dst_ip_addr, char *old_dst_ip_mask,
short old_ip_prot, long old_src_porthigh, long old_src_portlow, long old_dst_porthigh , long old_dst_portlow, short old_icmp_type,
short old_tos, short old_precedense, short new_filter_id, short new_basic_filt_order, short new_basic_filt_action,
char *new_src_mac_addr, char *new_dst_mac_addr, long new_ll_prot, char *new_src_ip_addr, char *new_src_ip_mask,
char *new_dst_ip_addr, char *new_dst_ip_mask, short new_ip_prot, long new_src_porthigh, long new_src_portlow,
long new_dst_porthigh, long new_dst_portlow, short new_icmp_type, short new_tos, short new_precedense)
{
	if ((pBasic_filter_data == NULL) || (TableName == NULL) || (old_src_mac_addr == NULL) || (old_dst_mac_addr == NULL) ||
		(old_src_ip_addr == NULL) || (old_src_ip_mask == NULL) || (old_dst_ip_addr == NULL) || (old_dst_ip_mask == NULL) ||
		(new_src_mac_addr == NULL) || (new_dst_mac_addr == NULL) || (new_src_ip_addr == NULL) ||
		(new_src_ip_mask == NULL) || (new_dst_ip_addr == NULL) || (new_dst_ip_mask == NULL))
	{
		return;
	}

	pBasic_filter_data->mtype = mtype;
	strcpy (pBasic_filter_data->msgData.TableName, TableName);
	pBasic_filter_data->msgData.Data_Op_Type = Data_Op_Type;

	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_filter_id = old_filter_id;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_basic_filt_order = old_basic_filt_order;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_basic_filt_action = old_basic_filt_action;
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_src_mac_addr, old_src_mac_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_dst_mac_addr, old_dst_mac_addr);
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_ll_prot = old_ll_prot;
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_src_ip_addr, old_src_ip_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_src_ip_mask, old_src_ip_mask);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_dst_ip_addr, old_dst_ip_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_dst_ip_mask, old_dst_ip_mask);
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_ip_prot = old_ip_prot;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_src_porthigh = old_src_porthigh;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_src_portlow = old_src_portlow;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_dst_porthigh = old_dst_porthigh;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_dst_portlow = old_dst_portlow;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_icmp_type = old_icmp_type;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_tos = old_tos;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.old_precedense = old_precedense;

	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_filter_id = new_filter_id;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_basic_filt_order = new_basic_filt_order;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_basic_filt_action = new_basic_filt_action;
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_src_mac_addr, new_src_mac_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_dst_mac_addr, new_dst_mac_addr);
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_ll_prot = new_ll_prot;
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_src_ip_addr, new_src_ip_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_src_ip_mask, new_src_ip_mask);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_dst_ip_addr, new_dst_ip_addr);
	strcpy (pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_dst_ip_mask, new_dst_ip_mask);
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_ip_prot = new_ip_prot;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_src_porthigh = new_src_porthigh;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_src_portlow = new_src_portlow;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_dst_porthigh = new_dst_porthigh;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_dst_portlow = new_dst_portlow;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_icmp_type = new_icmp_type;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_tos = new_tos;
	pBasic_filter_data->msgData.lnxadmData.basicFilterData.new_precedense = new_precedense;

	return;
}


/*
 * Signature of function nmGetEth_interface_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pEth_interface_data     Eth Interface Data Structure  #*/
/*#                                                                     #*/
/*# OUT par     : pEth_interface_data     Eth Interface Data Structure  #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pEth_interface_data) with         #*/
/*#               elements from table: eth_interface.                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetEth_interface_data (lnxadm_msg_data *pEth_interface_data, long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_eth_intf_name, short  old_modulenr, short  old_portnr, short  old_portnr2, short  old_oosi,
short  old_admin_state, short  old_log_stat_group_id, short  old_interface_type, short  new_eth_intf_id, char *new_eth_intf_name,
short  new_modulenr, short  new_portnr, short  new_portnr2, short  new_oosi, short  new_admin_state, short  new_log_stat_group_id, short new_interface_type)
{
	if ((pEth_interface_data == NULL) || (TableName == NULL) || (old_eth_intf_name == NULL) || (new_eth_intf_name == NULL))
	{
		return;
	}

	pEth_interface_data->mtype = mtype;
	strcpy (pEth_interface_data->msgData.TableName, TableName);
	pEth_interface_data->msgData.Data_Op_Type = Data_Op_Type;

	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_eth_intf_id = old_eth_intf_id;
	strcpy (pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_eth_intf_name, old_eth_intf_name);
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_modulenr = old_modulenr;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_portnr = old_portnr;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_portnr2 = old_portnr2;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_oosi = old_oosi;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_admin_state = old_admin_state;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_log_stat_group_id = old_log_stat_group_id;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.old_interface_type = old_interface_type;

	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_eth_intf_id = new_eth_intf_id;
	strcpy (pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_eth_intf_name, new_eth_intf_name);
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_modulenr = new_modulenr;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_portnr = new_portnr;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_portnr2 = new_portnr2;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_oosi = new_oosi;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_admin_state = new_admin_state;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_log_stat_group_id = new_log_stat_group_id;
	pEth_interface_data->msgData.lnxadmData.ethInterfaceData.new_interface_type = new_interface_type;

	return;
}


/*
 * Signature of function nmGetEth_profile_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pEth_profile_data     Eth Profile Data Structure      #*/
/*#                                                                     #*/
/*# OUT par     : pEth_profile_data     Eth Profile Data Structure      #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pEth_profile_data) with           #*/
/*#               elements from table: eth_profile.                     #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetEth_profile_data (lnxadm_msg_data *pEth_profile_data, long int mtype, char *TableName, short Data_Op_Type,
short old_eth_profile_id, char *old_profile_name, short old_bandwidth, long old_mtu, short old_eth_mode, short new_eth_profile_id,
char *new_profile_name, short new_bandwidth, long new_mtu, short new_eth_mode)
{
	if ((pEth_profile_data == NULL) || (TableName == NULL) || (old_profile_name == NULL) || (new_profile_name == NULL))
	{
		return;
	}

	pEth_profile_data->mtype = mtype;
	strcpy (pEth_profile_data->msgData.TableName, TableName);
	pEth_profile_data->msgData.Data_Op_Type = Data_Op_Type;

	pEth_profile_data->msgData.lnxadmData.ethProfileData.old_eth_profile_id = old_eth_profile_id;
	strcpy (pEth_profile_data->msgData.lnxadmData.ethProfileData.old_profile_name, old_profile_name);
	pEth_profile_data->msgData.lnxadmData.ethProfileData.old_bandwidth = old_bandwidth;
	pEth_profile_data->msgData.lnxadmData.ethProfileData.old_mtu = old_mtu;
	pEth_profile_data->msgData.lnxadmData.ethProfileData.old_eth_mode = old_eth_mode;

	pEth_profile_data->msgData.lnxadmData.ethProfileData.new_eth_profile_id = new_eth_profile_id;
	strcpy (pEth_profile_data->msgData.lnxadmData.ethProfileData.new_profile_name, new_profile_name);
	pEth_profile_data->msgData.lnxadmData.ethProfileData.new_bandwidth = new_bandwidth;
	pEth_profile_data->msgData.lnxadmData.ethProfileData.new_mtu = new_mtu;
	pEth_profile_data->msgData.lnxadmData.ethProfileData.new_eth_mode = new_eth_mode;

	return;
}


/*
 * Signature of function nmGetEth_tag_rule_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pEth_tag_rule_data     Eth Tag Rule Data Structure    #*/
/*#                                                                     #*/
/*# OUT par     : pEth_tag_rule_data     Eth Tag Rule Data Structure    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pEth_tag_rule_data) with          #*/
/*#               elements from table: eth_tag_rule.                    #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetEth_tag_rule_data (lnxadm_msg_data *pEth_tag_rule_data, long int mtype, char *TableName, short Data_Op_Type,
short old_eth_intf_id, short old_vlan_id, short old_acc_untag_frms, short old_snd_tag_frms, short old_ins_prior, short old_prior_value,
short new_eth_intf_id, short new_vlan_id, short new_acc_untag_frms, short new_snd_tag_frms, short new_ins_prior, short new_prior_value)
{
	if ((pEth_tag_rule_data == NULL) || (TableName == NULL))
	{
		return;
	}

	pEth_tag_rule_data->mtype = mtype;
	strcpy (pEth_tag_rule_data->msgData.TableName, TableName);
	pEth_tag_rule_data->msgData.Data_Op_Type = Data_Op_Type;

	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_eth_intf_id = old_eth_intf_id;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_vlan_id = old_vlan_id;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_acc_untag_frms = old_acc_untag_frms;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_snd_tag_frms = old_snd_tag_frms;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_ins_prior = old_ins_prior;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.old_prior_value = old_prior_value;

	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_eth_intf_id = new_eth_intf_id;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_vlan_id = new_vlan_id;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_acc_untag_frms = new_acc_untag_frms;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_snd_tag_frms = new_snd_tag_frms;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_ins_prior = new_ins_prior;
	pEth_tag_rule_data->msgData.lnxadmData.ethTagRuleData.new_prior_value = new_prior_value;

	return;
}


/*
 * Signature of function nmGetFilter_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pFilter_data     Filter Data Structure                #*/
/*#                                                                     #*/
/*# OUT par     : pFilter_data     Filter Data Structure                #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pFilter_data) with elements       #*/
/*#               from table: filter.                                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetFilter_data (lnxadm_msg_data *pFilter_data, long int mtype, char *TableName, short Data_Op_Type,
short old_filter_id, char *old_filter_name, short new_filter_id, char *new_filter_name)
{
	if ((pFilter_data == NULL) || (TableName == NULL) || (old_filter_name == NULL) || (new_filter_name == NULL))
	{
		return;
	}

	pFilter_data->mtype = mtype;
	strcpy (pFilter_data->msgData.TableName, TableName);
	pFilter_data->msgData.Data_Op_Type = Data_Op_Type;

	pFilter_data->msgData.lnxadmData.filterData.old_filter_id = old_filter_id;
	strcpy (pFilter_data->msgData.lnxadmData.filterData.old_filter_name, old_filter_name);
	pFilter_data->msgData.lnxadmData.filterData.new_filter_id = new_filter_id;
	strcpy (pFilter_data->msgData.lnxadmData.filterData.new_filter_name, new_filter_name);

	return;
}


/*
 * Signature of function nmGetFilter_grp_member_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pFilter_grp_member_data     Filter Group Member Data  #*/
/*#                                           Structure                 #*/
/*#                                                                     #*/
/*# OUT par     : pFilter_grp_member_data     Filter Group Member Data  #*/
/*#                                           Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pFilter_grp_member_data) with     #*/
/*#               elements from table: filter_grp_member.               #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetFilter_grp_member_data (lnxadm_msg_data *pFilter_grp_member_data, long int mtype, char *TableName, short Data_Op_Type,
short old_intf_id, short old_sub_type, short old_filter_direction, short old_member_order, short old_filter_id,
short new_intf_id, short new_sub_type, short new_filter_direction, short new_member_order, short new_filter_id)
{
	if ((pFilter_grp_member_data == NULL) || (TableName == NULL))
	{
		return;
	}

	pFilter_grp_member_data->mtype = mtype;
	strcpy (pFilter_grp_member_data->msgData.TableName, TableName);
	pFilter_grp_member_data->msgData.Data_Op_Type = Data_Op_Type;

	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.old_intf_id = old_intf_id;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.old_sub_type = old_sub_type;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.old_filter_direction = old_filter_direction;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.old_member_order = old_member_order;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.old_filter_id = old_filter_id;

	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.new_intf_id = new_intf_id;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.new_sub_type = new_sub_type;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.new_filter_direction = new_filter_direction;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.new_member_order = new_member_order;
	pFilter_grp_member_data->msgData.lnxadmData.filterGrpMemberData.new_filter_id = new_filter_id;

	return;
}


/*
 * Signature of function nmGetIp_device_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pIp_device_data     Ip Device Data Structure          #*/
/*#                                                                     #*/
/*# OUT par     : pIp_device_data     Ip Device Data Structure          #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pIp_device_data) with elements    #*/
/*#               from table: ip_device.                                #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetIp_device_data (lnxadm_msg_data *pIp_device_data, long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_ip_addr, char *old_ip_mask, short old_function, short old_metric,
short  new_eth_intf_id, char *new_ip_addr, char *new_ip_mask, short new_function, short new_metric)
{
	if ((pIp_device_data == NULL) || (TableName == NULL) || (old_ip_addr == NULL) ||
		(old_ip_mask == NULL) || (new_ip_addr == NULL) || (new_ip_mask == NULL))
	{
		return;
	}

	pIp_device_data->mtype = mtype;
	strcpy (pIp_device_data->msgData.TableName, TableName);
	pIp_device_data->msgData.Data_Op_Type = Data_Op_Type;

	pIp_device_data->msgData.lnxadmData.ipDeviceData.old_eth_intf_id = old_eth_intf_id;
	strcpy (pIp_device_data->msgData.lnxadmData.ipDeviceData.old_ip_addr, old_ip_addr);
	strcpy (pIp_device_data->msgData.lnxadmData.ipDeviceData.old_ip_mask, old_ip_mask);
	pIp_device_data->msgData.lnxadmData.ipDeviceData.old_function = old_function;
	pIp_device_data->msgData.lnxadmData.ipDeviceData.old_metric = old_metric;

	pIp_device_data->msgData.lnxadmData.ipDeviceData.new_eth_intf_id = new_eth_intf_id;
	strcpy (pIp_device_data->msgData.lnxadmData.ipDeviceData.new_ip_addr, new_ip_addr);
	strcpy (pIp_device_data->msgData.lnxadmData.ipDeviceData.new_ip_mask, new_ip_mask);
	pIp_device_data->msgData.lnxadmData.ipDeviceData.new_function = new_function;
	pIp_device_data->msgData.lnxadmData.ipDeviceData.new_metric = new_metric;

	return;
}


/*
 * Signature of function nmGetIp_route_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pIp_route_data     Ip Route Data Structure            #*/
/*#                                                                     #*/
/*# OUT par     : pIp_route_data     Ip Route Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pIp_route_data) with elements     #*/
/*#               from table: ip_route.                                 #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetIp_route_data (lnxadm_msg_data *pIp_route_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_dest_ip_addr, char *old_gw_ip_addr, char *old_subnet_mask, short old_route_metric, short old_pmtu,
char *new_dest_ip_addr, char *new_gw_ip_addr, char *new_subnet_mask, short new_route_metric, short new_pmtu)
{
	if ((pIp_route_data == NULL) || (TableName == NULL) || (old_dest_ip_addr == NULL) || (old_gw_ip_addr == NULL) ||
		(old_subnet_mask == NULL) || (new_dest_ip_addr == NULL) || (new_gw_ip_addr == NULL) || (new_subnet_mask == NULL))
	{
		return;
	}

	pIp_route_data->mtype = mtype;
	strcpy (pIp_route_data->msgData.TableName, TableName);
	pIp_route_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.old_dest_ip_addr, old_dest_ip_addr);
	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.old_gw_ip_addr, old_gw_ip_addr);
	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.old_subnet_mask, old_subnet_mask);
	pIp_route_data->msgData.lnxadmData.ipRouteData.old_route_metric = old_route_metric;
	pIp_route_data->msgData.lnxadmData.ipRouteData.old_pmtu = old_pmtu;

	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.new_dest_ip_addr, new_dest_ip_addr);
	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.new_gw_ip_addr, new_gw_ip_addr);
	strcpy (pIp_route_data->msgData.lnxadmData.ipRouteData.new_subnet_mask, new_subnet_mask);
	pIp_route_data->msgData.lnxadmData.ipRouteData.new_route_metric = new_route_metric;
	pIp_route_data->msgData.lnxadmData.ipRouteData.new_pmtu = new_pmtu;

	return;
}


/*
 * Signature of function nmGetNe_hostname_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# IN par      : pNe_hostname_data     Ne Hostname Data Structure      #*/
/*#                                                                     #*/
/*# OUT par     : pNe_hostname_data     Ne Hostname Data Structure      #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pNe_hostname_data) with elements  #*/
/*#               from table: ne_hostname.                              #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetNe_hostname_data (lnxadm_msg_data * pNe_hostname_data, long int mtype, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id)
{
	if ((pNe_hostname_data == NULL) || (TableName == NULL) || (old_hostname == NULL) || (old_hostname1 == NULL) ||
		(old_hostname2 == NULL) || (new_hostname == NULL) || (new_hostname1 == NULL) || (new_hostname2 == NULL))
	{
		return;
	}

	pNe_hostname_data->mtype = mtype;
	strcpy (pNe_hostname_data->msgData.TableName, TableName);
	pNe_hostname_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.old_hostname, old_hostname);
	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.old_hostname1, old_hostname1);
	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.old_hostname2, old_hostname2);
	pNe_hostname_data->msgData.lnxadmData.neHostnameData.old_geosys_unit_id = old_geosys_unit_id;

	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.new_hostname, new_hostname);
	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.new_hostname1, new_hostname1);
	strcpy (pNe_hostname_data->msgData.lnxadmData.neHostnameData.new_hostname2, new_hostname2);
	pNe_hostname_data->msgData.lnxadmData.neHostnameData.new_geosys_unit_id = new_geosys_unit_id;

	return;
}


/*
 * Signature of function nmGetNe_hosts_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pNe_hosts_data     Ne Hosts Data Structure            #*/
/*#                                                                     #*/
/*# OUT par     : pNe_hosts_data     Ne Hosts Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pNe_hosts_data) with elements     #*/
/*#               from table: ne_hosts.                                 #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetNe_hosts_data (lnxadm_msg_data *pNe_hosts_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_host_srv_ip_addr, char *old_ip_domain_name, char *old_ip_host_name,
char *new_host_srv_ip_addr, char *new_ip_domain_name, char *new_ip_host_name)
{
	if ((pNe_hosts_data == NULL) || (TableName == NULL) || (old_host_srv_ip_addr == NULL) || (old_ip_domain_name == NULL) ||
		(old_ip_host_name == NULL) || (new_host_srv_ip_addr == NULL) || (new_ip_domain_name == NULL) || (new_ip_host_name == NULL))
	{
		return;
	}

	pNe_hosts_data->mtype = mtype;
	strcpy (pNe_hosts_data->msgData.TableName, TableName);
	pNe_hosts_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.old_host_srv_ip_addr, old_host_srv_ip_addr);
	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.old_ip_domain_name, old_ip_domain_name);
	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.old_ip_host_name, old_ip_host_name);

	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.new_host_srv_ip_addr, new_host_srv_ip_addr);
	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.new_ip_domain_name, new_ip_domain_name);
	strcpy (pNe_hosts_data->msgData.lnxadmData.neHostsData.new_ip_host_name, new_ip_host_name);

	return;
}


/*
 * Signature of function nmGetNe_srv_security_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pNe_srv_security_data     Ne Srv Security Data        #*/
/*#                                         Structure                   #*/
/*#                                                                     #*/
/*# OUT par     : pNe_srv_security_data     Ne Srv Security Data        #*/
/*#                                         Structure                   #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pNe_srv_security_data) with       #*/
/*#               elements from table: ne_srv_security.                 #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetNe_srv_security_data (lnxadm_msg_data *pNe_srv_security_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_srv_acc_ip_addr, short old_srv_scrt_name, short old_scrt_rule_action,
char *new_srv_acc_ip_addr, short new_srv_scrt_name, short new_scrt_rule_action)
{
	if ((pNe_srv_security_data == NULL) || (TableName == NULL) || (old_srv_acc_ip_addr == NULL) || (new_srv_acc_ip_addr == NULL))
	{
		return;
	}

	pNe_srv_security_data->mtype = mtype;
	strcpy (pNe_srv_security_data->msgData.TableName, TableName);
	pNe_srv_security_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.old_srv_acc_ip_addr, old_srv_acc_ip_addr);
	pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.old_srv_scrt_name = old_srv_scrt_name;
	pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.old_scrt_rule_action = old_scrt_rule_action;

	strcpy (pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.new_srv_acc_ip_addr, new_srv_acc_ip_addr);
	pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.new_srv_scrt_name = new_srv_scrt_name;
	pNe_srv_security_data->msgData.lnxadmData.neSrvSecurityData.new_scrt_rule_action = new_scrt_rule_action;

	return;
}


/*
 * Signature of function nmGetNe_user_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pNe_user_data     Ne User Data Structure              #*/
/*#                                                                     #*/
/*# OUT par     : pNe_user_data     Ne User Data Structure              #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pNe_user_data) with elements      #*/
/*#               from table: ne_user.                                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetNe_user_data (lnxadm_msg_data *pNe_user_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_username, char *old_user_password, char *old_calling_station_id, char *new_username, char *new_user_password, char *new_calling_station_id)
{
	if ((pNe_user_data == NULL) || (TableName == NULL) || (old_username == NULL) || (old_user_password == NULL) ||
		(old_calling_station_id == NULL) || (new_username == NULL) || (new_user_password == NULL) || (new_calling_station_id == NULL))
	{
		return;
	}

	pNe_user_data->mtype = mtype;
	strcpy (pNe_user_data->msgData.TableName, TableName);
	pNe_user_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.old_username, old_username);
	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.old_user_password, old_user_password);
	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.old_calling_station_id, old_calling_station_id);

	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.new_username, new_username);
	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.new_user_password, new_user_password);
	strcpy (pNe_user_data->msgData.lnxadmData.neUserData.new_calling_station_id, new_calling_station_id);

	return;
}


/*
 * Signature of function nmGetNodeData
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pNodeData   Adderss of Node Data Structure            #*/
/*#                                                                     #*/
/*# OUT par     : pNodeData   Node Data Structure                       #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pNodeData) with                   #*/
/*#               elements from table: (node).                          #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetNodeData (lnxadm_msg_data * pNodeData, long int mtype, char * TableName, short Data_Op_Type,
char old_prefered_unit_id, char old_geo_mode, char old_geoswtch_timeout, short old_pref_eth_port_id,
char new_prefered_unit_id, char new_geo_mode, char new_geoswtch_timeout, short new_pref_eth_port_id)
{
	if ((pNodeData == NULL) || (TableName == NULL))
	{
		return;
	}

	pNodeData->mtype = mtype;
	strcpy (pNodeData->msgData.TableName, TableName);
	pNodeData->msgData.Data_Op_Type = Data_Op_Type;

	pNodeData->msgData.lnxadmData.nodeData.old_prefered_unit_id = old_prefered_unit_id;
	pNodeData->msgData.lnxadmData.nodeData.old_geo_mode = old_geo_mode;
	pNodeData->msgData.lnxadmData.nodeData.old_geoswtch_timeout = old_geoswtch_timeout;
	pNodeData->msgData.lnxadmData.nodeData.old_pref_eth_port_id = old_pref_eth_port_id;

	pNodeData->msgData.lnxadmData.nodeData.new_prefered_unit_id = new_prefered_unit_id;
	pNodeData->msgData.lnxadmData.nodeData.new_geo_mode = new_geo_mode;
	pNodeData->msgData.lnxadmData.nodeData.new_geoswtch_timeout = new_geoswtch_timeout;
	pNodeData->msgData.lnxadmData.nodeData.new_pref_eth_port_id = new_pref_eth_port_id;

	return;
}


/*
 * Signature of function nmGetOs_user_group_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pOs_user_group_data     Os User Group Data Structure  #*/
/*#                                                                     #*/
/*# OUT par     : pOs_user_group_data     Os User Group Data Structure  #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pOs_user_group_data) with         #*/
/*#               elements from table: os_user_group.                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetOs_user_group_data (lnxadm_msg_data *pOs_user_group_data, long int mtype, char *TableName, short Data_Op_Type,
short old_os_user_group_id, char *old_os_group_name, short new_os_user_group_id, char *new_os_group_name)
{
	if ((pOs_user_group_data == NULL) || (TableName == NULL) || (old_os_group_name == NULL)  || (new_os_group_name == NULL))
	{
		return;
	}

	pOs_user_group_data->mtype = mtype;
	strcpy (pOs_user_group_data->msgData.TableName, TableName);
	pOs_user_group_data->msgData.Data_Op_Type = Data_Op_Type;

	pOs_user_group_data->msgData.lnxadmData.osUserGroupData.old_os_user_group_id = old_os_user_group_id;
	strcpy (pOs_user_group_data->msgData.lnxadmData.osUserGroupData.old_os_group_name, old_os_group_name);
	pOs_user_group_data->msgData.lnxadmData.osUserGroupData.new_os_user_group_id = new_os_user_group_id;
	strcpy (pOs_user_group_data->msgData.lnxadmData.osUserGroupData.new_os_group_name, new_os_group_name);

	return;
}


/*
 * Signature of function nmGetPort_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pPort_data     Port Data Structure                    #*/
/*#                                                                     #*/
/*# OUT par     : pPort_data     Port Data Structure                    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pPort_data) with elements         #*/
/*#               from table: port.                                     #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetPort_data (lnxadm_msg_data *pPort_data, long int mtype, char *TableName, short Data_Op_Type, short  old_portnr,
short  old_port_id, short  old_boardnr, short  old_port_equip, short  old_port_oosi, short old_sigtrace_req, short  old_port_mlb,
short  old_log_stat_group_id, short  old_port_profile_type, short  old_port_profile_id, char *old_port_sys_name, short  new_portnr, short  new_port_id, short  new_boardnr,
short  new_port_equip, short  new_port_oosi, short  new_sigtrace_req, short  new_port_mlb, short  new_log_stat_group_id, short  new_port_profile_type,
short new_port_profile_id, char *new_port_sys_name)
{
	if ((pPort_data == NULL) || (TableName == NULL) || (old_port_sys_name == NULL) || (new_port_sys_name == NULL))
	{
		return;
	}

	pPort_data->mtype = mtype;
	strcpy (pPort_data->msgData.TableName, TableName);
	pPort_data->msgData.Data_Op_Type = Data_Op_Type;

	pPort_data->msgData.lnxadmData.portData.old_portnr = old_portnr;
	pPort_data->msgData.lnxadmData.portData.old_port_id = old_port_id;
	pPort_data->msgData.lnxadmData.portData.old_boardnr = old_boardnr;
	pPort_data->msgData.lnxadmData.portData.old_port_equip = old_port_equip;
	pPort_data->msgData.lnxadmData.portData.old_port_oosi = old_port_oosi;
	pPort_data->msgData.lnxadmData.portData.old_sigtrace_req = old_sigtrace_req;
	pPort_data->msgData.lnxadmData.portData.old_port_mlb = old_port_mlb;
	pPort_data->msgData.lnxadmData.portData.old_log_stat_group_id = old_log_stat_group_id;
	pPort_data->msgData.lnxadmData.portData.old_port_profile_type = old_port_profile_type;
	pPort_data->msgData.lnxadmData.portData.old_port_profile_id = old_port_profile_id;
	strcpy(pPort_data->msgData.lnxadmData.portData.old_port_sys_name, old_port_sys_name);

	pPort_data->msgData.lnxadmData.portData.new_portnr = new_portnr;
	pPort_data->msgData.lnxadmData.portData.new_port_id = new_port_id;
	pPort_data->msgData.lnxadmData.portData.new_boardnr = new_boardnr;
	pPort_data->msgData.lnxadmData.portData.new_port_equip = new_port_equip;
	pPort_data->msgData.lnxadmData.portData.new_port_oosi = new_port_oosi;
	pPort_data->msgData.lnxadmData.portData.new_sigtrace_req = new_sigtrace_req;
	pPort_data->msgData.lnxadmData.portData.new_port_mlb = new_port_mlb;
	pPort_data->msgData.lnxadmData.portData.new_log_stat_group_id = new_log_stat_group_id;
	pPort_data->msgData.lnxadmData.portData.new_port_profile_type = new_port_profile_type;
	pPort_data->msgData.lnxadmData.portData.new_port_profile_id = new_port_profile_id;
	strcpy(pPort_data->msgData.lnxadmData.portData.new_port_sys_name, new_port_sys_name);

	return;
}


/*
 * Signature of function nmGetProtocolQos
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pProtocolQos  Adderss of Protocol QoS Data Structure  #*/
/*#                                                                     #*/
/*# OUT par     : pProtocolQos  Protocol QoS Data Structure             #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pProtocolQos) with                #*/
/*#               elements from table: (protocol_qos).                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetProtocolQos (lnxadm_msg_data * pProtocolQos, long int mtype, char * TableName, short Data_Op_Type, char new_protocol_type, char new_dscp)
{
	if ((pProtocolQos == NULL) || (TableName == NULL))
	{
		return;
	}

	pProtocolQos->mtype = mtype;
	strcpy (pProtocolQos->msgData.TableName, TableName);
	pProtocolQos->msgData.Data_Op_Type = Data_Op_Type;

	pProtocolQos->msgData.lnxadmData.protocolQosData.new_protocol_type = new_protocol_type;
	pProtocolQos->msgData.lnxadmData.protocolQosData.new_dscp = new_dscp;

	return;
}


/*
 * Signature of function nmGetRlt_user_group_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pRlt_user_group_data     Rlt User Group Data          #*/
/*#                                        Structure                    #*/
/*#                                                                     #*/
/*# OUT par     : pRlt_user_group_data     Rlt User Group Data          #*/
/*#                                        Structure                    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pRlt_user_group_data) with        #*/
/*#               elements from table: rlt_user_group.                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetRlt_user_group_data (lnxadm_msg_data *pRlt_user_group_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_username, short old_os_user_group_id, char *new_username, short new_os_user_group_id)
{
	if ((pRlt_user_group_data == NULL) || (TableName == NULL) || (old_username == NULL) || (new_username == NULL))
	{
		return;
	}

	pRlt_user_group_data->mtype = mtype;
	strcpy (pRlt_user_group_data->msgData.TableName, TableName);
	pRlt_user_group_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pRlt_user_group_data->msgData.lnxadmData.rltUserGroupData.old_username, old_username);
	pRlt_user_group_data->msgData.lnxadmData.rltUserGroupData.old_os_user_group_id = old_os_user_group_id;
	strcpy (pRlt_user_group_data->msgData.lnxadmData.rltUserGroupData.new_username, new_username);
	pRlt_user_group_data->msgData.lnxadmData.rltUserGroupData.new_os_user_group_id = new_os_user_group_id;

	return;
}


/*
 * Signature of function nmGetSquid_ne_port_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pSquid_ne_port_data     Squid Ne Port Data Structure  #*/
/*#                                                                     #*/
/*# OUT par     : pSquid_ne_port_data     Squid Ne Port Data Structure  #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pSquid_ne_port_data) with         #*/
/*#               elements from table: squid_ne_port.                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetSquid_ne_port_data (lnxadm_msg_data *pSquid_ne_port_data, long int mtype, char *TableName, short Data_Op_Type,
long old_squid_ne_port_nr, long new_squid_ne_port_nr)
{
	if ((pSquid_ne_port_data == NULL) || (TableName == NULL))
	{
		return;
	}

	pSquid_ne_port_data->mtype = mtype;
	strcpy (pSquid_ne_port_data->msgData.TableName, TableName);
	pSquid_ne_port_data->msgData.Data_Op_Type = Data_Op_Type;

	pSquid_ne_port_data->msgData.lnxadmData.squidNePortData.old_squid_ne_port_nr = old_squid_ne_port_nr;
	pSquid_ne_port_data->msgData.lnxadmData.squidNePortData.new_squid_ne_port_nr = new_squid_ne_port_nr;

	return;
}


/*
 * Signature of function nmGetSyslog_client_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pSyslog_client_data     Syslog Client Data Structure  #*/
/*#                                                                     #*/
/*# OUT par     : pSyslog_client_data     Syslog Client Data Structure  #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pSyslog_client_data) with         #*/
/*#               elements from table: syslog_client.                   #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetSyslog_client_data (lnxadm_msg_data *pSyslog_client_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_log_srv_ip, long old_log_srv_udp_port, short old_syslog_state, short old_syslog_severity,
char *new_log_srv_ip, long new_log_srv_udp_port, short new_syslog_state, short new_syslog_severity)
{
	if ((pSyslog_client_data == NULL) || (TableName == NULL) || (old_log_srv_ip == NULL) || (new_log_srv_ip == NULL))
	{
		return;
	}

	pSyslog_client_data->mtype = mtype;
	strcpy (pSyslog_client_data->msgData.TableName, TableName);
	pSyslog_client_data->msgData.Data_Op_Type = Data_Op_Type;

	strcpy (pSyslog_client_data->msgData.lnxadmData.syslogClientData.old_log_srv_ip, old_log_srv_ip);
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.old_log_srv_udp_port = old_log_srv_udp_port;
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.old_syslog_state = old_syslog_state;
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.old_syslog_severity = old_syslog_severity;

	strcpy (pSyslog_client_data->msgData.lnxadmData.syslogClientData.new_log_srv_ip, new_log_srv_ip);
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.new_log_srv_udp_port = new_log_srv_udp_port;
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.new_syslog_state = new_syslog_state;
	pSyslog_client_data->msgData.lnxadmData.syslogClientData.new_syslog_severity = new_syslog_severity;

	return;
}


/*
 * Signature of function nmGetpdatanmng_msg_port
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pdatanmng_msg_port     Port Data Structure            #*/
/*#                                                                     #*/
/*# OUT par     : pdatanmng_msg_port     Port Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pdatanmng_msg_port) with elements #*/
/*#               from table: port.                                     #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetpdatanmng_msg_port (nmng_msg_port *pdatanmng_msg_port, long int mtype, char *TableName, short Data_Op_Type,
short UserInfo, long old_nodeid, short old_portnr, short old_port_id, short old_boardnr, short old_port_equip, short old_port_oosi,
short old_sigtrace_req, short old_port_mlb, short old_log_stat_group_id, short old_port_profile_type, short old_port_profile_id, char *old_port_sys_name,
long new_nodeid, short new_portnr, short new_port_id, short new_boardnr, short new_port_equip, short new_port_oosi, short new_sigtrace_req,
short new_port_mlb, short new_log_stat_group_id, short new_port_profile_type, short new_port_profile_id, char *new_port_sys_name)
{
	if ((pdatanmng_msg_port == NULL) || (TableName == NULL) || (old_port_sys_name == NULL) || (new_port_sys_name == NULL))
	{
		return;
	}

	pdatanmng_msg_port->mtype = mtype;
	strcpy (pdatanmng_msg_port->port_type.TableName, TableName);
	pdatanmng_msg_port->port_type.Data_Op_Type = Data_Op_Type;
	pdatanmng_msg_port->port_type.UserInfo = UserInfo;

	pdatanmng_msg_port->port_type.old_nodeid = old_nodeid;
	pdatanmng_msg_port->port_type.old_portnr = old_portnr;
	pdatanmng_msg_port->port_type.old_port_id = old_port_id;
	pdatanmng_msg_port->port_type.old_boardnr = old_boardnr;
	pdatanmng_msg_port->port_type.old_port_equip = old_port_equip;
	pdatanmng_msg_port->port_type.old_port_oosi = old_port_oosi;
	pdatanmng_msg_port->port_type.old_sigtrace_req = old_sigtrace_req;
	pdatanmng_msg_port->port_type.old_port_mlb = old_port_mlb;
	pdatanmng_msg_port->port_type.old_log_stat_group_id = old_log_stat_group_id;
	pdatanmng_msg_port->port_type.old_port_profile_type = old_port_profile_type;
	pdatanmng_msg_port->port_type.old_port_profile_id = old_port_profile_id;
	strcpy (pdatanmng_msg_port->port_type.old_port_sys_name, old_port_sys_name);

	pdatanmng_msg_port->port_type.new_nodeid = new_nodeid;
	pdatanmng_msg_port->port_type.new_portnr = new_portnr;
	pdatanmng_msg_port->port_type.new_port_id = new_port_id;
	pdatanmng_msg_port->port_type.new_boardnr = new_boardnr;
	pdatanmng_msg_port->port_type.new_port_equip = new_port_equip;
	pdatanmng_msg_port->port_type.new_port_oosi = new_port_oosi;
	pdatanmng_msg_port->port_type.new_sigtrace_req = new_sigtrace_req;
	pdatanmng_msg_port->port_type.new_port_mlb = new_port_mlb;
	pdatanmng_msg_port->port_type.new_log_stat_group_id = new_log_stat_group_id;
	pdatanmng_msg_port->port_type.new_port_profile_type = new_port_profile_type;
	pdatanmng_msg_port->port_type.new_port_profile_id = new_port_profile_id;
	strcpy (pdatanmng_msg_port->port_type.new_port_sys_name, new_port_sys_name);

	return;
}


/*
 * Signature of function nmGetDnsConfigVoip
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pDnsConfig  Address of DNS Config Data Structure      #*/
/*#                                                                     #*/
/*# OUT par     : pDnsConfig  DNS Config Data Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pDnsConfig) with elements from    #*/
/*#               table: (dns_config_voip).                             #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetDnsConfigVoip (GU_T_DNS_CONFIG * pDnsConfig, char * domain_name, char * primary_server, char * secondary_server,
long dns_max_queries, long dns_cache_size, long dns_cache_ttl_max, char * tertiary_server, long srvr_fail_time)
{
	if ((pDnsConfig == NULL) || (domain_name == NULL) || (primary_server == NULL) ||
		(secondary_server == NULL) || (tertiary_server == NULL))
	{
		return;
	}

	strncpy (pDnsConfig->fd_f_domainname, domain_name, NMNG_MAX_DOMAIN_NAME_L);
	pDnsConfig->fd_f_domainname[NMNG_MAX_DOMAIN_NAME_L] = '\0';
	strncpy (pDnsConfig->fd_f_serverarr[0], primary_server, syn_cv_maxipaddrlen);
	pDnsConfig->fd_f_serverarr[0][syn_cv_maxipaddrlen] = '\0';
	strncpy (pDnsConfig->fd_f_serverarr[1], secondary_server, syn_cv_maxipaddrlen);
	pDnsConfig->fd_f_serverarr[1][syn_cv_maxipaddrlen] = '\0';
	strncpy (pDnsConfig->fd_f_serverarr[2], tertiary_server, syn_cv_maxipaddrlen);
	pDnsConfig->fd_f_serverarr[2][syn_cv_maxipaddrlen] = '\0';
	pDnsConfig->fd_f_maxqueries = dns_max_queries;
	pDnsConfig->fd_f_cachesize = dns_cache_size;
	pDnsConfig->fd_f_cachettlmax = dns_cache_ttl_max;
	pDnsConfig->fd_f_primaryserverfailtime = srvr_fail_time;

	return;
}


/*
 * Signature of function nmGetMng_msg_board
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Klemen Grilc                                          #*/
/*#                                                                     #*/
/*# Revisions   : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# IN par      : pMng_msg_board     Address of Board Data Structure    #*/
/*#                                                                     #*/
/*# OUT par     : pMng_msg_board     Board Data Structure               #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pMng_msg_board) with elements     #*/
/*#               from table: board.                                    #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetMng_msg_board (mng_msg_board * pMng_msg_board, long int mtype, short Data_Op_Type, short boardnr, short parent_boardnr,
short board_pos, short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id)
{
	if ((pMng_msg_board == NULL) || (req_board_id == NULL) || (act_board_id == NULL) || (board_serialnr == NULL) || (board_dsc == NULL))
	{
		return;
	}

	pMng_msg_board->mtype = mtype;
	pMng_msg_board->boardData.Data_Op_Type = Data_Op_Type;

	pMng_msg_board->boardData.boardnr = boardnr;
	pMng_msg_board->boardData.parent_boardnr = parent_boardnr;
	pMng_msg_board->boardData.board_pos = board_pos;
	pMng_msg_board->boardData.board_type = board_type;
	pMng_msg_board->boardData.board_equip = board_equip;
	pMng_msg_board->boardData.board_oosi = board_oosi;
	strcpy (pMng_msg_board->boardData.req_board_id, req_board_id);
	strcpy (pMng_msg_board->boardData.act_board_id, act_board_id);
	strcpy (pMng_msg_board->boardData.board_serialnr, board_serialnr);
	strcpy (pMng_msg_board->boardData.board_dsc, board_dsc);
	pMng_msg_board->boardData.board_profile_type = board_profile_type;
	pMng_msg_board->boardData.board_profile_id = board_profile_id;
	pMng_msg_board->boardData.s_node = s_node;
	pMng_msg_board->boardData.geosys_unit_id = geosys_unit_id;

	return;
}


/*
 * Signature of function nmGetBoard_data
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pBoard_data        Address of Board Data Structure    #*/
/*#                                                                     #*/
/*# OUT par     : pBoard_data        Board Data Structure               #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pBoard_data) with elements        #*/
/*#               from table: (board).                                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetBoard_data (lnxadm_msg_data * pBoard_data, long int mtype, char * TableName, short Data_Op_Type,
short boardnr, short parent_boardnr, short old_board_pos, short board_pos,
short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id)
{
	if ((pBoard_data == NULL) || (TableName == NULL) || (req_board_id == NULL) || (act_board_id == NULL) || (board_serialnr == NULL) || (board_dsc == NULL))
	{
		return;
	}

	pBoard_data->mtype = mtype;
	strcpy (pBoard_data->msgData.TableName, TableName);
	pBoard_data->msgData.Data_Op_Type = Data_Op_Type;

	pBoard_data->msgData.lnxadmData.boardData.Data_Op_Type = Data_Op_Type;
	pBoard_data->msgData.lnxadmData.boardData.boardnr = boardnr;
	pBoard_data->msgData.lnxadmData.boardData.parent_boardnr = parent_boardnr;
	pBoard_data->msgData.lnxadmData.boardData.old_board_pos = old_board_pos;
	pBoard_data->msgData.lnxadmData.boardData.board_pos = board_pos;

	pBoard_data->msgData.lnxadmData.boardData.board_type = board_type;
	pBoard_data->msgData.lnxadmData.boardData.board_equip = board_equip;
	pBoard_data->msgData.lnxadmData.boardData.board_oosi = board_oosi;
	strcpy (pBoard_data->msgData.lnxadmData.boardData.req_board_id, req_board_id);
	strcpy (pBoard_data->msgData.lnxadmData.boardData.act_board_id, act_board_id);
	strcpy (pBoard_data->msgData.lnxadmData.boardData.board_serialnr, board_serialnr);
	strcpy (pBoard_data->msgData.lnxadmData.boardData.board_dsc, board_dsc);
	pBoard_data->msgData.lnxadmData.boardData.board_profile_type = board_profile_type;
	pBoard_data->msgData.lnxadmData.boardData.board_profile_id = board_profile_id;
	pBoard_data->msgData.lnxadmData.boardData.s_node = s_node;
	pBoard_data->msgData.lnxadmData.boardData.geosys_unit_id = geosys_unit_id;

	return;
}


/*
 * Signature of function nmGetAgcfGlobalData
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pAgcfGlobal   Address of AGCF Global Data Structure   #*/
/*#                                                                     #*/
/*# OUT par     : pAgcfGlobal   AGCF Global Data Structure              #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pAgcfGlobal) with elements from   #*/
/*#               table: (agcf_global).                                 #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetAgcfGlobalData (GU_T_AGCFGLBDATA * pAgcfGlobal, short modulenr, short portnr, short cookie_method,
long agcf_dflt_expires, short agcf_tm_frst_rgstr, short agcf_tm_next_rgstr, short agcf_tm_fail_rgstr, char * agcf_pswd_dflt,
char * cnf_user, char * cnf_host, long max_nr_of_agcf_reg, char impi_add_domain, char * s3pty_user, char * s3pty_host,
char search_unreg_sts, long search_unreg_cycle, short search_unreg_tm, short reg_per_sec_tm, char sys_startup_reg,
long agcf_param1, long agcf_param2, char check_rgstr_subs, char scscf_assgn_type, char mon_nr_of_pings,
char pani_access_type, char * hot_user_code, char hotd_tm, short tm_check_rgstr_sts, char standalone_mode, 
char cnf_procedure, char pani_hndl)
{
	if ((pAgcfGlobal == NULL) || (agcf_pswd_dflt == NULL) || (cnf_user == NULL) || (cnf_host == NULL) ||
		(s3pty_user == NULL) || (s3pty_host == NULL) || (hot_user_code == NULL))
	{
		return;
	}

	pAgcfGlobal->fd_f_acc.fd_f_module = modulenr;
	pAgcfGlobal->fd_f_acc.fd_f_port = portnr;

	pAgcfGlobal->fd_f_cookiemethod = cookie_method;
	pAgcfGlobal->fd_f_dfltexpiretime = agcf_dflt_expires;
	pAgcfGlobal->fd_f_tmfirstrgstr = agcf_tm_frst_rgstr;
	pAgcfGlobal->fd_f_tmnextrgstr = agcf_tm_next_rgstr;
	pAgcfGlobal->fd_f_tmfailrgstr = agcf_tm_fail_rgstr;
	strcpy (pAgcfGlobal->fd_f_pswd, agcf_pswd_dflt);
	pAgcfGlobal->fd_f_maxagcfrgstrinst = max_nr_of_agcf_reg;
	pAgcfGlobal->fd_f_impiadddomain = impi_add_domain;
	pAgcfGlobal->fd_f_searchunregsts = search_unreg_sts;
	pAgcfGlobal->fd_f_searchunregcycle = search_unreg_cycle;
	pAgcfGlobal->fd_f_searchunregtm = search_unreg_tm;
	pAgcfGlobal->fd_f_repstm = reg_per_sec_tm;
	pAgcfGlobal->fd_f_sysstartupreg = sys_startup_reg;
	pAgcfGlobal->fd_f_agcfparam1 = agcf_param1;
	pAgcfGlobal->fd_f_agcfparam2 = agcf_param2;
	pAgcfGlobal->fd_f_checkrgstrsts = check_rgstr_subs;
	pAgcfGlobal->fd_f_scscfassgntype = scscf_assgn_type;
	pAgcfGlobal->fd_f_monnrofpings = mon_nr_of_pings;
	pAgcfGlobal->fd_f_accesstype = pani_access_type;
	strcpy (pAgcfGlobal->fd_f_hotusercode, hot_user_code);
	pAgcfGlobal->fd_f_hotdtm = hotd_tm;
	pAgcfGlobal->fd_f_tmcheckrgstrsts = tm_check_rgstr_sts;
	pAgcfGlobal->fd_f_standalonemode = standalone_mode;
	pAgcfGlobal->fd_f_conferenceprocedure = cnf_procedure;
	pAgcfGlobal->fd_f_panihndl = pani_hndl;

	strcpy (pAgcfGlobal->fd_f_conferenceuri.fd_f_alias, cnf_user);
	strcpy (pAgcfGlobal->fd_f_conferenceuri.fd_f_domain, cnf_host);
	pAgcfGlobal->fd_f_conferenceuri.fd_f_uritype = syn_cv_uritypesip;

	if ((strlen(cnf_user) > 0) && (strlen(cnf_host) > 0))
	{
		pAgcfGlobal->fd_f_conferenceuri.fd_f_include = SDL_TRUE;
	}
	else
	{
		pAgcfGlobal->fd_f_conferenceuri.fd_f_include = SDL_FALSE;
	}

	strcpy (pAgcfGlobal->fd_f_3pty_uri.fd_f_alias, s3pty_user);
	strcpy (pAgcfGlobal->fd_f_3pty_uri.fd_f_domain, s3pty_host);
	pAgcfGlobal->fd_f_3pty_uri.fd_f_uritype = syn_cv_uritypesip;

	if ((strlen(s3pty_user) > 0) && (strlen(s3pty_host) > 0))
	{
		pAgcfGlobal->fd_f_3pty_uri.fd_f_include = SDL_TRUE;
	}
	else
	{
		pAgcfGlobal->fd_f_3pty_uri.fd_f_include = SDL_FALSE;
	}

	return;
}


/*
 * Signature of function nmGetInterCarrTraff
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pCarrTraff  Address of Carrier Traffic Data Structure #*/
/*#                                                                     #*/
/*# OUT par     : pCarrTraff  Carrier Traffic Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pCarrTraff) with elements from    #*/
/*#               table: (inter_carr_traff).                            #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetInterCarrTraff (GU_T_CARRINOUTTRAFFDATA * pCarrTraff, short cid, long cid_overplus,
long cid_inc_traffic, long cid_out_traffic, long cid_traffic_diff, short cid_dyndistprio)
{
	if (pCarrTraff == NULL)
	{
		return;
	}

	pCarrTraff->fd_f_cid = cid;
	pCarrTraff->fd_f_allowedoverplus = cid_overplus;
	pCarrTraff->fd_f_inctraffic = cid_inc_traffic;
	pCarrTraff->fd_f_outtraffic = cid_out_traffic;
	pCarrTraff->fd_f_trafficdiff = cid_traffic_diff;
	pCarrTraff->fd_f_dyndistprio = cid_dyndistprio;

	return;
}


/*
 * Signature of function nmGetIMS_GlobalLicence
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : GlbLicence  Address of IMS Global Licence Structure   #*/
/*#                                                                     #*/
/*# OUT par     : GlbLicence  IMS Global Licence Structure              #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (GlbLicence) with elements:        #*/
/*#               (ims_bsts, ims_asts, ims_tm_valid) from table:        #*/
/*#               (ims_global).                                         #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIMS_GlobalLicence (GU_T_IMS_GLOBALLICENCE * GlbLicence, char * ims_bsts, char * ims_asts, long ims_tm_valid, char * ims_ssts)
{
	if ((GlbLicence == NULL) || (ims_bsts == NULL) || (ims_asts == NULL) || (ims_ssts == NULL))
	{
		return;
	}

	strcpy (GlbLicence->fd_f_bsts, ims_bsts);
	strcpy (GlbLicence->fd_f_asts, ims_asts);
	GlbLicence->fd_f_tmvalid = ims_tm_valid;
	strcpy (GlbLicence->fd_f_ssts, ims_ssts);

	return;
}


/*
 * Signature of function nmGetIMS_Global
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : ImsGlobal   Address of IMS Global Data Structure      #*/
/*#                                                                     #*/
/*# OUT param   : ImsGlobal   IMS Global Data Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (ImsGlobal) with elements from     #*/
/*#               table: (ims_global).                                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIMS_Global (GU_T_IMS_GLOBAL * ImsGlobal, short ims_cscf_type, short ims_diam_variant, short ims_nr_of_impu, char * ims_homedomain,
long ims_param1, long ims_param2, short ims_saporttype, long ims_as_mode, short ims_ndc_id, short tariff_dir, char siptoteluri, char sh_activation, 
char handle_uri_num, char survive_mode, short spec_func, char user_delete)
{
	if ((ImsGlobal == NULL) || (ims_homedomain == NULL))
	{
		return;
	}

	ImsGlobal->fd_f_type = ims_cscf_type;
	ImsGlobal->fd_f_diamvariant = ims_diam_variant;
	ImsGlobal->fd_f_nrofimpu = ims_nr_of_impu;
	strcpy (ImsGlobal->fd_f_homedomain, ims_homedomain);
	ImsGlobal->fd_f_param1 = ims_param1;
	ImsGlobal->fd_f_param2 = ims_param2;
	ImsGlobal->fd_f_saportalloctype = ims_saporttype;
	ImsGlobal->fd_f_asmode = ims_as_mode;
	ImsGlobal->fd_f_ndcid = ims_ndc_id;
	ImsGlobal->fd_f_tariffdir = tariff_dir;
	ImsGlobal->fd_f_siptoteluri = siptoteluri;
	ImsGlobal->fd_f_shactivation = sh_activation;
	ImsGlobal->fd_f_numinsipuri = handle_uri_num;
	ImsGlobal->fd_f_survivemode = survive_mode;
	ImsGlobal->fd_f_specfunc = spec_func;
//	ImsGlobal->fd_f_postponeuserdelete = user_delete;

	ImsGlobal->fd_f_pcscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_pcscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_pcscf.fd_f_uri, "");
	ImsGlobal->fd_f_pcscf.fd_f_transport = 0;
	ImsGlobal->fd_f_pcscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_icscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_icscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_icscf.fd_f_uri, "");
	ImsGlobal->fd_f_icscf.fd_f_transport = 0;
	ImsGlobal->fd_f_icscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_scscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_scscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_scscf.fd_f_uri, "");
	ImsGlobal->fd_f_scscf.fd_f_transport = 0;
	ImsGlobal->fd_f_scscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_as.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_as.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_as.fd_f_uri, "");
	ImsGlobal->fd_f_as.fd_f_transport = 0;
	ImsGlobal->fd_f_as.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_agcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_agcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_agcf.fd_f_uri, "");
	ImsGlobal->fd_f_agcf.fd_f_transport = 0;
	ImsGlobal->fd_f_agcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_mgcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_mgcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_mgcf.fd_f_uri, "");
	ImsGlobal->fd_f_mgcf.fd_f_transport = 0;
	ImsGlobal->fd_f_mgcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_bgcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_bgcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_bgcf.fd_f_uri, "");
	ImsGlobal->fd_f_bgcf.fd_f_transport = 0;
	ImsGlobal->fd_f_bgcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_ecscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_ecscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_ecscf.fd_f_uri, "");
	ImsGlobal->fd_f_ecscf.fd_f_transport = 0;
	ImsGlobal->fd_f_ecscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_sccas.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_sccas.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_sccas.fd_f_uri, "");
	ImsGlobal->fd_f_sccas.fd_f_transport = 0;
	ImsGlobal->fd_f_sccas.fd_f_statgrpid = 0;

	return;
}


/*
 * Signature of function nmGetIMS_GlobalREG
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : ImsGlobal   Address of IMS Global Registration Data   #*/
/*#                           Structure                                 #*/
/*#                                                                     #*/
/*# OUT param   : ImsGlobal   IMS Global Registration Data Structure    #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (ImsGlobal) with elements from     #*/
/*#               table: (ims_global).                                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIMS_GlobalREG (GU_T_IMS_GLOBALREG * ImsGlobal, long nodeid, char * ims_ownnetworkid,
long ims_nr_of_acc, short ims_auth_alg, short ims_ipsec, short pcscf_subscription, short ims_min_expires, long ims_max_expires,
short sip_feature_id, char ims_3gpprls, short typesc, char ims_magcf, char * ims_bsts, char * ims_asts,
long ims_tm_valid, char replica_synch, char impi_add_domain, char store_srv_prf, char ims_chrg_mode, 
char alarm_sts, char * ims_ssts, short user_properties)
{
	if ((ImsGlobal == NULL) || (ims_ownnetworkid == NULL) || (ims_bsts == NULL) || (ims_asts == NULL) || (ims_ssts == NULL))
	{
		return;
	}

	ImsGlobal->fd_f_nodeid = nodeid;
	strcpy (ImsGlobal->fd_f_ownnetid, ims_ownnetworkid);
	ImsGlobal->fd_f_nrofacc = ims_nr_of_acc;
	ImsGlobal->fd_f_authalg = ims_auth_alg;
	ImsGlobal->fd_f_secmechname = ims_ipsec;
	ImsGlobal->fd_f_pcscfsubscription = pcscf_subscription;
	ImsGlobal->fd_f_minexpires = ims_min_expires;
	ImsGlobal->fd_f_maxexpires = ims_max_expires;
	ImsGlobal->fd_f_sipfeatureid = sip_feature_id;
	ImsGlobal->fd_f_3gpprelease = ims_3gpprls;
	ImsGlobal->fd_f_typeofsc = typesc;
	ImsGlobal->fd_f_magcf = ims_magcf;
	strcpy (ImsGlobal->fd_f_bsts, ims_bsts);
	strcpy (ImsGlobal->fd_f_asts, ims_asts);
	ImsGlobal->fd_f_tmvalid = ims_tm_valid;
	ImsGlobal->fd_f_replicasynch = replica_synch;
	ImsGlobal->fd_f_impiadddomain = impi_add_domain;
	ImsGlobal->fd_f_storesrvprf = store_srv_prf;
	ImsGlobal->fd_f_chrgmode = ims_chrg_mode;
	ImsGlobal->fd_f_alarmsts = alarm_sts;
	strcpy (ImsGlobal->fd_f_ssts, ims_ssts);
	ImsGlobal->fd_f_userproperties = user_properties;

	return;
}


/*
 * Signature of function nmGetIMS_NeConfig
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : ImsGlobal   Address of IMS Global Data Structure      #*/
/*#                                                                     #*/
/*# OUT param   : ImsGlobal   IMS Global Data Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (ImsGlobal) with elements from     #*/
/*#               table: (ims_ne_config).                               #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIMS_NeConfig (GU_T_IMS_GLOBAL * ImsGlobal, long nodeid, short ims_ne_type,
short ims_ne_useownip, long ims_ne_port, char * ims_ne_uri, short ims_ne_transport, short stat_group_id)
{
	if ((ImsGlobal == NULL) || (ims_ne_uri == NULL))
	{
		return;
	}

	ImsGlobal->fd_f_type = syn_cv_cscf_type_undefined;
	ImsGlobal->fd_f_diamvariant = syn_cv_tispandiamvariant;
	ImsGlobal->fd_f_nrofimpu = 3;
	strcpy (ImsGlobal->fd_f_homedomain, "");
	ImsGlobal->fd_f_param1 = 0;
	ImsGlobal->fd_f_param2 = 0;
	ImsGlobal->fd_f_saportalloctype = syn_cv_sa_fixedports;
	ImsGlobal->fd_f_asmode = syn_cv_undefined_as_mode;
	ImsGlobal->fd_f_ndcid = 1;
	ImsGlobal->fd_f_tariffdir = 0;
	ImsGlobal->fd_f_siptoteluri = 1;
	ImsGlobal->fd_f_shactivation = syn_cv_diameter_disablesh;
	ImsGlobal->fd_f_numinsipuri = syn_cv_dummynumhandling;

	ImsGlobal->fd_f_pcscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_pcscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_pcscf.fd_f_uri, "");
	ImsGlobal->fd_f_pcscf.fd_f_transport = 0;
	ImsGlobal->fd_f_pcscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_icscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_icscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_icscf.fd_f_uri, "");
	ImsGlobal->fd_f_icscf.fd_f_transport = 0;
	ImsGlobal->fd_f_icscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_scscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_scscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_scscf.fd_f_uri, "");
	ImsGlobal->fd_f_scscf.fd_f_transport = 0;
	ImsGlobal->fd_f_scscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_as.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_as.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_as.fd_f_uri, "");
	ImsGlobal->fd_f_as.fd_f_transport = 0;
	ImsGlobal->fd_f_as.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_agcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_agcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_agcf.fd_f_uri, "");
	ImsGlobal->fd_f_agcf.fd_f_transport = 0;
	ImsGlobal->fd_f_agcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_mgcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_mgcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_mgcf.fd_f_uri, "");
	ImsGlobal->fd_f_mgcf.fd_f_transport = 0;
	ImsGlobal->fd_f_mgcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_bgcf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_bgcf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_bgcf.fd_f_uri, "");
	ImsGlobal->fd_f_bgcf.fd_f_transport = 0;
	ImsGlobal->fd_f_bgcf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_ecscf.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_ecscf.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_ecscf.fd_f_uri, "");
	ImsGlobal->fd_f_ecscf.fd_f_transport = 0;
	ImsGlobal->fd_f_ecscf.fd_f_statgrpid = 0;

	ImsGlobal->fd_f_sccas.fd_f_useownip = SDL_FALSE;
	ImsGlobal->fd_f_sccas.fd_f_port = 0;
	strcpy (ImsGlobal->fd_f_sccas.fd_f_uri, "");
	ImsGlobal->fd_f_sccas.fd_f_transport = 0;
	ImsGlobal->fd_f_sccas.fd_f_statgrpid = 0;

	switch (ims_ne_type)
	{
		case syn_cv_cscf_type_p:
			ImsGlobal->fd_f_pcscf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_pcscf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_pcscf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_pcscf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_pcscf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_i:
			ImsGlobal->fd_f_icscf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_icscf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_icscf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_icscf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_icscf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_s:
			ImsGlobal->fd_f_scscf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_scscf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_scscf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_scscf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_scscf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_as:
			ImsGlobal->fd_f_as.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_as.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_as.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_as.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_as.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_agcf:
			ImsGlobal->fd_f_agcf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_agcf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_agcf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_agcf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_agcf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_mgcf:
			ImsGlobal->fd_f_mgcf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_mgcf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_mgcf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_mgcf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_mgcf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_bgcf:
			ImsGlobal->fd_f_bgcf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_bgcf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_bgcf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_bgcf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_bgcf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_e:
			ImsGlobal->fd_f_ecscf.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_ecscf.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_ecscf.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_ecscf.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_ecscf.fd_f_statgrpid = stat_group_id;
			break;

		case syn_cv_cscf_type_sccas:
			ImsGlobal->fd_f_sccas.fd_f_useownip = ims_ne_useownip;
			ImsGlobal->fd_f_sccas.fd_f_port = ims_ne_port;
			strcpy (ImsGlobal->fd_f_sccas.fd_f_uri, ims_ne_uri);
			ImsGlobal->fd_f_sccas.fd_f_transport = ims_ne_transport;
			ImsGlobal->fd_f_sccas.fd_f_statgrpid = stat_group_id;

		default:
			break;
	}

	return;
}


/*
 * Signature of function nmGetSigtraceNumbers
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : StrcNrData       Address of Signal Trace Number Data  #*/
/*#                                Structure                            #*/
/*#                                                                     #*/
/*# OUT par     : StrcNrData       Signal Trace Number Data Structure   #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (StrcNrData) with elements from    #*/
/*#               table: (sigtrace_numbers).                            #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetSigtraceNumbers (GU_T_STRCNRDATA * StrcNrData, short trnkgrpid, char sigtrace_nr_type, char * sigtrace_nr, short sigtrace_req)
{
	if ((StrcNrData == NULL) || (sigtrace_nr == NULL))
	{
		return;
	}

	StrcNrData->fd_f_trnkgrpid = trnkgrpid;
	StrcNrData->fd_f_nrtype = sigtrace_nr_type;
	strncpy (StrcNrData->fd_f_number, sigtrace_nr, NMNG_SIGTRACE_NR);
	StrcNrData->fd_f_number[NMNG_SIGTRACE_NR] = '\0';
	StrcNrData->fd_f_sigtracereq = sigtrace_req;

	return;
}


/*
 * Signature of function nmGetPingTestData
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pPingTest   Adderss of Ping Test Data Structure       #*/
/*#                                                                     #*/
/*# OUT par     : pPingTest   Ping Test Data Structure                  #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pPingTest) with                   #*/
/*#               elements from table: (ping_test).                     #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetPingTestData (nmng_ping_test * pPingTest, long int my_type, char * TableName, short Data_Op_Type,
long old_rtt_min_threshold, long old_rtt_max_threshold, short old_test_period,
long new_rtt_min_threshold, long new_rtt_max_threshold, short new_test_period)
{
	if ((pPingTest == NULL) || (TableName == NULL))
	{
		return;
	}

	pPingTest->my_type = my_type;
	strcpy (pPingTest->PingTestData.TableName, TableName);
	pPingTest->PingTestData.Data_Op_Type = Data_Op_Type;

	pPingTest->PingTestData.old_rtt_min_threshold = old_rtt_min_threshold;
	pPingTest->PingTestData.old_rtt_max_threshold = old_rtt_max_threshold;
	pPingTest->PingTestData.old_test_period = old_test_period;

	pPingTest->PingTestData.new_rtt_min_threshold = new_rtt_min_threshold;
	pPingTest->PingTestData.new_rtt_max_threshold = new_rtt_max_threshold;
	pPingTest->PingTestData.new_test_period = new_test_period;

	return;
}


/*
 * Signature of function nmGetIMS_Net
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : ImsElement  Address of IMS Element Structure          #*/
/*#                                                                     #*/
/*# OUT param   : ImsElement  IMS Element Structure                     #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (ImsElement) with elements from    #*/
/*#               table: (ims_net).                                     #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIMS_Net (GU_T_IMS_ELEMENT * ImsElement, char include, short ims_dns_type, char * ims_ip_addr, long ims_ip_port,
short ims_ip_transport, char * ims_uri, short d_attr_id, short tm_retransmit, short stat_group_id, short ims_rmt_device_id, long cpb_set_id, char * realm_domain)
{
	if ((ImsElement == NULL) || (ims_ip_addr == NULL) || (ims_uri == NULL) || (realm_domain == NULL))
	{
		return;
	}

	if (include == 0)
	{
		ImsElement->fd_f_include = SDL_FALSE;
	}
	else
	{
		ImsElement->fd_f_include = SDL_TRUE;
	}

	ImsElement->fd_f_dnstype = ims_dns_type;

	strcpy (ImsElement->fd_f_addr.fd_f_sendaddr, ims_ip_addr);
	ImsElement->fd_f_addr.fd_f_sendport = ims_ip_port;
	ImsElement->fd_f_addr.fd_f_trnsprt = ims_ip_transport;

	strcpy (ImsElement->fd_f_uri, ims_uri);
	ImsElement->fd_f_attrid = d_attr_id;
	ImsElement->fd_f_tmretransmit = tm_retransmit;
	ImsElement->fd_f_statgroupid = stat_group_id;
	ImsElement->fd_f_rmtdeviceid = ims_rmt_device_id;
	ImsElement->fd_f_cpbsetid = cpb_set_id;
	strcpy (ImsElement->fd_f_realmdomain, realm_domain);

	return;
}


/*
 * Signature of function nmGetCRSrvSts
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : CRSrvSts    Address of Call Reduction Service Status  #*/
/*#                           Structure                                 #*/
/*#                                                                     #*/
/*# OUT param   : CRSrvSts    Call Reduction Service Status Structure   #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (CRSrvSts) with elements of        #*/
/*#               attributes: (crcg_state, crcb_state, crr_state)       #*/
/*#               from table: (start_execution).                        #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetCRSrvSts (GU_T_CRSRVSTS * CRSrvSts, char crcg_state, char crcb_state, char crr_state)
{
	if (CRSrvSts == NULL)
	{
		return;
	}

	CRSrvSts->fd_f_crcgind = crcg_state;
	CRSrvSts->fd_f_crcbind = crcb_state;
	CRSrvSts->fd_f_crrind = crr_state;

	return;
}


/*
 * Signature of function nmGetCRFeature
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : CRFeature   Address of Call Reduction Feature Data    #*/
/*#                           Structure                                 #*/
/*#                                                                     #*/
/*# OUT param   : CRFeature   Call Reduction Feature Data Structure     #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (CRFeature) with elements of       #*/
/*#               attributes from table: (cr_feature).                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetCRFeature (GU_T_CRDATADB * CRFeature, short cr_grp_id, char * cr_grp_name, long crcg_interval, char crcb_percent, char crr_max_conn)
{
	if ((CRFeature == NULL) || (cr_grp_name == NULL))
	{
		return;
	}

	CRFeature->fd_f_crgrpid = cr_grp_id;
	strncpy (CRFeature->fd_f_name, cr_grp_name, NMNG_CR_GRP_NAME);
	CRFeature->fd_f_name[NMNG_CR_GRP_NAME] = '\0';
	CRFeature->fd_f_crcginterval = (SDL_DURATION) crcg_interval;
	CRFeature->fd_f_crcbpercent = crcb_percent;
	CRFeature->fd_f_crrmaxconn = crr_max_conn;

	return;
}


/*
 * Signature of function nmGetGlobalTrnkDataConfig
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : TrnkConfig       Address of Global Trunk Data Config  #*/
/*#                                Structure                            #*/
/*#                                                                     #*/
/*# OUT param   : TrnkConfig       Global Trunk Data Config Structure   #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (TrnkConfig) with elements of      #*/
/*#               attributes: (cbact_list_id, cbact_call_rej,           #*/
/*#               sldr_mode, li_prefix_act) from table: (node).         #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetGlobalTrnkDataConfig (GU_T_GLOBALTRNKDATACONFIG * TrnkConfig, short cbact_list_id, char cbact_call_rej, char sldr_mode, char li_prefix_act)
{
	if (TrnkConfig == NULL)
	{
		return;
	}

	TrnkConfig->fd_f_cbactlistid = cbact_list_id;
	TrnkConfig->fd_f_cbactcallrej = cbact_call_rej;
	TrnkConfig->fd_f_sldrmode = sldr_mode;
	TrnkConfig->fd_f_li_act = li_prefix_act;

	return;
}


/*
 * Signature of function nmGetSIP_Transaction
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : SIP_Trans   Address of SIP Transaction Structure      #*/
/*#                                                                     #*/
/*# OUT param   : SIP_Trans   SIP Transaction Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (SIP_Trans) with elements of       #*/
/*#               attributes from table: (sip_transaction).             #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetSIP_Transaction (GU_T_SIPTRANSACTION * SIP_Trans, long nodeid, long ims_max_nr,
short alr_report_val, short alr_remove_val, short alr_avg_time, char * resp_ims_max_nr, long ims_tm_valid)
{
	if ((SIP_Trans == NULL) || (resp_ims_max_nr == NULL))
	{
		return;
	}

	SIP_Trans->fd_f_nodeid = nodeid;
	SIP_Trans->fd_f_imsmaxnr = ims_max_nr;
	SIP_Trans->fd_f_alrreportval = alr_report_val;
	SIP_Trans->fd_f_alrremoveval = alr_remove_val;
	SIP_Trans->fd_f_alravgtime = alr_avg_time;
	strcpy (SIP_Trans->fd_f_md5resp, resp_ims_max_nr);
	SIP_Trans->fd_f_timevalid = ims_tm_valid;

	return;
}


/*
 * Signature of function nmGetSIP_SubsData
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : SubsData    Address of SIP Subs Data Structure        #*/
/*#                                                                     #*/
/*# OUT param   : SubsData    SIP Subs Data Structure                   #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (SubsData) with elements of        #*/
/*#               attributes from table: (sip_access).                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetSIP_SubsData (GU_T_SIP_SUBSDATA * SubsData, long nodeid, short modulenr, short portnr,
char * acc_ip_addr, char * acc_sec_ip_addr, char * acc_eth_addr, char * acc_sec_eth_addr, short acc_eth_ident_mode,
short sip_feature_id, short sigtrace_req, short mad_id, char * sip_subs_alias, short sip_alias_type, short sip_acc_param1, long sip_acc_param2,
char * sip_acc_param3, short reg_mode, char * sip_key, long expires, short sip_authent_mode, short sip_display, short local_tone,
long acc_ip_port, char * sip_domain_name, short ims_routeid, long subscribe_expires, char * md5_nonce,
char acc_ip_transport, short pri_ccf_id, short sec_ccf_id, char ims_alarm_sts, char * p_gruu,
short refer_notify, char refer_handling, char ims_reg_entity, char sip_acc_alrind, char * acc_cntct, char integrity_protected, char rem_auth_req)
{
	if ((SubsData == NULL) || (acc_ip_addr == NULL) || (acc_sec_ip_addr == NULL) || (acc_eth_addr == NULL) ||
		(acc_sec_eth_addr == NULL) || (sip_subs_alias == NULL) || (sip_acc_param3 == NULL) || (sip_key == NULL) ||
		(sip_domain_name == NULL) || (md5_nonce == NULL) || (p_gruu == NULL) || (acc_cntct == NULL))
	{
		return;
	}

	SubsData->fd_f_nodeid = nodeid;
	SubsData->fd_f_acc.fd_f_module = modulenr;
	SubsData->fd_f_acc.fd_f_port = portnr;

	strcpy (SubsData->fd_f_ipaddr, acc_ip_addr);
	strcpy (SubsData->fd_f_secipaddr, acc_sec_ip_addr);
	strcpy (SubsData->fd_f_ethernetaddr, acc_eth_addr);
	strcpy (SubsData->fd_f_secethernetaddr, acc_sec_eth_addr);
	SubsData->fd_f_ethidentmode = acc_eth_ident_mode;
	SubsData->fd_f_featureid = sip_feature_id;
	SubsData->fd_f_sigtracereq = sigtrace_req;
	SubsData->fd_f_mad_id = mad_id;
	strcpy (SubsData->fd_f_alias, sip_subs_alias);
	SubsData->fd_f_aliastype = sip_alias_type;
	SubsData->fd_f_param1 = sip_acc_param1;
	SubsData->fd_f_param2 = sip_acc_param2;
	strcpy (SubsData->fd_f_param3, sip_acc_param3);
	SubsData->fd_f_regmode = reg_mode;
	strcpy (SubsData->fd_f_key, sip_key);
	SubsData->fd_f_expires = expires;
	SubsData->fd_f_authentmode = sip_authent_mode;
	SubsData->fd_f_dispver = sip_display;
	SubsData->fd_f_genloctone = local_tone;
	SubsData->fd_f_ipport = acc_ip_port;
	strcpy (SubsData->fd_f_domainname, sip_domain_name);
	SubsData->fd_f_routeid = ims_routeid;
	SubsData->fd_f_subscribeexpires = subscribe_expires;
	strcpy (SubsData->fd_f_md5nonce, md5_nonce);
	SubsData->fd_f_iptrnsprt = acc_ip_transport;
	SubsData->fd_f_priccfid = pri_ccf_id;
	SubsData->fd_f_secccfid = sec_ccf_id;
	SubsData->fd_f_imsstsalarm = ims_alarm_sts;
	strcpy (SubsData->fd_f_pgruu, p_gruu);
	SubsData->fd_f_refernotify = refer_notify;
	SubsData->fd_f_referhandling = refer_handling;
	SubsData->fd_f_imsregentity = ims_reg_entity;
	SubsData->fd_f_alrind = sip_acc_alrind;
	strcpy (SubsData->fd_f_contact, acc_cntct);
	SubsData->fd_f_integrityprotected = integrity_protected;
	SubsData->fd_f_remauthreq = rem_auth_req;

	return;
}


/*
 * Signature of function nmGetFMCCommonData
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : FMC         Address of FMC Common Data Structure      #*/
/*#                                                                     #*/
/*# OUT param   : FMC         FMC Common Data Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (FMC) with elements of             #*/
/*#               attributes from table: (node).                        #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetFMCCommonData (GU_T_FMCCOMMONDATA * FMC, char fmc_crosspath, char fmc_mobile_acc, char fmc_universal_dialing)
{
	if (FMC == NULL)
	{
		return;
	}

	FMC->fd_f_crosspath = fmc_crosspath;
	FMC->fd_f_mobileacc = fmc_mobile_acc;
	FMC->fd_f_universaldialing = fmc_universal_dialing;

	return;
}


/*
 * Signature of function nmGetSCP_Global
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : SCPGlobal   Address of SCP Global Data Structure      #*/
/*#                                                                     #*/
/*# OUT param   : SCPGlobal   SCP Global Data Structure                 #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (SCPGlobal) with elements of       #*/
/*#               attributes from table: (scp_global).                  #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetSCP_Global (GU_T_SCPGLOBAL * SCPGlobal, short loop_trnk_dstid, char * cntr_code, short tm_msrnvalidity, short tm_msrncheckvalidity,
char project_spec_id, short tm_loopvalidity, short tm_loopcheckvalidity, char np_foreign_nr_mode, short np_dflt_conv_id)
{
	if ((SCPGlobal == NULL) || (cntr_code == NULL))
	{
		return;
	}

	SCPGlobal->fd_f_localloop = loop_trnk_dstid;
	strcpy (SCPGlobal->fd_f_cntrcode, cntr_code);
	SCPGlobal->fd_f_tm_msrnvalidity = tm_msrnvalidity;
	SCPGlobal->fd_f_tm_msrncheckvalidity = tm_msrncheckvalidity;
	SCPGlobal->fd_f_projectspecid = project_spec_id;
	SCPGlobal->fd_f_tm_loopvalidity = tm_loopvalidity;
	SCPGlobal->fd_f_tm_loopcheckvalidity = tm_loopcheckvalidity;
	SCPGlobal->fd_f_npforeignnrmode = np_foreign_nr_mode;
	SCPGlobal->fd_f_npdfltconvid = np_dflt_conv_id;

	return;
}


/*
 * Signature of function nmGetIP_TrunkSession
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN param    : IpTrunkSession  Address of IP Trunk Session Structure #*/
/*#                                                                     #*/
/*# OUT param   : IpTrunkSession  IP Trunk Session Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (IpTrunkSession) with elements of  #*/
/*#               attributes from table: (ip_trunk_session).            #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetIP_TrunkSession (GU_T_IPTRUNKSESSION * IpTrunkSession, long nodeid, long max_ip_trunk_sess,
short alr_report_val, short alr_remove_val, char * resp_ip_trunk_sess, long ip_trunk_tm_valid, long max_fmc_sess, char * resp_fmc_sess)
{
	if ((IpTrunkSession == NULL) || (resp_ip_trunk_sess == NULL) || (resp_fmc_sess == NULL))
	{
		return;
	}

	IpTrunkSession->fd_f_nodeid = nodeid;
	IpTrunkSession->fd_f_maxiptrunksess = max_ip_trunk_sess;
	IpTrunkSession->fd_f_alrreportval = alr_report_val;
	IpTrunkSession->fd_f_alrremoveval = alr_remove_val;
	strcpy (IpTrunkSession->fd_f_md5resp, resp_ip_trunk_sess);
	IpTrunkSession->fd_f_timevalid = ip_trunk_tm_valid;
	IpTrunkSession->fd_f_maxfmcsess = max_fmc_sess;
	strcpy (IpTrunkSession->fd_f_md5respfmc, resp_fmc_sess);

	return;
}

/*******************************************/
/* Functions for Regular System Parameters */
/*******************************************/

/*
 * Signature of function nmGetRegSysParamArp
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : Arp_data    Address of Arp Data Structure             #*/
/*#                                                                     #*/
/*# OUT par     : Arp_data    Arp Data Structure                        #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (Arp_data) with elements           #*/
/*#               from table: (arp).                                    #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetRegSysParamArp (nmng_reg_sys_param_data * Arp_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_ip_addr, char * old_mac_addr, short old_arp_proxy, char * new_ip_addr, char * new_mac_addr, short new_arp_proxy)
{
	if ((Arp_data == NULL) || (TableName == NULL) || (old_ip_addr == NULL) || (old_mac_addr == NULL) ||
		(new_ip_addr == NULL) || (new_mac_addr == NULL))
	{
		return;
	}

	Arp_data->my_type = my_type;
	strcpy (Arp_data->RegSysParam.TableName, TableName);
	Arp_data->RegSysParam.Data_Op_Type = Data_Op_Type;

	strcpy (Arp_data->RegSysParam.SysParam.arpData.old_ip_addr, old_ip_addr);
	strcpy (Arp_data->RegSysParam.SysParam.arpData.old_mac_addr, old_mac_addr);
	Arp_data->RegSysParam.SysParam.arpData.old_arp_proxy = old_arp_proxy;

	strcpy (Arp_data->RegSysParam.SysParam.arpData.new_ip_addr, new_ip_addr);
	strcpy (Arp_data->RegSysParam.SysParam.arpData.new_mac_addr, new_mac_addr);
	Arp_data->RegSysParam.SysParam.arpData.new_arp_proxy = new_arp_proxy;

	return;
}


/*
 * Signature of function nmGetRegSysParamIpDevice
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : Ip_device_data    Address of Ip Device Data Structure #*/
/*#                                                                     #*/
/*# OUT par     : Ip_device_data    Ip Device Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (Ip_device_data) with elements     #*/
/*#               from table: (ip_device).                              #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetRegSysParamIpDevice (nmng_reg_sys_param_data * Ip_device_data, long int my_type, char * TableName, short Data_Op_Type,
short old_eth_intf_id, char * old_ip_addr, char * old_ip_mask, short old_function, short old_metric,
short new_eth_intf_id, char * new_ip_addr, char * new_ip_mask, short new_function, short new_metric)
{
	if ((Ip_device_data == NULL) || (TableName == NULL) || (old_ip_addr == NULL) ||
		(old_ip_mask == NULL) || (new_ip_addr == NULL) || (new_ip_mask == NULL))
	{
		return;
	}

	Ip_device_data->my_type = my_type;
	strcpy (Ip_device_data->RegSysParam.TableName, TableName);
	Ip_device_data->RegSysParam.Data_Op_Type = Data_Op_Type;

	Ip_device_data->RegSysParam.SysParam.ipDeviceData.old_eth_intf_id = old_eth_intf_id;
	strcpy (Ip_device_data->RegSysParam.SysParam.ipDeviceData.old_ip_addr, old_ip_addr);
	strcpy (Ip_device_data->RegSysParam.SysParam.ipDeviceData.old_ip_mask, old_ip_mask);
	Ip_device_data->RegSysParam.SysParam.ipDeviceData.old_function = old_function;
	Ip_device_data->RegSysParam.SysParam.ipDeviceData.old_metric = old_metric;

	Ip_device_data->RegSysParam.SysParam.ipDeviceData.new_eth_intf_id = new_eth_intf_id;
	strcpy (Ip_device_data->RegSysParam.SysParam.ipDeviceData.new_ip_addr, new_ip_addr);
	strcpy (Ip_device_data->RegSysParam.SysParam.ipDeviceData.new_ip_mask, new_ip_mask);
	Ip_device_data->RegSysParam.SysParam.ipDeviceData.new_function = new_function;
	Ip_device_data->RegSysParam.SysParam.ipDeviceData.new_metric = new_metric;

	return;
}


/*
 * Signature of function nmGetRegSysParamIpRoute
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : Ip_route_data      Address of Ip Route Data Structure #*/
/*#                                                                     #*/
/*# OUT par     : Ip_route_data      Ip Route Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (Ip_route_data) with elements      #*/
/*#               from table: (ip_route).                               #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetRegSysParamIpRoute (nmng_reg_sys_param_data * Ip_route_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_dest_ip_addr, char * old_gw_ip_addr, char * old_subnet_mask, short old_route_metric, short old_pmtu,
char * new_dest_ip_addr, char * new_gw_ip_addr, char * new_subnet_mask, short new_route_metric, short new_pmtu)
{
	if ((Ip_route_data == NULL) || (TableName == NULL) || (old_dest_ip_addr == NULL) || (old_gw_ip_addr == NULL) ||
		(old_subnet_mask == NULL) || (new_dest_ip_addr == NULL) || (new_gw_ip_addr == NULL) || (new_subnet_mask == NULL))
	{
		return;
	}

	Ip_route_data->my_type = my_type;
	strcpy (Ip_route_data->RegSysParam.TableName, TableName);
	Ip_route_data->RegSysParam.Data_Op_Type = Data_Op_Type;

	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.old_dest_ip_addr, old_dest_ip_addr);
	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.old_gw_ip_addr, old_gw_ip_addr);
	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.old_subnet_mask, old_subnet_mask);
	Ip_route_data->RegSysParam.SysParam.ipRouteData.old_route_metric = old_route_metric;
	Ip_route_data->RegSysParam.SysParam.ipRouteData.old_pmtu = old_pmtu;

	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.new_dest_ip_addr, new_dest_ip_addr);
	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.new_gw_ip_addr, new_gw_ip_addr);
	strcpy (Ip_route_data->RegSysParam.SysParam.ipRouteData.new_subnet_mask, new_subnet_mask);
	Ip_route_data->RegSysParam.SysParam.ipRouteData.new_route_metric = new_route_metric;
	Ip_route_data->RegSysParam.SysParam.ipRouteData.new_pmtu = new_pmtu;

	return;
}


/*
 * Signature of function nmGetRegSysParamNeHostname
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : Ne_hostname_data    Address of Ne Hostname Data       #*/
/*#                                   Structure                         #*/
/*#                                                                     #*/
/*# OUT par     : Ne_hostname_data    Ne Hostname Data Structure        #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (Ne_hostname_data) with elements   #*/
/*#               from table: (ne_hostname).                            #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetRegSysParamNeHostname (nmng_reg_sys_param_data * Ne_hostname_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id)
{
	if ((Ne_hostname_data == NULL) || (TableName == NULL) || (old_hostname == NULL) || (old_hostname1 == NULL) ||
		(old_hostname2 == NULL) || (new_hostname == NULL) || (new_hostname1 == NULL) || (new_hostname2 == NULL))
	{
		return;
	}

	Ne_hostname_data->my_type = my_type;
	strcpy (Ne_hostname_data->RegSysParam.TableName, TableName);
	Ne_hostname_data->RegSysParam.Data_Op_Type = Data_Op_Type;

	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.old_hostname, old_hostname);
	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.old_hostname1, old_hostname1);
	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.old_hostname2, old_hostname2);
	Ne_hostname_data->RegSysParam.SysParam.neHostnameData.old_geosys_unit_id = old_geosys_unit_id;

	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.new_hostname, new_hostname);
	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.new_hostname1, new_hostname1);
	strcpy (Ne_hostname_data->RegSysParam.SysParam.neHostnameData.new_hostname2, new_hostname2);
	Ne_hostname_data->RegSysParam.SysParam.neHostnameData.new_geosys_unit_id = new_geosys_unit_id;

	return;
}


/*
 * Signature of function nmGetRegSysParamNeHosts
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : Ne_hosts_data      Address of Ne Hosts Data Structure #*/
/*#                                                                     #*/
/*# OUT par     : Ne_hosts_data      Ne Hosts Data Structure            #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (Ne_hosts_data) with elements      #*/
/*#               from table: (ne_hosts).                               #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void	nmGetRegSysParamNeHosts (nmng_reg_sys_param_data * Ne_hosts_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_host_srv_ip_addr, char * old_ip_domain_name, char * old_ip_host_name,
char * new_host_srv_ip_addr, char * new_ip_domain_name, char * new_ip_host_name)
{
	if ((Ne_hosts_data == NULL) || (TableName == NULL) || (old_host_srv_ip_addr == NULL) || (old_ip_domain_name == NULL) ||
		(old_ip_host_name == NULL) || (new_host_srv_ip_addr == NULL) || (new_ip_domain_name == NULL) || (new_ip_host_name == NULL))
	{
		return;
	}

	Ne_hosts_data->my_type = my_type;
	strcpy (Ne_hosts_data->RegSysParam.TableName, TableName);
	Ne_hosts_data->RegSysParam.Data_Op_Type = Data_Op_Type;

	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.old_host_srv_ip_addr, old_host_srv_ip_addr);
	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.old_ip_domain_name, old_ip_domain_name);
	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.old_ip_host_name, old_ip_host_name);

	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.new_host_srv_ip_addr, new_host_srv_ip_addr);
	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.new_ip_domain_name, new_ip_domain_name);
	strcpy (Ne_hosts_data->RegSysParam.SysParam.neHostsData.new_ip_host_name, new_ip_host_name);

	return;
}


/*
 * Signature of function nmGetGU_T_RSTRRANGEDATAStruct
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Tadeja Markun                                         #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : pRstrRangeData     Restriction Range Data             #*/
/*#                                                                     #*/
/*# OUT par     : pRstrRangeData     Restriction Range Data             #*/
/*#                                                                     #*/
/*# Description : Fill in structure: (pRstrRangeData) with elements     #*/
/*#               from table: ip_geo_rstr_range.                        #*/
/*#               Function is used in XML configuration files.          #*/
/*#                                                                     #*/
/*#######################################################################*/

void nmGetGU_T_RSTRRANGEDATAStruct (GU_T_RSTRRANGEDATA *pRstrRangeData, long ip_geo_group_id, char *start_rng_ip_addr, char *end_rng_ip_addr)

{
	if ((pRstrRangeData == NULL) || (start_rng_ip_addr == NULL) || (end_rng_ip_addr == NULL))
	{
		return;
	}

	pRstrRangeData->fd_f_ipgroupid = ip_geo_group_id;
	strcpy (pRstrRangeData->fd_f_startrngipaddr, start_rng_ip_addr);
	strcpy (pRstrRangeData->fd_f_endrngipaddr, end_rng_ip_addr);

	return;
}

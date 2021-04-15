/*########################################################################*/
/*#                                                                      #*/
/*#                                                                      #*/
/*#               Copyright (c) 2006 Iskratel                            #*/
/*#                                                                      #*/
/*#                                                                      #*/
/*# Name:         nmStructureDefinition.h                                #*/
/*#                                                                      #*/
/*# Description:  Functions for filling stuctures for notification       #*/
/*#                                                                      #*/
/*# Code:         BNYC - XAB2782                                         #*/
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
/* static char unixid_nmStructureDefinition_h[] = "@(#)BNYC.z	1.2.3.23	15/01/22	nmStructureDefinition.h -O-"; */

#ifndef _NM_STRUCTURE_DEFINITION_H
#define _NM_STRUCTURE_DEFINITION_H

/*************/
/* LIBRARIES */
/*************/

#include "g2_appli.h"
#include "g2_com.h"
#include "s_cs_appl.h"
#include "b_cva.h"
#include "r_iuamng.h"
#include "r_dnsmng.h"
#include "r_agcf_mng.h"
#include "r_sip_mux.h"

#include "notify_msg.h"

/*************/
/* CONSTANTS */
/*************/

#define	NMNG_SIGTRACE_NR	20	/* Maximal Length of Signalling Trace Number	*/
#define	NMNG_CR_GRP_NAME	30	/* Maximal Length of Call Reduction Group Name	*/

/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

extern	void nmGetGU_T_DPDATAStruct (GU_T_DPDATA * dpdata, short fd_f_dpnr, short fd_f_dpsrvname, short fd_f_dptype,
short seqnr, short fd_f_dparming, short fd_f_dpcriteriaid, short fd_f_dppropertyid, short dpsrvgroup, short dpsrvgroupseqnr);

extern	void nmGetGU_T_PFXDATAStruct(GU_T_PFXDATA *pPfxData, short ndcid, char *dstpfx, long bgid, short cgid,
short dstctg, short dstid, short ststgrp, short nplanid);

extern	void nmGetGU_T_QDATAStruct(GU_T_QDATA *pQdata, short cgido, short cgidt, char *qpfx, short crgcode);

extern	void nmGetnmng_msg_tsf_settingsStruct(nmng_msg_tsf_settings *p_nmng_msg_tsf_settings, long int mtype,char *TableName,
short Data_Op_Type, short UserInfo, long old_nodeid, short old_tsf_settings_id, char *old_ftp_settings, long new_nodeid,
short new_tsf_settings_id, char *new_ftp_settings);

extern	void nmGetGU_T_TARIFFPFXDATAStruct (GU_T_TARIFFPFXDATA *pTariffPfxData, short ndcid, char *dstpfx, long bgid,
short cid, short dstchcode, short ststgrp, short nplanid);

extern	void nmGetGU_T_TRUNKStruct (GU_T_TRUNK *ptrunk, short module, short port, short ci);

extern	void nmGetIua_access_data (GU_T_IUA_ACCESSDATA *pIua_access_data, long sysintfid, long sysaccessid,
long intiid, char *textiid, short equipped, short block, short state);

extern	void nmGetArp_data(lnxadm_msg_data *pArp_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_ip_addr, char *old_mac_addr, short old_arp_proxy, char *new_ip_addr, char *new_mac_addr, short new_arp_proxy);

extern	void nmGetBasic_filter_data(lnxadm_msg_data *pBasic_filter_data,long int mtype, char *TableName, short Data_Op_Type,
short old_filter_id, short old_basic_filt_order, short old_basic_filt_action, char *old_src_mac_addr,
char *old_dst_mac_addr, long old_ll_prot, char *old_src_ip_addr, char *old_src_ip_mask, char *old_dst_ip_addr, char *old_dst_ip_mask,
short old_ip_prot, long old_src_porthigh, long old_src_portlow, long old_dst_porthigh , long old_dst_portlow, short old_icmp_type, 
short old_tos, short old_precedense, short new_filter_id, short new_basic_filt_order, short new_basic_filt_action,
char *new_src_mac_addr, char *new_dst_mac_addr, long new_ll_prot, char *new_src_ip_addr, char *new_src_ip_mask,
char *new_dst_ip_addr, char *new_dst_ip_mask, short new_ip_prot, long new_src_porthigh, long new_src_portlow, 
long new_dst_porthigh, long new_dst_portlow, short new_icmp_type, short new_tos, short new_precedense);

extern	void nmGetEth_interface_data(lnxadm_msg_data *pEth_interface_data,long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_eth_intf_name, short  old_modulenr, short  old_portnr, short  old_portnr2, short  old_oosi,
short  old_admin_state, short  old_log_stat_group_id, short  old_interface_type, short  new_eth_intf_id, char *new_eth_intf_name,
short  new_modulenr, short  new_portnr, short  new_portnr2, short  new_oosi, short  new_admin_state, short  new_log_stat_group_id, short new_interface_type);

extern	void nmGetEth_profile_data (lnxadm_msg_data *pEth_profile_data,long int mtype, char *TableName, short Data_Op_Type,
short old_eth_profile_id, char *old_profile_name, short old_bandwidth, long old_mtu, short old_eth_mode, short new_eth_profile_id,
char *new_profile_name, short new_bandwidth, long new_mtu, short new_eth_mode);

extern	void nmGetEth_tag_rule_data(lnxadm_msg_data *pEth_tag_rule_data,long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, short  old_vlan_id, short  old_acc_untag_frms, short  old_snd_tag_frms, short  old_ins_prior, short  old_prior_value,
short  new_eth_intf_id, short  new_vlan_id, short  new_acc_untag_frms, short  new_snd_tag_frms, short  new_ins_prior, short new_prior_value);

extern	void nmGetFilter_data(lnxadm_msg_data *pFilter_data,long int mtype, char *TableName, short Data_Op_Type, short old_filter_id,
char *old_filter_name, short new_filter_id, char *new_filter_name);

extern	void nmGetFilter_grp_member_data(lnxadm_msg_data *pFilter_grp_member_data,long int mtype, char *TableName, short Data_Op_Type,
short old_intf_id, short old_sub_type, short old_filter_direction, short old_member_order, short old_filter_id, short new_intf_id, short new_sub_type,
short new_filter_direction, short new_member_order, short new_filter_id);

extern	void nmGetIp_device_data(lnxadm_msg_data *pIp_device_data,long int mtype, char *TableName, short Data_Op_Type,
short  old_eth_intf_id, char *old_ip_addr, char *old_ip_mask, short old_function, short old_metric,
short  new_eth_intf_id, char *new_ip_addr, char *new_ip_mask, short new_function, short new_metric);

extern	void nmGetIp_route_data(lnxadm_msg_data *pIp_route_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_dest_ip_addr, char *old_gw_ip_addr, char *old_subnet_mask, short old_route_metric, short old_pmtu, char *new_dest_ip_addr,
char *new_gw_ip_addr, char *new_subnet_mask, short new_route_metric, short new_pmtu);

extern	void	nmGetNe_hostname_data (lnxadm_msg_data * pNe_hostname_data, long int mtype, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id);

extern	void nmGetNe_hosts_data(lnxadm_msg_data *pNe_hosts_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_host_srv_ip_addr, char *old_ip_domain_name, char *old_ip_host_name, char *new_host_srv_ip_addr,
char *new_ip_domain_name, char *new_ip_host_name);

extern	void nmGetNe_srv_security_data(lnxadm_msg_data *pNe_srv_security_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_srv_acc_ip_addr, short old_srv_scrt_name, short old_scrt_rule_action, char *new_srv_acc_ip_addr,
short new_srv_scrt_name, short new_scrt_rule_action);

extern	void nmGetNe_user_data(lnxadm_msg_data *pNe_user_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_username, char *old_user_password, char *old_calling_station_id, char *new_username, char *new_user_password, char *new_calling_station_id);

extern	void	nmGetNodeData (lnxadm_msg_data * pNodeData, long int mtype, char * TableName, short Data_Op_Type,
char old_prefered_unit_id, char old_geo_mode, char old_geoswtch_timeout, short old_pref_eth_port_id,
char new_prefered_unit_id, char new_geo_mode, char new_geoswtch_timeout, short new_pref_eth_port_id);

extern	void nmGetOs_user_group_data(lnxadm_msg_data *pOs_user_group_data,long int mtype, char *TableName, short Data_Op_Type,
short old_os_user_group_id, char *old_os_group_name, short new_os_user_group_id, char *new_os_group_name);

extern	void nmGetPort_data(lnxadm_msg_data *pPort_data,long int mtype, char *TableName, short Data_Op_Type, short  old_portnr,
short  old_port_id, short  old_boardnr, short  old_port_equip, short  old_port_oosi, short old_sigtrace_req, short  old_port_mlb,
short  old_log_stat_group_id, short  old_port_profile_type, short  old_port_profile_id, char *old_port_sys_name, short  new_portnr, short  new_port_id, short  new_boardnr,
short  new_port_equip, short  new_port_oosi, short  new_sigtrace_req, short  new_port_mlb, short  new_log_stat_group_id, short  new_port_profile_type,
short new_port_profile_id, char *new_port_sys_name);

extern	void	nmGetProtocolQos (lnxadm_msg_data * pProtocolQos, long int mtype, char * TableName, short Data_Op_Type, char new_protocol_type, char new_dscp);

extern	void nmGetRlt_user_group_data(lnxadm_msg_data *pRlt_user_group_data, long int mtype, char *TableName, short Data_Op_Type,
char *old_username, short old_os_user_group_id, char *new_username, short new_os_user_group_id);

extern	void nmGetSquid_ne_port_data(lnxadm_msg_data *pSquid_ne_port_data,long int mtype, char *TableName, short Data_Op_Type,
long old_squid_ne_port_nr, long new_squid_ne_port_nr);

extern	void nmGetSyslog_client_data(lnxadm_msg_data *pSyslog_client_data,long int mtype, char *TableName, short Data_Op_Type,
char *old_log_srv_ip, long old_log_srv_udp_port, short old_syslog_state, short old_syslog_severity,
char *new_log_srv_ip, long new_log_srv_udp_port, short new_syslog_state, short new_syslog_severity);

extern	void nmGetpdatanmng_msg_port (nmng_msg_port *pdatanmng_msg_port, long int mtype, char *TableName, short Data_Op_Type,
short UserInfo, long old_nodeid, short old_portnr, short old_port_id, short old_boardnr, short old_port_equip, short old_port_oosi,
short old_sigtrace_req, short old_port_mlb, short old_log_stat_group_id, short old_port_profile_type, short old_port_profile_id, char *old_port_sys_name,
long new_nodeid, short new_portnr, short new_port_id, short new_boardnr, short new_port_equip, short new_port_oosi, short new_sigtrace_req,
short new_port_mlb, short new_log_stat_group_id, short new_port_profile_type, short new_port_profile_id, char *new_port_sys_name);

extern	void	nmGetDnsConfigVoip (GU_T_DNS_CONFIG * pDnsConfig, char * domain_name, char * primary_server, char * secondary_server,
long dns_max_queries, long dns_cache_size, long dns_cache_ttl_max, char * tertiary_server, long srvr_fail_time);

extern void	nmGetMng_msg_board (mng_msg_board * pMng_msg_board, long int mtype, short Data_Op_Type, short boardnr, short parent_boardnr,
short board_pos, short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id);

extern void	nmGetBoard_data (lnxadm_msg_data * pBoard_data, long int mtype, char * TableName, short Data_Op_Type,
short boardnr, short parent_boardnr, short old_board_pos, short board_pos,
short board_type, short board_equip, short board_oosi, char * req_board_id, char * act_board_id,
char * board_serialnr, char * board_dsc, short board_profile_type, short board_profile_id, char s_node, char geosys_unit_id);

extern	void	nmGetAgcfGlobalData (GU_T_AGCFGLBDATA * pAgcfGlobal, short modulenr, short portnr, short cookie_method,
long agcf_dflt_expires, short agcf_tm_frst_rgstr, short agcf_tm_next_rgstr, short agcf_tm_fail_rgstr, char * agcf_pswd_dflt,
char * cnf_user, char * cnf_host, long max_nr_of_agcf_reg, char impi_add_domain, char * s3pty_user, char * s3pty_host,
char search_unreg_sts, long search_unreg_cycle, short search_unreg_tm, short reg_per_sec_tm, char sys_startup_reg,
long agcf_param1, long agcf_param2, char check_rgstr_subs, char scscf_assgn_type, char mon_nr_of_pings,
char pani_access_type, char * hot_user_code, char hotd_tm, short tm_check_rgstr_sts, char standalone_mode, char cnf_procedure, char pani_hndl);

extern	void	nmGetInterCarrTraff (GU_T_CARRINOUTTRAFFDATA * pCarrTraff, short cid, long cid_overplus,
long cid_inc_traffic, long cid_out_traffic, long cid_traffic_diff, short cid_dyndistprio);

extern	void	nmGetIMS_GlobalLicence (GU_T_IMS_GLOBALLICENCE * GlbLicence, char * ims_bsts, char * ims_asts, long ims_tm_valid, char * ims_ssts);

extern	void	nmGetIMS_Global (GU_T_IMS_GLOBAL * ImsGlobal, short ims_cscf_type, short ims_diam_variant, short ims_nr_of_impu, char * ims_homedomain,
long ims_param1, long ims_param2, short ims_saporttype, long ims_as_mode, short ims_ndc_id, short tariff_dir, char siptoteluri, char sh_activation, 
char handle_uri_num, char survive_mode, short spec_func, char user_delete);

extern	void	nmGetIMS_GlobalREG (GU_T_IMS_GLOBALREG * ImsGlobal, long nodeid, char * ims_ownnetworkid,
long ims_nr_of_acc, short ims_auth_alg, short ims_ipsec, short pcscf_subscription, short ims_min_expires, long ims_max_expires,
short sip_feature_id, char ims_3gpprls, short typesc, char ims_magcf, char * ims_bsts, char * ims_asts,
long ims_tm_valid, char replica_synch, char impi_add_domain, char store_srv_prf, char ims_chrg_mode, char alarm_sts, 
char * ims_ssts, short user_properties);

extern	void	nmGetIMS_NeConfig (GU_T_IMS_GLOBAL * ImsGlobal, long nodeid, short ims_ne_type,
short ims_ne_useownip, long ims_ne_port, char * ims_ne_uri, short ims_ne_transport, short stat_group_id);

extern	void	nmGetSigtraceNumbers (GU_T_STRCNRDATA * StrcNrData, short trnkgrpid, char sigtrace_nr_type, char * sigtrace_nr, short sigtrace_req);

extern	void	nmGetPingTestData (nmng_ping_test * pPingTest, long int my_type, char * TableName, short Data_Op_Type,
long old_rtt_min_threshold, long old_rtt_max_threshold, short old_test_period,
long new_rtt_min_threshold, long new_rtt_max_threshold, short new_test_period);

extern	void	nmGetIMS_Net (GU_T_IMS_ELEMENT * ImsElement, char include, short ims_dns_type, char * ims_ip_addr, long ims_ip_port,
short ims_ip_transport, char * ims_uri, short d_attr_id, short tm_retransmit, short stat_group_id, short ims_rmt_device_id, long cpb_set_id, char * realm_domain);

extern	void	nmGetCRSrvSts (GU_T_CRSRVSTS * CRSrvSts, char crcg_state, char crcb_state, char crr_state);

extern	void	nmGetCRFeature (GU_T_CRDATADB * CRFeature, short cr_grp_id, char * cr_grp_name, long crcg_interval, char crcb_percent, char crr_max_conn);

extern	void	nmGetGlobalTrnkDataConfig (GU_T_GLOBALTRNKDATACONFIG * TrnkConfig, short cbact_list_id, char cbact_call_rej, char sldr_mode, char li_prefix_act);

extern	void	nmGetSIP_Transaction (GU_T_SIPTRANSACTION * SIP_Trans, long nodeid, long ims_max_nr,
short alr_report_val, short alr_remove_val, short alr_avg_time, char * resp_ims_max_nr, long ims_tm_valid);

extern	void	nmGetSIP_SubsData (GU_T_SIP_SUBSDATA * SubsData, long nodeid, short modulenr, short portnr,
char * acc_ip_addr, char * acc_sec_ip_addr, char * acc_eth_addr, char * acc_sec_eth_addr, short acc_eth_ident_mode,
short sip_feature_id, short sigtrace_req, short mad_id, char * sip_subs_alias, short sip_alias_type, short sip_acc_param1, long sip_acc_param2,
char * sip_acc_param3, short reg_mode, char * sip_key, long expires, short sip_authent_mode, short sip_display, short local_tone,
long acc_ip_port, char * sip_domain_name, short ims_routeid, long subscribe_expires, char * md5_nonce,
char acc_ip_transport, short pri_ccf_id, short sec_ccf_id, char ims_alarm_sts, char * p_gruu,
short refer_notify, char refer_handling, char ims_reg_entity, char sip_acc_alrind, char * acc_cntct, char integrity_protected, char rem_auth_req);

extern	void	nmGetFMCCommonData (GU_T_FMCCOMMONDATA * FMC, char fmc_crosspath, char fmc_mobile_acc, char fmc_universal_dialing);

extern	void	nmGetSCP_Global (GU_T_SCPGLOBAL * SCPGlobal, short loop_trnk_dstid, char * cntr_code, short tm_msrnvalidity, short tm_msrncheckvalidity,
char project_spec_id, short tm_loopvalidity, short tm_loopcheckvalidity, char np_foreign_nr_mode, short np_dflt_conv_id);

extern	void	nmGetIP_TrunkSession (GU_T_IPTRUNKSESSION * IpTrunkSession, long nodeid, long max_ip_trunk_sess,
short alr_report_val, short alr_remove_val, char * resp_ip_trunk_sess, long ip_trunk_tm_valid, long max_fmc_sess, char * resp_fmc_sess);

extern	void	nmGetRegSysParamArp (nmng_reg_sys_param_data * Arp_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_ip_addr, char * old_mac_addr, short old_arp_proxy, char * new_ip_addr, char * new_mac_addr, short new_arp_proxy);

extern	void	nmGetRegSysParamIpDevice (nmng_reg_sys_param_data * Ip_device_data, long int my_type, char * TableName, short Data_Op_Type,
short old_eth_intf_id, char * old_ip_addr, char * old_ip_mask, short old_function, short old_metric,
short new_eth_intf_id, char * new_ip_addr, char * new_ip_mask, short new_function, short new_metric);

extern	void	nmGetRegSysParamIpRoute (nmng_reg_sys_param_data * Ip_route_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_dest_ip_addr, char * old_gw_ip_addr, char * old_subnet_mask, short old_route_metric, short old_pmtu,
char * new_dest_ip_addr, char * new_gw_ip_addr, char * new_subnet_mask, short new_route_metric, short new_pmtu);

extern	void	nmGetRegSysParamNeHostname (nmng_reg_sys_param_data * Ne_hostname_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_hostname, char * old_hostname1, char * old_hostname2, char old_geosys_unit_id,
char * new_hostname, char * new_hostname1, char * new_hostname2, char new_geosys_unit_id);

extern	void	nmGetRegSysParamNeHosts (nmng_reg_sys_param_data * Ne_hosts_data, long int my_type, char * TableName, short Data_Op_Type,
char * old_host_srv_ip_addr, char * old_ip_domain_name, char * old_ip_host_name,
char * new_host_srv_ip_addr, char * new_ip_domain_name, char * new_ip_host_name);

extern  void	nmGetGU_T_RSTRRANGEDATAStruct (GU_T_RSTRRANGEDATA *pRstrRangeData, long ip_geo_group_id, char *start_rng_ip_addr, char *end_rng_ip_addr);

#endif /* _NM_STRUCTURE_DEFINITION_H */


/******************************************************/
/*                                                    */
/* DO NOT EDIT!                                       */
/* Generated file by: XdbdNMGenNotification.cpp       */
/*                                                    */
/******************************************************/

/************************************************************************/
/* LIBRARIES                                                            */
/************************************************************************/
#ifdef SS_UNIX
#  include "sqlxdbd.h"
#else
#  include <windows.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include <sqlext.h>
#include <assert.h>

#include "g2_appli.h"
#include "g2_com.h"
#include <itcommon/ipcs_keys.h>
#include "notify_msg.h"
#include <r_vxmlmm.h>
#include "clip_string.h"	// clip_string(), equal()
#include "nmSolidNtwrk.h"

/************************************************************************/
/* VARIABLE DECLARATIONS                                                */
/************************************************************************/
static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void* nmFunc_vxml_config_load (int n, char* p[]);
int nmFunc_vxml_config_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_vxml_config_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_vxml_config_load (int n, char* p[])
{
   return 0;
}

int nmFunc_vxml_config_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "vxml_config";
   static const SQLSMALLINT	UserInfo;



   SQLSMALLINT	new_vxml_active = *(SQLSMALLINT*)(newLine + 48);
   SQLSMALLINT	old_vxml_active = *(SQLSMALLINT*)(oldLine + 48);
   SQLINTEGER	new_vxml_active_null = (((*(SQLINTEGER*)(newLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_vxml_active_null = (((*(SQLINTEGER*)(oldLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_vxml_folder = (SQLVARCHAR*)(newLine + 80);
   SQLVARCHAR*	old_vxml_folder = (SQLVARCHAR*)(oldLine + 80);
   SQLINTEGER	new_vxml_folder_null = (((*(SQLINTEGER*)(newLine + 5)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_vxml_folder_null = (((*(SQLINTEGER*)(oldLine + 5)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_fetch_timeout = *(SQLINTEGER*)(newLine + 152);
   SQLINTEGER	old_fetch_timeout = *(SQLINTEGER*)(oldLine + 152);
   SQLINTEGER	new_fetch_timeout_null = (((*(SQLINTEGER*)(newLine + 5)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_fetch_timeout_null = (((*(SQLINTEGER*)(oldLine + 5)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_input_mode = *(SQLSMALLINT*)(newLine + 160);
   SQLSMALLINT	old_input_mode = *(SQLSMALLINT*)(oldLine + 160);
   SQLINTEGER	new_input_mode_null = (((*(SQLINTEGER*)(newLine + 6)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_input_mode_null = (((*(SQLINTEGER*)(oldLine + 6)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_interdigit_timeout = *(SQLSMALLINT*)(newLine + 168);
   SQLSMALLINT	old_interdigit_timeout = *(SQLSMALLINT*)(oldLine + 168);
   SQLINTEGER	new_interdigit_timeout_null = (((*(SQLINTEGER*)(newLine + 6)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_interdigit_timeout_null = (((*(SQLINTEGER*)(oldLine + 6)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_safe_caching = *(SQLSMALLINT*)(newLine + 176);
   SQLSMALLINT	old_safe_caching = *(SQLSMALLINT*)(oldLine + 176);
   SQLINTEGER	new_safe_caching_null = (((*(SQLINTEGER*)(newLine + 7)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_safe_caching_null = (((*(SQLINTEGER*)(oldLine + 7)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_term_dtmf_code = (SQLVARCHAR*)(newLine + 184);
   SQLVARCHAR*	old_term_dtmf_code = (SQLVARCHAR*)(oldLine + 184);
   SQLINTEGER	new_term_dtmf_code_null = (((*(SQLINTEGER*)(newLine + 7)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_term_dtmf_code_null = (((*(SQLINTEGER*)(oldLine + 7)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_term_timeout = *(SQLINTEGER*)(newLine + 192);
   SQLINTEGER	old_term_timeout = *(SQLINTEGER*)(oldLine + 192);
   SQLINTEGER	new_term_timeout_null = (((*(SQLINTEGER*)(newLine + 8)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_term_timeout_null = (((*(SQLINTEGER*)(oldLine + 8)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_noinput_timeout = *(SQLINTEGER*)(newLine + 200);
   SQLINTEGER	old_noinput_timeout = *(SQLINTEGER*)(oldLine + 200);
   SQLINTEGER	new_noinput_timeout_null = (((*(SQLINTEGER*)(newLine + 8)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_noinput_timeout_null = (((*(SQLINTEGER*)(oldLine + 8)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_js_memory_size = *(SQLINTEGER*)(newLine + 208);
   SQLINTEGER	old_js_memory_size = *(SQLINTEGER*)(oldLine + 208);
   SQLINTEGER	new_js_memory_size_null = (((*(SQLINTEGER*)(newLine + 9)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_js_memory_size_null = (((*(SQLINTEGER*)(oldLine + 9)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_dmg_block_size = *(SQLINTEGER*)(newLine + 216);
   SQLINTEGER	old_dmg_block_size = *(SQLINTEGER*)(oldLine + 216);
   SQLINTEGER	new_dmg_block_size_null = (((*(SQLINTEGER*)(newLine + 9)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dmg_block_size_null = (((*(SQLINTEGER*)(oldLine + 9)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_dmg_block_count = *(SQLINTEGER*)(newLine + 224);
   SQLINTEGER	old_dmg_block_count = *(SQLINTEGER*)(oldLine + 224);
   SQLINTEGER	new_dmg_block_count_null = (((*(SQLINTEGER*)(newLine + 10)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dmg_block_count_null = (((*(SQLINTEGER*)(oldLine + 10)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_dmg_proxy_addr = (SQLVARCHAR*)(newLine + 232);
   SQLVARCHAR*	old_dmg_proxy_addr = (SQLVARCHAR*)(oldLine + 232);
   SQLINTEGER	new_dmg_proxy_addr_null = (((*(SQLINTEGER*)(newLine + 10)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dmg_proxy_addr_null = (((*(SQLINTEGER*)(oldLine + 10)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_dmg_proxy_port = *(SQLSMALLINT*)(newLine + 248);
   SQLSMALLINT	old_dmg_proxy_port = *(SQLSMALLINT*)(oldLine + 248);
   SQLINTEGER	new_dmg_proxy_port_null = (((*(SQLINTEGER*)(newLine + 11)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dmg_proxy_port_null = (((*(SQLINTEGER*)(oldLine + 11)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_dmg_thread_count = *(SQLINTEGER*)(newLine + 256);
   SQLINTEGER	old_dmg_thread_count = *(SQLINTEGER*)(oldLine + 256);
   SQLINTEGER	new_dmg_thread_count_null = (((*(SQLINTEGER*)(newLine + 11)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dmg_thread_count_null = (((*(SQLINTEGER*)(oldLine + 11)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_rec_final_silence = *(SQLSMALLINT*)(newLine + 264);
   SQLSMALLINT	old_rec_final_silence = *(SQLSMALLINT*)(oldLine + 264);
   SQLINTEGER	new_rec_final_silence_null = (((*(SQLINTEGER*)(newLine + 12)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_rec_final_silence_null = (((*(SQLINTEGER*)(oldLine + 12)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_rec_max_time = *(SQLSMALLINT*)(newLine + 272);
   SQLSMALLINT	old_rec_max_time = *(SQLSMALLINT*)(oldLine + 272);
   SQLINTEGER	new_rec_max_time_null = (((*(SQLINTEGER*)(newLine + 12)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_rec_max_time_null = (((*(SQLINTEGER*)(oldLine + 12)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_trans_conn_timeout = *(SQLINTEGER*)(newLine + 280);
   SQLINTEGER	old_trans_conn_timeout = *(SQLINTEGER*)(oldLine + 280);
   SQLINTEGER	new_trans_conn_timeout_null = (((*(SQLINTEGER*)(newLine + 13)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_trans_conn_timeout_null = (((*(SQLINTEGER*)(oldLine + 13)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	new_trans_call_maxtime = *(SQLINTEGER*)(newLine + 288);
   SQLINTEGER	old_trans_call_maxtime = *(SQLINTEGER*)(oldLine + 288);
   SQLINTEGER	new_trans_call_maxtime_null = (((*(SQLINTEGER*)(newLine + 13)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_trans_call_maxtime_null = (((*(SQLINTEGER*)(oldLine + 13)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_bargein = *(SQLSMALLINT*)(newLine + 296);
   SQLSMALLINT	old_bargein = *(SQLSMALLINT*)(oldLine + 296);
   SQLINTEGER	new_bargein_null = (((*(SQLINTEGER*)(newLine + 14)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_bargein_null = (((*(SQLINTEGER*)(oldLine + 14)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_best_effort_level = *(SQLSMALLINT*)(newLine + 304);
   SQLSMALLINT	old_best_effort_level = *(SQLSMALLINT*)(oldLine + 304);
   SQLINTEGER	new_best_effort_level_null = (((*(SQLINTEGER*)(newLine + 14)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_best_effort_level_null = (((*(SQLINTEGER*)(oldLine + 14)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_lite_level_lc_nr = *(SQLSMALLINT*)(newLine + 312);
   SQLSMALLINT	old_lite_level_lc_nr = *(SQLSMALLINT*)(oldLine + 312);
   SQLINTEGER	new_lite_level_lc_nr_null = (((*(SQLINTEGER*)(newLine + 15)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_lite_level_lc_nr_null = (((*(SQLINTEGER*)(oldLine + 15)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_simple_level_lc_nr = *(SQLSMALLINT*)(newLine + 320);
   SQLSMALLINT	old_simple_level_lc_nr = *(SQLSMALLINT*)(oldLine + 320);
   SQLINTEGER	new_simple_level_lc_nr_null = (((*(SQLINTEGER*)(newLine + 15)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_simple_level_lc_nr_null = (((*(SQLINTEGER*)(oldLine + 15)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_medium_level_lc_nr = *(SQLSMALLINT*)(newLine + 328);
   SQLSMALLINT	old_medium_level_lc_nr = *(SQLSMALLINT*)(oldLine + 328);
   SQLINTEGER	new_medium_level_lc_nr_null = (((*(SQLINTEGER*)(newLine + 16)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_medium_level_lc_nr_null = (((*(SQLINTEGER*)(oldLine + 16)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_advanc_level_lc_nr = *(SQLSMALLINT*)(newLine + 336);
   SQLSMALLINT	old_advanc_level_lc_nr = *(SQLSMALLINT*)(oldLine + 336);
   SQLINTEGER	new_advanc_level_lc_nr_null = (((*(SQLINTEGER*)(newLine + 16)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_advanc_level_lc_nr_null = (((*(SQLINTEGER*)(oldLine + 16)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_full_level_lc_nr = *(SQLSMALLINT*)(newLine + 344);
   SQLSMALLINT	old_full_level_lc_nr = *(SQLSMALLINT*)(oldLine + 344);
   SQLINTEGER	new_full_level_lc_nr_null = (((*(SQLINTEGER*)(newLine + 17)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_full_level_lc_nr_null = (((*(SQLINTEGER*)(oldLine + 17)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_vxml_file_transfer = *(SQLSMALLINT*)(newLine + 352);
   SQLSMALLINT	old_vxml_file_transfer = *(SQLSMALLINT*)(oldLine + 352);
   SQLINTEGER	new_vxml_file_transfer_null = (((*(SQLINTEGER*)(newLine + 17)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_vxml_file_transfer_null = (((*(SQLINTEGER*)(oldLine + 17)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_vxml_proxy_type = *(SQLSMALLINT*)(newLine + 448);
   SQLSMALLINT	old_vxml_proxy_type = *(SQLSMALLINT*)(oldLine + 448);
   SQLINTEGER	new_vxml_proxy_type_null = (((*(SQLINTEGER*)(newLine + 23)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_vxml_proxy_type_null = (((*(SQLINTEGER*)(oldLine + 23)) & 16) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 4:
         {
            if (
            (new_vxml_active != old_vxml_active)
            )
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
if ((old_vxml_active == 0) && (new_vxml_active > 0)) {
SDL_OUTPUT_trg_vxml_start (G2P_FINDPID(vxmlmm));
}
if ((old_vxml_active > 0) && (new_vxml_active == 0)) {
SDL_OUTPUT_trg_vxml_stop (G2P_FINDPID(vxmlmm));
}
}
            }
            if (
            (strncmp ((char*) new_vxml_folder, (char*) old_vxml_folder, 70) != 0)
            || (new_fetch_timeout != old_fetch_timeout)
            || (new_input_mode != old_input_mode)
            || (new_interdigit_timeout != old_interdigit_timeout)
            || (new_safe_caching != old_safe_caching)
            || (strncmp ((char*) new_term_dtmf_code, (char*) old_term_dtmf_code, 1) != 0)
            || (new_term_timeout != old_term_timeout)
            || (new_noinput_timeout != old_noinput_timeout)
            || (new_js_memory_size != old_js_memory_size)
            || (new_dmg_block_size != old_dmg_block_size)
            || (new_dmg_block_count != old_dmg_block_count)
            || (strncmp ((char*) new_dmg_proxy_addr, (char*) old_dmg_proxy_addr, 15) != 0)
            || (new_dmg_proxy_port != old_dmg_proxy_port)
            || (new_dmg_thread_count != old_dmg_thread_count)
            || (new_rec_final_silence != old_rec_final_silence)
            || (new_rec_max_time != old_rec_max_time)
            || (new_trans_conn_timeout != old_trans_conn_timeout)
            || (new_trans_call_maxtime != old_trans_call_maxtime)
            || (new_bargein != old_bargein)
            || (new_best_effort_level != old_best_effort_level)
            || (new_vxml_proxy_type != old_vxml_proxy_type)
            )
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_configdata (G2P_FINDPID(vxmlmm));
}
            }
            if (
            (new_lite_level_lc_nr != old_lite_level_lc_nr)
            || (new_simple_level_lc_nr != old_simple_level_lc_nr)
            || (new_medium_level_lc_nr != old_medium_level_lc_nr)
            || (new_advanc_level_lc_nr != old_advanc_level_lc_nr)
            || (new_full_level_lc_nr != old_full_level_lc_nr)
            )
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_licencedata (G2P_FINDPID(vxmlmm));
}
            }
            if (
            (new_vxml_file_transfer != old_vxml_file_transfer)
            )
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_filetransfer (G2P_FINDPID(vxmlmm));
}
            }
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_vxml_config_unload (void* context)
{
   // No code ..
}


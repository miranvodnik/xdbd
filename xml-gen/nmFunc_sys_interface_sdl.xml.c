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
#include <r_mgcc_mux.h>
#include "clip_string.h"	// clip_string(), equal()
#include "nmSolidNtwrk.h"

/************************************************************************/
/* VARIABLE DECLARATIONS                                                */
/************************************************************************/
static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void* nmFunc_sys_interface_load (int n, char* p[]);
int nmFunc_sys_interface_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_sys_interface_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_sys_interface_load (int n, char* p[])
{
   return 0;
}

int nmFunc_sys_interface_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "sys_interface";
   static const SQLSMALLINT	UserInfo;



   SQLINTEGER	new_sys_intf_id = *(SQLINTEGER*)(newLine + 16);
   SQLINTEGER	old_sys_intf_id = *(SQLINTEGER*)(oldLine + 16);
   SQLINTEGER	new_sys_intf_id_null = (((*(SQLINTEGER*)(newLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_sys_intf_id_null = (((*(SQLINTEGER*)(oldLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLSMALLINT	new_sys_intf_type = *(SQLSMALLINT*)(newLine + 24);
   SQLSMALLINT	old_sys_intf_type = *(SQLSMALLINT*)(oldLine + 24);
   SQLINTEGER	new_sys_intf_type_null = (((*(SQLINTEGER*)(newLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_sys_intf_type_null = (((*(SQLINTEGER*)(oldLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 4:
         {
            if (
            (new_sys_intf_type != old_sys_intf_type)
            )
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_h248_systeminterfacetypechanged (G2P_FINDPID(mgcc_mux), (GU_T_SYSINTERFACEID) new_sys_intf_id);
}
            }
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_sys_interface_unload (void* context)
{
   // No code ..
}


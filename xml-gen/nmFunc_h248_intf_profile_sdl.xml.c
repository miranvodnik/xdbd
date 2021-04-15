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
#include <r_h248mng.h>
#include "clip_string.h"	// clip_string(), equal()
#include "nmSolidNtwrk.h"

/************************************************************************/
/* VARIABLE DECLARATIONS                                                */
/************************************************************************/
static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void* nmFunc_h248_intf_profile_load (int n, char* p[]);
int nmFunc_h248_intf_profile_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_h248_intf_profile_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_h248_intf_profile_load (int n, char* p[])
{
   return 0;
}

int nmFunc_h248_intf_profile_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "h248_intf_profile";
   static const SQLSMALLINT	UserInfo;



   SQLSMALLINT	new_h248_intf_prf_id = *(SQLSMALLINT*)(newLine + 24);
   SQLSMALLINT	old_h248_intf_prf_id = *(SQLSMALLINT*)(oldLine + 24);
   SQLINTEGER	new_h248_intf_prf_id_null = (((*(SQLINTEGER*)(newLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_h248_intf_prf_id_null = (((*(SQLINTEGER*)(oldLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 4:
         {
            static unsigned char	cond_mask[] = {
               0, 0, 17, 17, 17, 17, 17, 17, 17, 17, 17, 1, 0, 0, 0, 0, 
               255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 
            };

            if (equal (newLine, oldLine, cond_mask, sizeof cond_mask) != 0)
            {
               /*	CHANGE MASK CODE	*/
SDL_OUTPUT_trg_h248_interfaceprofiledatachanged (G2P_FINDPID(h248mng), (SDL_NATURAL)new_h248_intf_prf_id);
            }
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_h248_intf_profile_unload (void* context)
{
   // No code ..
}


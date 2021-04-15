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
void* nmFunc_statistics_group_load (int n, char* p[]);
int nmFunc_statistics_group_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_statistics_group_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_statistics_group_load (int n, char* p[])
{
   return 0;
}

int nmFunc_statistics_group_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "statistics_group";
   static const SQLSMALLINT	UserInfo;



   SQLSMALLINT	new_stat_group_type = *(SQLSMALLINT*)(newLine + 32);
   SQLSMALLINT	old_stat_group_type = *(SQLSMALLINT*)(oldLine + 32);
   SQLINTEGER	new_stat_group_type_null = (((*(SQLINTEGER*)(newLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_stat_group_type_null = (((*(SQLINTEGER*)(oldLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 2:
         {
            /*	INSERT CODE	*/
SDL_OUTPUT_trg_vxml_ststdata (G2P_FINDPID(vxmlmm));
         }
         break;
      case 4:
         {
            if (
            (new_stat_group_type != old_stat_group_type)
            )
            {
               /*	CHANGE MASK CODE	*/
SDL_OUTPUT_trg_vxml_ststdata (G2P_FINDPID(vxmlmm));
            }
         }
         break;
      case 8:
         {
            /*	DELETE CODE	*/
SDL_OUTPUT_trg_vxml_ststdata (G2P_FINDPID(vxmlmm));
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_statistics_group_unload (void* context)
{
   // No code ..
}


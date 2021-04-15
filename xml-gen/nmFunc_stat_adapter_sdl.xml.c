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
#include <StatisticalAdapterThread.h>
#include "clip_string.h"	// clip_string(), equal()
#include "nmSolidNtwrk.h"

/************************************************************************/
/* VARIABLE DECLARATIONS                                                */
/************************************************************************/
static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void* nmFunc_stat_adapter_load (int n, char* p[]);
int nmFunc_stat_adapter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_stat_adapter_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_stat_adapter_load (int n, char* p[])
{
   return 0;
}

int nmFunc_stat_adapter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "stat_adapter";
   static const SQLSMALLINT	UserInfo;



   SQLSMALLINT	new_adapter_id = *(SQLSMALLINT*)(newLine + 16);
   SQLSMALLINT	old_adapter_id = *(SQLSMALLINT*)(oldLine + 16);
   SQLINTEGER	new_adapter_id_null = (((*(SQLINTEGER*)(newLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_adapter_id_null = (((*(SQLINTEGER*)(oldLine + 2)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_stat_dir_name = (SQLVARCHAR*)(newLine + 24);
   SQLVARCHAR*	old_stat_dir_name = (SQLVARCHAR*)(oldLine + 24);
   SQLINTEGER	new_stat_dir_name_null = (((*(SQLINTEGER*)(newLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_stat_dir_name_null = (((*(SQLINTEGER*)(oldLine + 3)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_dest_dir_name = (SQLVARCHAR*)(newLine + 280);
   SQLVARCHAR*	old_dest_dir_name = (SQLVARCHAR*)(oldLine + 280);
   SQLINTEGER	new_dest_dir_name_null = (((*(SQLINTEGER*)(newLine + 3)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dest_dir_name_null = (((*(SQLINTEGER*)(oldLine + 3)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_owner_name = (SQLVARCHAR*)(newLine + 536);
   SQLVARCHAR*	old_owner_name = (SQLVARCHAR*)(oldLine + 536);
   SQLINTEGER	new_owner_name_null = (((*(SQLINTEGER*)(newLine + 4)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_owner_name_null = (((*(SQLINTEGER*)(oldLine + 4)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_group_name = (SQLVARCHAR*)(newLine + 600);
   SQLVARCHAR*	old_group_name = (SQLVARCHAR*)(oldLine + 600);
   SQLINTEGER	new_group_name_null = (((*(SQLINTEGER*)(newLine + 4)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_group_name_null = (((*(SQLINTEGER*)(oldLine + 4)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_dir_permissions = (SQLVARCHAR*)(newLine + 664);
   SQLVARCHAR*	old_dir_permissions = (SQLVARCHAR*)(oldLine + 664);
   SQLINTEGER	new_dir_permissions_null = (((*(SQLINTEGER*)(newLine + 5)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_dir_permissions_null = (((*(SQLINTEGER*)(oldLine + 5)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLVARCHAR*	new_file_permissions = (SQLVARCHAR*)(newLine + 680);
   SQLVARCHAR*	old_file_permissions = (SQLVARCHAR*)(oldLine + 680);
   SQLINTEGER	new_file_permissions_null = (((*(SQLINTEGER*)(newLine + 5)) & 16) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_file_permissions_null = (((*(SQLINTEGER*)(oldLine + 5)) & 16) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 2:
         {
            /*	INSERT CODE	*/
StatisticalAdapterThread::InsertAdapter(new_adapter_id);
         }
         break;
      case 4:
         {
            static unsigned char	cond_mask[] = {
               0, 0, 17, 17, 17, 17, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 
               255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 255, 
            };

            if (equal (newLine, oldLine, cond_mask, sizeof cond_mask) != 0)
            {
               /*	CHANGE MASK CODE	*/
StatisticalAdapterThread::UpdateAdapter(new_adapter_id, new_stat_dir_name, new_dest_dir_name, new_owner_name, new_group_name, new_dir_permissions, new_file_permissions);
            }
         }
         break;
      case 8:
         {
            /*	DELETE CODE	*/
StatisticalAdapterThread::DeleteAdapter(old_adapter_id);
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_stat_adapter_unload (void* context)
{
   // No code ..
}


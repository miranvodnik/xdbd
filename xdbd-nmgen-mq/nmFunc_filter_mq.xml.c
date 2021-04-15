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
#include "clip_string.h"	// clip_string(), equal()
#include "nmSolidNtwrk.h"

/************************************************************************/
/* VARIABLE DECLARATIONS                                                */
/************************************************************************/
static  GX_PID_T g2x_self __attribute__ ((unused)) = SDL_NULL;

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void* nmFunc_filter_load (int n, char* p[]);
int nmFunc_filter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_filter_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_filter_load (int n, char* p[])
{
   return 0;
}

int nmFunc_filter_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "filter";
   static const SQLSMALLINT	UserInfo;



   SQLINTEGER	new_nodeid = *(SQLINTEGER*)(newLine + 8);
   SQLINTEGER	old_nodeid = *(SQLINTEGER*)(oldLine + 8);
   SQLINTEGER	new_nodeid_null = (((*(SQLINTEGER*)(newLine + 2)) & 1) != 0) ? SQL_NULL_DATA : 0;
   SQLINTEGER	old_nodeid_null = (((*(SQLINTEGER*)(oldLine + 2)) & 1) != 0) ? SQL_NULL_DATA : 0;


   switch (Data_Op_Type)
   {
      case 4:
         {
            if (
            (new_nodeid != old_nodeid)
            )
            {
               /*	CHANGE MASK CODE	*/
            }
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_filter_unload (void* context)
{
   // No code ..
}


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
void* nmFunc_vxml_server_load (int n, char* p[]);
int nmFunc_vxml_server_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size);
void nmFunc_vxml_server_unload (void *context);

/************************************************************************/
/* USER DEFINED FUNCTIONS THAT SENDS SIGNALS                            */
/************************************************************************/
void* nmFunc_vxml_server_load (int n, char* p[])
{
   return 0;
}

int nmFunc_vxml_server_fire (void *context, SQLINTEGER Data_Op_Type, SQLVARCHAR* oldLine, SQLVARCHAR* newLine, SQLINTEGER size)
{

   static const char*		TableName = "vxml_server";
   static const SQLSMALLINT	UserInfo;





   switch (Data_Op_Type)
   {
      case 2:
         {
            /*	INSERT CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_serverdata (G2P_FINDPID(vxmlmm));
}
         }
         break;
      case 4:
         {
            static unsigned char	cond_mask[] = {
               0, 0, 17, 17, 17, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 
               255, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 255, 255, 255, 255, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 
               255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
               255, 255, 0, 0, 0, 0, 0, 0, 
            };

            if (equal (newLine, oldLine, cond_mask, sizeof cond_mask) != 0)
            {
               /*	CHANGE MASK CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_serverdata (G2P_FINDPID(vxmlmm));
}
            }
         }
         break;
      case 8:
         {
            /*	DELETE CODE	*/
NMNG_ENABLE_OR_DISABLE
SDL_OUTPUT_trg_vxml_serverdata (G2P_FINDPID(vxmlmm));
}
         }
         break;
      default:
         break;
   }
   return 0;
}

void nmFunc_vxml_server_unload (void* context)
{
   // No code ..
}


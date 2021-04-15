/*#######################################################################*/
/*#                                                                     #*/
/*#                                                                     #*/
/*#            Copyright (c) 2002 IskraTEL                              #*/
/*#                                                                     #*/
/*#                                                                     #*/
/*# Name        : clip_string.c                                         #*/
/*#                                                                     #*/
/*# Description : Function: clip_string                                 #*/
/*#                                                                     #*/
/*# Code        : GTBV - XAE6437                                        #*/
/*#                                                                     #*/
/*# Date        : Jul 2002                                              #*/
/*#                                                                     #*/
/*# Author      : Matej Susnik                                          #*/
/*#                                                                     #*/
/*# Translation :                                                       #*/
/*#                                                                     #*/
/*# Remarks     :                                                       #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*#######################################################################*/

/****************************************************************/
/*          UNIX - SCCS  VERSION DESCRIPTION                    */
/****************************************************************/
/* static char	unixid[] = "@(#)GTBV.z	1.4	12/02/13	clip_string.c -O-"; */

/*
 * clip_string
 */

/*#######################################################################*/
/*#                                                                     #*/
/*# Author      : Janez Dolenc                                          #*/
/*#                                                                     #*/
/*# Revisions   :                                                       #*/
/*#                                                                     #*/
/*# IN par      : string                                                #*/
/*#                                                                     #*/
/*# OUT par     : string                                                #*/
/*#                                                                     #*/
/*# Description : Discard trailing blanks from the string.              #*/
/*#                                                                     #*/
/*#######################################################################*/

void clip_string (char* str)
{
        char    *begin = str;

        while (*str++);
        for (str -= 2 ; str >= begin && *str == ' '; str--);
        *(str + 1) = '\0';

	return;
}

int equal (unsigned char* newLine, unsigned char* oldLine, unsigned char* mask, int lineSize)
{
	for (; (lineSize > 0) && ((*mask == 0) || (((*newLine ^ *oldLine) & *mask) == 0));
		++newLine, ++oldLine, ++mask, --lineSize)
		;
	return lineSize;
}

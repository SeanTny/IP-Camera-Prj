/*
 * page_manager.c
 *
 *  Created on: 2017Äê5ÔÂ30ÈÕ
 *      Author: tusen
 */

#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <page/page_manager.h>



static PT_PageAction g_ptPageActionHead;


int RegisterPageAction(PT_PageAction ptPageAction)
{
	PT_PageAction ptTmp;

	if (!g_ptPageActionHead)
	{
		g_ptPageActionHead   = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPageActionHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}





void ShowPages(void)
{
	int i = 0;
	PT_PageAction ptTmp = g_ptPageActionHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}




PT_PageAction Page(char *pcName)
{
	PT_PageAction ptTmp = g_ptPageActionHead;

	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}




int PagesInit(void)
{
	int iError=0;



	return iError;
}

























/*
 * stdout.c
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */
#include <config.h>
#include <debug/debug_manager.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>



static int StdoutDebugInit(void);
static int StdoutDebugExit(void);
static int StdoutDebugPrint(const char *StrData);


static T_DebugClass gs_tStdoutDebugObj={
		.pcName="stdout",
		.iIsCanUse=1,
		.DebugInit=StdoutDebugInit,
		.DebugExit=StdoutDebugExit,
		.DebugPrint=StdoutDebugPrint,
};


static int StdoutDebugInit(void)
{

	return 0;
}


static int StdoutDebugExit(void)
{
	return 0;
}


static int StdoutDebugPrint(const char *strData)
{

	/* 直接把信息打印出来即可 */
	printf("%s",strData);

	return (strlen(strData));
}




extern int RegisterDebugObj(PT_DebugClass ptDebugObj  );
/* 对外的函数 */
int AddStdoutObj(void)
{
	int iError;

	iError= RegisterDebugObj(&gs_tStdoutDebugObj);

	return  iError;
}













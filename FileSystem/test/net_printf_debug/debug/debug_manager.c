/*
 * debug_manager.c
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */
#include <config.h>
#include <stdarg.h>
#include <debug/debug_manager.h>
#include <string.h>


static PT_DebugClass gs_ptDebugObjHead=NULL;
static int g_iDebugLeveLimit = 8;




/******************************** 构造 *************************************************/
/* 初始化函数，构造manager串起来下面的对象 */
extern int AddStdoutObj(void);
extern int AddNetPrintObj(void);
int DebugObjInit(void)
{
	int iError;

	iError=AddStdoutObj();

	iError|=AddNetPrintObj();

	return iError;
}


/* 注册函数：在初始化过程中回调注册 */
int RegisterDebugObj(PT_DebugClass ptDebugObj  )
{
	PT_DebugClass ptTmpObj;

	if(!gs_ptDebugObjHead) //如果链表头是空的话
	{
		gs_ptDebugObjHead=ptDebugObj;
		gs_ptDebugObjHead->ptNext=NULL;
	}
	else
	{
		ptTmpObj=gs_ptDebugObjHead;
		while(ptTmpObj->ptNext)
		{
			ptTmpObj=ptTmpObj->ptNext;
		}
		ptTmpObj->ptNext=ptDebugObj;
		ptDebugObj->ptNext=NULL;
	}
	return 0;
}






/******************************* 使用 *************************************************/
/* 提供对上层主函数使用的 */

int ShowAllDebugObj(void)
{
	int iOprCount=0;
	PT_DebugClass ptTmpObj=gs_ptDebugObjHead;

	while(ptTmpObj)
	{
		printf("%02d %s\n", iOprCount++, ptTmpObj->pcName);
		ptTmpObj=ptTmpObj->ptNext;
	}

	return 0;
}


/* 依据名字获取对象指针 */
PT_DebugClass GetBebugObjp(char *pcName)
{
	PT_DebugClass ptTmpObj=gs_ptDebugObjHead;

	while(ptTmpObj)
	{
		if(strcmp(ptTmpObj->pcName,pcName)==0)
		{
			return ptTmpObj;
		}
		ptTmpObj=ptTmpObj->ptNext;
	}
	return NULL;
}


/* 设置打印级别
 * strBuf="debuglevel=<0-7>"
 * */
int  SetDebugLeve(char *strBuf)
{
	g_iDebugLeveLimit=strBuf[11]-'0';
	return 0;
}



/* 设置打印通道
 * strBuf=	 * stdout=0				  :关闭stdout打印
			 * stdout=1				  :开启studout打印
			 * netprint=0			  :关闭netprint打印
			 * netprint=1			  :开启netprint打印
 * */
int SetDebugChanel(char *strBuf)
{
	char *pstrTmp;
	char strName[100];
	PT_DebugClass ptTmpObj;

	/* 获取名字 */
	pstrTmp=strchr(strBuf,'='); //如果有等于号的话 pstrTmp就指向这个等于号（第一次出现）
	if(!pstrTmp)
	{
		return -1;
	}
	else
	{
		strncpy(strName,strBuf,pstrTmp-strBuf);
		strName[pstrTmp-strBuf]='\0';

		/* 依据名字找到Obj将canUse设置为0 */
		ptTmpObj=GetBebugObjp(strName);
		if( NULL == ptTmpObj )
		{
			return -1;
		}
		if(pstrTmp[1]=='0')
		{
			ptTmpObj->iIsCanUse=0;
		}
		else if(pstrTmp[1]=='1' )
		{
			ptTmpObj->iIsCanUse=1;
		}
		return 0;
	}

	return 0;
}



int managerDebugPrint(const char *pcFormat, ... )
{
	char strTmpBuf[1000];
	char *pcTmp=strTmpBuf;
	va_list tArg;
	int iNum;
	PT_DebugClass ptTmpObj=gs_ptDebugObjHead;
	int dbglevel=DEFAULT_DEBUG_LEVEL;

	/* 获取字符数据到临时缓冲区 */
	va_start(tArg,pcFormat);
	iNum=vsprintf(strTmpBuf,pcFormat,tArg); //将数据放入一个临时缓冲区
	va_end(tArg);
	strTmpBuf[iNum]='\0'; //保险起见

	/* 根据打印级别确定是否打印 */
	if((strTmpBuf[0]=='<') && (strTmpBuf[2]=='>'))
	{
		dbglevel=strTmpBuf[1]-'0';
		if((dbglevel>=0) && (dbglevel<=9))
		{
			pcTmp=strTmpBuf + 3;
			//pcTmp+=3;
		}
		else
		{
			dbglevel=DEFAULT_DEBUG_LEVEL;
		}
	}

	if(dbglevel>g_iDebugLeveLimit) //我们传入的信息高于 打印限制的话
	{
		return -1;
	}


	/* 调用链表中所有的iIsCanUse为1的结构体的Debugprint函数*/
	while(ptTmpObj)
	{
		if(ptTmpObj->iIsCanUse==1)
		{
			ptTmpObj->DebugPrint(pcTmp);
		}
		ptTmpObj=ptTmpObj->ptNext;
	}

	return 0;
}





int InitDebugChanel(void)
{

	PT_DebugClass ptTmpObj=gs_ptDebugObjHead;

	while(ptTmpObj)
	{
		if((ptTmpObj->iIsCanUse==1) && (ptTmpObj->DebugInit) )
		{
			//printf("DebugInit !!! the name is %s \n ",ptTmpObj->pcName);
			ptTmpObj->DebugInit() ;
		}


		ptTmpObj=ptTmpObj->ptNext;

	}



	return 0;
}














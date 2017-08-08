/*
 * debug_manager.c
 *
 *  Created on: 2017��5��28��
 *      Author: tusen
 */
#include <config.h>
#include <stdarg.h>
#include <debug/debug_manager.h>
#include <string.h>


static PT_DebugClass gs_ptDebugObjHead=NULL;
static int g_iDebugLeveLimit = 8;




/******************************** ���� *************************************************/
/* ��ʼ������������manager����������Ķ��� */
extern int AddStdoutObj(void);
extern int AddNetPrintObj(void);
int DebugObjInit(void)
{
	int iError;

	iError=AddStdoutObj();

	iError|=AddNetPrintObj();

	return iError;
}


/* ע�ắ�����ڳ�ʼ�������лص�ע�� */
int RegisterDebugObj(PT_DebugClass ptDebugObj  )
{
	PT_DebugClass ptTmpObj;

	if(!gs_ptDebugObjHead) //�������ͷ�ǿյĻ�
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






/******************************* ʹ�� *************************************************/
/* �ṩ���ϲ�������ʹ�õ� */

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


/* �������ֻ�ȡ����ָ�� */
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


/* ���ô�ӡ����
 * strBuf="debuglevel=<0-7>"
 * */
int  SetDebugLeve(char *strBuf)
{
	g_iDebugLeveLimit=strBuf[11]-'0';
	return 0;
}



/* ���ô�ӡͨ��
 * strBuf=	 * stdout=0				  :�ر�stdout��ӡ
			 * stdout=1				  :����studout��ӡ
			 * netprint=0			  :�ر�netprint��ӡ
			 * netprint=1			  :����netprint��ӡ
 * */
int SetDebugChanel(char *strBuf)
{
	char *pstrTmp;
	char strName[100];
	PT_DebugClass ptTmpObj;

	/* ��ȡ���� */
	pstrTmp=strchr(strBuf,'='); //����е��ںŵĻ� pstrTmp��ָ��������ںţ���һ�γ��֣�
	if(!pstrTmp)
	{
		return -1;
	}
	else
	{
		strncpy(strName,strBuf,pstrTmp-strBuf);
		strName[pstrTmp-strBuf]='\0';

		/* ���������ҵ�Obj��canUse����Ϊ0 */
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

	/* ��ȡ�ַ����ݵ���ʱ������ */
	va_start(tArg,pcFormat);
	iNum=vsprintf(strTmpBuf,pcFormat,tArg); //�����ݷ���һ����ʱ������
	va_end(tArg);
	strTmpBuf[iNum]='\0'; //�������

	/* ���ݴ�ӡ����ȷ���Ƿ��ӡ */
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

	if(dbglevel>g_iDebugLeveLimit) //���Ǵ������Ϣ���� ��ӡ���ƵĻ�
	{
		return -1;
	}


	/* �������������е�iIsCanUseΪ1�Ľṹ���Debugprint����*/
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














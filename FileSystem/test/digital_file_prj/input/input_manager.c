#include <config.h>
#include <input_manager.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>


static PT_InputOpr g_ptInputOprHead;
static 	fd_set g_tRFds;
static int g_iMaxFd=-1;

int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		g_ptInputOprHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptInputOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}

	return 0;
}


void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_InputOpr GetInputpOpr(char *pcName)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;

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



int  AllInputDeviceInit(void )
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError=-1;

	FD_ZERO(&g_tRFds);

	while (ptTmp)
	{
		if(0==(ptTmp->DeviceInit()))
		{
			FD_SET(ptTmp->iFd,&g_tRFds);
			if(g_iMaxFd<ptTmp->iFd)
				g_iMaxFd=ptTmp->iFd;
			iError=0; //ֻҪ��һ���ɹ���ʼ�������Ϊ��
		}
		ptTmp  = ptTmp->ptNext;
	}
	g_iMaxFd++;
	return iError;
}


int GetInputEvevt(PT_InputEvent ptInputEvent)
{
/* ʹ��select�������stdin,touchscreen
 * ������ʱ���ٵ���stdin��touchscreen����þ�����¼�
 * */
	PT_InputOpr ptTmp=g_ptInputOprHead;
	fd_set tRFds;
	int iRet;

	tRFds=g_tRFds;

	iRet=select(g_iMaxFd,&tRFds,NULL,NULL,NULL);
	if(iRet>0)
	{
		while (ptTmp)
		{
			if(FD_ISSET(ptTmp->iFd,&tRFds))
			{
				iRet=ptTmp->GetInputEvent(ptInputEvent);
				if(iRet==0)
				{
					return 0;
				}
			}
			ptTmp  = ptTmp->ptNext;
		}


	}
	return -1;



//	/* ��ѯ��ʽ���������е�InputOpr��GetInputEvent����������һ�Σ�һ�������ݾͷ��� */
//	PT_InputOpr ptTmp = g_ptInputOprHead;
//	int iError=-1;
//
//	while (ptTmp)
//	{
//		if(0==ptTmp->GetInputEvent(ptInputEvent))
//		{
//			return 0;
//		}
//		ptTmp  = ptTmp->ptNext;
//	}
//	//û�д�while���淵�أ���û��ɨ�赽�¼�
//	return -1;
}


int InputInit(void)
{
	int iError;

	iError  = StdinInit();

	iError |= TouchScreenInit();

	return iError;
}


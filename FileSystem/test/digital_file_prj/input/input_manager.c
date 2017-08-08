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
			iError=0; //只要有一个成功初始化这个就为零
		}
		ptTmp  = ptTmp->ptNext;
	}
	g_iMaxFd++;
	return iError;
}


int GetInputEvevt(PT_InputEvent ptInputEvent)
{
/* 使用select函数检测stdin,touchscreen
 * 有数据时候再调用stdin，touchscreen来获得具体的事件
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



//	/* 轮询方式：把链表中的InputOpr的GetInputEvent函数都调用一次，一旦有数据就返回 */
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
//	//没有从while里面返回，就没有扫描到事件
//	return -1;
}


int InputInit(void)
{
	int iError;

	iError  = StdinInit();

	iError |= TouchScreenInit();

	return iError;
}


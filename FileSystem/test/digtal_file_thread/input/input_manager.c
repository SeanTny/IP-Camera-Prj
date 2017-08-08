
#include <config.h>
#include <input_manager.h>
#include <string.h>
#include <pthread.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent gtInputEvent;
static pthread_mutex_t gtMutex =PTHREAD_MUTEX_INITIALIZER;//������
static pthread_cond_t gtCondvar=PTHREAD_COND_INITIALIZER; //��������

int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		ptInputOpr->ptNext = NULL;
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


static void* inputEventThreadFunction (void *pVoid)
{

	T_InputEvent tInputEvent;
	int iError;
	int (*tmGetInputEvent)(PT_InputEvent ptInputEvent);


	tmGetInputEvent=(int (*)(PT_InputEvent ))pVoid;

	while(1)
	{
		iError=tmGetInputEvent(&tInputEvent);
		if (iError==0)							//��ȡ�¼��ɹ�
		{
			/* ����������,��tInputEvent��ֵ��ֵ��һ��ȫ�ֱ��� */
			/* ȫ�ֱ����ķ��ʱ���Ҫ��û������ſ��� */
			pthread_mutex_lock(&gtMutex);

			gtInputEvent=tInputEvent;
			pthread_cond_signal(&gtCondvar); //���ѵȴ�����������߳�

			pthread_mutex_unlock(&gtMutex);
		}
	}

	return (void *)NULL;
}

int AllInputDevicesInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())
		{
			/* �������߳� */
			iError=pthread_create(&ptTmp->tThreadID,NULL,
								   inputEventThreadFunction,ptTmp->GetInputEvent);
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* �������� */

	pthread_mutex_lock(&gtMutex);
	pthread_cond_wait(&gtCondvar,&gtMutex);

	/* ����֮��ʼ���� */
	*ptInputEvent=gtInputEvent;
	pthread_mutex_unlock(&gtMutex);

	return 0;
}

int InputInit(void)
{
	int iError;
	iError = StdinInit();
	iError |= TouchScreenInit();
	return iError;
}


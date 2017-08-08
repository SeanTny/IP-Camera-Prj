
#include <config.h>
#include <input_manager.h>
#include <string.h>
#include <pthread.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent gtInputEvent;
static pthread_mutex_t gtMutex =PTHREAD_MUTEX_INITIALIZER;//互斥量
static pthread_cond_t gtCondvar=PTHREAD_COND_INITIALIZER; //条件变量

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
		if (iError==0)							//获取事件成功
		{
			/* 唤醒主进程,把tInputEvent的值赋值给一个全局变量 */
			/* 全局变量的访问必须要获得互次锁才可以 */
			pthread_mutex_lock(&gtMutex);

			gtInputEvent=tInputEvent;
			pthread_cond_signal(&gtCondvar); //唤醒等待这个变量的线程

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
			/* 创建子线程 */
			iError=pthread_create(&ptTmp->tThreadID,NULL,
								   inputEventThreadFunction,ptTmp->GetInputEvent);
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 这里休眠 */

	pthread_mutex_lock(&gtMutex);
	pthread_cond_wait(&gtCondvar,&gtMutex);

	/* 唤醒之后开始动作 */
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


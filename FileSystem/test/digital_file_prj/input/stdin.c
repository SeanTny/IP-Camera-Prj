/*
 * stdin.c
 *
 *  Created on: 2017年4月6日
 *      Author: tusen
 *     *5.1：使用轮询的方法，
 *      5.2：使用select方式
 *      5.3：使用多线程的方式
 */
#include <input_manager.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>




static int  StdinDevInit(void);
static int  StdinDevExit(void);
static int  StdinGetInputEvent(PT_InputEvent ptInputEvent);


static T_InputOpr g_tStdinOpr={
	.name			="stdin",
	.DeviceInit		=StdinDevInit,
	.DeviceExit		=StdinDevExit,
	.GetInputEvent  =StdinGetInputEvent,
};



static int  StdinDevInit(void)
{
    struct termios tTTYState;

	//get the terminal state
	tcgetattr(STDIN_FILENO,&tTTYState);

	//trun off canonical mode
	tTTYState.c_lflag &= ~ICANON;

	//minimum of number input read
	tTTYState.c_cc[VMIN]=1;   				/* 有一个数据时，立刻返回 */

	//set the terminal attributes.
	tcsetattr(STDIN_FILENO,TCSANOW,&tTTYState);

	g_tStdinOpr.iFd=STDIN_FILENO;

	return 0;
}


static int  StdinDevExit(void)
{
	struct  termios tTTYState;

	//get the terminal state
	tcgetattr(STDIN_FILENO,&tTTYState);

	//trun on canonical mode
	tTTYState.c_lflag |= ~ICANON;

	//set the terminal attributes.
	tcsetattr(STDIN_FILENO,TCSANOW,&tTTYState);

	return 0;

}




static int  StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	/*查询的方式*/
	/* 如果有数据就读取、处理、返回
	 * 如果没有数据, 立刻返回, 不等待
	 */
	/* select, poll 可以参数 UNIX环境高级编程 */

	struct timeval tTv;
	fd_set tFDs;
	char c;

	tTv.tv_sec=0;
	tTv.tv_usec=0;
	FD_ZERO(&tFDs);

	FD_SET(STDIN_FILENO,&tFDs);					//STDIN_FILENO is 0
	select(STDIN_FILENO+1,&tFDs,NULL,NULL,&tTv);

	if(FD_ISSET(STDIN_FILENO,&tFDs))
	{
		/* 处理数据 */

		ptInputEvent->iType=INPUT_TYPE_STDIN;
		gettimeofday(&ptInputEvent->tTime,NULL);

		c=fgetc(stdin);
		if(c=='u')
		{
			ptInputEvent->iVal=INPUT_VALUE_UP;
		}
		else if (c == 'n')
		{
			ptInputEvent->iVal = INPUT_VALUE_DOWN;
		}
		else if (c == 'q')
		{
			ptInputEvent->iVal = INPUT_VALUE_EXIT;
		}
		else
		{
			ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
		}
		return 0;
	}else
	{
		return -1;

	}
}



int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}



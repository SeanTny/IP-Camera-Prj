/*
 * netprint.c
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */
#include <config.h>
#include <debug/debug_manager.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>



static int NetPrintDebugInit(void);
static int NetPrintbugExit(void);
static int NetPrintbugPrint(const char *strData);


static T_DebugClass gs_tNetPrintDebugObj={
		.pcName="netprint",
		.iIsCanUse=1,
		.DebugInit=NetPrintDebugInit,
		.DebugExit=NetPrintbugExit,
		.DebugPrint=NetPrintbugPrint,
};



#define SERVER_PORT 	5000
#define PRINT_BUF_SIZE  (16*1024)
#define RECV_BUF_SIZE 	1000


static int 				    g_iSocketSerFd;
static struct  sockaddr_in  g_tSockServAddr, g_tSockClienAddr;
static int g_iHaveConnected=0;
static char					*g_pcNetPrintBuffer;
static int 	g_iReadPos=0;
static int 	g_iWritePos=0;

static  pthread_t g_tSendThreadID;
static  pthread_t g_tRecvThreadID;

static pthread_mutex_t g_NetDebugSendMutex =PTHREAD_MUTEX_INITIALIZER;//互斥量
static pthread_cond_t  g_NetDebugSendCondvar=PTHREAD_COND_INITIALIZER; //条件变量



static int GetData(char *pcVal);
static int isEmpty(void);

static void* NetDebugSendThreadFunction (void *pVoid)
{
	char strTmpBuf[512];
	char cVal;
	ssize_t iSendLenEd;
	int i;

	printf("NetDebugSendThreadFunction Created \n");

	while(1)
	{
		/* 平时在休眠状态 */
		/* 这里休眠 */
		pthread_mutex_lock(&g_NetDebugSendMutex);
		pthread_cond_wait(&g_NetDebugSendCondvar,&g_NetDebugSendMutex);
		pthread_mutex_unlock(&g_NetDebugSendMutex);

		/* 唤醒之后开始动作 */

		TS_PRINT("Netprint send thread is wake !!!\n");

		/* 如果有客户端链接了,并且非空，就通过网络发送给客户端 */
		while((!isEmpty())&&(g_iHaveConnected)) //不是空的才往下面跑
		{
			TS_PRINT("the buffer is empty and ihave Connected\n" );

			/* 把环形缓冲区的数据取出来到一个临时的buf里面，最多取512字节 */
			i=0;
			while( (i<512) && (0 == GetData(&cVal)) )
			{
				strTmpBuf[i]=cVal;
				i++;
			}
			/*使用sendto函数发送临时缓冲区的打印信息给客户端 */
			iSendLenEd=sendto(g_iSocketSerFd,
					strTmpBuf,
					i,
					0,
					(struct sockaddr *)&g_tSockClienAddr,
					sizeof(g_tSockClienAddr));

		}

		if(isEmpty() )
		{
			TS_PRINT(" the buf is empty \n ");
		}
		else if( g_iHaveConnected==0 )
		{
			TS_PRINT("the client is not connect \n");
		}




	}
	return NULL;
}


/* 反过来调用上层的函数需要进行extern声明 */
extern int  SetDebugLeve(char *strBuf);
extern int  SetDebugChanel(char *strBuf);
static void* NetDebugRecvThreadFunction (void *pVoid)
{

	socklen_t iAddrLen;
	ssize_t  iByteRecv;
	struct sockaddr_in tSockClienAddr;
	char cRecvBuf[RECV_BUF_SIZE];

	TS_PRINT("NetDebugRecvThreadFunction Created \n");

	while(1)
	{
		iAddrLen=sizeof(tSockClienAddr);
		/* recvfrom 没有数据的话本身就处于休眠状态 */
		iByteRecv=recvfrom(g_iSocketSerFd,cRecvBuf, RECV_BUF_SIZE-1,0,
	                        (struct sockaddr *)&tSockClienAddr, &iAddrLen);
		if(cRecvBuf>0)
		{
			cRecvBuf[iByteRecv]='\0';
			/* 解析数据：
			 * debuglevel=0,1,2....   :修改打印级别
			 * setclient              :设置接受打印信息的客户端
			 * stdout=0				  :关闭stdout打印
			 * stdout=1				  :开启studout打印
			 * netprint=0			  :关闭netprint打印
			 * netprint=1			  :开启netprint打印
			 * */
			if(strcmp(cRecvBuf,"setclient")==0)
			{
				g_tSockClienAddr=tSockClienAddr;
				g_iHaveConnected=1;

				/* 客户机第一次链接了 */
				/* 唤醒发送线程，他会将环形缓冲区内的数据发送出去 */
				pthread_mutex_lock(&g_NetDebugSendMutex);
				pthread_cond_signal(&g_NetDebugSendCondvar); //唤醒等待这个变量的线程
				pthread_mutex_unlock(&g_NetDebugSendMutex);
			}
			else if(strncmp(cRecvBuf,"debuglevel=",11)==0)
			{
				SetDebugLeve(cRecvBuf); //这两个函数都应该放在 manager.c里面实现
			}
			else
			{
				SetDebugChanel(cRecvBuf);//这两个函数都应该放在 manager.c里面实现
			}
		}
	}

	return NULL;
}





static int NetPrintDebugInit(void)
{
	/* socket 初始化 */

	int iError=0;


	g_iSocketSerFd=socket( AF_INET , SOCK_DGRAM, 0);
	if(-1==g_iSocketSerFd)
	{
		perror("socket");
		return -1;
	}

	memset(&g_tSockServAddr,0,sizeof(g_tSockServAddr));  //清零，保持其他为零最好
	g_tSockServAddr.sin_family=AF_INET;
	g_tSockServAddr.sin_addr.s_addr=htonl(INADDR_ANY); //本机上的所有ip
	g_tSockServAddr.sin_port=htons(SERVER_PORT);

	iError=bind(g_iSocketSerFd,(const struct sockaddr *)&g_tSockServAddr,
			sizeof(g_tSockServAddr));
	if(-1==iError)
	{
		perror("bind");
		return -1;
	}

	g_pcNetPrintBuffer=malloc(PRINT_BUF_SIZE);
	if(NULL==g_pcNetPrintBuffer)
	{
		perror("malloc");
		close(g_iSocketSerFd);
		return -1;
	}

	/* 创建netprint的发送和接收线程
	 * 发送线程：发送打印信息给客户端
	 * 接收线程：用来接收控制信息，比如修改打印级别，打开或者关闭打印
	 * */
	/* 发送线程：发送打印信息给客户端 */
	iError=pthread_create(&g_tSendThreadID,
			NULL,
			NetDebugSendThreadFunction,
			NULL);

	/* 接收线程：用来接收控制信息，比如修改打印级别，打开或者关闭打印 */
	iError=pthread_create(&g_tRecvThreadID,
			NULL,
			NetDebugRecvThreadFunction,
			NULL);

	//TS_PRINT("created threat form nteprint the iError is %d \n",iError);

	return iError;
}


static int NetPrintbugExit(void)
{
	/* 关闭socket */
	close(g_iSocketSerFd);
	free(g_pcNetPrintBuffer);
	return 0;
}

static int isFull(void)
{
	return(((g_iWritePos+1) % PRINT_BUF_SIZE)==g_iReadPos) ;
}

static int isEmpty(void)
{
	return (g_iWritePos==g_iReadPos );
}

/* 唤醒缓冲区读写数据的函数 */
static int  PutData(char cVal)
{
	if(isFull())
	{
		return -1;
	}
	else
	{
		g_pcNetPrintBuffer[g_iWritePos]=cVal;
		g_iWritePos=(g_iWritePos+1) % PRINT_BUF_SIZE;
		return 0;
	}
	return 0;
}

static int GetData(char *pcVal)
{
	if(isEmpty())
		return -1;
	else
	{
		*pcVal=g_pcNetPrintBuffer[g_iReadPos];
		g_iReadPos=(g_iReadPos+1) % PRINT_BUF_SIZE;
		return 0;
	}
return 0;
}



/***********************************************************************
 * 当我们调用 NetPrintbugPrint的时候，
 * 1)将数据放入环形缓冲区里面去,
 * 2)立刻唤醒发送线程
 * */
static int NetPrintbugPrint(const char *strData)
{
	/* 把数据放入环形缓冲区内 */
	int i;

	for(i=0;i<strlen(strData);i++)
	{
		if(0 !=PutData(strData[i]))
		{
			break;
		}
	}
	/* 唤醒netprint的发送线程 */
	/* 唤醒发送的那个线程 */
	pthread_mutex_lock(&g_NetDebugSendMutex);
	pthread_cond_signal(&g_NetDebugSendCondvar); //唤醒等待这个变量的线程
	pthread_mutex_unlock(&g_NetDebugSendMutex);

	return i;
}


extern int RegisterDebugObj(PT_DebugClass ptDebugObj  );
/* 对外的函数 */
int AddNetPrintObj(void)
{
	int iError;

	iError= RegisterDebugObj(&gs_tNetPrintDebugObj);

	return  iError;
}








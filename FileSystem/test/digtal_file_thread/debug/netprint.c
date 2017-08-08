/*
 * netprint.c
 *
 *  Created on: 2017��5��28��
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

static pthread_mutex_t g_NetDebugSendMutex =PTHREAD_MUTEX_INITIALIZER;//������
static pthread_cond_t  g_NetDebugSendCondvar=PTHREAD_COND_INITIALIZER; //��������



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
		/* ƽʱ������״̬ */
		/* �������� */
		pthread_mutex_lock(&g_NetDebugSendMutex);
		pthread_cond_wait(&g_NetDebugSendCondvar,&g_NetDebugSendMutex);
		pthread_mutex_unlock(&g_NetDebugSendMutex);

		/* ����֮��ʼ���� */

		TS_PRINT("Netprint send thread is wake !!!\n");

		/* ����пͻ���������,���ҷǿգ���ͨ�����緢�͸��ͻ��� */
		while((!isEmpty())&&(g_iHaveConnected)) //���ǿյĲ���������
		{
			TS_PRINT("the buffer is empty and ihave Connected\n" );

			/* �ѻ��λ�����������ȡ������һ����ʱ��buf���棬���ȡ512�ֽ� */
			i=0;
			while( (i<512) && (0 == GetData(&cVal)) )
			{
				strTmpBuf[i]=cVal;
				i++;
			}
			/*ʹ��sendto����������ʱ�������Ĵ�ӡ��Ϣ���ͻ��� */
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


/* �����������ϲ�ĺ�����Ҫ����extern���� */
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
		/* recvfrom û�����ݵĻ�����ʹ�������״̬ */
		iByteRecv=recvfrom(g_iSocketSerFd,cRecvBuf, RECV_BUF_SIZE-1,0,
	                        (struct sockaddr *)&tSockClienAddr, &iAddrLen);
		if(cRecvBuf>0)
		{
			cRecvBuf[iByteRecv]='\0';
			/* �������ݣ�
			 * debuglevel=0,1,2....   :�޸Ĵ�ӡ����
			 * setclient              :���ý��ܴ�ӡ��Ϣ�Ŀͻ���
			 * stdout=0				  :�ر�stdout��ӡ
			 * stdout=1				  :����studout��ӡ
			 * netprint=0			  :�ر�netprint��ӡ
			 * netprint=1			  :����netprint��ӡ
			 * */
			if(strcmp(cRecvBuf,"setclient")==0)
			{
				g_tSockClienAddr=tSockClienAddr;
				g_iHaveConnected=1;

				/* �ͻ�����һ�������� */
				/* ���ѷ����̣߳����Ὣ���λ������ڵ����ݷ��ͳ�ȥ */
				pthread_mutex_lock(&g_NetDebugSendMutex);
				pthread_cond_signal(&g_NetDebugSendCondvar); //���ѵȴ�����������߳�
				pthread_mutex_unlock(&g_NetDebugSendMutex);
			}
			else if(strncmp(cRecvBuf,"debuglevel=",11)==0)
			{
				SetDebugLeve(cRecvBuf); //������������Ӧ�÷��� manager.c����ʵ��
			}
			else
			{
				SetDebugChanel(cRecvBuf);//������������Ӧ�÷��� manager.c����ʵ��
			}
		}
	}

	return NULL;
}





static int NetPrintDebugInit(void)
{
	/* socket ��ʼ�� */

	int iError=0;


	g_iSocketSerFd=socket( AF_INET , SOCK_DGRAM, 0);
	if(-1==g_iSocketSerFd)
	{
		perror("socket");
		return -1;
	}

	memset(&g_tSockServAddr,0,sizeof(g_tSockServAddr));  //���㣬��������Ϊ�����
	g_tSockServAddr.sin_family=AF_INET;
	g_tSockServAddr.sin_addr.s_addr=htonl(INADDR_ANY); //�����ϵ�����ip
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

	/* ����netprint�ķ��ͺͽ����߳�
	 * �����̣߳����ʹ�ӡ��Ϣ���ͻ���
	 * �����̣߳��������տ�����Ϣ�������޸Ĵ�ӡ���𣬴򿪻��߹رմ�ӡ
	 * */
	/* �����̣߳����ʹ�ӡ��Ϣ���ͻ��� */
	iError=pthread_create(&g_tSendThreadID,
			NULL,
			NetDebugSendThreadFunction,
			NULL);

	/* �����̣߳��������տ�����Ϣ�������޸Ĵ�ӡ���𣬴򿪻��߹رմ�ӡ */
	iError=pthread_create(&g_tRecvThreadID,
			NULL,
			NetDebugRecvThreadFunction,
			NULL);

	//TS_PRINT("created threat form nteprint the iError is %d \n",iError);

	return iError;
}


static int NetPrintbugExit(void)
{
	/* �ر�socket */
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

/* ���ѻ�������д���ݵĺ��� */
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
 * �����ǵ��� NetPrintbugPrint��ʱ��
 * 1)�����ݷ��뻷�λ���������ȥ,
 * 2)���̻��ѷ����߳�
 * */
static int NetPrintbugPrint(const char *strData)
{
	/* �����ݷ��뻷�λ������� */
	int i;

	for(i=0;i<strlen(strData);i++)
	{
		if(0 !=PutData(strData[i]))
		{
			break;
		}
	}
	/* ����netprint�ķ����߳� */
	/* ���ѷ��͵��Ǹ��߳� */
	pthread_mutex_lock(&g_NetDebugSendMutex);
	pthread_cond_signal(&g_NetDebugSendCondvar); //���ѵȴ�����������߳�
	pthread_mutex_unlock(&g_NetDebugSendMutex);

	return i;
}


extern int RegisterDebugObj(PT_DebugClass ptDebugObj  );
/* ����ĺ��� */
int AddNetPrintObj(void)
{
	int iError;

	iError= RegisterDebugObj(&gs_tNetPrintDebugObj);

	return  iError;
}








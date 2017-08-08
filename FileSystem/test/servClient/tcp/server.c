/*
 * server.c
 *
 *  Created on: 2017��5��28��
 *      Author: tusen
 */

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>



#define SERVER_PORT 	5000
#define BACKLOG     	10
#define RECV_BUF_SIZE 	1000


/* �������������� */
/*
 * socket
 * bind
 * listen
 * accept
 * send/recv
 * */
int main(int argc, char ** argv )
{
	int iSocketSerFd,iSocketClientFd;
	struct sockaddr_in tSockServAddr,tSockClienAddr;
	int iError=0;
	int iAddrLen;
	ssize_t  iByteRecv;
	char cRecvBuf[RECV_BUF_SIZE];
	int iClientNum=-1;

	signal(SIGCHLD,SIG_IGN); //�����̽����ӽ��̵�������Ϣ������ʬ

	iSocketSerFd=socket( AF_INET , SOCK_STREAM, 0);
	if(-1==iSocketSerFd)
	{
		perror("socket");
		return -1;
	}

	memset(&tSockServAddr,0,sizeof(tSockServAddr));  //���㣬��������Ϊ�����
	tSockServAddr.sin_family=AF_INET;
	tSockServAddr.sin_addr.s_addr=htonl(INADDR_ANY); //�����ϵ�����ip
	tSockServAddr.sin_port=htons(SERVER_PORT);

	iError=bind(iSocketSerFd,(const struct sockaddr *)&tSockServAddr,
			sizeof(tSockServAddr));
	if(-1==iError)
	{
		perror("bind");
		return -1;
	}


	iError=listen(iSocketSerFd,BACKLOG);
	if(-1==iError)
	{
		perror("listen");
		return -1;
	}

	while(1)
	{
		iAddrLen=sizeof(tSockClienAddr);  //�����öԶ˴������ݺͳ��ȣ����Ǵ��������������пռ����
		iSocketClientFd=accept(iSocketSerFd,
				(struct sockaddr *)&tSockClienAddr,
				&iAddrLen);				 //�ȴ���������,���Դ��ضԶˣ��ͻ��ˣ��ĵ�ַ
		if(-1==iSocketClientFd)
		{
			perror("accept");
			return -1;
		}
		iClientNum++;
		printf("Get connet from client %d IP: %s \n",iClientNum,inet_ntoa(tSockClienAddr.sin_addr));

		if(!fork())  	//�ӽ��̷��ص��� 0
		{
			/* �ӽ���Դ�� */
			while(1)
			{
				/* ���ͻ��̷��͵�������ʾ���� */
				iByteRecv=recv(iSocketClientFd,
						cRecvBuf,
						RECV_BUF_SIZE-1,
						(int)0);//������һ������������
				if(iByteRecv<=0) //��ȡ����ĩβ���Զ˹رգ�����0 ���߶�ȡ������-1
				{
					printf("the client stream is shutdown or read error \n" );
					close(iSocketClientFd);
					return -1;
				}
				else
				{
					cRecvBuf[iByteRecv]='\0';//��֤�и���������������
					printf ("Get Msg From Client %d: %s \n",iClientNum,cRecvBuf);

					/* echo ���صõ������� */
					//send(iSocketClientFd,cRecvBuf,strlen(cRecvBuf),0);
				}

			}
		}
		else
		{
			/* ������Դ�� */
		}
	}
	close(iSocketSerFd);
	return 0;
}




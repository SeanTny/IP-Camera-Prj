/*
 * server.c
 *
 *  Created on: 2017年5月28日
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


/* 服务器的主函数 */
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

	signal(SIGCHLD,SIG_IGN); //父进程接受子进程的死亡消息，并收尸

	iSocketSerFd=socket( AF_INET , SOCK_STREAM, 0);
	if(-1==iSocketSerFd)
	{
		perror("socket");
		return -1;
	}

	memset(&tSockServAddr,0,sizeof(tSockServAddr));  //清零，保持其他为零最好
	tSockServAddr.sin_family=AF_INET;
	tSockServAddr.sin_addr.s_addr=htonl(INADDR_ANY); //本机上的所有ip
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
		iAddrLen=sizeof(tSockClienAddr);  //可以让对端传来数据和长度，这是传出参数，必须有空间才行
		iSocketClientFd=accept(iSocketSerFd,
				(struct sockaddr *)&tSockClienAddr,
				&iAddrLen);				 //等待接受链接,可以传回对端（客户端）的地址
		if(-1==iSocketClientFd)
		{
			perror("accept");
			return -1;
		}
		iClientNum++;
		printf("Get connet from client %d IP: %s \n",iClientNum,inet_ntoa(tSockClienAddr.sin_addr));

		if(!fork())  	//子进程返回的是 0
		{
			/* 子进程源码 */
			while(1)
			{
				/* 将客户短发送的数据显示出来 */
				iByteRecv=recv(iSocketClientFd,
						cRecvBuf,
						RECV_BUF_SIZE-1,
						(int)0);//至少留一个做结束符号
				if(iByteRecv<=0) //读取到了末尾（对端关闭）返回0 或者读取出错返回-1
				{
					printf("the client stream is shutdown or read error \n" );
					close(iSocketClientFd);
					return -1;
				}
				else
				{
					cRecvBuf[iByteRecv]='\0';//保证有个结束符号在里面
					printf ("Get Msg From Client %d: %s \n",iClientNum,cRecvBuf);

					/* echo 返回得到的数据 */
					//send(iSocketClientFd,cRecvBuf,strlen(cRecvBuf),0);
				}

			}
		}
		else
		{
			/* 父进程源码 */
		}
	}
	close(iSocketSerFd);
	return 0;
}




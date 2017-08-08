/*
 * Client.c
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


#define SERVER_PORT 	5000
#define SEND_BUF_SIZE   1000
#define RECV_BUF_SIZE   1000

/*客户端编程
 * socket
 * connect
 * send/recv
 * */
int main (int argc, char  **argv)
{

	int iSocketClentFd;
	int iError;
	struct sockaddr_in tServerAddr;
	unsigned char ucSendBuffer[SEND_BUF_SIZE];
	unsigned char ucRecvbuffer[RECV_BUF_SIZE];
	ssize_t  iRecvLenEd;
	ssize_t iSendLenEd;

	if(argc!=2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n",argv[0]);
		return -1;
	}

	/*socket*/
    iSocketClentFd=socket(AF_INET, SOCK_STREAM, 0);
	if(-1==iSocketClentFd)
	{
		perror("socket");
		return -1;
	}

	/* connect */
	memset(&tServerAddr,0,sizeof(tServerAddr)); //清零，保持其他为零最好
	tServerAddr.sin_family=AF_INET;
	tServerAddr.sin_port=htons(SERVER_PORT);
	if(0==inet_aton(argv[1],
			&tServerAddr.sin_addr))
	{
		perror("inet_aton invalid server ip ");
		return -1;
	}
	iError=connect(iSocketClentFd, (struct sockaddr *)&tServerAddr,sizeof(tServerAddr));
	if(-1==iError)
	{
		perror("connect");
		return -1;
	}

    while(1)
    {
    	if(fgets(ucSendBuffer,SEND_BUF_SIZE-1 , stdin))//保证最后一个是\0
    	{
    		iSendLenEd=send(iSocketClentFd, ucSendBuffer, strlen(ucSendBuffer), 0); //这里是字符串长度，不是sizeof测量
    		if(iSendLenEd<=0)
    		{
    			perror("send error");
    			close(iSocketClentFd);
    			return -1;
    		}

//    		iRecvLenEd=recv(iSocketClentFd, ucRecvbuffer,RECV_BUF_SIZE-1,(int)0);//至少留一个做结束符号
//    		printf("%s",ucRecvbuffer);
    	}

    }

	return 0;
}



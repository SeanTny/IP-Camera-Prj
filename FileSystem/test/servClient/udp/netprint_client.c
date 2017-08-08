/*
 * netprint_client.c
 *
 *  Created on: 2017年5月29日
 *      Author: tusen
 */

/*
 * client.c
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */


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
 * send/recv  sendto/recvfrom
 *
 * ./netprint_client <server_ip> debuglevel=<0-9>
 * ./netprint_client <server_ip> setclient
 * ./netprint_client <server_ip> stdout=0|1
 * ./netprint_client <server_ip> netprint=0|1
 * ./netprint_client <server_ip> show 			// setclient,接收并且打印信息
 *
 * */
int main (int argc, char  **argv)
{

	int iSocketClentFd;
	int iError;
	struct sockaddr_in tServerAddr;
	unsigned char ucSendBuffer[SEND_BUF_SIZE];
	unsigned char ucRecvbuffer[RECV_BUF_SIZE];
	ssize_t  iRecvLenEd;
	ssize_t  iSendLenEd;
	socklen_t iAddrServLen;

	if(argc!=3)
	{
		printf("Usage:\n");
		printf("%s  <server_ip> debuglevel=<0-9>\n",argv[0]);
		printf("%s  <server_ip> stdout=0|1      \n",argv[0]);
		printf("%s  <server_ip> netprint=0|1    \n",argv[0]);
		printf("%s  <server_ip> show 			\n",argv[0]);
		return -1;
	}

	/*socket*/
    iSocketClentFd=socket(AF_INET, SOCK_DGRAM, 0);
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



	if(strcmp(argv[2],"show")==0)
	{
		/* 发送数据 */
		iSendLenEd=sendto(iSocketClentFd,
				"setclient",
				strlen("setclient"),
				0,
				(struct sockaddr *)&tServerAddr, sizeof(tServerAddr));
		if(iSendLenEd<=0)
		{
			perror("send error");
			close(iSocketClentFd);
			return -1;
		}

		/* 循环从网络读取数据打印出来 */
		while(1)
		{
			iAddrServLen=sizeof(tServerAddr);
			iRecvLenEd=recvfrom(iSocketClentFd,ucRecvbuffer, RECV_BUF_SIZE-1,0,
		                        (struct sockaddr *)&tServerAddr, &iAddrServLen);

			if(iRecvLenEd>0)
			{
				ucRecvbuffer[iRecvLenEd]='\0';
				printf("%s\n",ucRecvbuffer);
			}

		}
	}
	else
	{
		/* 发送数据 */
		iSendLenEd=sendto(iSocketClentFd,
				argv[2],
				strlen(argv[2]),
				0,
				(struct sockaddr *)&tServerAddr, sizeof(tServerAddr));
		if(iSendLenEd<=0)
		{
			perror("send error");
			close(iSocketClentFd);
			return -1;
		}
	}

	return 0;
}







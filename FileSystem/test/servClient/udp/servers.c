/*
 * servers.c
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */


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
#define RECV_BUF_SIZE 	1000


/* 服务器的主函数 */
/*
 * socket
 * bind
 * listen
 * accept
 * sendto/recvfrom
 * */
int main(int argc, char ** argv )
{
	int iSocketSerFd,iSocketClientFd;
	struct sockaddr_in tSockServAddr,tSockClienAddr;
	int iError=0;
	socklen_t iAddrLen;
	ssize_t  iByteRecv;
	char cRecvBuf[RECV_BUF_SIZE];



	iSocketSerFd=socket( AF_INET , SOCK_DGRAM, 0);
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

	while(1)
	{
		iAddrLen=sizeof(tSockClienAddr);
		iByteRecv=recvfrom(iSocketSerFd,cRecvBuf, RECV_BUF_SIZE-1,0,
	                        (struct sockaddr *)&tSockClienAddr, &iAddrLen);

		if(iByteRecv>0)
		{
			cRecvBuf[iByteRecv]='\0';
			printf("Get Msg From %s : %s",inet_ntoa(tSockClienAddr.sin_addr),cRecvBuf);
		}


	}
	close(iSocketSerFd);
	return 0;
}




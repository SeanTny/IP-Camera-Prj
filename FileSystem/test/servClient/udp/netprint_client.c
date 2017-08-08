/*
 * netprint_client.c
 *
 *  Created on: 2017��5��29��
 *      Author: tusen
 */

/*
 * client.c
 *
 *  Created on: 2017��5��28��
 *      Author: tusen
 */


/*
 * Client.c
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


#define SERVER_PORT 	5000
#define SEND_BUF_SIZE   1000
#define RECV_BUF_SIZE   1000


/*�ͻ��˱��
 * socket
 * connect
 * send/recv  sendto/recvfrom
 *
 * ./netprint_client <server_ip> debuglevel=<0-9>
 * ./netprint_client <server_ip> setclient
 * ./netprint_client <server_ip> stdout=0|1
 * ./netprint_client <server_ip> netprint=0|1
 * ./netprint_client <server_ip> show 			// setclient,���ղ��Ҵ�ӡ��Ϣ
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
	memset(&tServerAddr,0,sizeof(tServerAddr)); //���㣬��������Ϊ�����
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
		/* �������� */
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

		/* ѭ���������ȡ���ݴ�ӡ���� */
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
		/* �������� */
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







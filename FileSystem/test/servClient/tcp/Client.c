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
	memset(&tServerAddr,0,sizeof(tServerAddr)); //���㣬��������Ϊ�����
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
    	if(fgets(ucSendBuffer,SEND_BUF_SIZE-1 , stdin))//��֤���һ����\0
    	{
    		iSendLenEd=send(iSocketClentFd, ucSendBuffer, strlen(ucSendBuffer), 0); //�������ַ������ȣ�����sizeof����
    		if(iSendLenEd<=0)
    		{
    			perror("send error");
    			close(iSocketClentFd);
    			return -1;
    		}

//    		iRecvLenEd=recv(iSocketClentFd, ucRecvbuffer,RECV_BUF_SIZE-1,(int)0);//������һ������������
//    		printf("%s",ucRecvbuffer);
    	}

    }

	return 0;
}



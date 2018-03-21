#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "../Mesg.h"
#include "serverconf.h"

void* recvfromclient(void* arg);
int recvstudentinfo(int comfd);

//获得一个新的socket编号
int getnewsocket(){
	int sockfd;
	// 创建一个socket：
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() failed! code:%d\n", errno);
		return -1;
	}
	return sockfd;
}

//绑定端口到sockfd
int bindsocket(int sockfd, struct sockaddr_in* serverAddr){
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(SERVER_PORT);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serverAddr->sin_zero), 8);
	if(bind(sockfd, (struct sockaddr *)serverAddr, sizeof(*serverAddr)) == -1)
	{
		printf("bind() failed! code:%d\n", errno);
		close(sockfd);
		return -1;
	}
	return 0;
}


int main()
{	
	int sockfd, comfd;
	pthread_t ps[10];
	struct sockaddr_in serverAddr, clientAddr;
	int iClientSize;
	// 创建一个socket：
    sockfd = getnewsocket();
	//binding
	bindsocket(sockfd, &serverAddr);
	// 侦听控制连接请求：
	if(listen(sockfd, 5) == -1)
	{
		printf("listen() failed! code:%d\n", errno);
		close(sockfd);
		return -1;
	}
		
	printf("Waiting for client connecting!\n");
	printf("tips : Ctrl+c to quit!\n");
	int cnt = 0;
	char hello[7] = "hello!";
	while(1){
		//接受客户端连接请求：
		iClientSize = sizeof(struct sockaddr_in);
		if((comfd = accept(sockfd, (struct sockaddr *)&clientAddr,(socklen_t *) &iClientSize)) == -1)
		{
			printf("accept() failed! code:%d\n", errno);
			close(sockfd);
			return -1;
		}
		printf("Accepted client: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		if(send(comfd, (char *)&hello, sizeof(hello), 0) == -1)
		{
			printf("send() failed!\n");
			close(sockfd);
			return -1;
		}
		printf("sent respose!\n");
		cnt++;
		pthread_create(ps + cnt,NULL, recvfromclient, &comfd);
		
	}
	close(sockfd);//关闭套接
	return 0;
}

void* recvfromclient(void* arg){
	int comfd = *(int*)arg;
	int cnt = 0;

	while(1){
		cnt++;
		// if(recvstuinfo(comfd)==-1){
		// 	printf("Recv stu info failed!\n");
		// 	return (void*)-1;
		// }
		recvMesg(comfd);
	}
	close(comfd);
	return (void*)0;
}

int recvstuinfo(int comfd){
	int ret = -1;
	struct student stu;
	void *ptr;
	//接收客户端的数据：		
	int nLeft = sizeof(stu);
	ptr = &stu;
	while(nLeft >0)
	{
		//接收数据：
		ret = recv(comfd, ptr, nLeft, 0);
		if(ret <= 0)
		{
			printf("recv() failed!\n");
			close(comfd);
			return -1;
		}
	
		nLeft -= ret;
		ptr = (char *)ptr + ret;
	}
	printf("Recv student:\tname: %s\tage:%d\n",stu.name, stu.age);
	return 0;
}
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

struct ClientCONN clist[MAXCONN];
int main()
{	
	int sockfd, comfd;
	for(int i = 0; i < MAXCONN ;i++){
		clist[i].id = -1;
	}
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
	char hello[7] = "hello!";
	//main loop
	while(1){
		//接受客户端连接请求：
		iClientSize = sizeof(struct sockaddr_in);
		if((comfd = accept(sockfd, (struct sockaddr *)&clientAddr,(socklen_t *) &iClientSize)) == -1)
		{
			printf("accept() failed! code:%d\n", errno);
			close(sockfd);
			return -1;
		}
		for(int i = 0; i < MAXCONN ;i ++){
			if(clist[i].id!=-1)
				continue;
			if(i==MAXCONN){
				printf("Error!!! Overload!\n");
				break;
			}
			clist[i].id = i;
			clist[i].sockid = comfd;
			clist[i].client = clientAddr;
			printf("Accepted client: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			if(send(comfd, (char *)&hello, sizeof(hello), 0) == -1)
			{
				printf("send() failed!\n");
				close(sockfd);
				return -1;
			}
			printf("sent respose!\n");
			pthread_create(&(clist[i].thread),NULL, recvfromclient, &comfd);
			break;
		}
	}
	close(sockfd);//关闭套接
	return 0;
}

void* recvfromclient(void* arg){
	int comfd = *(int*)arg;
	int cnt = 0;
	int thisid;
	struct Mesg mesg;
	for(int i = 0; i <MAXCONN ;i ++)
		if(clist[i].sockid == comfd){
			thisid = i;
			break;
		}

	while(1){
		cnt++;
		mesg = ser_recvMesg(comfd);
		switch(mesg.t){
			case ask_time:{
				sentcurtime(comfd);
				break;
			}
			case ask_client_list:{
				int num[MAXCONN];
				memset(num,0,sizeof(int)*MAXCONN);
				int sum = 0;
				for(int i=0; i<MAXCONN;i++){
					if(clist[i].id!=-1){
						num[sum++]=i;
					}
				}
				struct Mesg mesg;
				mesg.t = ask_client_list;
				mesg.curtime = time(NULL);
				mesg.buflen = 0;
				sentMesg(comfd,mesg);
				write(comfd,&sum,sizeof(sum));
				for(int i = 0; i < sum; i++){
					write(comfd,&clist[num[i]],sizeof(struct ClientCONN));
				}
				break;
			}
			case ask_name:{
				sentserverinfo(comfd);
				break;
			}
			case send_mesg:{
				char buf[mesg.buflen];
				int destid;
				recvtext(comfd,buf,mesg.buflen);
				destid = recvint(comfd);
				if(clist[destid].id==-1){
					printf("No such client:%d\n",destid);
					sendtext(comfd,"Ohh,He/She is offline",thisid);
				}
				else
					sendtext(clist[destid].sockid,buf,thisid);
				break;
			}
			case logout:{
				struct Mesg mesg;
				mesg.buflen = 0;
				mesg.t = logout;
				mesg.curtime = time(NULL);
				sentMesg(comfd,mesg);
				clist[thisid].id=-1;
				close(comfd);
				return (void*)0;
				break;
			}
			default:{
				printf("unknown request, closing dangerous client\n");
				clist[thisid].id=-1;
				close(comfd);
				return (void*)0;
				break;
			}
   		}
	}
	close(comfd);
	return (void*)0;
}


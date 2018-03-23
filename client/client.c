#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "../server/serverconf.h"
#include "../Mesg.h"

int sentstuinfo(int sockfd);
void* waitrespon(void* arg);
int getnewconnect(){
	int sockfd;
	struct sockaddr_in serverAddr;
	
	// 创建一个socket：
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() failed! code:%d\n", errno);
		return -1;
	}
		
	// 设置服务器的地址信息：
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	bzero(&(serverAddr.sin_zero), 8);
	//客户端发出连接请求：
	printf("connecting!\n");
	if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("connect() failed! code:%d\n", errno);
		close(sockfd);
		return -1;
	}
	printf("Connected!\n");
    return sockfd;
}

int main(int argc, char *argv[])
{	
    int sockfd;
    sockfd = getnewconnect();
    char buf[200];
	
	if(read(sockfd,(char*)&buf,7)==-1){
		printf("get respond failed!\n");
        return -1;
	}
	printf("%s\n",buf);
	int cmd;
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, waitrespon, (void*)&sockfd);
	while(1){
		scanf("%d",&cmd);
		switch(cmd){
			case 1:{
				if(askcurtime(sockfd)==-1)
					printf("ask time failed!\n");
				else printf("sent request!\n");
				break;
			}
			case 2:{
				askserverinfo(sockfd);
				break;
			}
			case 3:{
				int id;
				char text[200];
				memset(text,0,200);
				scanf("%s",text);
				scanf("%d",&id);
				sendtext(sockfd,(const char*)text,id);
				break;
			}
		}
		usleep(100000);
	}
	close(sockfd);//关闭套接字
	return 0;
}

int sentstuinfo(int sockfd){
    struct student stu;
    strcpy(stu.name, "fzq");
	stu.age = atoi("20");
    if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
    {
        printf("send() failed!\n");
        close(sockfd);
        return -1;
    }
    printf("student info has been sent!\n");
    usleep(2000000);
    return 0;
}

void* waitrespon(void* arg){
    int sockfd = *(int*)arg;
	struct Mesg mesg;
    while(1){
		mesg = ser_recvMesg(sockfd);
		switch(mesg.t){
			case reply_time:{
				printf("%s",ctime(&mesg.curtime));
				break;
			}
			case reply_name:{
				recvserverinfo(sockfd);
				break;
			}
			case send_mesg:{
				char buf[mesg.buflen];
				int srcid;
				recvtext(sockfd,buf,mesg.buflen);
				srcid = recvint(sockfd);
				printf("Client No.%d gives a Message: %s\n",srcid,buf);
				break;
			}
		}	
	}
}
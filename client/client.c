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
void printmenu();
int getnewconnect(){
	int sockfd;
	struct sockaddr_in serverAddr;
	char buf[100];
	int portnumber;
	in_addr_t destaddr;
	printf("Please press \'login\' to connect or \'local\' for test or \'exit\' for exit:\n");
	fgets(buf,100,stdin);
	if(strncmp(buf,"local",5)==0){
		portnumber = SERVER_PORT;
        destaddr = inet_addr(SERVER_ADDR);

	}
	else if (strncmp(buf,"login",5)==0){
		printf("Please input server ipv4 addr: \n");
		memset(buf,100,0);
		fgets(buf,100,stdin);
		destaddr = inet_addr(buf);
		printf("Please input server port: \n");
		scanf("%d",&portnumber);
	}
	else if (strncmp(buf,"exit",4)==0){
		printf("Bye bye~\n");
		return -1;
	}
	// 创建一个socket：
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{

		printf("socket() failed! code:%d\n", errno);
		return -1;
	}
		
	// 设置服务器的地址信息：
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portnumber);
	serverAddr.sin_addr.s_addr = destaddr;
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
int inapp(int sockfd);
int main(int argc, char *argv[])
{	
	int sockfd;
	while(1){
		sockfd = getnewconnect();
    	if(sockfd==-1) break;
		inapp(sockfd);
	}
	return 0;
}

int inapp(int sockfd){
	char buf[200];
	if(read(sockfd,(char*)&buf,7)==-1){
		printf("get respond failed!\n");
        return -1;
	}
	printf("%s\n",buf);
	int cmd;
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, waitrespon, (void*)&sockfd);
	printmenu();
	while(1){
		scanf("%d",&cmd);
		setbuf(stdin,NULL);
		switch(cmd){
			case 1:{
				if(askcurtime(sockfd)==-1)
					printf("ask time failed!\n");
				else printf("The current time is:\n");
				break;
			}
			case 2:{
				askserverinfo(sockfd);
				break;
			}
			case 3:{
				struct Mesg mesg;
				mesg.buflen = 0;
				mesg.t = ask_client_list;
				mesg.curtime = time(NULL);
				sentMesg(sockfd,mesg);
				break;
			}
			case 4:{
				int id;
				char text[200];
				memset(text,0,200);
				printf("please input message content:\n");
				fgets(text,sizeof(text)/sizeof(text[0]),stdin);
				text[strlen(text)-1]='\0';
				printf("Which client send to?\n");
				scanf("%d",&id);
				//printf("input:%s\nto:%d\n",text,id);
				sendtext(sockfd,(const char*)text,id);
				break;
			}
			case 5:{
				struct Mesg mesg;
				mesg.buflen = 0;
				mesg.t = logout;
				mesg.curtime = time(NULL);
				sentMesg(sockfd,mesg);
				usleep(100);
				return 0;
				break;
			}
		}
		usleep(100000);
	}
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
		// usleep(100);//avoid crash
		mesg = cli_recvMesg(sockfd);
		switch(mesg.t){
			case reply_time:{
				printf("%s",ctime(&mesg.curtime));
				break;
			}
			case reply_name:{
				recvserverinfo(sockfd,mesg.buflen);
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
			case ask_client_list:{
				int sum = recvint(sockfd);
				struct ClientCONN cnn;
				for(int i = 0; i < sum; i++){
					read(sockfd,&cnn,sizeof(cnn));
					printClientconninfo(cnn);
				}
				break;
			}
			case logout:{
				printf("Byebye\n");
				close(sockfd);//关闭套接字
				return (void*)0;
				break;
			}
			default:{
				printf("unknown reply!\n");
				return (void*)-1;
				break;
			}	
		}	
	}
}

void printmenu(){
	printf("Welcome to our client, operating guides:\n");
	printf(" \
	1.\tget current server time\n \
	2.\tget server information\n \
	3.\tget all connected client from server\n \
	4.\tsend message to specific client\n \
	5.\tlogout\n");
}
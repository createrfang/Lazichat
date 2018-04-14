#include "Mesg.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>


struct Mesg ser_recvMesg(int sockfd){
    struct Mesg mesg;
    if(read(sockfd,(struct Mesg*)&mesg,sizeof(mesg))==-1){
        printf("recieve Mesg head failed\n");
        exit(1);
    }
    printf("recv packet:%d, %d, %s",mesg.t,mesg.buflen,ctime(&mesg.curtime));
    
    //todo
    return mesg;
}

struct Mesg cli_recvMesg(int sockfd){
    struct Mesg mesg;
    if(read(sockfd,(struct Mesg*)&mesg,sizeof(mesg))==-1){
        printf("recieve Mesg head failed\n");
        exit(1);
    }
    // printf("recv packet:%d, %d, %s",mesg.t,mesg.buflen,ctime(&mesg.curtime));
    
    //todo
    return mesg;
}

int sentserverinfo(int sockfd){
    struct utsname buf;
    struct Mesg mesg;
    mesg.t = reply_name;
    mesg.buflen = sizeof(buf);
    mesg.curtime = time(NULL);
    if(uname(&buf)){
        printf("get name failed \n");
    }
    sentMesg(sockfd,mesg);
    if(write(sockfd,&buf,sizeof(buf))==-1){
        return -1;
    }
    return 0;
}

int askserverinfo(int sockfd){
    struct Mesg mesg;
    mesg.t = ask_name;
    mesg.curtime = time(NULL);
    mesg.buflen = 0;
    if(sentMesg(sockfd,mesg)==-1)
        printf("send time failed\n");
    return 0;
}

int recvserverinfo(int sockfd, int buflen){
    struct utsname buf;
    printf("size:%d\n",sizeof(buf));
    read(sockfd,(struct utsname*)&buf,buflen);
    printserverinfo(buf);
    return 0;
}

void printserverinfo(struct utsname buf){
    printf("sysname:%s\n",buf.sysname);
    printf("nodename:%s\n",buf.nodename);
    printf("release:%s\n",buf.release);
    printf("version:%s\n",buf.version);
    printf("machine:%s\n",buf.machine);
}

void printClientconninfo(struct ClientCONN cnn){
    printf("Client No.%d:\t addr:%s:%d\n",cnn.id,inet_ntoa(cnn.client.sin_addr), ntohs(cnn.client.sin_port));
}

int sentMesg(int sockfd, struct Mesg mesg){
    // printf("send packet:%d, %d, %s",mesg.t,mesg.buflen,ctime(&mesg.curtime));
    if(write(sockfd,&mesg,sizeof(mesg))==-1){
        return -1;
    }
    //todo
    return 0;
}

int askcurtime(int sockfd){
    struct Mesg mesg;
    mesg.t = ask_time;
    mesg.curtime = time(NULL);
    mesg.buflen = 0;
    if(sentMesg(sockfd,mesg)==-1)
        printf("send time failed\n");
    return 0;
}

int sentcurtime(int sockfd){
    struct Mesg mesg;
    mesg.t = reply_time;
    mesg.curtime = time(NULL);
    mesg.buflen = 0;
    if(sentMesg(sockfd,mesg)==-1)
        printf("send time failed\n");
    return 0;
}

int sendtext(int sockfd, const char* text, int id){
    struct Mesg mesg;
    mesg.t = send_mesg;
    mesg.curtime = time(NULL);
    mesg.buflen = strlen(text)+1;
    if(sentMesg(sockfd,mesg)==-1){
        printf("send mesg failed\n");
        return -1;
    }
    if(write(sockfd,text,strlen(text)+1)==-1){
        return -1;
    }
    if(write(sockfd,&id,sizeof(id))==-1){
        return -1;
    }
    // printf("sent message to %d\n",id);
    return 0;
}

int recvtext(int sockfd, char* buf, size_t len){
    if(read(sockfd,(char*)buf,len)==-1)
        return -1;
    return 0;
}

int recvint(int sockfd){
    int buf;
    if(read(sockfd,(int*)&buf,sizeof(buf))==-1){
        printf("recv integer failed\n");
        return -1;
    }
    // printf("recv integer:%d\n",buf);
    return buf;
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
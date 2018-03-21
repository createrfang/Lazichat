#include "Mesg.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

struct Mesg recvMesg(int sockfd){
    struct Mesg mesg;
    if(read(sockfd,(struct Mesg*)&mesg,sizeof(mesg))==-1){
        printf("recieve Mesg head failed\n");
        exit(1);
    }
    printf("recv packet:%d, %d, %s",mesg.t,mesg.buflen,ctime(&mesg.curtime));
    switch(mesg.t){
        case reply_time:{
            printf("%s",ctime(&mesg.curtime));
            break;
        }
        case ask_time:{
            sentcurtime(sockfd);
        }
        case ask_client_list:{
            break;
        }
        case ask_name:{
            sentserverinfo(sockfd);
            break;
        }
        case reply_name:{
            recvserverinfo(sockfd);
            break;
        }
        case send_mesg:{
            break;
        }
        case logout:{
            break;
        }
        case NmesgType:{
            break;
        }
    }
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

int recvserverinfo(int sockfd){
    struct utsname buf;
    read(sockfd,(struct utsname*)&buf,sizeof(buf));
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

int sentMesg(int sockfd, struct Mesg mesg){
    printf("send packet:%d, %d, %s",mesg.t,mesg.buflen,ctime(&mesg.curtime));
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

int senttext(const char* text){
    return 0;
}

int recvtext(){
    return 0;
}
#include<time.h>
#include<sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
enum mesg_type 
{
    reply_time=2,       //respon to time request
    trans_text,         //transfer text to dest ip
    reply_name,         //response to name request
    ask_name,           //name request
    ask_time,           //ask for server time
    ask_client_list,    //ask for client list
    send_mesg,          //send message to another client
    logout,             //logout from server
    NmesgType};

extern int errno;

//客户端向服务器传送的结构：
struct student
{
	char name[32];
	int age;
};

struct Mesg
{
    enum mesg_type t;
    int buflen;
    time_t curtime;
};

struct ClientCONN
{
    int id;
    int sockid;
    struct sockaddr_in client;
    pthread_t thread;
};
struct Mesg ser_recvMesg(int sockfd);
struct Mesg cli_recvMesg(int sockfd);
int sentMesg(int sockfd, struct Mesg mesg);
int askcurtime(int sockfd);
int sentcurtime(int sockfd);
struct Mesg recvMesg(int sockfd);
struct Mesg ser_recvMesg(int sockfd);
void printserverinfo(struct utsname buf);
void printClientconninfo(struct ClientCONN cnn);
int sentserverinfo(int sockfd);
int recvserverinfo(int sockfd, int buflen);
int askserverinfo(int sockfd);
int recvstudentinfo(int comfd);
int sendtext(int sockfd, const char* text, int id);
int recvtext(int sockfd, char* buf, size_t len);
int recvint(int sockfd);
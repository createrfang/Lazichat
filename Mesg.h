#include<time.h>
#include<sys/utsname.h>
enum mesg_type 
{
    reply_time=2,       //respon to time request
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

int sentMesg(int sockfd, struct Mesg mesg);
int askcurtime(int sockfd);
int sentcurtime(int sockfd);
struct Mesg recvMesg(int sockfd);
void printserverinfo(struct utsname buf);
int sentserverinfo(int sockfd);
int askserverinfo(int sockfd);
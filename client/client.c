/* 
*client.c 
*overred
*notice:file's tail(return 0)will cause client to exit
*the server will be getting a SIGPIPE signal while write buffer has unsent stuff
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "common.h"

#define socknum 50
#define loopnum 100000 
int sockfds[socknum];

void connect_flood()
{
    //char *host="192.168.93.129";
    //char *host="172.17.122.80";
    char *host="127.0.0.1";
    int port=8888;
    struct sockaddr_in servaddr;
    int sockfd;
    
    for(int i=0;i<socknum;i++)
    {
        sockfds[i] = Socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(host);
        servaddr.sin_port = htons(port);

        Connect(sockfds[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
    }
}

void request_flood()
{
    char msg[10] = {0};
    char msg2[10] = {0};
    *(short*)msg = 10;
    msg[2] = 1;
    msg[3] = 3;
    msg[4] = 'e';
    msg[5] = 'e';
    msg[6] = 'e';
    msg[7] = 'e';
    msg[8] = 'e';
    msg[9] = 0;


    int nread = 0;
    for(int i=0;i<loopnum;i++)
    {
	for(int j=0; j<socknum; j++)
        {
	    msg[2] = i%128;
	    msg[3] = j%128;
    	    if((nread = write(sockfds[j],msg,sizeof(msg)))==-1)
	    {
		printf("\n** write_1**\n");
	    }
	    printf("---: %d %d %s\n", msg[2], msg[3], msg+4);
       	    int n = read(sockfds[j], msg2, sizeof(msg2));
	    printf("+++: %d %d %s\n", msg2[2], msg2[3], msg2+4);
	    usleep(10);
	    if(nread != n)
            {
		printf("xxxxxxxxx \n");
		return;
	    }
	}
	usleep(10);
    }
}

int main(int argc, char *argv[])
{
    connect_flood();
    request_flood();

    return 0;  
}


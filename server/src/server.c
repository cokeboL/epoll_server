#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "commen.h"
#include "reader.h"
#include "writer.h"
#include "handler.h"
#include "server.h"



void start_server(const char *ip, unsigned short port)
{
    signal(SIGPIPE,SIG_IGN);

    start_readers();
    start_writers();
    start_handler();

    // create a TCP socket, bind and listen
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);

    bind(server_sock, (struct sockaddr *) &addr, sizeof(addr));

    listen(server_sock, 10);

    fcntl(server_sock, F_SETFL, O_NONBLOCK);

    int nfds;

    struct epoll_event event;
    struct epoll_event events[1024];
    int epoll_fd = epoll_create(65535);

    int curr_reader = 0;


    memset(&event, 0, sizeof(event));
    event.data.fd = server_sock;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event);

    int sockfd;
    Sock *psock;
    while(1)
    {
        nfds = epoll_wait(epoll_fd, events, 20, sizeof(events));  
        for(int i = 0; i < nfds; i++)  
        {
            if(events[i].data.fd == server_sock) //有新的连接  
            {
                sockfd= accept(server_sock, 0, 0); //accept这个连接  
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
                event.data.fd = sockfd;
                event.events = EPOLLIN | EPOLLET;  
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event); //将新的fd添加到epoll的监听队列中

                g_socks[sockfd] = create_sock(sockfd);
            }
            else if(events[i].events & EPOLLIN ) //接收到数据，读socket  
            {
                curr_reader++;
                curr_reader = curr_reader % READER_NUM;
                write(g_readers[curr_reader]->fds[1], (char*)&g_socks[events[i].data.fd], sizeof(Sock*));
            }
            /*
            else if(events[i].events & EPOLLOUT) //有数据待发送，写socket  
            {
            }
            else  
            {  
            }
            */
        }
    }


    return 0;
}

void stop_server()
{
    stop_readers();
    stop_writers();
}
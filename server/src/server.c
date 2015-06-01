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

#include "handler.h"
#include "timer.h"
#include "server.h"

Server *g_servers[SERVER_NUM] = {0};

static int server_sock = 0;

static Sock *sock_fd_map[MAX_FD_NUM] = {0};

static void *server_thread(void *arg)
{
    Server *server = (Server*)arg;

    int nfds;

    struct epoll_event event;
    struct epoll_event events[1024];
    int epoll_fd = epoll_create(MAX_CLIENT_NUM);

    int curr_reader = 0;


    memset(&event, 0, sizeof(event));
    event.data.fd = server->fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->fd, &event);

    int sockfd;
    Sock *psock;

    while(server->running)
    {
        nfds = epoll_wait(epoll_fd, events, 20, sizeof(events));  
        for(int i = 0; i < nfds; i++)  
        {
            if(events[i].data.fd == server_sock) //有新的连接  
            {
                sockfd= accept(server_sock, 0, 0); //accept这个连接  
                if((SAFE_LIST_SIZE(g_sock_list) < MAX_CLIENT_NUM) && (sockfd < MAX_FD_NUM))
                {
                    fcntl(sockfd, F_SETFL, O_NONBLOCK);
                    event.data.fd = sockfd;
                    event.events = EPOLLIN | EPOLLET;  
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event); //将新的fd添加到epoll的监听队列中

                    Sock *sock = create_sock(sockfd, epoll_fd);
                    
                    sock_fd_map[sockfd] = sock;

                    SAFE_LIST_PUSH(g_sock_list, sock, &sock->list_node);
                    printf("create_sock: %d\n", SAFE_LIST_SIZE(g_sock_list));
                }
                else
                {
                    close(sockfd);
                }
            }
            else if(events[i].events & EPOLLIN ) //接收到数据，读socket  
            {
                write(g_handlers[events[i].data.fd % HANDLER_NUM]->fds[1], (char*)&sock_fd_map[events[i].data.fd], sizeof(Sock*));
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
}


static Server *create_server(int listen_fd)
{
    Server *server = (Server*)Malloc(sizeof(Server));
    server->fd = listen_fd;
    server->running = true;

    int err = pthread_create(&server->tid, NULL, &server_thread, (void*)server); //创建线程  

    return server;
}

static void remove_server(Server *server)
{
    server->running = 0;    
}

static void free_server(Server *server)
{
    close(server->fd);
    Free(server);   
}

void start_server(const char *ip, unsigned short port)
{
    signal(SIGPIPE, SIG_IGN);

    init_sock_list();
    start_handlers();
    start_timer();

    int err = 0;

    // create a TCP socket, bind and listen
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(server_sock, F_SETFL, O_NONBLOCK);

    struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    //addr.sin_addr.s_addr = htons(ip);
    addr.sin_port = htons(port);

    err = bind(server_sock, (struct sockaddr *) &addr, sizeof(addr));
    if(err)
    {
	   printf("bind error: %m\n"); 
    }

    listen(server_sock, 10);

    Server *g_servers[SERVER_NUM] = {0};

    for(int i=0; i < SERVER_NUM; i++)
    {
        g_servers[i] = create_server(server_sock);
    }

    return 0;
}

void stop_server()
{
    stop_timer();
    stop_handlers();
    destroy_sock_list();

    close(server_sock);

    for(int i=0; i<SERVER_NUM; i++)
    {
        remove_server(g_servers[i]);
    }
}

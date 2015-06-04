#ifndef _server_h_
#define _server_h_

#include "commen.h"
#include "sock.h"


typedef struct Server
{
	int fd;
	pthread_t tid;
	bool running;
	lua_State *state;
}Server;


void start_server(const char *ip, unsigned short port);

void stop_server();

extern Server *g_server;

extern Sock *g_sock_fd_map[MAX_FD_NUM];

#endif // _server_h_


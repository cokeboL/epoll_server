#ifndef _server_h_
#define _server_h_

#include "commen.h"

typedef struct Server
{
	int fd;
	pthread_t tid;
	bool running;
}Server;

void start_server(const char *ip, unsigned short port);

void stop_server();

extern Server *g_servers[SERVER_NUM];

#endif // _server_h_


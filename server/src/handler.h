#ifndef _handler_h_
#define _handler_h_

#include "commen.h"
#include "sock.h"

typedef struct Handler
{
	int fds[2];
	pthread_t tid; 
	int running;
}Handler;

void start_handler();

void stop_handler();

extern Handler *g_handler;

#endif // _handler_h_

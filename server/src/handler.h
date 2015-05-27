#ifndef _handler_h_
#define _handler_h_

#include "commen.h"
#include "sock.h"

typedef struct Handler
{
	int fds[2];
	pthread_t tid; 
	bool running;
}Handler;

void start_handler();

void stop_handler();

extern Handler *g_global_handlers[G_HANDLER_NUM];
extern Handler *g_normal_handlers[N_HANDLER_NUM];

#endif // _handler_h_

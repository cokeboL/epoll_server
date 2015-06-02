#ifndef _reader_h_
#define _reader_h_

#include "commen.h"
#include "sock.h"

typedef struct Handler
{
	int fds[2];
	pthread_t tid;
	char buf[MAX_MSG_LEN];
	bool running;
}Handler;

void start_handlers();

void stop_handlers();

extern Handler *g_handlers[HANDLER_NUM];

#endif // _reader_h_

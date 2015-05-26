#ifndef _writer_h_
#define _writer_h_

#include "commen.h"
#include "sock.h"

typedef struct Writer
{
	int fds[2];
	pthread_t tid;
	int running;
}Writer;

void start_writers();

void stop_writers();

extern Writer *g_writers[WRITER_NUM];

#endif // _writer_h_

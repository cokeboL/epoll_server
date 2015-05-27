#ifndef _reader_h_
#define _reader_h_

#include "commen.h"
#include "sock.h"

typedef struct Reader
{
	int fds[2];
	pthread_t tid;
	bool running;
}Reader;

void start_readers();

void stop_readers();

extern Reader *g_readers[READER_NUM];

#endif // _reader_h_

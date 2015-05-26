#ifndef _reader_h_
#define _reader_h_

#include "commen.h"
#include "sock.h"

typedef struct Reader
{
	int fds[2];
	pthread_t tid;
	int running;
	char buf[MAX_MSG_LEN];
}Reader;

void start_readers();

void stop_readers();

extern Reader *g_readers[READER_NUM];

#endif // _reader_h_

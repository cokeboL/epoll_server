#ifndef _timer_h_
#define _timer_h_

#include "commen.h"

typedef struct Timer
{
	pthread_t tid;
	bool running;
}Timer;

void start_timer();

void stop_timer();

#endif // _timer_h_

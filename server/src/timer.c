#include <time.h>
#include "sock.h"
#include "timer.h"

static Timer timer;
static time_t curr_time;

static inline bool check_sock(void *data)
{
	Sock *sock = (Sock*)data;
	if((curr_time - sock->last_pack_time) >= SOCK_TIME_OUT)
	{
		remove_sock(sock, true);
		return true;
	}

	return false;
}

static void *timer_thread(void *arg)
{
	pthread_detach(pthread_self());

	Sock *sock;
	int sum = 0, n = 0, p_sock;
	while(timer.running)
	{
		curr_time = time(0);
		printf("--- timer check begin 111\n");
		SAFE_LIST_REMOVE_IF(g_sock_list, check_sock);
		printf("--- timer check begin 222\n");

		usleep(HEART_BEAT_TIME);
	}
}

void start_timer()
{
	timer.running = true;
    int err = pthread_create(&(timer.tid), NULL, &timer_thread, 0); 
}

void stop_timer()
{
	timer.running = false;
}
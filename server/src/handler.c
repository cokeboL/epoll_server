#include "handler.h"
#include "writer.h"

Handler *g_handler = 0;

static void free_handler(Handler *handler);

static void handle_msg(SockMsg *msg)
{
	static int curr_writer = 0;
	curr_writer++;
	curr_writer = curr_writer % WRITER_NUM;
	write(g_writers[curr_writer]->fds[1], (char*)&msg, sizeof(SockMsg*));
}

static void *handler_thread(void *arg)
{
	Handler *handler = (Handler*)arg;
	pthread_detach(pthread_self());

	SockMsg *msg;
	int sum = 0, n = 0, p_sock_msg;
	while(handler->running)
	{
		while(1)
		{
			n = read(handler->fds[0], (char*)&msg+sum, sizeof(msg)-sum);
			if(n < 0)
			{

			}
			else
			{
				sum += n;
			}
			if(sum == sizeof(msg))
			{
				break;
			}
		}
		
		handle_msg(msg);
		sum = 0;
	}

	free_handler(handler);
}

static Handler *create_handler()
{
	Handler *handler = (Handler*)Malloc(sizeof(Handler));
	
	pipe(handler->fds);
	
	handler->running = 1;
    int err = pthread_create(&handler->tid, NULL, &handler_thread, (void*)handler); //创建线程  

    return handler;
}

static void remove_handler(Handler *handler)
{
	handler->running = 0;	
}

static void free_handler(Handler *handler)
{
	close(handler->fds[0]);
	close(handler->fds[1]);
	Free(handler);	
}

void start_handler()
{
	g_handler = create_handler();
}

void stop_handler()
{
	remove_handler(g_handler);
}

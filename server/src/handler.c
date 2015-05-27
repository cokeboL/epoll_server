#include "handler.h"
#include "writer.h"

Handler *g_global_handlers[G_HANDLER_NUM] = {0};
Handler *g_normal_handlers[N_HANDLER_NUM] = {0};
//#define G_HANDLER_NUM 1
//#define N_HANDLER_NUM 1

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
	
	handler->running = true;
    int err = pthread_create(&handler->tid, NULL, &handler_thread, (void*)handler); //创建线程  

    return handler;
}

static void remove_handler(Handler *handler)
{
	handler->running = false;	
}

static void free_handler(Handler *handler)
{
	close(handler->fds[0]);
	close(handler->fds[1]);
	Free(handler);	
}

void start_handler()
{
	for(int i = 0; i < G_HANDLER_NUM; i++)
	{
		g_global_handlers[i] = create_handler();
	}
	for(int i = 0; i < N_HANDLER_NUM; i++)
	{
		g_normal_handlers[i] = create_handler();
	}
}

void stop_handler()
{
	for(int i = 0; i < G_HANDLER_NUM; i++)
	{
		remove_handler(g_global_handlers[i]);
	}
	for(int i = 0; i < N_HANDLER_NUM; i++)
	{
		remove_handler(g_normal_handlers[i]);
	}
}

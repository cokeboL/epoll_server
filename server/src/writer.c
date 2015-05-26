#include "writer.h"

static void free_writer(Writer *writer);

Writer *g_writers[WRITER_NUM] = {0};

static void *writer_thread(void *arg)
{
	Writer *writer = (Writer*)arg;
	pthread_detach(pthread_self());

	SockMsg *sock_msg;
	int sum = 0, n = 0;
	while(writer->running)
	{
		while(1)
		{
			n = read(writer->fds[0], (char*)&sock_msg+sum, sizeof(sock_msg)-sum);
			if(n < 0)
			{

			}
			else
			{
				sum += n;
			}

			if(sum == sizeof(sock_msg))
			{
				break;
			}
		}
		int wlen = write(sock_msg->sock->fd, sock_msg->data, sock_msg->len);
		msg_release(sock_msg);
		sum = 0;
	}

	free_writer(writer);
}

static Writer *create_writer()
{
	Writer *writer = (Writer*)Malloc(sizeof(Writer));
	
	pipe(writer->fds);
	
	writer->running = 1;
    int err = pthread_create(&writer->tid, NULL, &writer_thread, (void*)writer); //创建线程  

    return writer;
}

static void remove_writer(Writer *writer)
{
	writer->running = 0;	
}

static void free_writer(Writer *writer)
{
	close(writer->fds[0]);
	close(writer->fds[1]);
	Free(writer);	
}

void start_writers()
{
    for(int i=0; i<WRITER_NUM; i++)
    {
        g_writers[i] = create_writer();
    }
}

void stop_writers()
{
    for(int i=0; i<WRITER_NUM; i++)
    {
        remove_writer(g_writers[i]);
    }
}

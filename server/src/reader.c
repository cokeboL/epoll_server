#include "reader.h"
#include "handler.h"

static void free_reader(Reader *reader);

Reader *g_readers[READER_NUM] = {0};

static void read_data(Sock *sock)
{
	int nread = 0;
	while(1)
	{
	if(!sock->msg)
	{
		nread = read(sock->fd, sock->head+sock->len_readed, PACK_HEAD_LEN - sock->len_readed);
		if(nread <= 0)
		if((nread < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
		{
			return;
		}
		else if(nread == 0)
		{
			goto Error;
		}

		sock->len_readed += nread;
		if(sock->len_readed >= PACK_HEAD_LEN)
		{
			sock->msg = create_recv_msg(sock);
			sock->len_total = sock->msg->len-PACK_HEAD_LEN;
			sock->len_readed = 0;
		}
		else
		{
			return;
		}
	}
	nread = read(sock->fd, sock->msg->buf + sock->len_readed, sock->len_total - sock->len_readed);
		if((nread < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
		{
			return;
		}
		else if(nread == 0)
		{
			goto Error;
		}

	sock->len_readed += nread;

	if(sock->len_readed == sock->len_total)
	{
		write(g_handler->fds[1], (char*)&sock->msg, sizeof(SockMsg*));

		sock->msg = 0;
		sock->len_readed = 0;
	}
	}
	return;
	//sock->setLastPackTime(time(0));
	
Error:
	remove_sock(sock);
}

static void *reader_thread(void *arg)
{
	Reader *reader = (Reader*)arg;
	pthread_detach(pthread_self());

	Sock *sock;
	int sum = 0, n = 0, p_sock;
	while(reader->running)
	{
		while(1)
		{
			n = read(reader->fds[0], (char*)&sock+sum, sizeof(sock)-sum);
			if(n < 0)
			{

			}
			else
			{
				sum += n;
			}
			if(sum == sizeof(sock))
			{
				break;
			}
		}
		read_data(sock);

		sum = 0;
	}

	free_reader(reader);
}

static Reader *create_reader()
{
	Reader *reader = (Reader*)Malloc(sizeof(Reader));
	
	pipe(reader->fds);
	
	reader->running = 1;
    int err = pthread_create(&reader->tid, NULL, &reader_thread, (void*)reader); //创建线程  

    return reader;
}

static void remove_reader(Reader *reader)
{
	reader->running = 0;	
}

static void free_reader(Reader *reader)
{
	close(reader->fds[0]);
	close(reader->fds[1]);
	Free(reader);	
}

void start_readers()
{
    for(int i=0; i<READER_NUM; i++)
    {
        g_readers[i] = create_reader();
    }
}

void stop_readers()
{
    for(int i=0; i<READER_NUM; i++)
    {
        remove_reader(g_readers[i]);
    }
}
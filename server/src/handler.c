#include "handler.h"

static void free_handler(Handler*);

Handler *g_handlers[HANDLER_NUM] = {0};

inline bool wirte_msg(SockMsg *msg)
{
	msg->len
	int nwrite = 0;
	while(1)
	{
		nwrite = write(msg->sock->fd, msg->data, msg->len);
		if(nwrite > 0)
		{
			msg->len -= nwrite;
			if(msg->len == 0)
			{
				break;
			}
			else if(msg->len > 0)
			{
				usleep(SEND_SLEEP_US);
			}
			else
			{
				goto Error;
			}
		}
		else if((nwrite < 0) && (errno == EAGAIN) || (errno == EINTR) || (errno == EWOULDBLOCK))
		{
			
		}
		else if(nwrite == 0)
		{
			goto Error;
		}

		usleep(SEND_SLEEP_US);
	}

	msg_release(msg);
	return true;

Error:
	return false;
}

static inline bool handle_msg(SockMsg *msg)
{
	return wirte_msg(msg);
}

static inline void read_data(Sock *sock)
{
	int pack_count = 0;
	int curr_normal_handler_idx = 0;
	int curr_global_handler_idx = 0;

	int nread = 0;
	while(1)
	{
		if(!sock->msg)
		{
			nread = read(sock->fd, sock->head+sock->len_readed, PACK_HEAD_LEN - sock->len_readed);
			if((nread < 0) && (errno == EAGAIN) || (errno == EINTR) || (errno == EWOULDBLOCK))
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
		if((nread < 0) && (errno == EAGAIN) || (errno == EINTR) || (errno == EWOULDBLOCK))
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
			if(handle_msg(sock->msg))
			{
				sock->msg = 0;
				sock->len_readed = 0;
			}
			else
			{
				goto Error;
			}
		}
	}
	return;
	//sock->setLastPackTime(time(0));
	
Error:
	remove_sock(sock);
}

static void *handler_thread(void *arg)
{
	Handler *handler = (Handler*)arg;
	pthread_detach(pthread_self());

	Sock *sock;
	int sum = 0, n = 0, p_sock;
	while(handler->running)
	{
		while(1)
		{
			n = read(handler->fds[0], (char*)&sock+sum, sizeof(sock)-sum);
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

void start_handlers()
{
    for(int i = 0; i < HANDLER_NUM; i++)
    {
        g_handlers[i] = create_handler();
    }
}

void stop_handlers()
{
    for(int i = 0; i < HANDLER_NUM; i++)
    {
        remove_handler(g_handlers[i]);
    }
}

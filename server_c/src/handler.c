#include "handler.h"

static void free_handler(Handler*);

Handler *g_handlers[HANDLER_NUM] = {0};


bool write_msg(SockMsg *msg)
{
	//return write(msg->sock->fd, msg->data, msg->len) == msg->len;
	msg_retain(msg); // 加正式的handler之后应把此句删掉

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
			/*
			else ifnwrite > msg->len)
			{
				goto Error;
			}
			*/
		}
		else if((nwrite < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
		{

		}
		else
		{
			goto Error;
		}

		usleep(SEND_SLEEP_US);
	}

	msg_release(msg);
	return true;
	
Error:
	msg_release(msg);
	return false;
}

static bool tmp_handler(SockMsg *msg)
{
	msg_retain(msg);
}

static bool handle_msg(SockMsg *msg)
{
	bool ret = false;
	switch(MSG_CMD(msg))
	{
		#if 0
		case 1:
			//ret = handler1();
			break;
		case 2:
			//ret = handler2();
			break;
		#endif
		default:
			ret = write_msg(msg);
			break;
	}

	msg_release(msg);

	return ret;
}

//读数据
static void read_data(Sock *sock)
{
	//int pack_count = 0;
	//int curr_normal_handler_idx = 0;
	//int curr_global_handler_idx = 0;

	//设置心跳 -> 最后响应时间
	heart_beat_sock(sock);

	int nread = 0;
	while(1)
	{
		//如果sock->msg为空，则说明包头还没读取到
		if(!sock->msg)
		{
			//读数据头，len_readed在sock创建时、读到包头、读完一个msg后都会置0
			nread = read(sock->fd, sock->head+sock->len_readed, PACK_HEAD_LEN - sock->len_readed);
			//非阻塞，无数据，说明本次数据已经读完，返回下次再读
			if((nread < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
			{
				return;
			}
			//对方关闭或其他意外，关闭清理sock
			else if(nread <= 0)
			{
				goto Error;
			}

			//读到的长度累加
			sock->len_readed += nread;
			//读到包头长度根据包头的消息长度信息创建msg（动态分配内存）
			if(sock->len_readed == PACK_HEAD_LEN)
			{
				sock->msg = create_recv_msg(sock);
				//记录剩余消息总长度
				sock->len_total = sock->msg->len-PACK_HEAD_LEN;
				//已读长度置0
				sock->len_readed = 0;
			}
			//读到的长度不够包头，则返回，下次再读
			else
			{
				return;
			}
		}

		//读取包内容
		nread = read(sock->fd, sock->msg->buf + sock->len_readed, sock->len_total - sock->len_readed);
		//非阻塞，无数据，说明本次数据已经读完，返回下次再读
		if((nread < 0) && (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK))
		{
			return;
		}
		//对方关闭或其他意外，关闭清理sock
		else if(nread <= 0)
		{
			goto Error;
		}

		//读到的长度累加
		sock->len_readed += nread;

		//读到完整包内容，则处理msg
		if(sock->len_readed == sock->len_total)
		{
			bool ret = handle_msg(sock->msg);
			sock->msg = 0;
			sock->len_readed = 0;
			//处理过程中出错，关闭清理sock
			if(!ret)
			{
				goto Error;
			}
		}
	}
	//return;
	
Error:
	remove_sock(sock, false);
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

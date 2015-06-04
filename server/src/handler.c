#include "handler.h"
#include "clua.h"

bool connect_to(const char *ip, unsigned short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    

    struct sockaddr_in addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("connect failed!\n");
		close(sockfd);
		return false;
	}

	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	return true;
}

bool write_msg(SockMsg *msg)
{
	//return write(msg->sock->fd, msg->data, msg->len) == msg->len;
	//msg_retain(msg); // 加正式的handler之后应把此句删掉
	printf("==== 888\n");
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
	printf("==== 999\n");
	msg_release(msg);
	return true;
	
Error:
	msg_release(msg);
	return false;
}

static inline bool handle_msg(lua_State *state, SockMsg *msg)
{
#if 0
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
#endif
	bool ret = lua_msg_handler(state, msg);

	msg_release(msg);

	return ret;
}

//读数据
void read_data(lua_State *state, Sock *sock)
{
	//int pack_count = 0;
	//int curr_normal_handler_idx = 0;
	//int curr_global_handler_idx = 0;

	//设置心跳 -> 最后响应时间
	heart_beat_sock(sock);
	printf("==== 333 sock->fd: %d efd: %d\n", sock->fd, sock->epoll_fd);
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
			printf("==== 444\n");
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
		printf("==== 555\n");
		//读到完整包内容，则处理msg
		if(sock->len_readed == sock->len_total)
		{
			printf("==== 666\n");
			bool ret = handle_msg(state, sock->msg);
			sock->msg = 0;
			sock->len_readed = 0;
			//处理过程中出错，关闭清理sock
			if(!ret)
			{
				goto Error;
			}
			break;
		}
	}
	return;
	
Error:
	printf("==== 777 1\n");
	remove_sock(sock, false);
	printf("==== 777 2\n");
}

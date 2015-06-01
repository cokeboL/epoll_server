#include <sys/epoll.h>
#include "sock.h"
#include "server.h"


int g_max_sock_fd = 0;
Sock *g_socks[MAX_CLIENTS_NUM] = {0};

SafeList *g_sock_list = 0;

void socks_online_foreach(SafeListNodeHandler handler)
{
	/*
    for(int i = 0; i < g_max_sock_fd; i++)
    {
        if(g_socks[i])
        {
            handler(g_socks[i]);
        }
    }
    */
    SAFE_LIST_TRAVERSE(g_sock_list, handler);
}

int sock_num = 0;
inline Sock *create_sock(int fd)
{
	Sock *sock = (Sock*)Malloc(sizeof(Sock));
	memset(sock, 0, sizeof(Sock));
	sock->fd = fd;
	sock->legal = true;

	g_max_sock_fd = (g_max_sock_fd < fd) ? fd : g_max_sock_fd;

	sock_num++;
	printf("create_sock: %d\n", sock_num);

	return sock;
}

static inline void free_sock(void *data)
{
	Sock *sock = (Sock*)data;
	//close(sock->fd);
	//Free(sock);
}

inline void remove_sock(Sock *sock)
{
	if(sock->msg)
	{
		Free(sock->msg);
	}
	epoll_ctl(sock->epoll_fd, EPOLL_CTL_DEL, sock->fd, 0);

	if(g_max_sock_fd == sock->fd)
	{
		for(int i = g_max_sock_fd; i >= 0; i--)
		{
			if(g_socks[i])
			{
				g_max_sock_fd = i;
				break;
			}
		}
	}
	
	SAFE_LIST_REMOVE(g_sock_list, sock->list_node);

	close(sock->fd);
	g_socks[sock->fd] = 0;
	Free(sock);

	sock_num--;
	printf("remove_sock: %d - %d\n", sock_num, SAFE_LIST_SIZE(g_sock_list));
}

inline SockMsg *create_recv_msg(Sock *sock)
{
	int len = *(uint16_t*)&sock->head;
	int len_total = sizeof(SockMsg) + len;
	SockMsg *msg = (SockMsg*)Malloc(len_total);
	memset(msg, 0, len_total);
	msg->sock = sock;
	*(int*)msg->data = *(int*)sock->head;
	msg->data[2] = sock->head[2];
	msg->data[3] = sock->head[3];
	msg->len = len;// - PACK_HEAD_LEN;
	msg->buf = msg->data + PACK_HEAD_LEN;
	msg->count++;
	

	return msg;
}

inline SockMsg *create_send_msg(Sock *sock, uint16_t len, int8_t cmd, int8_t action, char *msg)
{
	int len_total = sizeof(SockMsg) + PACK_HEAD_LEN + len;
	SockMsg *sock_msg = (SockMsg*)Malloc(len_total);
	memset(sock_msg, 0, len_total);

	sock_msg->sock = sock;
	//sock_msg->buf = sock_msg->data;
	*(uint16_t*)sock_msg->data = PACK_HEAD_LEN + len;
	sock_msg->data[2] = cmd;
	sock_msg->data[3] = action;
	sock_msg->len = PACK_HEAD_LEN + len;
	memcpy(sock_msg->data+PACK_HEAD_LEN, msg, len);

	sock_msg->count++;

	return sock_msg;
}

inline void msg_retain(SockMsg *msg)
{
	msg->count++;
}

inline void msg_release(SockMsg *msg)
{
	msg->count--;
	if(msg->count == 0)
	{
		Free(msg);
	}
}

void init_sock_list()
{
	g_sock_list = SAFE_LIST_CREATE();
	SAFE_LIST_INIT(g_sock_list, free_sock);
}

void destroy_sock_list()
{
	SAFE_LIST_DESTROY(g_sock_list);
}

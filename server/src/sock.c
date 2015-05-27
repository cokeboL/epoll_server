#include "sock.h"

Sock *g_socks[MAX_CLIENTS_NUM] = {0};

inline Sock *create_sock(int fd)
{
	Sock *sock = (Sock*)Malloc(sizeof(Sock));
	memset(sock, 0, sizeof(Sock));
	sock->fd = fd;

	return sock;
}

inline void remove_sock(Sock *sock)
{
	if(sock->msg)
	{
		Free(sock->msg);
	}
	close(sock->fd);
	g_socks[sock->fd] = 0;
	Free(sock);
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
	if(msg->count <= 0)
	{
		Free(msg);
	}
}

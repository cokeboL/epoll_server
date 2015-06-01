#ifndef _sock_h_
#define _sock_h_

#include "commen.h"
#include "safelist.h"

#define PACK_HEAD_LEN 4
#define MSG_HEAD_NOT_READED  0 //未读到msg长度
#define MSG_HEAD_READED  1     //已读到msg长度
#define MSG_READED  2         //msg收完 

#define REGIST_TIMEOUT 3      //认证超时时间
#define HEARTBEAT_TIMEOUT 30  //心跳超时时间

typedef struct SockMsg SockMsg;

typedef struct Sock
{
	//来源
	int fd;

	int epoll_fd;

	SafeListNode *list_node;

	//收到数据，对方此消息的总长度
	uint16_t len_total;

	//已经读到的数据长度
	uint16_t len_readed;

	//最后响应时间
	time_t last_pack_time;

	//当前读到的数据的状态
	int8_t status;
	
	//加timer后要做连接的认证超时，连接默认非法，非法连接来包丢弃；
	//在xx时间内session认证通过转为合法，才能进行正常收发包处理，如果超时未认证通过，踢掉；
	bool legal;

	//包序号标识，从15-0，对包长进行按位异或
	int8_t time_count;

	//校验标识
	bool verify;

	//存储读到的数据
	int8_t head[PACK_HEAD_LEN];
	SockMsg *msg;
}Sock;

struct SockMsg
{
	Sock *sock;     //Msg来源：sock->fd
	uint16_t len;   //SockMsg.msg指向的buf长度
	int16_t count;//引用计数
	//int16_t cmd;
	//int16_t action;
	char *buf;
	char data[0];    //消息体：去掉整个Msg的head四个字节
};

#define MSG_CMD(msg) ((msg)->data[2])
#define MSG_ACTION(msg) ((msg)->data[3])


extern SafeList *g_sock_list;

extern Sock *create_sock(int fd, int epoll_fd);

extern void remove_sock(Sock *sock, bool left_in_list);

extern void heart_beat_sock(Sock *sock);

extern SockMsg *create_recv_msg(Sock *sock);

extern SockMsg *create_send_msg(Sock *sock, uint16_t len, int8_t cmd, int8_t action, char *msg);

extern void msg_retain(SockMsg *msg);

extern void msg_release(SockMsg *msg);

extern void init_sock_list();

extern void destroy_sock_list();

extern void socks_online_foreach(SafeListNodeHandler handler);

#endif // _sock_h_

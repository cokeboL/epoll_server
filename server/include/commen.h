#include <stdio.h>  
#include <pthread.h>  
#include <string.h>  
#include <sys/types.h>  
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "debug.h"



//使用第三方库
#define Malloc malloc
#define Free   free


//define bool
#define bool int8_t
#define true 1
#define false 0


//socket配置
#define MAX_FD_NUM        0x1FFFF   //最大fd值
#define MAX_CLIENT_NUM    0xFFFF    //最大连接数
#define MAX_MSG_LEN       (1024*10) //最大消息长度


//数据库配置
#define MAX_REDIS_CMD_LEN (1024*10)


//时间参数配置
#define USEC_PER_SEC    (1000*1000)        //每秒毫秒数
#define HEART_BEAT_TIME (USEC_PER_SEC * 5) //心跳间隔
#define SOCK_TIME_OUT   10                 //连接超时时间
#define SEND_SLEEP_US   5                  //重新发包间隔





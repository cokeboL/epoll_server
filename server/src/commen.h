#include <stdio.h>  
#include <pthread.h>  
#include <string.h>  
#include <sys/types.h>  
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "debug.h"

#define Malloc malloc
#define Free   free

#define SERVER_NUM 2    //监听事件的server线程数
#define HANDLER_NUM 3   //处理io事件及逻辑的handler线程数

#define MAX_FD_NUM      0xFFFFF //最大fd值
#define MAX_CLIENT_NUM  0xFFFF  //最大连接数
#define MAX_MSG_LEN     1024*12 //最大消息长度
#define HEART_BEAT_TIME 1000*30 //心跳间隔
#define SOCK_TIME_OUT   30      //连接超时时间

#define bool int8_t
#define true 1
#define false 0

#define SEND_SLEEP_US 5

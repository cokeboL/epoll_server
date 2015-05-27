#include <stdio.h>  
#include <pthread.h>  
#include <string.h>  
#include <sys/types.h>  
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#define Malloc malloc
#define Free   free

#define READER_NUM 2    //接收数据线程数
#define WRITER_NUM 1    //发送数据线程数
#define SERVER_NUM 2    //监听事件的server线程数
#define G_HANDLER_NUM 1 //需要使用全局资源的handler线程数
#define N_HANDLER_NUM 1 //不需要使用全局资源的handler线程数

#define MAX_CLIENTS_NUM 0xFFFF //最大连接数
#define MAX_MSG_LEN 1024*12

#define bool int8_t
#define true 1
#define false 0

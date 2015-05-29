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

#define MAX_CLIENTS_NUM 0xFFFF //最大连接数
#define MAX_MSG_LEN 1024*12

#define bool int8_t
#define true 1
#define false 0



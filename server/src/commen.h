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

#define READER_NUM 2
#define WRITER_NUM 1

#define MAX_MSG_LEN 1024*12

#define bool int8_t
#define true 1
#define false 0

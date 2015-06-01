#ifndef _debug_h_
#define _debug_h_

#include<stdio.h>

#define LOG_LEVELE LLEVEL_INFO

typedef enum LogLevel
{
	LLEVEL_INFO,
	LLEVEL_DEBUG,
	LLEVEL_WARNING,
	LLEVEL_ERROR,
}LogLevel;

#define LOG(level, format) fprintf(stderr,"[LOG] %s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__)

#endif // _debug_h_

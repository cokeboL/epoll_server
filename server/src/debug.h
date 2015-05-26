#ifndef _debug_h_
#define _debug_h_

#ifdef DEBUG

#include<stdio.h>

#define INFO(format) fprintf(stderr,"%s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__)
#define LOG(format, ...) fprintf(stderr,"%s:%d:%s -> " format "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)

#else

#define LOG(...)
#define INFO(...)

#endif // DEBUG

#endif // _debug_h_

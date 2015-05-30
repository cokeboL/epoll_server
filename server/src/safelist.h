#ifndef _safelist_h_
#define _safelist_h_

#include <stdlib.h>
#include <stdint.h>
#include "mutex.h"

#ifndef Malloc
#define Malloc malloc
#endif
#ifndef Free
#define Free free
#endif


/*
#define SAFE_LIST_CREATE()
#define SAFE_LIST_INIT(list, handler)
#define SAFE_LIST_HEAD(list) (list->__head)
#define SAFE_LIST_TAIL(list) (list->__tail)
#define SAFE_LIST_PUSH(list, value)
#define SAFE_LIST_REMOVE(list, node)
#define SAFE_LIST_REMOVE_IF(list, rm_if)
#define SAFE_LIST_TRAVERSE(list, handler)
#define SAFE_LIST_TRAVERSE2(list, handler)
#define SAFE_LIST_DESTROY(list)
*/

typedef void (*ListNodeHandler)(void *);

typedef struct ListNode
{
struct ListNode *__pre;
struct ListNode *__next;
void *__data;
}ListNode;

typedef struct List
{
ListNode *__head;
ListNode *__tail;
ListNodeHandler __release_handler;
mutex_t __mutex;
uint32_t __count;
}List;


#define SAFE_LIST_CREATE() (List*)Malloc(sizeof(List))


#define SAFE_LIST_INIT(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
if (__list)\
{\
__list->__head = 0; \
__list->__tail = 0; \
__list->__count = 0; \
__list->__release_handler = (ListNodeHandler)__handler; \
mutex_init(&__list->__mutex, 0);\
}\
}\
while(0)


#define SAFE_LIST_HEAD(list) (list->__head)


#define SAFE_LIST_TAIL(list) (list->__tail)


#define SAFE_LIST_SIZE(list) (list->__count)


#define SAFE_LIST_PUSH(list, value)\
do\
{\
List *__list = (list);\
void *__value = (value);\
mutex_lock(&__list->__mutex);\
ListNode *__node = (ListNode*)Malloc(sizeof(ListNode)); \
if (__list->__head)\
{\
__node->__pre = __list->__tail; \
__node->__next = 0; \
__node->__data = (void*)__value; \
__list->__tail->__next = __node; \
__list->__tail = __node; \
}\
else\
{\
__node->__data = (void*)__value; \
__list->__head = __list->__tail = __node; \
__node->__pre = __node->__next = 0; \
}\
__list->__count++;\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_POP(list)\
do\
{\
List *__list = (list);\
mutex_lock(&__list->__mutex);\
if (__list->__head)\
{\
ListNode *__tmp = __list->__tail; \
if (__list->__tail->__pre)\
{\
__list->__tail = __list->__tail->__pre; \
__list->__tail->__next = 0; \
}\
else\
{\
__list->__head = __list->__head = 0;\
}\
__list->__release_handler ? __list->__release_handler(__tmp->__data) : 0; \
Free(__tmp); \
}\
__list->__count--;\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_REMOVE(list, node)\
do\
{\
List *__list = (list);\
ListNode *__node = (node);\
mutex_lock(&__list->__mutex);\
if (__node->__pre)\
{\
__node->__pre->__next = __node->__next; \
if (__node->__next)\
{\
__node->__next->__pre = __node->__pre; \
}\
else\
{\
__list->__tail = __node->__pre; \
}\
}\
else\
{\
__list->__head = __node->__next; \
if (__node->__next)\
{\
__node->__next->__pre = 0; \
}\
else\
{\
__list->__tail = 0; \
}\
}\
if (__list->__release_handler)\
{\
__list->__release_handler(__node->__data); \
}\
Free(__node); \
__list->__count--;\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_REMOVE_IF(list, __rm_if)\
do\
{\
List *__list = (list);\
mutex_lock(&__list->__mutex);\
ListNode **__node = &__list->__head, *__entry; \
while (*__node)\
{\
__entry = *__node; \
if (__rm_if(__entry->__data))\
{\
*__node = __entry->__next; \
if (*__node == __list->__head)\
{\
(*__node)->__pre = 0;\
}\
if (__list->__release_handler)\
{\
__list->__release_handler(__entry->__data); \
}\
Free(__entry); \
__list->__count--;\
}\
else\
{\
__node = &__entry->__next; \
}\
}\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_TRAVERSE(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
mutex_lock(&__list->__mutex);\
ListNode *__tmp = __list->__head; \
while (__tmp)\
{\
__handler(__tmp->__data); \
__tmp = __tmp->__next; \
}\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_TRAVERSE2(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
mutex_lock(&__list->__mutex);\
ListNode *__tmp = __list->__tail; \
while (__tmp)\
{\
__handler(__tmp->__data); \
__tmp = __tmp->__pre; \
}\
mutex_unlock(&__list->__mutex);\
} while (0)


#define SAFE_LIST_DESTROY(list)\
do\
{\
List *__list = (list);\
mutex_lock(&__list->__mutex);\
ListNode *__curr = __list->__head, *__tmp;\
while (__curr)\
{\
if (__list->__release_handler)\
{\
__list->__release_handler(__curr->__data); \
}\
__tmp = __curr->__next; \
Free(__curr); \
__curr = __tmp;\
}\
__list->__head = __list->__tail = 0;\
__list->__count = 0;\
Free(__list);\
mutex_unlock(&__list->__mutex);\
} while (0)


#endif // _safelist_h_

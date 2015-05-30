#ifndef _safelist_h_
#define _safelist_h_

#include <stdlib.h>
#include <stdint.h>

#ifndef Malloc
#define Malloc malloc
#endif
#ifndef Free
#define Free free
#endif


/*
#define LIST_CREATE()
#define LIST_INIT(list, handler)
#define LIST_HEAD(list) (list->__head)
#define LIST_TAIL(list) (list->__tail)
#define LIST_PUSH(list, value)
#define LIST_REMOVE(list, node)
#define LIST_REMOVE_IF(list, rm_if)
#define LIST_TRAVERSE(list, handler)
#define LIST_TRAVERSE2(list, handler)
#define LIST_DESTROY(list)
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
uint32_t __count;
}List;


#define LIST_CREATE() (List*)Malloc(sizeof(List))


#define LIST_INIT(list, handler)\
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
}\
}\
while(0)


#define LIST_HEAD(list) (list->__head)


#define LIST_TAIL(list) (list->__tail)


#define LIST_SIZE(list) (list->__count)


#define LIST_PUSH(list, value)\
do\
{\
List *__list = (list);\
void *__value = (value);\
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
} while (0)


#define LIST_POP(list)\
do\
{\
List *__list = (list);\
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
} while (0)


#define LIST_REMOVE(list, node)\
do\
{\
List *__list = (list);\
ListNode *__node = (node);\
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
} while (0)


#define LIST_REMOVE_IF(list, __rm_if)\
do\
{\
List *__list = (list);\
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
} while (0)


#define LIST_TRAVERSE(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
ListNode *__tmp = __list->__head; \
while (__tmp)\
{\
__handler(__tmp->__data); \
__tmp = __tmp->__next; \
}\
} while (0)


#define LIST_TRAVERSE2(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
ListNode *__tmp = __list->__tail; \
while (__tmp)\
{\
__handler(__tmp->__data); \
__tmp = __tmp->__pre; \
}\
} while (0)


#define LIST_DESTROY(list)\
do\
{\
List *__list = (list);\
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
} while (0)


#endif // _safelist_h_

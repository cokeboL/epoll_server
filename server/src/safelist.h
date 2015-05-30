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
#define LIST_CREATE()
#define LIST_INIT(list, handler)
#define LIST_HEAD(list) (list->head)
#define LIST_TAIL(list) (list->tail)
#define LIST_PUSH(list, value)
#define LIST_REMOVE(list, node)
#define LIST_REMOVE_IF(list, rm_if)
#define LIST_TRAVERSE(list, handler)
#define LIST_TRAVERSE2(list, handler)
#define LIST_DESTROY(list)
*/

typedef void(*ListNodeHandler)(void *);

typedef struct ListNode
{
struct ListNode *pre;
struct ListNode *next;
void *data;
}ListNode;

typedef struct List
{
ListNode *head;
ListNode *tail;
ListNodeHandler release_handler;
mutex_t mutex;
uint32_t count;
}List;


#define LIST_CREATE() (List*)Malloc(sizeof(List))


#define LIST_INIT(list, handler)\
do\
{\
if (list)\
{\
list->head = 0; \
list->tail = 0; \
list->count = 0; \
list->release_handler = (ListNodeHandler)handler; \
mutex_init(&list->mutex, 0);\
}\
}\
while(0)

#define LIST_HEAD(list) (list->head)


#define LIST_TAIL(list) (list->tail)


#define LIST_PUSH(list, value)\
do\
{\
mutex_lock(&list->mutex);\
if (list->head)\
{\
ListNode *node = (ListNode*)Malloc(sizeof(ListNode)); \
node->pre = list->tail; \
node->next = 0; \
node->data = (void*)value; \
list->tail->next = node; \
list->tail = node; \
}\
else\
{\
ListNode *node = (ListNode*)Malloc(sizeof(ListNode)); \
node->data = (void*)value; \
list->head = list->tail = node; \
node->pre = node->next = 0; \
}\
list->count++;\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_POP(list)\
do\
{\
mutex_lock(&list->mutex);\
if (list->head)\
{\
ListNode *tmp = list->tail; \
if (list->tail->pre)\
{\
list->tail = list->tail->pre; \
list->tail->next = 0; \
}\
else\
{\
list->head = list->head = 0;\
}\
list->release_handler ? list->release_handler(tmp->data) : 0; \
Free(tmp); \
}\
list->count--;\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_REMOVE(list, node)\
do\
{\
mutex_lock(&list->mutex);\
ListNode *tmp = node; \
if (tmp->pre)\
{\
tmp->pre->next = tmp->next; \
if (tmp->next)\
{\
tmp->next->pre = tmp->pre; \
}\
else\
{\
list->tail = tmp->pre; \
}\
}\
else\
{\
list->head = tmp->next; \
if (tmp->next)\
{\
tmp->next->pre = 0; \
}\
else\
{\
list->tail = 0; \
}\
}\
if (list->release_handler)\
{\
list->release_handler(tmp->data); \
}\
Free(tmp); \
list->count--;\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_REMOVE_IF(list, rm_if)\
do\
{\
mutex_lock(&list->mutex);\
ListNode **node = &list->head, *entry; \
while (*node)\
{\
entry = *node; \
if (rm_if(entry->data))\
{\
*node = entry->next; \
if (*node == list->head)\
{\
(*node)->pre = 0;\
}\
if (list->release_handler)\
{\
list->release_handler(entry->data); \
}\
Free(entry); \
list->count--;\
}\
else\
{\
node = &entry->next; \
}\
}\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_TRAVERSE(list, handler)\
do\
{\
mutex_lock(&list->mutex);\
ListNode *tmp = list->head; \
while (tmp)\
{\
handler(tmp->data); \
tmp = tmp->next; \
}\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_TRAVERSE2(list, handler)\
do\
{\
mutex_lock(&list->mutex);\
ListNode *tmp = list->tail; \
while (tmp)\
{\
handler(tmp->data); \
tmp = tmp->pre; \
}\
mutex_unlock(&list->mutex);\
} while (0)


#define LIST_DESTROY(list)\
do\
{\
mutex_lock(&list->mutex);\
ListNode *curr = list->head, *tmp;\
while (curr)\
{\
if (list->release_handler)\
{\
list->release_handler(curr->data); \
}\
tmp = curr->next; \
Free(curr); \
curr = tmp;\
}\
list->head = list->tail = 0;\
list->count = 0;\
mutex_unlock(&list->mutex);\
} while (0)

#endif // _safelist_h_

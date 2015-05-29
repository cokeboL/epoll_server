#ifndef _list_h_
#define _list_h_

#if 0
#include <stdlib.h>
#define Malloc malloc
#define Free free
#endif

#include "commen.h"

typedef void (*ListNodeReleaseHandler)(void *);
typedef void (*ListNodeTraverseHandler)(void *);


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
	int count;
	ListNodeReleaseHandler release_handler;
}List;

#define LIST_CREATE() \
(List*)Malloc(sizeof(List))

#define LIST_INIT(list, handler) \
if (list){ \
list->head = 0; \
list->tail = 0; \
list->count = 0; \
list->release_handler = (ListNodeReleaseHandler)handler; \
}

#define LIST_PUSH(list, value) \
do{ \
if(list->head){ \
ListNode *node = (ListNode*)Malloc(sizeof(ListNode)); \
node->pre = list->tail; \
node->next = 0; \
node->data = (void*)value; \
list->tail->next = node; \
list->tail = node; \
}else{ \
ListNode *node = (ListNode*)Malloc(sizeof(ListNode)); \
node->data = (void*)value; \
list->head = list->tail = node; \
node->pre = node->next = 0; \
} \
}while (0);


#define LIST_POP(list) \
do{\
if (list->head){\
ListNode *tmp = list->tail; \
if (list->tail->pre){\
list->tail = list->tail->pre; \
list->tail->next = 0; \
} \
list->release_handler ? list->release_handler(tmp->data) : 0; \
Free(tmp); \
} \
} while (0);

#define LIST_REMOVE(list, node) \
do{\
if (node->pre) \
{ \
node->pre->next = node->next; \
if (node->next) \
{ \
node->next->pre = (ListNode*)node->pre; \
} \
else{\
list->tail = (ListNode*)node->pre; \
} \
}else{\
list->head = list->tail = 0; \
}\
if (list->release_handler){\
list->release_handler(node->data); \
} \
Free(node); \
} while (0);

#define LIST_TRAVERSE(list, handler) \
do{\
ListNode *tmp = list->head; \
while (tmp){ \
handler(tmp->data); \
tmp = tmp->next; \
} \
} while (0);

#define LIST_TRAVERSE2(list, handler) \
do{\
ListNode *tmp = list->tail; \
while (tmp){\
handler(tmp->data); \
tmp = tmp->pre; \
} \
} while (0);

#endif // _list_h_

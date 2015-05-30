#ifndef _list_h_
#define _list_h_

#include <stdlib.h>
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
int count;
ListNodeHandler release_handler;
}List;

#define LIST_CREATE() (List*)Malloc(sizeof(List))

#define LIST_INIT(list, handler)\
if (list)\
{\
list->head = 0; \
list->tail = 0; \
list->count = 0; \
list->release_handler = (ListNodeHandler)handler; \
}

#define LIST_HEAD(list) (list->head)

#define LIST_TAIL(list) (list->tail)

#define LIST_PUSH(list, value)\
do\
{\
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
} while (0)


#define LIST_POP(list)\
do\
{\
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
} while (0)

#define LIST_REMOVE(list, node)\
do\
{\
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
} while (0)

#define LIST_REMOVE_IF(list, rm_if)\
do\
{\
ListNode **node = &list->head, *entry; \
while (*node)\
{\
entry = *node; \
if (rm_if(entry->data))\
{\
*node = entry->next; \
LIST_REMOVE(list, entry); \
}\
else\
{\
node = &entry->next; \
}\
}\
} while (0)

#define LIST_TRAVERSE(list, handler)\
do\
{\
ListNode *tmp = list->head; \
while (tmp)\
{\
handler(tmp->data); \
tmp = tmp->next; \
}\
} while (0)

#define LIST_TRAVERSE2(list, handler)\
do\
{\
ListNode *tmp = list->tail; \
while (tmp)\
{\
handler(tmp->data); \
tmp = tmp->pre; \
}\
} while (0)

#define LIST_DESTROY(list)\
do\
{\
while (list->head)\
{\
LIST_POP(list); \
}\
} while (0)

#endif // _list_h_

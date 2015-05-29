#ifndef _list_h_
#define _list_h_

#include "commen.h"

typedef void (*ListNodeReleaseHandler)(void *);

typedef struct ListNode
{
	void *pre;
	void *next;
	void *data;
}ListNode;

typedef struct List
{
	ListNode *head;
	ListNode *tail;
	uint32_t count;
	ListNodeReleaseHandler release_handler;
}List;

#define LIST_CREATE() \
	(List*)Malloc(sizeof(List))

#define LIST_INIT(list, release_handler) \
	if(list){ \
		list->head = list->tail = list->count = 0; \
		list->release_handler = release_handler; \
	}

#define LIST_PUSH(list, data) \
	if(list->head){ \
		ListNode *node = Malloc(sizeof(ListNode)); \
		node->pre = list->tail; \
		node->next = 0; \
		node->data = data; \
		list->tail->next = node; \
		list->tail = node; \
	}else{ \
		ListNode *node = Malloc(sizeof(ListNode)); \
		list->head = list->tail = node; \
		node->pre = node->next = 0; \
	}


#define LIST_POP(list) \
	if(list->head){ \
		ListNode *tmp = list->tail; \
		list->tail = list->tail->pre; \
		list->tail->next = 0; \
		list->release_handler ? list->release_handler(tmp->data) : 0; \
		Free(tmp); \
	}

#define LIST_REMOVE(list, node) \
	if(list->head){ \
		node->next ? 0 : list->tail = node->pre; \
		node->pre ? (node->pre->next = node->next) : list->head = list->tail = 0; \
		list->release_handler ? list->release_handler(node->data) : 0; \
		Free(node); \
	}



#endif // _list_h_

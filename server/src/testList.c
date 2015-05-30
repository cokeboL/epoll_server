#include "list.h"
#include <stdio.h>

/*
#define LIST_CREATE()
#define LIST_INIT(list, release_handler)
#define LIST_PUSH(list, data)
#define LIST_POP(list)
#define LIST_REMOVE(list, node)
*/

void release(void *data)
{
	printf("%d\n", (int)data);
}

void print(void *data)
{
	printf("%d\n", (int)data);
}

int rm(void *data)
{
	if ((int)data % 2)
	{
		return 1;
	}
	return 0;
}
int main()
{
	List *list = LIST_CREATE();
	LIST_INIT(list, release);

	ListNode *p = 0;
	int i = 0;
	for (i = 1; i<11; i++)
	{
		LIST_PUSH(list, i);
		if (i == 3)
		{
			p = list->tail;
		}
	}
	
	LIST_TRAVERSE(list, print);
	printf("**********\n");
	LIST_REMOVE_IF(list, rm);
	LIST_TRAVERSE(list, print);
	printf("**********\n");
	printf("head: %d\n", (int)list->head->data);
	printf("head: %p %p\n", list->head->pre, list->head->next);
	LIST_REMOVE(list, list->head);
	printf("tail: %d\n", (int)list->tail->data);
	printf("tail: %p %p\n", list->tail->pre, list->tail->next);
	printf("**********\n");
	LIST_TRAVERSE(list, print);
	printf("**********\n");
	for (i = 1; i<11; i++)
	{
		LIST_PUSH(list, i);
		if (i == 3)
		{
			p = list->tail;
		}
	}
	LIST_TRAVERSE(list, print);
	LIST_DESTROY(list);
	printf("**********\n");
	LIST_TRAVERSE(list, print);
	getchar();
}
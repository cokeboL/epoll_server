#include "safelist.h"
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
	SafeList *list = SAFE_LIST_CREATE();
	SAFE_LIST_INIT(list, release);

	int i = 0;
	for (i = 1; i<11; i++)
	{
		SAFE_LIST_PUSH(list, i);
	}
	
	SAFE_LIST_TRAVERSE(list, print);
	printf("**********\n");
	SAFE_LIST_REMOVE_IF(list, rm);
	SAFE_LIST_TRAVERSE(list, print);
	printf("**********\n");
	printf("head: %d\n", (int)list->__head->__data);
	printf("head: %p %p\n", list->__head->__pre, list->__head->__next);
	SAFE_LIST_REMOVE(list, list->__head);
	printf("tail: %d\n", (int)list->__tail->__data);
	printf("tail: %p %p\n", list->__tail->__pre, list->__tail->__next);
	printf("**********\n");
	SAFE_LIST_TRAVERSE(list, print);
	printf("**********\n");
	for (i = 1; i<11; i++)
	{
		SAFE_LIST_PUSH(list, i);
		if (i == 3)
		{
			p = list->__tail;
		}
	}
	SAFE_LIST_TRAVERSE(list, print);
	SAFE_LIST_DESTROY(list);
	printf("**********\n");
	SAFE_LIST_TRAVERSE(list, print);
	getchar();
}

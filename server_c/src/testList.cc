#include "list.h"
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

int rmif(void *data)
{
	return 1;
}


int main()
{
#if 1
	SafeList *list = SAFE_LIST_CREATE();
	SAFE_LIST_INIT(list, release);

	int i = 0;
	for (i = 1; i<11; i++)
	{
		SAFE_LIST_PUSH(list, i, 0);
	}
	
	SAFE_LIST_TRAVERSE(list, print);
	printf("**********\n");
	
	printf("remove if 111\n");
	SAFE_LIST_REMOVE_IF(list, rmif);
	printf("remove if 222\n");
	printf("size: %d\n", SAFE_LIST_SIZE(list));
	SAFE_LIST_TRAVERSE(list, print);
	SAFE_LIST_DESTROY(list);
	printf("**********\n");
#endif
	
	getchar();
}

#include "list.h"
#include <stdio.h>

/*
#define LIST_CREATE()
#define LIST_INIT(list, release_handler)
#define LIST_PUSH(list, data) 
#define LIST_POP(list) 
#define LIST_REMOVE(list, node)
*/

void print(void *data)
{
	printf("%d\n", (int)data);
}

int main()
{
	List *list = LIST_CREATE();
	LIST_INIT(list, 0);

	int i=0;
	for(i=0; i<10; i++)
	{
		LIST_PUSH(list, i);
	}

	LIST_TRAVERSE(list, print);
}
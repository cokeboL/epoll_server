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

int main()
{
#if 1
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
	printf("head: %d\n", (int)list->_sl_head_->_sl_data_);
	printf("head: %p %p\n", list->_sl_head_->_sl_pre_, list->_sl_head_->_sl_next_);
	SAFE_LIST_REMOVE(list, list->_sl_head_);
	printf("tail: %d\n", (int)list->_sl_tail_->_sl_data_);
	printf("tail: %p %p\n", list->_sl_tail_->_sl_pre_, list->_sl_tail_->_sl_next_);
	printf("**********\n");
	SAFE_LIST_TRAVERSE(list, print);
	printf("**********\n");
	for (i = 1; i<11; i++)
	{
		SAFE_LIST_PUSH(list, i);
	}
	SAFE_LIST_TRAVERSE(list, print);
	SAFE_LIST_DESTROY(list);
	printf("**********\n");
	SAFE_LIST_TRAVERSE(list, print);
#endif

#if 1
	printf("--------------------------------------------\n");
	List *list2 = LIST_CREATE();
	LIST_INIT(list2, release);

	for (i = 1; i<11; i++)
	{
		LIST_PUSH(list2, i);
	}
	
	LIST_TRAVERSE(list2, print);
	printf("**********\n");
	LIST_REMOVE_IF(list2, rm);
	LIST_TRAVERSE(list2, print);
	printf("**********\n");
	printf("head: %d\n", (int)list2->_list_head_->_list_data_);
	printf("head: %p %p\n", list2->_list_head_->_list_pre_, list2->_list_head_->_list_next_);
	LIST_REMOVE(list, list2->_list_head_);
	printf("tail: %d\n", (int)list2->_list_tail_->_list_data_);
	printf("tail: %p %p\n", list2->_list_tail_->_list_pre_, list2->_list_tail_->_list_next_);
	printf("**********\n");
	LIST_TRAVERSE(list2, print);
	printf("**********\n");
	for (i = 1; i<11; i++)
	{
		LIST_PUSH(list2, i);
	}
	LIST_TRAVERSE(list2, print);
	LIST_DESTROY(list2);
	printf("**********\n");
	LIST_TRAVERSE(list2, print);
#endif
	
	getchar();
}

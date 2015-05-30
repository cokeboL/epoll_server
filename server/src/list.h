#ifndef _list_h_
#define _list_h_

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
#define LIST_HEAD(list) (list->_sl_head_)
#define LIST_TAIL(list) (list->_sl_tail_)
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
	struct ListNode *_sl_pre_;
	struct ListNode *_sl_next_;
	void *_sl_data_;
}ListNode;

typedef struct List
{
	ListNode *_sl_head_;
	ListNode *_sl_tail_;
	ListNodeHandler _sl_release_handler_;
	uint32_t _sl_count_;
}List;


#define LIST_CREATE() (List*)Malloc(sizeof(List))


#define LIST_INIT(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
if (__list)\
{\
__list->_sl_head_ = 0; \
__list->_sl_tail_ = 0; \
__list->_sl_count_ = 0; \
__list->_sl_release_handler_ = (ListNodeHandler)__handler; \
}\
}\
while(0)


#define LIST_HEAD(list) (list->_sl_head_)


#define LIST_TAIL(list) (list->_sl_tail_)


#define LIST_SIZE(list) (list->_sl_count_)


#define LIST_PUSH(list, value)\
do\
{\
List *__list = (list);\
void *__value = (value);\
ListNode *__node = (ListNode*)Malloc(sizeof(ListNode)); \
if (__list->_sl_head_)\
{\
__node->_sl_pre_ = __list->_sl_tail_; \
__node->_sl_next_ = 0; \
__node->_sl_data_ = (void*)__value; \
__list->_sl_tail_->_sl_next_ = __node; \
__list->_sl_tail_ = __node; \
}\
else\
{\
__node->_sl_data_ = (void*)__value; \
__list->_sl_head_ = __list->_sl_tail_ = __node; \
__node->_sl_pre_ = __node->_sl_next_ = 0; \
}\
__list->_sl_count_++;\
} while (0)


#define LIST_POP(list)\
do\
{\
List *__list = (list);\
if (__list->_sl_head_)\
{\
ListNode *__tmp = __list->_sl_tail_; \
if (__list->_sl_tail_->_sl_pre_)\
{\
__list->_sl_tail_ = __list->_sl_tail_->_sl_pre_; \
__list->_sl_tail_->_sl_next_ = 0; \
}\
else\
{\
__list->_sl_head_ = __list->_sl_head_ = 0;\
}\
__list->_sl_release_handler_ ? __list->_sl_release_handler_(__tmp->_sl_data_) : 0; \
Free(__tmp); \
}\
__list->_sl_count_--;\
} while (0)


#define LIST_REMOVE(list, node)\
do\
{\
List *__list = (list);\
ListNode *__node = (node);\
if (__node->_sl_pre_)\
{\
__node->_sl_pre_->_sl_next_ = __node->_sl_next_; \
if (__node->_sl_next_)\
{\
__node->_sl_next_->_sl_pre_ = __node->_sl_pre_; \
}\
else\
{\
__list->_sl_tail_ = __node->_sl_pre_; \
}\
}\
else\
{\
__list->_sl_head_ = __node->_sl_next_; \
if (__node->_sl_next_)\
{\
__node->_sl_next_->_sl_pre_ = 0; \
}\
else\
{\
__list->_sl_tail_ = 0; \
}\
}\
if (__list->_sl_release_handler_)\
{\
__list->_sl_release_handler_(__node->_sl_data_); \
}\
Free(__node); \
__list->_sl_count_--;\
} while (0)


#define LIST_REMOVE_IF(list, __rm_if)\
do\
{\
List *__list = (list);\
ListNode **__node = &__list->_sl_head_, *__entry; \
while (*__node)\
{\
__entry = *__node; \
if (__rm_if(__entry->_sl_data_))\
{\
*__node = __entry->_sl_next_; \
if (*__node == __list->_sl_head_)\
{\
(*__node)->_sl_pre_ = 0;\
}\
if (__list->_sl_release_handler_)\
{\
__list->_sl_release_handler_(__entry->_sl_data_); \
}\
Free(__entry); \
__list->_sl_count_--;\
}\
else\
{\
__node = &__entry->_sl_next_; \
}\
}\
} while (0)


#define LIST_TRAVERSE(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
ListNode *__tmp = __list->_sl_head_; \
while (__tmp)\
{\
__handler(__tmp->_sl_data_); \
__tmp = __tmp->_sl_next_; \
}\
} while (0)


#define LIST_TRAVERSE2(list, handler)\
do\
{\
List *__list = (list);\
ListNodeHandler __handler = (handler);\
ListNode *__tmp = __list->_sl_tail_; \
while (__tmp)\
{\
__handler(__tmp->_sl_data_); \
__tmp = __tmp->_sl_pre_; \
}\
} while (0)


#define LIST_DESTROY(list)\
do\
{\
List *__list = (list);\
ListNode *__curr = __list->_sl_head_, *__tmp;\
while (__curr)\
{\
if (__list->_sl_release_handler_)\
{\
__list->_sl_release_handler_(__curr->_sl_data_); \
}\
__tmp = __curr->_sl_next_; \
Free(__curr); \
__curr = __tmp;\
}\
__list->_sl_head_ = __list->_sl_tail_ = 0;\
__list->_sl_count_ = 0;\
Free(__list);\
} while (0)


#endif // _list_h_

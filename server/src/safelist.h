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
#define SAFE_LIST_CREATE()
#define SAFE_LIST_INIT(list, handler)
#define SAFE_LIST_HEAD(list) (list->_sl_head_)
#define SAFE_LIST_TAIL(list) (list->_sl_tail_)
#define SAFE_LIST_PUSH(list, value)
#define SAFE_LIST_REMOVE(list, node)
#define SAFE_LIST_REMOVE_IF(list, rm_if)
#define SAFE_LIST_TRAVERSE(list, handler)
#define SAFE_LIST_TRAVERSE2(list, handler)
#define SAFE_LIST_DESTROY(list)
*/

typedef void (*SafeListNodeHandler)(void *);

typedef struct SafeListNode
{
	struct SafeListNode *_sl_pre_;
	struct SafeListNode *_sl_next_;
	void *_sl_data_;
}SafeListNode;

typedef struct SafeList
{
	SafeListNode *_sl_head_;
	SafeListNode *_sl_tail_;
	SafeListNodeHandler _sl_release_handler_;
	mutex_t _sl_mutex_;
	uint32_t _sl_count_;
}SafeList;


#define SAFE_LIST_CREATE() (SafeList*)Malloc(sizeof(SafeList))


#define SAFE_LIST_INIT(list, handler)\
do\
{\
SafeList *__list = (list);\
SafeListNodeHandler __handler = (handler);\
if (__list)\
{\
__list->_sl_head_ = 0; \
__list->_sl_tail_ = 0; \
__list->_sl_count_ = 0; \
__list->_sl_release_handler_ = (SafeListNodeHandler)__handler; \
mutex_init(&__list->_sl_mutex_, 0);\
}\
}\
while(0)


#define SAFE_LIST_HEAD(list) (list->_sl_head_)


#define SAFE_LIST_TAIL(list) (list->_sl_tail_)


#define SAFE_LIST_SIZE(list) (list->_sl_count_)


#define SAFE_LIST_PUSH(list, value)\
do\
{\
SafeList *__list = (list);\
void *_sl_value_ = (value);\
mutex_lock(&__list->_sl_mutex_);\
SafeListNode *__node = (SafeListNode*)Malloc(sizeof(SafeListNode)); \
if (__list->_sl_head_)\
{\
__node->_sl_pre_ = __list->_sl_tail_; \
__node->_sl_next_ = 0; \
__node->_sl_data_ = (void*)_sl_value_; \
__list->_sl_tail_->_sl_next_ = __node; \
__list->_sl_tail_ = __node; \
}\
else\
{\
__node->_sl_data_ = (void*)_sl_value_; \
__list->_sl_head_ = __list->_sl_tail_ = __node; \
__node->_sl_pre_ = __node->_sl_next_ = 0; \
}\
__list->_sl_count_++;\
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_POP(list)\
do\
{\
SafeList *__list = (list);\
mutex_lock(&__list->_sl_mutex_);\
if (__list->_sl_head_)\
{\
SafeListNode *__tmp = __list->_sl_tail_; \
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
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_REMOVE(list, node)\
do\
{\
SafeList *__list = (list);\
SafeListNode *__node = (node);\
mutex_lock(&__list->_sl_mutex_);\
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
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_REMOVE_IF(list, __rm_if)\
do\
{\
SafeList *__list = (list);\
mutex_lock(&__list->_sl_mutex_);\
SafeListNode **__node = &__list->_sl_head_, *__entry; \
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
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_TRAVERSE(list, handler)\
do\
{\
SafeList *__list = (list);\
SafeListNodeHandler __handler = (handler);\
mutex_lock(&__list->_sl_mutex_);\
SafeListNode *__tmp = __list->_sl_head_; \
while (__tmp)\
{\
__handler(__tmp->_sl_data_); \
__tmp = __tmp->_sl_next_; \
}\
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_TRAVERSE2(list, handler)\
do\
{\
SafeList *__list = (list);\
SafeListNodeHandler __handler = (handler);\
mutex_lock(&__list->_sl_mutex_);\
SafeListNode *__tmp = __list->_sl_tail_; \
while (__tmp)\
{\
__handler(__tmp->_sl_data_); \
__tmp = __tmp->_sl_pre_; \
}\
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_DESTROY(list)\
do\
{\
SafeList *__list = (list);\
mutex_lock(&__list->_sl_mutex_);\
SafeListNode *__curr = __list->_sl_head_, *__tmp;\
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
mutex_unlock(&__list->_sl_mutex_);\
} while (0)


#endif // _safelist_h_

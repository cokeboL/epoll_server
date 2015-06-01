#ifndef _safelist_h_
#define _safelist_h_

#include <stdlib.h>
#include <stdint.h>
#include "mutex.h"

#if UseDefaultAllocator
#ifndef Malloc
#define Malloc malloc
#endif
#ifndef Free
#define Free free
#endif
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
SafeList *_tmp_list = (list);\
SafeListNodeHandler _tmp_handler = (handler);\
if (_tmp_list)\
{\
_tmp_list->_sl_head_ = 0; \
_tmp_list->_sl_tail_ = 0; \
_tmp_list->_sl_count_ = 0; \
_tmp_list->_sl_release_handler_ = (SafeListNodeHandler)_tmp_handler; \
mutex_init(&_tmp_list->_sl_mutex_, 0);\
}\
}\
while(0)


#define SAFE_LIST_HEAD(list) (list->_sl_head_)


#define SAFE_LIST_TAIL(list) (list->_sl_tail_)


#define SAFE_LIST_SIZE(list) (list->_sl_count_)


#define SAFE_LIST_PUSH(list, value, list_node)\
do\
{\
SafeList *_tmp_list = (list);\
SafeListNode **_tmp_list_node = list_node;\
void *_sl_value_ = (void*)(value);\
mutex_lock(&_tmp_list->_sl_mutex_);\
SafeListNode *_tmp_node = (SafeListNode*)Malloc(sizeof(SafeListNode)); \
if (_tmp_list->_sl_head_)\
{\
_tmp_node->_sl_pre_ = _tmp_list->_sl_tail_; \
_tmp_node->_sl_next_ = 0; \
_tmp_node->_sl_data_ = (void*)_sl_value_; \
_tmp_list->_sl_tail_->_sl_next_ = _tmp_node; \
_tmp_list->_sl_tail_ = _tmp_node; \
}\
else\
{\
_tmp_node->_sl_data_ = (void*)_sl_value_; \
_tmp_list->_sl_head_ = _tmp_list->_sl_tail_ = _tmp_node; \
_tmp_node->_sl_pre_ = _tmp_node->_sl_next_ = 0; \
}\
_tmp_list->_sl_count_++;\
if(_tmp_list_node)\
{\
*_tmp_list_node = _tmp_node;\
}\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_POP(list)\
do\
{\
SafeList *_tmp_list = (list);\
mutex_lock(&_tmp_list->_sl_mutex_);\
if (_tmp_list->_sl_head_)\
{\
SafeListNode *_tmp_tmp = _tmp_list->_sl_tail_; \
if (_tmp_list->_sl_tail_->_sl_pre_)\
{\
_tmp_list->_sl_tail_ = _tmp_list->_sl_tail_->_sl_pre_; \
_tmp_list->_sl_tail_->_sl_next_ = 0; \
}\
else\
{\
_tmp_list->_sl_head_ = _tmp_list->_sl_head_ = 0;\
}\
_tmp_list->_sl_release_handler_ ? _tmp_list->_sl_release_handler_(_tmp_tmp->_sl_data_) : 0; \
Free(_tmp_tmp); \
}\
_tmp_list->_sl_count_--;\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_REMOVE(list, node)\
do\
{\
SafeList *_tmp_list = (list);\
SafeListNode *_tmp_node = (node);\
mutex_lock(&_tmp_list->_sl_mutex_);\
if (_tmp_node->_sl_pre_)\
{\
_tmp_node->_sl_pre_->_sl_next_ = _tmp_node->_sl_next_; \
if (_tmp_node->_sl_next_)\
{\
_tmp_node->_sl_next_->_sl_pre_ = _tmp_node->_sl_pre_; \
}\
else\
{\
_tmp_list->_sl_tail_ = _tmp_node->_sl_pre_; \
}\
}\
else\
{\
_tmp_list->_sl_head_ = _tmp_node->_sl_next_; \
if (_tmp_node->_sl_next_)\
{\
_tmp_node->_sl_next_->_sl_pre_ = 0; \
}\
else\
{\
_tmp_list->_sl_tail_ = 0; \
}\
}\
if (_tmp_list->_sl_release_handler_)\
{\
_tmp_list->_sl_release_handler_(_tmp_node->_sl_data_); \
}\
Free(_tmp_node); \
_tmp_list->_sl_count_--;\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_REMOVE_IF(list, _tmp_rm_if)\
do\
{\
SafeList *_tmp_list = (list);\
mutex_lock(&_tmp_list->_sl_mutex_);\
SafeListNode **_tmp_node = &_tmp_list->_sl_head_, *_tmp_entry; \
while (*_tmp_node)\
{\
_tmp_entry = *_tmp_node; \
if (_tmp_rm_if(_tmp_entry->_sl_data_))\
{\
*_tmp_node = _tmp_entry->_sl_next_; \
if (*_tmp_node == _tmp_list->_sl_head_)\
{\
(*_tmp_node)->_sl_pre_ = 0;\
}\
if (_tmp_list->_sl_release_handler_)\
{\
_tmp_list->_sl_release_handler_(_tmp_entry->_sl_data_); \
}\
Free(_tmp_entry); \
_tmp_list->_sl_count_--;\
}\
else\
{\
_tmp_node = &_tmp_entry->_sl_next_; \
}\
}\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_TRAVERSE(list, handler)\
do\
{\
SafeList *_tmp_list = (list);\
SafeListNodeHandler _tmp_handler = (handler);\
mutex_lock(&_tmp_list->_sl_mutex_);\
SafeListNode *_tmp_tmp = _tmp_list->_sl_head_; \
while (_tmp_tmp)\
{\
_tmp_handler(_tmp_tmp->_sl_data_); \
_tmp_tmp = _tmp_tmp->_sl_next_; \
}\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_TRAVERSE2(list, handler)\
do\
{\
SafeList *_tmp_list = (list);\
SafeListNodeHandler _tmp_handler = (handler);\
mutex_lock(&_tmp_list->_sl_mutex_);\
SafeListNode *_tmp_tmp = _tmp_list->_sl_tail_; \
while (_tmp_tmp)\
{\
_tmp_handler(_tmp_tmp->_sl_data_); \
_tmp_tmp = _tmp_tmp->_sl_pre_; \
}\
mutex_unlock(&_tmp_list->_sl_mutex_);\
} while (0)


#define SAFE_LIST_DESTROY(list)\
do\
{\
SafeList *_tmp_list = (list);\
mutex_lock(&_tmp_list->_sl_mutex_);\
SafeListNode *_tmp_curr = _tmp_list->_sl_head_, *_tmp_tmp;\
while (_tmp_curr)\
{\
if (_tmp_list->_sl_release_handler_)\
{\
_tmp_list->_sl_release_handler_(_tmp_curr->_sl_data_); \
}\
_tmp_tmp = _tmp_curr->_sl_next_; \
Free(_tmp_curr); \
_tmp_curr = _tmp_tmp;\
}\
_tmp_list->_sl_head_ = _tmp_list->_sl_tail_ = 0;\
_tmp_list->_sl_count_ = 0;\
mutex_unlock(&_tmp_list->_sl_mutex_);\
Free(_tmp_list);\
} while (0)


#endif // _safelist_h_

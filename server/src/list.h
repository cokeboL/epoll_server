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
#define LIST_HEAD(list) (list->_list_head_)
#define LIST_TAIL(list) (list->_list_tail_)
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
	struct ListNode *_list_pre_;
	struct ListNode *_list_next_;
	void *_list_data_;
}ListNode;

typedef struct List
{
	ListNode *_list_head_;
	ListNode *_list_tail_;
	ListNodeHandler _list_release_handler_;
	uint32_t _list_count_;
}List;


#define LIST_CREATE() (List*)Malloc(sizeof(List))


#define LIST_INIT(list, handler)\
do\
{\
List *_tmp_list = (list);\
ListNodeHandler _tmp_handler = (handler);\
if (_tmp_list)\
{\
_tmp_list->_list_head_ = 0; \
_tmp_list->_list_tail_ = 0; \
_tmp_list->_list_count_ = 0; \
_tmp_list->_list_release_handler_ = (ListNodeHandler)_tmp_handler; \
}\
}\
while(0)


#define LIST_HEAD(list) (list->_list_head_)


#define LIST_TAIL(list) (list->_list_tail_)


#define LIST_SIZE(list) (list->_list_count_)


#define LIST_PUSH(list, value)\
do\
{\
List *_tmp_list = (list);\
void *_tmp_value = (void*)(value);\
ListNode *_tmp_node = (ListNode*)Malloc(sizeof(ListNode)); \
if (_tmp_list->_list_head_)\
{\
_tmp_node->_list_pre_ = _tmp_list->_list_tail_; \
_tmp_node->_list_next_ = 0; \
_tmp_node->_list_data_ = (void*)_tmp_value; \
_tmp_list->_list_tail_->_list_next_ = _tmp_node; \
_tmp_list->_list_tail_ = _tmp_node; \
}\
else\
{\
_tmp_node->_list_data_ = (void*)_tmp_value; \
_tmp_list->_list_head_ = _tmp_list->_list_tail_ = _tmp_node; \
_tmp_node->_list_pre_ = _tmp_node->_list_next_ = 0; \
}\
_tmp_list->_list_count_++;\
} while (0)


#define LIST_POP(list)\
do\
{\
List *_tmp_list = (list);\
if (_tmp_list->_list_head_)\
{\
ListNode *_tmp_tmp = _tmp_list->_list_tail_; \
if (_tmp_list->_list_tail_->_list_pre_)\
{\
_tmp_list->_list_tail_ = _tmp_list->_list_tail_->_list_pre_; \
_tmp_list->_list_tail_->_list_next_ = 0; \
}\
else\
{\
_tmp_list->_list_head_ = _tmp_list->_list_head_ = 0;\
}\
_tmp_list->_list_release_handler_ ? _tmp_list->_list_release_handler_(_tmp_tmp->_list_data_) : 0; \
Free(_tmp_tmp); \
}\
_tmp_list->_list_count_--;\
} while (0)


#define LIST_REMOVE(list, node)\
do\
{\
List *_tmp_list = (list);\
ListNode *_tmp_node = (node);\
if (_tmp_node->_list_pre_)\
{\
_tmp_node->_list_pre_->_list_next_ = _tmp_node->_list_next_; \
if (_tmp_node->_list_next_)\
{\
_tmp_node->_list_next_->_list_pre_ = _tmp_node->_list_pre_; \
}\
else\
{\
_tmp_list->_list_tail_ = _tmp_node->_list_pre_; \
}\
}\
else\
{\
_tmp_list->_list_head_ = _tmp_node->_list_next_; \
if (_tmp_node->_list_next_)\
{\
_tmp_node->_list_next_->_list_pre_ = 0; \
}\
else\
{\
_tmp_list->_list_tail_ = 0; \
}\
}\
if (_tmp_list->_list_release_handler_)\
{\
_tmp_list->_list_release_handler_(_tmp_node->_list_data_); \
}\
Free(_tmp_node); \
_tmp_list->_list_count_--;\
} while (0)


#define LIST_REMOVE_IF(list, _tmp_rm_if)\
do\
{\
List *_tmp_list = (list);\
ListNode **_tmp_node = &_tmp_list->_list_head_, *_tmp_entry; \
while (*_tmp_node)\
{\
_tmp_entry = *_tmp_node; \
if (_tmp_rm_if(_tmp_entry->_list_data_))\
{\
*_tmp_node = _tmp_entry->_list_next_; \
if (*_tmp_node == _tmp_list->_list_head_)\
{\
(*_tmp_node)->_list_pre_ = 0;\
}\
if (_tmp_list->_list_release_handler_)\
{\
_tmp_list->_list_release_handler_(_tmp_entry->_list_data_); \
}\
Free(_tmp_entry); \
_tmp_list->_list_count_--;\
}\
else\
{\
_tmp_node = &_tmp_entry->_list_next_; \
}\
}\
} while (0)


#define LIST_TRAVERSE(list, handler)\
do\
{\
List *_tmp_list = (list);\
ListNodeHandler _tmp_handler = (handler);\
ListNode *_tmp_tmp = _tmp_list->_list_head_; \
while (_tmp_tmp)\
{\
_tmp_handler(_tmp_tmp->_list_data_); \
_tmp_tmp = _tmp_tmp->_list_next_; \
}\
} while (0)


#define LIST_TRAVERSE2(list, handler)\
do\
{\
List *_tmp_list = (list);\
ListNodeHandler _tmp_handler = (handler);\
ListNode *_tmp_tmp = _tmp_list->_list_tail_; \
while (_tmp_tmp)\
{\
_tmp_handler(_tmp_tmp->_list_data_); \
_tmp_tmp = _tmp_tmp->_list_pre_; \
}\
} while (0)


#define LIST_DESTROY(list)\
do\
{\
List *_tmp_list = (list);\
ListNode *_tmp_curr = _tmp_list->_list_head_, *_tmp_tmp;\
while (_tmp_curr)\
{\
if (_tmp_list->_list_release_handler_)\
{\
_tmp_list->_list_release_handler_(_tmp_curr->_list_data_); \
}\
_tmp_tmp = _tmp_curr->_list_next_; \
Free(_tmp_curr); \
_tmp_curr = _tmp_tmp;\
}\
_tmp_list->_list_head_ = _tmp_list->_list_tail_ = 0;\
_tmp_list->_list_count_ = 0;\
Free(_tmp_list);\
} while (0)


#endif // _list_h_

#ifndef _user_h_
#define _user_h_

#include "commen.h"


#define MAX_USER_DATA_LEN       (1024*10) //最大用户数据长度
#define MAX_USER_BASIC_INFO_LEN (1024)    //最大用户基础数据长度
#define MAX_USER_CARD_LIST_LEN  (1024*5)  //最大用户卡牌列表数据长度


typedef struct User
{
	uint32_t id;
	char basic[MAX_USER_BASIC_INFO_LEN];
	char cards[MAX_USER_CARD_LIST_LEN];
	bool dirty;
}User;

#endif // _user_h_

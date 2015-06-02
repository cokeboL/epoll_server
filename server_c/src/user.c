#include "user.h"


inline void init_user_basic(User *user, const char *data)
{
	strncpy(user->basic, data, sizeof(user->basic)-1);
}

inline void init_user_card_list(User *user, const char *data)
{
	strncpy(user->cards, data, sizeof(user->cards)-1);
}

User *create_user(uint32_t id)
{
	User *user = (User*)Malloc(sizeof(User));
	user->id = id;
	user->dirty = false;

	return user;
}


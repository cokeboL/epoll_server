#ifndef _c_lua_
#define _c_lua_

#include "commen.h"
#include "server.h"

lua_State *open_lua();

void close_lua(lua_State *state);

void lua_set_handler(lua_State* state, Server *server);

bool lua_msg_handler(lua_State* state, SockMsg* msg);



#endif // _c_lua_

#include "commen.h"
#include "timer.h"
#include "clua.h"
#include "sock.h"
#include "server.h"
#include "hiredis.h"


static lua_State * lua_state = 0;

static char redis_cmd[MAX_REDIS_CMD_LEN] = { 0 };

/*
void lua_set_handler(lua_State* state, Server *server)
{
	lua_getglobal(state, "SetHandler");               //调用lua中的函数sum
	lua_pushinteger(state, (int)server);

	bool ret = lua_pcall(state, 1, 0, 0);	
	if(ret != 0)
	{
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return false;
	}
}
*/


bool lua_msg_handler(lua_State* state, SockMsg* msg)
{
	lua_getglobal(state, "MsgHandler");               //调用lua中的函数sum
	lua_pushinteger(state, (int)MSG_SOCK(msg)->fd);
	lua_pushinteger(state, MSG_CMD(msg));
	lua_pushinteger(state, MSG_ACTION(msg));
	lua_pushlstring(state, MSG_MSG(msg), MSG_LEN(msg));
	
	bool ret = lua_pcall(state, 4, 1, 0);
	if(ret != 0)
	{
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return false;
	}
	
	ret = lua_toboolean(state,-1);

    return ret;
}

static int lua_send_msg(lua_State* state)
{
	size_t len;
	//Server *server = (Server*)lua_tointeger(state, 1);
	int msg_sock_fd = lua_tointeger(state, 1);
	Sock *msg_sock = g_sock_fd_map[msg_sock_fd];
	int msg_cmd = lua_tointeger(state, 2);
	int msg_action = lua_tointeger(state, 3);
	int msg_len = 0;
	const char *msg_msg = lua_tolstring(state, 4, &msg_len);

	SockMsg *msg = create_send_msg(msg_sock, msg_len, msg_cmd, msg_action, msg_msg);
	write_msg(msg);

	return 0;
}

static int lua_redis_client(lua_State* L)
{
	size_t len;
	const char *ip = lua_tolstring(L, 1, &len);
	int port = lua_tointeger(L, 2);

	redisContext *c = redisConnect(ip, port);
	if (c && c->err)
	{
		//LLog("Error: %s\n", c->errstr);
		lua_pushinteger(L, 0);
		return 1;
	}

	lua_pushinteger(L, (int)c);

	return 1;
}

static int lua_redis_get(lua_State* L)
{
	size_t lenKey;
	int cli = lua_tointeger(L, 1);
	const char *key = lua_tolstring(L, 2, &lenKey);

	//redisReply *reply = redis_get(c, key);
	sprintf(redis_cmd, "get %s", key);

	redisContext *c = (redisContext*)cli;
	redisReply* reply = (redisReply*)redisCommand(c, redis_cmd);
	if (reply->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(reply);
		lua_pushstring(L, "");
		return 1;
	}

	lua_pushstring(L, reply->str);
	freeReplyObject(reply);

	return 1;
}

static int lua_redis_set(lua_State* L)
{
	size_t lenKey, lenValue;
	int cli = lua_tointeger(L, 1);
	const char *key = lua_tolstring(L, 2, &lenKey);
	const char *value = lua_tolstring(L, 3, &lenValue);

	sprintf(redis_cmd, "set %s %s", key, value);

	redisContext *c = (redisContext*)cli;
	redisReply* reply = (redisReply*)redisCommand(c, redis_cmd);    // 执行命令，结果强转成redisReply*类型  
	if (NULL == reply)
	{
		freeReplyObject(reply);
		lua_pushboolean(L, false);
		return 1;
	}
	if (!(reply->type == REDIS_REPLY_STATUS && ((reply->str[0] == 'o' || reply->str[0] == 'O') && (reply->str[1] == 'k' || reply->str[1] == 'K'))))
	{
		freeReplyObject(reply);
		lua_pushboolean(L, false);
		return 1;
	}

	freeReplyObject(reply);
	lua_pushboolean(L, true);

	return 1;
}

static int lua_connect(lua_State* L)
{
	const char *ip = lua_tostring(L, 1);
	int port = lua_tointeger(L, 2);
	
	bool ret = connect_to(ip, port);
	
	lua_pushboolean(L, ret);

	return 1;
}

static const struct luaL_reg c_lua_api_lib[] = {
	{ "sendMsg", lua_send_msg },
	{ "redisClient", lua_redis_client },
	{ "redisGet", lua_redis_get },
	{ "redisSet", lua_redis_set },
	{ "connect", lua_connect },
	{ NULL, NULL },
};

lua_State *open_lua()
{
	lua_State *state = luaL_newstate();
	luaL_openlibs(state);

	luaL_register(state, "clua", c_lua_api_lib);
	
	printf("-- open_lua, state: %p\n", state);
	int ret = luaL_loadfile(state, "init.lua");
	if(ret != 0)
	{
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return 0;
	}
	ret = lua_pcall(state, 0, 0, 0);
	if(ret != 0)
	{
		printf("lua_pcall failed: %s\n", lua_tostring(state,-1));
		return 0;
	}
	lua_pop(state, 1);

	return state;
}

void close_lua(lua_State *state)
{
	lua_close(state);
}

#ifndef _reader_h_
#define _reader_h_

#include "commen.h"
#include "sock.h"


bool write_msg(SockMsg *msg);

void read_data(lua_State *state, Sock *sock);

#endif // _reader_h_

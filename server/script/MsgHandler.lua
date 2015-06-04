local sock_fd_map = {}

local db    = require("Framework/db")

local Chat  = require("Logic/Chat")
local Logic = require("Logic/Login")


function  RemoveSock(fd)
	sock_fd_map[fd] = nil
end

function MsgHandler(fd, cmd, action, msg)
	sock_fd_map[fd] = sock_fd_map[fd] or 0
	sock_fd_map[fd] = sock_fd_map[fd] + 1

	print("++ sock msg count: ", sock_fd_map[fd])

	clua.sendMsg(fd, cmd, action, msg)

	local key = "sock_" .. fd .. "_msg_" .. sock_fd_map[fd]
	local value = key
	db.set(redisClient, key, value)
	local v = db.get(key)
	print("   sock msg save: ", key, v)

	return true
end


local db = {}
local map = {}

local redisClient = clua.redisClient("127.0.0.1", 6379)
print("-- open db, redisClient: ", redisClient)

function db.get(k)
	if map[k] then
		return map[k]
	elseif k then
		map[k] = clua.redisGet(redisClient, key)
		return map[k]
	end

	return nil
end

function db.set(k, v)
	if k and v then
		clua.redisSet(redisClient, k, v)
		map[k] = v
	end
end

return db

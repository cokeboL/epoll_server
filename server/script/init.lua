require("config")
require("Framework/handler")
require("Framework/db")
require("MsgHandler")

collectgarbage("setpause", config.collectpause) 
collectgarbage("setstepmul", config.collectinternal)

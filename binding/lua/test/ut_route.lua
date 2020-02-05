local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Setup --

local store = "RAM"

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

rc, flags = sender:store("HELLO WORLD", 1000)
runner.check(rc)
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bp.check_flags(flags, {}))

-- Test --

-----------------------------------------------------------------------
print(string.format('%s: Test 1 - nominal', src))
rc, node, serv = bplib.route(bundle)
runner.check(rc)
runner.check(node == dst_node)
runner.check(serv == dst_serv)

-- Clean Up --

sender:flush()
sender:close()

-- Report Results --

runner.report(bplib)


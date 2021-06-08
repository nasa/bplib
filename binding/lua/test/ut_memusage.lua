local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local rd = runner.rootdir(arg[0])
local src = runner.srcscript()

-- Setup --

local store = arg[1] or "RAM"
runner.setup(bplib, store)

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 128
local timeout = 2

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - store bundles and close', store, src))

currmem, highmem = bplib.memstat()
print(string.format('Before Open:  %8d %8d', currmem, highmem))

-- open channel --
local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)
currmem, highmem = bplib.memstat()
print(string.format('After Open:  %8d %8d', currmem, highmem))

-- store payloads --
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)
    rc, flags = sender:store(payload, 1000)
end
currmem, highmem = bplib.memstat()
print(string.format('After Store: %8d %8d', currmem, highmem))

-- close channel --
sender:close()
bplib.shutdown() -- special case, calling here before final memstat check
currmem, highmem = bplib.memstat()
print(string.format('After Close: %8d %8d', currmem, highmem))

-- check memory usage --
runner.check(highmem ~= 0, "No memory allocated")
runner.check(currmem == 0, "Memory not cleaned up")

-- Clean Up --

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


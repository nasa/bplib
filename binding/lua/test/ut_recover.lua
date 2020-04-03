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

local receiver = bplib.open(dst_node, dst_serv, src_node, src_serv, store)

rc = receiver:setopt("DACS_RATE", timeout)
runner.check(rc)

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - store and drop all bundles', store, src))

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload --
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on store")
end

sender:close()

-- reopen channel, no recovery --
local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

-- check stats --
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {stored_bundles=0, stored_payloads=0, stored_dacs=0}))

sender:close()

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - store and recover all bundles', store, src))

local attributes = { persistent_storage = 1 }
local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store, attributes)

for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload --
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on store")
end

sender:close()

-- reopen channel, with recovery --
local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store, attributes)

-- check stats --
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {stored_bundles=num_bundles, stored_payloads=0, stored_dacs=0}))

-- do not close channel so that test below
-- can use recovered bundles

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 3 - store and recover some bundles', store, src))

-- load half of the recovered bundles
for i=1,num_bundles/2 do
    payload = string.format('HELLO WORLD %d', i)

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}), "flags set on load")
    runner.check(bp.find_payload(bundle, payload), string.format('Error - wrong payload when checking for %s', payload))

    -- process bundle --
    rc, flags = receiver:process(bundle, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on process")
end

-- load DACS --
bplib.sleep(timeout)
rc, bundle, flags = receiver:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {"routeneeded"}))

-- process DACS --
rc, flags = sender:process(bundle, 1000)
runner.check(rc, string.format('Error(%d) - failed to process DACS: %s', errno, rc))
runner.check(bp.check_flags(flags, {}))

sender:close()

-- reopen channel, with recovery --
local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store, attributes)

-- check stats --
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {stored_bundles=num_bundles/2, stored_payloads=0, stored_dacs=0}))

sender:close()

-- Clean Up --

receiver:close()

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


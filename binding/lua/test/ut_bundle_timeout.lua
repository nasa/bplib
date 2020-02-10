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

local num_bundles = 10
local timeout = 5

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

rc = sender:setopt("TIMEOUT", timeout)
runner.check(rc)

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - store bundles', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload --
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on store")
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - load bundles', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}), "flags set on load")
    runner.check(bp.find_payload(bundle, payload), string.format('Error - wrong payload when checking for %s', payload))
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 4 - no bundle', store, src))
rc, bundle, flags = sender:load(1000)
runner.check(rc == false)
runner.check(bundle == nil)

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 5 - timeout bundle', store, src))
payload = string.format('HELLO WORLD %d', 1)
bplib.sleep(timeout+1)

-- load timedout bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {}), "flags set on load")
runner.check(bp.find_payload(bundle, payload), string.format('Error - wrong payload when checking for %s', payload))

-- Clean Up --

sender:flush()
sender:close()

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


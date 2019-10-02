local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Setup --

-- runner.set_exit_on_error(true)

local store = arg[1] or "MSGQ"
if store == "FILE" then
    os.execute("rm -Rf .pfile")
    os.execute("mkdir -p .pfile")
end

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 16384
local timeout = 10

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

local exp_stats = {   lost = 0,
                      expired = 0,
                      acknowledged = 0,
                      transmitted = 0,
                      retransmitted = 0,
                      received = 0,
                      delivered = 0,
                      bundles = 0,
                      payloads = 0,
                      records = 0,
                      active = 0    }

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - store/load bundles', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload -- 
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on store")
    rc, stats = sender:stats()
    runner.check(rc)
    exp_stats["bundles"] = exp_stats["bundles"] + 1
    runner.check(bp.check_stats(stats, exp_stats))

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}), "flags set on load")
    rc, stats = sender:stats()
    runner.check(rc)
    exp_stats["active"] = exp_stats["active"] + 1
    exp_stats["transmitted"] = exp_stats["transmitted"] + 1
    runner.check(bp.check_stats(stats, exp_stats))
    
    -- check payload --
    runner.check(bp.find_payload(bundle, payload), string.format('Error - could not find payload: %s', payload))
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - timeout bundles', store, src))
bplib.sleep(timeout + 1)
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}))
    rc, stats = sender:stats()
    runner.check(rc)
    exp_stats["transmitted"] = exp_stats["transmitted"] + 1
    exp_stats["retransmitted"] = exp_stats["retransmitted"] + 1
    runner.check(bp.check_stats(stats, exp_stats))
    
    -- check payload --
    runner.check(bp.find_payload(bundle, payload), string.format('Error - could not find payload: %s', payload))
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 3 - wrap block', store, src))

-- load new bundle, no room --
rc, bundle, flags = sender:load(1000)
runner.check(rc == false)
runner.check(bundle == nil)
runner.check(bp.check_flags(flags, {"activetablewrap"}))
rc, stats = sender:stats()
runner.check(rc)
runner.check(bp.check_stats(stats, exp_stats))

-- Clean Up --

sender:flush()
sender:close()

if store == "FILE" then
    os.execute("rm -Rf .pfile")
end

-- Report Results --

runner.report()


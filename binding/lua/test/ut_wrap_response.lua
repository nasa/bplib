local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Setup --

runner.set_exit_on_error(true)

local store = arg[1] or "RAM"
if store == "FILE" then
    os.execute("rm -Rf .pfile")
    os.execute("mkdir -p .pfile")
end

local retx_order = arg[2] or bp.RETX_SMALLEST_CID

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 16384
local timeout = 10
local attributes = {retransmit_order=retx_order}

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store, attributes)

local exp_stats = {   lost = 0,
                      expired = 0,
                      unrecognized = 0,
                      transmitted_bundles = 0,
                      transmitted_dacs = 0,
                      retransmitted_bundles = 0,
                      delivered_payloads = 0,
                      received_bundles = 0,
                      forwarded_bundles = 0,
                      received_dacs = 0,
                      stored_bundles = 0,
                      stored_payloads = 0,
                      stored_dacs = 0,
                      acknowledged_bundles = 0,
                      active_bundles = 0    }

runner.check(sender:setopt("TIMEOUT", timeout))

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
    exp_stats["stored_bundles"] = exp_stats["stored_bundles"] + 1
    runner.check(bp.check_stats(stats, exp_stats))

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}), "flags set on load")
    rc, stats = sender:stats()
    runner.check(rc)
    exp_stats["active_bundles"] = exp_stats["active_bundles"] + 1
    exp_stats["transmitted_bundles"] = exp_stats["transmitted_bundles"] + 1
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
    exp_stats["retransmitted_bundles"] = exp_stats["retransmitted_bundles"] + 1
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


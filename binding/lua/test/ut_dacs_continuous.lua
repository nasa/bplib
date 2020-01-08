local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local rd = runner.rootdir(arg[0])
local src = runner.srcscript()

-- Setup --

-- runner.set_exit_on_error(true)

local store = arg[1] or "RAM"
if store == "FILE" then
    os.execute("rm -Rf .pfile")
    os.execute("mkdir -p .pfile")
end

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 512
local timeout = 10

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)
local receiver = bplib.open(dst_node, dst_serv, src_node, src_serv, store)

rc = receiver:setopt("DACS_RATE", timeout)
runner.check(rc)

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - continuous sequence', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload -- 
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on store")

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil)
    runner.check(bp.check_flags(flags, {}), "flags set on load")

    -- process bundle -- 
    rc, flags = receiver:process(bundle, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "flags set on process")
end

do return end




-- check stats -- 
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_payloads=num_bundles}))

for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- accept bundle -- 
    rc, app_payload, flags = receiver:accept(1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}))
    runner.check(bp.match_payload(app_payload, payload), string.format('Error - payload %s did not match: %s', app_payload, payload))
end

-- check stats -- 
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {transmitted_bundles=num_bundles, stored_bundles=num_bundles, active_bundles=num_bundles}))
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_payloads=0, received_bundles=num_bundles, delivered_payloads=num_bundles}))

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

-- timeout bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc == false)
runner.check(bundle == nil)
runner.check(bp.check_flags(flags, {}))

-- check stats -- 
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {transmitted_bundles=num_bundles, stored_bundles=0, active_bundles=0, acknowledged_bundles=512, received_dacs=1}))
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_payloads=0, received_bundles=num_bundles, delivered_payloads=num_bundles, transmitted_dacs=1}))

-- Clean Up --

sender:close()
receiver:close()

if store == "FILE" then
    os.execute("rm -Rf .pfile")
end

-- Report Results --

runner.report()


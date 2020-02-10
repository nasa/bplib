local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local rd = runner.rootdir(arg[0])
local src = runner.srcscript()

-- Setup --

local store = arg[1] or "RAM"
runner.setup(bplib, store)

local cidreuse = (arg[2] == "CID_REUSE") or false

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 512
local timeout = 5

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)
local receiver = bplib.open(dst_node, dst_serv, src_node, src_serv, store)

runner.check(sender:setopt("TIMEOUT", timeout))
runner.check(sender:setopt("CID_REUSE", cidreuse))
runner.check(receiver:setopt("DACS_RATE", timeout))

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - skip every other one', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- store payload --
    rc, flags = sender:store(payload, 1000)
    runner.check(rc)
    runner.check(bp.check_flags(flags, {}), "Flags set on sender store")

    -- load bundle --
    rc, bundle, flags = sender:load(1000)
    runner.check(rc)
    runner.check(bundle ~= nil, string.format('Sender failed to load bundle %s', payload))

    -- process bundle --
    if i % 2 == 0 then
        rc, flags = receiver:process(bundle, 1000)
        runner.check(rc)
    end
end

-- check stats --
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_payloads=(num_bundles/2)}))

for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- accept bundle --
    if i % 2 == 0 then
        rc, app_payload, flags = receiver:accept(1000)
        runner.check(rc)
        runner.check(bp.check_flags(flags, {}), "Flags set on receiver accept")
        runner.check(bp.match_payload(app_payload, payload), string.format('Error - payload %s did not match: %s', app_payload, payload))
    end
end

-- check stats --
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {transmitted_bundles=num_bundles, stored_bundles=num_bundles, active_bundles=num_bundles}))
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {received_bundles=(num_bundles/2), delivered_payloads=(num_bundles/2)}))

-- load DACS --
local drain = 0
while true do
    rc, bundle, flags = receiver:load(1000)
    if rc then
        runner.check(bundle ~= nil, string.format('Receiver failed to load bundle %s', payload))
        runner.check(bp.check_flags(flags, {"routeneeded"}), "Unexpected flags set on receiver load")

        -- process DACS --
        rc, flags = sender:process(bundle, 1000)
        runner.check(rc)
        runner.check(bp.check_flags(flags, {}), "Flags set on sender process")
    elseif drain < 1 then
        bplib.sleep(timeout + 1)
        drain = drain + 1
    else
        break
    end
end

-- check stats --
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_dacs=0, transmitted_dacs=8}))

-- reload timed out bundles --
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    if i % 2 == 1 then
        -- load bundle --
        rc, bundle, flags = sender:load(1000)
        runner.check(rc)
        runner.check(bundle ~= nil, string.format('Sender failed to load bundle %s', payload))
        runner.check(bp.check_flags(flags, {}), "Flags set on sender load")

        -- process bundle --
        rc, flags = receiver:process(bundle, 1000)
        runner.check(rc)
    end
end

for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)

    -- accept bundle --
    if i % 2 == 1 then
        rc, app_payload, flags = receiver:accept(1000)
        runner.check(rc)
        runner.check(bp.check_flags(flags, {}), "Flags set on receiver accept")
        runner.check(bp.match_payload(app_payload, payload), string.format('Error - payload %s did not match: %s', app_payload, payload))
    end
end

-- load DACS --
local drain = 0
while true do
    rc, bundle, flags = receiver:load(1000)
    if rc then
        runner.check(bundle ~= nil, string.format('Receiver failed to load bundle %s', payload))
        runner.check(bp.check_flags(flags, {"routeneeded"}), "Unexpected flags set on receiver load")

        -- process DACS --
        rc, flags = sender:process(bundle, 1000)
        runner.check(rc)
        runner.check(bp.check_flags(flags, {}), "Flags set on sender process")
    elseif drain < 1 then
        bplib.sleep(timeout + 1)
        drain = drain + 1
    else
        break
    end
end

-- timeout bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc == false)
runner.check(bundle == nil)
runner.check(bp.check_flags(flags, {}), "Flags set on sender load, when timeout expected")

-- check stats --
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {transmitted_bundles=num_bundles, retransmitted_bundles=(num_bundles * 0.5), stored_bundles=0, active_bundles=0, acknowledged_bundles=512}))
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {stored_payloads=0, received_bundles=num_bundles, delivered_payloads=num_bundles}))

-- Clean Up --

sender:close()
receiver:close()

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


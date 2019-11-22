local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local rd = runner.rootdir(arg[0])
local src = runner.srcscript()

-- Setup --

local store = arg[1] or "RAM"
if store == "FILE" then
    os.execute("rm -Rf .pfile")
    os.execute("mkdir -p .pfile")
end

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local timeout = 2
local lifetime = 5

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)
local receiver = bplib.open(dst_node, dst_serv, src_node, src_serv, store)

runner.check(sender:setopt("TIMEOUT", timeout))
runner.check(sender:setopt("LIFETIME", lifetime))
runner.check(receiver:setopt("DACS_RATE", timeout/2))

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - load', store, src))
payload = string.format('TEST 1')

-- store payload -- 
rc, flags = sender:store(payload, 1000)
runner.check(rc)
runner.check(bp.check_flags(flags, {}))

-- load bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {}))

-- timeout bundle --
bplib.sleep(timeout)
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {}))

-- check stats -- 
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {expired=0}))

-- attempt load of expired bundle --
bplib.sleep(lifetime)
rc, bundle, flags = sender:load(1000)
runner.check(rc == false)
runner.check(bundle == nil, "Sender incorrectly returned expired bundle")
runner.check(bp.check_flags(flags, {}))

-- check stats -- 
rc, stats = sender:stats()
runner.check(bp.check_stats(stats, {expired=1}))

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - process', store, src))
payload = string.format('TEST 2')

-- store payload -- 
rc, flags = sender:store(payload, 1000)
runner.check(rc)
runner.check(bp.check_flags(flags, {}))

-- load bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {}))

-- attempt process of expired bundle -- 
bplib.sleep(lifetime)
rc, flags = receiver:process(bundle, 1000)
runner.check(rc == false)
runner.check(bp.check_flags(flags, {}))

-- check stats -- 
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {expired=1}))

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 3 - accept', store, src))
payload = string.format('TEST 3')

-- store payload -- 
rc, flags = sender:store(payload, 1000)
runner.check(rc)
runner.check(bp.check_flags(flags, {}))

-- load bundle --
rc, bundle, flags = sender:load(1000)
runner.check(rc)
runner.check(bundle ~= nil)
runner.check(bp.check_flags(flags, {}))

-- process bundle -- 
rc, flags = receiver:process(bundle, 1000)
runner.check(rc)
runner.check(bp.check_flags(flags, {}))

-- attempt accept of expired bundle -- 
bplib.sleep(lifetime)
rc, app_payload, flags = receiver:accept(1000)
runner.check(rc == false)
runner.check(app_payload == nil)
runner.check(bp.check_flags(flags, {}))

-- check stats -- 
rc, stats = receiver:stats()
runner.check(bp.check_stats(stats, {expired=2}))


-- Clean Up --

sender:close()
receiver:close()

if store == "FILE" then
    os.execute("rm -Rf .pfile")
end

-- Report Results --

runner.report()


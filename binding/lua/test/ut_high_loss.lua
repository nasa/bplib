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

local num_bundles = arg[2] or 256

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local timeout = 2
local cidreuse = false

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store)
local receiver = bplib.open(dst_node, dst_serv, src_node, src_serv, store)

runner.check(sender:setopt("TIMEOUT", timeout))
runner.check(sender:setopt("CID_REUSE", cidreuse))
runner.check(receiver:setopt("DACS_RATE", timeout/2))

local stored_bundles = 0
local stored_payloads = 0

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - higher input rate', store, src))
for k=1,num_bundles do
	for i=1,k do
		payload = string.format('HELLO WORLD %d.%d', k, i)

		-- store payload -- 
		rc, flags = sender:store(payload, 1000)
		runner.check(rc)
		runner.check(bp.check_flags(flags, {}))
		stored_bundles = stored_bundles + 1
	end

	for j=1,k/2 do
		-- load bundle --
		rc, bundle, flags = sender:load(1000)
		runner.check(rc)
		runner.check(bundle ~= nil)
		runner.check(bp.check_flags(flags, {}))

		-- process bundle -- 
		rc, flags = receiver:process(bundle, 1000)
		runner.check(rc)
		runner.check(bp.check_flags(flags, {}))
		stored_payloads = stored_payloads + 1
	end
	
	if k % 64 == 0 then
        bplib.sleep(timeout)
	end

	-- load DACS --
	rc, bundle, flags = receiver:load(0)
	if rc then
		runner.check(rc)
		runner.check(bundle ~= nil)
		runner.check(bp.check_flags(flags, {"routeneeded"}))

		-- process DACS --
		rc, flags = sender:process(bundle, 1000)
		runner.check(rc, string.format('Error - failed to process DACS: %s', rc))
		runner.check(bp.check_flags(flags, {}))
	end
end

-- check stats -- 
rc, rx_stats = receiver:stats()
rc, tx_stats = sender:stats()
acks = tx_stats["acknowledged_bundles"]
runner.check(bp.check_stats(rx_stats, {stored_payloads=stored_payloads}))
runner.check(bp.check_stats(tx_stats, {stored_bundles=stored_bundles - acks}))

-- drain remaining bundles --
runner.check(sender:setopt("TIMEOUT", 5))
runner.check(sender:setopt("CID_REUSE", true))
num_loops = 0
prev_bundles = 0
new_bundles_sent = 0
repeat

	-- load bundle --
	rc, bundle, flags = sender:load(1000)
	if rc then
		runner.check(bundle ~= nil)
		new_bundles_sent = new_bundles_sent + 1

		-- process bundle -- 
		rc, flags = receiver:process(bundle, 0)
		runner.check(rc)
		runner.check(bp.check_flags(flags, {}))
	end

	-- load DACS --
	rc, bundle, flags = receiver:load(0)
	if rc then
		runner.check(bundle ~= nil)
		runner.check(bp.check_flags(flags, {"routeneeded"}))

		-- process DACS --
		rc, flags = sender:process(bundle, 0)
		runner.check(rc, string.format('Error - failed to process DACS: %s', rc))
		runner.check(bp.check_flags(flags, {}))
	end

	-- print loop stats --
	num_loops = num_loops + 1
	rc, tx_stats = sender:stats()
	if prev_bundles ~= tx_stats["stored_bundles"] then
		print("Loop " .. num_loops .. ": " .. tx_stats["stored_bundles"] .. "/" .. new_bundles_sent)
	end
	prev_bundles = tx_stats["stored_bundles"]

until (tx_stats["stored_bundles"] == 0)

-- check stats -- 
rc, rx_stats = receiver:stats()
rc, tx_stats = sender:stats()
acks = tx_stats["acknowledged_bundles"]
stored_payloads = tx_stats["transmitted_bundles"]
runner.check(bp.check_stats(rx_stats, {stored_payloads=stored_payloads}))
runner.check(bp.check_stats(tx_stats, {stored_bundles=stored_bundles - acks}))

print("Stored: " .. stored_bundles .. ", Acknowledged: " .. acks)

-- Clean Up --
sender:close()
receiver:close()

if store == "FILE" then
    os.execute("rm -Rf .pfile")
end

-- Report Results --

runner.report()


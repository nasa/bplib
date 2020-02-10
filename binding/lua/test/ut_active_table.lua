local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Setup --

local store = arg[1] or "RAM"
runner.setup(bplib, store)

local retx_order = arg[2] or "SMALLEST"
if retx_order == "SMALLEST" then
	retx_order = bp.RETX_SMALLEST_CID
else
	retx_order = bp.RETX_OLDEST_BUNDLE
end

local src_node = 4
local src_serv = 3
local dst_node = 72
local dst_serv = 43

local num_bundles = 8
local timeout = 1
local cid_reuse = (retx_order == bp.RETX_SMALLEST_CID) and 0 or 1
local attributes = {cid_reuse=cid_reuse, retransmit_order=retx_order, active_table_size=num_bundles, timeout=timeout}

local sender = bplib.open(src_node, src_serv, dst_node, dst_serv, store, attributes)

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - walk active bundle', store, src))
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)
	print("Generating... " .. payload)

	-- store payload --
	rc, flags = sender:store(payload, 1000)
	runner.check(rc)
	runner.check(bp.check_flags(flags, {}), "flags set on store")

	-- load bundle --
	rc, bundle, flags = sender:load(1000)
	runner.check(rc)
	runner.check(bundle ~= nil)
	runner.check(bp.check_flags(flags, {}), "flags set on load")

	-- check payload --
	runner.check(bp.find_payload(bundle, payload), string.format('Error - could not find payload: %s', payload))

	-- Timed-Out Load
	bplib.sleep(timeout + 1)
	for j=1,i do
		payload = string.format('HELLO WORLD %d', j)

		-- load bundle --
		rc, bundle, flags = sender:load(1000)
		runner.check(rc)
		runner.check(bundle ~= nil, string.format('Error - failed to load payload: %s', payload))
		runner.check(bp.check_flags(flags, {}), "flags set on load")

		-- check payload --
		if bundle then
			runner.check(bp.find_payload(bundle, payload), string.format('Error - could not find payload: %s', payload))
		end
	end
end

-- Clean Up --

sender:flush()
sender:close()

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


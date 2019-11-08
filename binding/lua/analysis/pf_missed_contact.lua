local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Parameters --

local retx_order 		= arg[1] or bp.RETX_SMALLEST_CID
local bundle_tx_rate 	= arg[2] or 16 -- bundles/second
local timeout 			= arg[3] or 20 -- seconds
local dacs_rate 		= arg[4] or 10 -- seconds per dacs
local active_table  	= arg[5] or 200 -- bundles
local contact_time  	= arg[6] or 420 -- seconds (7 minutes)
local bundles_per_orbit	= arg[7] or 5300 -- bundles
local store 			= arg[8] or "RAM"
local perf_filename 	= arg[9] or 'perf.csv'

-- Write Meta File --

local meta = io.open(perf_filename .. '.meta', 'w')
meta:write('Store: ' .. store .. '\n')
meta:write('Retx Order: ' .. retx_order .. '\n')
meta:write('Bundle Rate: ' .. bundle_tx_rate .. '\n')
meta:write('Timeout: ' .. timeout .. '\n')
meta:write('DACS Rate: ' .. dacs_rate .. '\n')
meta:write('Active Table: ' .. active_table .. '\n')
meta:write('Contact Time: ' .. contact_time .. '\n')
meta:write('Bundles per Orbit: ' .. bundles_per_orbit .. '\n')

-- Create Sender --

if retx_order == "SMALLEST_CID" then retx_order = bp.RETX_SMALLEST_CID 
elseif retx_order == "OLDEST_BUNDLE" then retx_order = bp.RETX_OLDEST_BUNDLE end
local attributes = {retransmit_order=retx_order, timeout=timeout, active_table_size=active_table}
local sender = bplib.open(4, 3, 72, 43, store, attributes)

-- Create Receiver --

local attributes = {dacs_rate=dacs_rate}
local receiver = bplib.open(72, 43, 4, 3, store, attributes)

-- Handle File Storage Service --

if store == "FILE" then
    os.execute("rm -Rf .pfile")
    os.execute("mkdir -p .pfile")
end

-- Start Performance File --

local perf = io.open(perf_filename, 'w')
perf:write('time,bidir,new,retran,ack,bundles,bid\n')


-- Utility Functions --

bundle_id = 0

local function simulate_back_orbit() 
	for i=1,bundles_per_orbit do
		payload = string.format('%d', bundle_id)
		rc, flags = sender:store(payload, 0)
		bundle_id = bundle_id + 1
	end
end

local function simulate_contact(bidirectional)
	now = os.time()
	for i=1,contact_time do
		if bidirectional then
            smallest_bundle_id = bundle_id
        else
            smallest_bundle_id = 0
        end
		for j=1,bundle_tx_rate do
			rc, bundle, flags = sender:load(0)
			if bidirectional then
				rc, flags = receiver:process(bundle, 0)
				rc, payload, flags = receiver:accept(0)
				payload_bundle_id = tonumber(payload)
				if payload_bundle_id < smallest_bundle_id then 
					smallest_bundle_id = payload_bundle_id
				end
			end
		end

		-- check for and process dacs --
		if bidirectional then
			rc, bundle, flags = receiver:load(0)
			if rc then rc, flags = sender:process(bundle, 0) end
		end

		-- collect stats --
		rc, stats = sender:stats()
		perf:write(string.format('%d,%d,%d,%d,%d,%d,%d\n', os.time() - start, bidirectional and 1 or 0, stats["transmitted"] - stats["retransmitted"], stats["retransmitted"], stats["acknowledged"], stats["bundles"], smallest_bundle_id))
		perf:flush()

		-- synchronize time to 1 execution per second --
		time_adjust = (now + i) - os.time()
		if time_adjust > 0 then bplib.sleep(time_adjust)
	 	elseif time_adjust < 0 then print(string.format('Simulation slower than real-time by %d seconds', time_adjust)) end
	end
end

-- Performance Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 1 - store first orbit of bundles (orbit 1)', store, src))
simulate_back_orbit()
start = os.time()

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 2 - send bundles with no acknowledgments (missed contact 1)', store, src))
simulate_contact(false)

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 3 - store second orbit of bundles (orbit 2)', store, src))
simulate_back_orbit()

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 4 - send bundles with acknowledgments (contact 2)', store, src))
simulate_contact(true)

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 5 - store third orbit of bundles (orbit 3)', store, src))
simulate_back_orbit()

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 6 - send bundles with acknowledgments (contact 3)', store, src))
simulate_contact(true)

-- Clean Up --

sender:flush()
sender:close()

receiver:close()

meta:close()
perf:close()

if store == "FILE" then
    os.execute("rm -Rf .pfile")
end

-- Report Results --

runner.report()


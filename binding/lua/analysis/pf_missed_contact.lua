local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()

-- Parameters --

local store 		= arg[1] or "RAM"
local retx_order 	= arg[2] or bp.RETX_SMALLEST_CID
local bundle_rate 	= arg[3] or 16 -- bundles/second
local timeout 		= arg[4] or 20 -- seconds
local dacs_rate 	= arg[5] or 10 -- seconds per dacs
local active_table  = arg[6] or 200 -- bundles
local contact_time  = arg[7] or 300 -- seconds
local perf_filename = arg[8] or 'perf.csv'

-- Write Meta File --

local meta = io.open(perf_filename .. '.meta', 'w')
meta:write('Store: ' .. store .. '\n')
meta:write('Retx Order: ' .. retx_order .. '\n')
meta:write('Bundle Rate: ' .. bundle_rate .. '\n')
meta:write('Timeout: ' .. timeout .. '\n')
meta:write('DACS Rate: ' .. dacs_rate .. '\n')
meta:write('Active Table: ' .. active_table .. '\n')
meta:write('Contact Time: ' .. contact_time .. '\n')

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
perf:write('time,bidir,new,retran,ack\n')

-- Performance Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 1 - store bundles', store, src))
local num_bundles = contact_time * bundle_rate * 2
for i=1,num_bundles do
    payload = string.format('HELLO WORLD %d', i)
    rc, flags = sender:store(payload, 0)
end

rc, stats = sender:stats()
meta:write('Number of Bundles: ' .. stats["bundles"] .. '\n')

start = os.time()

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 2 - send bundles with no acknowledgments (missed contact)', store, src))
now = os.time()
for i=1,contact_time do
	for j=1,bundle_rate do
	    rc, bundle, flags = sender:load(0)
	end
	
	-- collect stats --
    rc, stats = sender:stats()
	perf:write(string.format('%d,0,%d,%d,%d\n', os.time() - start, stats["transmitted"] - stats["retransmitted"], stats["retransmitted"], stats["acknowledged"]))
	perf:flush()

	-- synchronize time to 1 execution per second --
	time_adjust = (now + i) - os.time()
	if time_adjust > 0 then bplib.sleep(time_adjust)
 	elseif time_adjust < 0 then print(string.format('Simulation (step 2) slower than real-time by %d seconds', time_adjust)) end
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Step 3 - resend bundles with acknowledgments', store, src))
now = os.time()
for i=1,contact_time do
	for j=1,bundle_rate do
	    rc, bundle, flags = sender:load(0)
		rc, flags = receiver:process(bundle, 0)
	end

	-- check for and process dacs --
    rc, bundle, flags = receiver:load(0)
	if rc then rc, flags = sender:process(bundle, 0) end

	-- collect stats --
    rc, stats = sender:stats()
	perf:write(string.format('%d,1,%d,%d,%d\n', os.time() - start, stats["transmitted"] - stats["retransmitted"], stats["retransmitted"], stats["acknowledged"]))
	perf:flush()

	-- synchronize time to 1 execution per second --
	time_adjust = (now + i) - os.time()
	if time_adjust > 0 then bplib.sleep(time_adjust)
 	elseif time_adjust < 0 then print(string.format('Simulation (step 3) slower than real-time by %d seconds', time_adjust)) end
end

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


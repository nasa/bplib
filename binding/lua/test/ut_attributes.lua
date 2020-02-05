local bplib = require("bplib")
local runner = require("bptest")
local bp = require("bp")
local src = runner.srcscript()
local rd = runner.rootdir(arg[0])

-- Setup --

store = arg[1] or "RAM"

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - lifetime', store, src))
lifetime = 100
attributes = {lifetime=lifetime}
ch = bplib.open(4, 3, 72, 43, store, attributes)
runner.check(ch ~= nil)
if ch then
	rc, d = ch:getopt("LIFETIME")
    runner.check(rc == true)
    runner.check(d == lifetime, string.format('Failed to set lifetime to %d, value was %d instead\n', lifetime, d))
	ch:close()
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - active table size', store, src))
active_table_size = 10
attributes = {active_table_size=active_table_size}
ch = bplib.open(4, 3, 72, 43, store, attributes)
runner.check(ch ~= nil)
if ch then
	-- fill table --
	for i=1,active_table_size do
		payload = string.format('HELLO WORLD %d', i)

		-- store payload --
		rc, flags = ch:store(payload, 1000)
		runner.check(rc)
		runner.check(bp.check_flags(flags, {}), "flags set on store")

		-- load bundle --
		rc, bundle, flags = ch:load(1000)
		runner.check(rc)
		runner.check(bundle ~= nil)
		runner.check(bp.check_flags(flags, {}), "flags set on load")
	end

	-- full table --
	rc, bundle, flags = ch:load(1000)
	runner.check(rc == false)
	runner.check(bundle == nil)
	runner.check(bp.check_flags(flags, {"activetablewrap"}))

	-- close channel --
	ch:close()
end

-- Clean Up --

-- Report Results --

runner.report(bplib)


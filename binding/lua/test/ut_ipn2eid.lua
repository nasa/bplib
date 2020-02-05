local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()

-- Setup --

runner.setup(bplib, "RAM")

-- Test --

-----------------------------------------------------------------------
print(string.format('%s: Test 1 - nominal', src))
rc, eid = bplib.ipn2eid(4, 3)
runner.check('rc == true')
runner.check('eid == "ipn:4.3"')

-----------------------------------------------------------------------
print(string.format('%s: Test 2 - nominal', src))
rc, eid = bplib.ipn2eid(72, 43)
runner.check('rc == true')
runner.check('eid == "ipn:72.43"')

-----------------------------------------------------------------------
print(string.format('%s: Test 3 - bad node parameter', src))
rc, eid = bplib.ipn2eid("seventy two", 43)
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 4 - bad service parameter', src))
rc, eid = bplib.ipn2eid(72, "forty three")
runner.check('rc == false')

-- Clean Up --

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


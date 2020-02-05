local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()

-- Setup --

runner.setup(bplib, "RAM")

-- Test --

-----------------------------------------------------------------------
print(string.format('%s: Test 1 - nominal', src))
rc, node, serv = bplib.eid2ipn("ipn:4.3")
runner.check('rc == true')
runner.check('node == 4')
runner.check('serv == 3')

-----------------------------------------------------------------------
print(string.format('%s: Test 2 - nominal', src))
rc, node, serv = bplib.eid2ipn("ipn:72.43")
runner.check('rc == true')
runner.check('node == 72')
runner.check('serv == 43')

-----------------------------------------------------------------------
print(string.format('%s: Test 3 - bad schema', src))
rc, node, serv = bplib.eid2ipn("eid:72.43")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 4 - bad node', src))
rc, node, serv = bplib.eid2ipn("ipn:nasa.43")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 5 - bad service', src))
rc, node, serv = bplib.eid2ipn("ipn:72.nasa")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 6 - no schema', src))
rc, node, serv = bplib.eid2ipn("2343.343")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 7 - too short', src))
rc, node, serv = bplib.eid2ipn("ipn:23")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s: Test 8 - too long', src))
eid = "ipn:"
for i=1,64,1 do eid = eid .. "123" end
rc, node, serv = bplib.eid2ipn(eid)
runner.check('rc == false')

-- Clean Up --

runner.cleanup(bplib, store)

-- Report Results --

runner.report(bplib)


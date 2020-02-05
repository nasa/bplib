local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()

-- Setup --

local test = arg[1] or "ALL"
-- Test --

-----------------------------------------------------------------------
print(string.format('\n%s: Test 1 - unittest(%s)', src, test))
local numfails = bplib.unittest(test)
runner.check(numfails == 0, string.format('Failures detected by bplib unittest: %d', numfails))

-- Clean Up --

-- Report Results --

runner.report(bplib)


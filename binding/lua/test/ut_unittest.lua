local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()

-- Setup --

-- Test --

-----------------------------------------------------------------------
print(string.format('%s: Test 1 - unittest', src))
local numfails = bplib.unittest()
runner.check(numfails == 0, string.format('Failures detected by bplib unittest: %d', numfails))

-- Clean Up --

-- Report Results --

runner.report()


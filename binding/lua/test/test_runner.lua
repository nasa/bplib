local bplib = require("bplib")
local runner = require("bptest")
local rd = runner.rootdir(arg[0])

--runner.set_exit_on_error(true)

-- Run Unit Tests --

runner.script(rd .. "ut_open_close.lua", {"RAM"})
runner.script(rd .. "ut_open_close.lua", {"FILE"})
runner.script(rd .. "ut_open_close.lua", {"FLASH"})
runner.script(rd .. "ut_attributes.lua")
runner.script(rd .. "ut_getset_opt.lua")
runner.script(rd .. "ut_eid2ipn.lua")
runner.script(rd .. "ut_ipn2eid.lua")
runner.script(rd .. "ut_route.lua")
runner.script(rd .. "ut_expiration.lua")
runner.script(rd .. "ut_expiration.lua", {"FLASH"})
runner.script(rd .. "ut_active_table.lua", {"RAM", "SMALLEST"})
runner.script(rd .. "ut_active_table.lua", {"RAM", "OLDEST"})
runner.script(rd .. "ut_active_table.lua", {"FLASH", "SMALLEST"})
runner.script(rd .. "ut_active_table.lua", {"FLASH", "OLDEST"})
runner.script(rd .. "ut_bundle_timeout.lua", {"RAM"})
runner.script(rd .. "ut_bundle_timeout.lua", {"FILE"})
runner.script(rd .. "ut_bundle_timeout.lua", {"FLASH"})
runner.script(rd .. "ut_wrap_response.lua", {"RAM"})
runner.script(rd .. "ut_wrap_response.lua", {"FILE"})
runner.script(rd .. "ut_wrap_response.lua", {"FLASH"})
runner.script(rd .. "ut_dacs_continuous.lua", {"RAM"})
runner.script(rd .. "ut_dacs_continuous.lua", {"FILE"})
runner.script(rd .. "ut_dacs_continuous.lua", {"FLASH"})
runner.script(rd .. "ut_dacs_skip.lua", {"RAM"})
runner.script(rd .. "ut_dacs_skip.lua", {"FILE"})
runner.script(rd .. "ut_dacs_skip.lua", {"FLASH"})
runner.script(rd .. "ut_high_loss.lua", {"RAM"})
runner.script(rd .. "ut_high_loss.lua", {"FILE"})
runner.script(rd .. "ut_high_loss.lua", {"FLASH", 100})
runner.script(rd .. "ut_unittest.lua")

-- Check for Memory Leaks --

currmem, highmem = bplib.memstat()
runner.check(currmem == 0, string.format('Memory leak detected: %d!', currmem))

-- Report Results --

runner.report(bplib)


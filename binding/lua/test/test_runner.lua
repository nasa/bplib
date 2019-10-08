local runner = require("bptest")
local rd = runner.rootdir(arg[0])

-- Run Unit Tests --

runner.script(rd .. "ut_open_close.lua", {"RAM"})
runner.script(rd .. "ut_open_close.lua", {"FILE"})
runner.script(rd .. "ut_getset_opt.lua")
runner.script(rd .. "ut_eid2ipn.lua")
runner.script(rd .. "ut_ipn2eid.lua")
runner.script(rd .. "ut_route.lua")
runner.script(rd .. "ut_wrap_response.lua", {"RAM"})
runner.script(rd .. "ut_wrap_response.lua", {"FILE"})
runner.script(rd .. "ut_dacs_continuous.lua", {"RAM"})
runner.script(rd .. "ut_dacs_continuous.lua", {"FILE"})
runner.script(rd .. "ut_dacs_skip.lua", {"RAM"})
runner.script(rd .. "ut_dacs_skip.lua", {"FILE"})
runner.script(rd .. "ut_high_loss.lua", {"RAM"})
runner.script(rd .. "ut_high_loss.lua", {"FILE"})
runner.script(rd .. "ut_unittest.lua")

-- Report Results --

runner.report()


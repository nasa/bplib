local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()

-- Setup --

store = arg[1] or "RAM"
src_node = 4
src_serv = 3
dst_node = 72
dst_serv = 43
rpt_node = 0
rpt_serv = 0
cst_node = 4
cst_serv = 3
dflt_paytype = 1
dflt_timeout = 10
dflt_bundlelen = 4096
dflt_dacsrate = 5
ch = bplib.open(src_node, src_serv, dst_node, dst_serv, store)

local function check_option(opt, default, valid_new, invalid_new)
    default_val = default -- global for check
    valid_new_val = valid_new -- global for check

    rc, d = ch:getopt(opt)
    runner.check('rc == true')
    runner.check('d == default_val')

    rc = ch:setopt(opt, valid_new_val)
    runner.check('rc == true')

    rc, d = ch:getopt(opt)
    runner.check('rc == true')
    runner.check('d == valid_new_val')

    rc = ch:setopt(opt, invalid_new)
    runner.check('rc == false')

    rc, d = ch:getopt(opt)
    runner.check('rc == true')
    runner.check('d == valid_new_val')

    rc = ch:setopt(opt, default_val)
    runner.check('rc == true')

    rc, d = ch:getopt(opt)
    runner.check('rc == true')
    runner.check('d == default_val')
end

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - lifetime', store, src))
check_option("LIFETIME", 86400, 100, "bob")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - custody request', store, src))
check_option("REQUEST_CUSTODY", true, false, "postman")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 3 - administrative record', store, src))
check_option("ADMIN_RECORD", false, true, "dog")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 4 - integrity check', store, src))
check_option("INTEGRITY_CHECK", true, false, "mailman")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 5 - allow fragmentation', store, src))
check_option("ALLOW_FRAGMENTATION", false, true, "tightrope")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 6 - payload crc', store, src))
check_option("CIPHER_SUITE", dflt_paytype, 3, "billy")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 7 - timeout', store, src))
check_option("TIMEOUT", dflt_timeout, 50, "santa")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 8 - bundle length', store, src))
check_option("MAX_LENGTH", dflt_bundlelen, 5000, "elves")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 9 - custody id reuse', store, src))
check_option("CID_REUSE", false, true, "organ")

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 10 - acs rate', store, src))
check_option("DACS_RATE", dflt_dacsrate, 60, "tea")

-- Clean Up --

ch:close()

-- Report Results --

runner.report(bplib)


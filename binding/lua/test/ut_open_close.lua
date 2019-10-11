local bplib = require("bplib")
local runner = require("bptest")
local src = runner.srcscript()
local rd = runner.rootdir(arg[0])

-- Setup --

store = arg[1] or "RAM"
max_channels = 4
ch = {}

-- Test --

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 1 - nominal', store, src))
ch[1] = bplib.open(4, 3, 72, 43, store)
runner.check('ch[1]')
ch[1]:close()

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 2 - multiple channels', store, src))
for i=1,max_channels do
    ch[i] = bplib.open(4, i, 72, 43, store)
    eval = string.format('ch[%d]', i)
    runner.check(eval)
end
for i=1,max_channels do
    ch[i]:close()
end

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 3 - out of order channel create', store, src))
ch[1] = bplib.open(4, 1, 72, 43, store)
runner.check('ch[1]')
ch[2] = bplib.open(4, 2, 72, 43, store)
runner.check('ch[2]')
ch[3] = bplib.open(4, 3, 72, 43, store)
runner.check('ch[3]')
ch[4] = bplib.open(4, 4, 72, 43, store)
runner.check('ch[4]')
ch[2]:close()
ch[4]:close()
ch[2] = bplib.open(4, 2, 72, 43, store)
runner.check('ch[2]')
ch[4] = bplib.open(4, 4, 72, 43, store)
runner.check('ch[4]')
ch[1]:close()
ch[2]:close()
ch[3]:close()
ch[4]:close()

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 4 - closed channel access', store, src))
ch[1] = bplib.open(4, 1, 72, 43, store)
runner.check('ch[1]')
ch[1]:close()
rc, dstnode = ch[1]:getopt("TIMEOUT")
runner.check('rc == false')

-----------------------------------------------------------------------
print(string.format('%s/%s: Test 5 - invalid store', store, src))
ch[1] = bplib.open(4, 1, 72, 43, "PARSLEY")
runner.check('ch[1] == nil')

-- Clean Up --

-- Report Results --

runner.report()


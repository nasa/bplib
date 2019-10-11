local all_flags = {   "noncompliant", "incomplete", "unreliabletime", "filloverflow", 
                      "toomanyfills", "cidwentbackwards", "routeneeded", "storefailure", 
                      "sdnvoverflow", "sdnincomplete", "activetablewrap",
                      "duplicates", "rbtreefull"  }

--------------------------------------------------------------------------------------
-- check_flags  -
--
--  checks that all flags in setlist are set, and all flags not in setlist are not set
--------------------------------------------------------------------------------------
local function check_flags(flags, setlist)
    local status = true
    checklist = {}
    for k,v in pairs(setlist) do
        checklist[v] = true
        if flags[v] ~= true then
            print(string.format('Error - %s was not set', v))
            status = false
        end
    end
    for k,v in pairs(all_flags) do
        if checklist[v] == nil then
            if flags[v] == true then
                print(string.format('Error - %s was set', v))
                status = false
            end
        end
    end
    return status
end

--------------------------------------------------------------------------------------
-- check_stats  -
--
--  checks that stats table matches the exp(ected) table
--------------------------------------------------------------------------------------
local function check_stats(stats, exp)
    local status = true
    for k,v in pairs(exp) do
        if stats[k] ~= v then
            print(string.format('Error - %s was %d, expecting %d', k, stats[k], v))
            status = false
        end
    end
    return status
end

--------------------------------------------------------------------------------------
-- print_flags  -
--
--  pretty prints flags table
--------------------------------------------------------------------------------------
local function print_flags(flags)
    for k,v in pairs(flags) do
        print(string.format('%s = %s', k, v))
    end
end

--------------------------------------------------------------------------------------
-- print_stats  -
--
--  pretty prints flags table
--------------------------------------------------------------------------------------
local function print_stats(stats)
    for k,v in pairs(stats) do
        print(string.format('%s = %d', k, v))
    end
end

--------------------------------------------------------------------------------------
-- find_payload  -
--
--  finds the payload inside the bundle
--------------------------------------------------------------------------------------
local function find_payload(bundle, payload) 
    if bundle == nil or payload == nil then
        return false
    end
    bytes_bundle = {string.byte(bundle, 0, -1)}
    bytes_payload = {string.byte(payload, 0, -1)}
    for j=1,#bytes_bundle do
        for k=1,#bytes_payload do
            if bytes_bundle[j + k] ~= bytes_payload[k] then
                break
            end
        end
        return true
    end
    return false
end

--------------------------------------------------------------------------------------
-- find_payload  -
--
--  compares both payloads and verifies that they are the same
--------------------------------------------------------------------------------------
local function match_payload(payload1, payload2) 
    if payload1 == nil or payload2 == nil then
        return false
    end
    bytes1 = {string.byte(payload1, 0, -1)}
    bytes2 = {string.byte(payload2, 0, -1)}
    if #bytes1 ~= #bytes2 then
        return false
    end
    for i=1,#bytes1 do
        if bytes1[i] ~= bytes2[i] then
            return false
        end
    end
    return true
end

--------------------------------------------------------------------------------------
-- Return Local Package
--------------------------------------------------------------------------------------
local package = {
    check_flags = check_flags,
    check_stats = check_stats,
    print_flags = print_flags,
    print_stats = print_stats,
    find_payload = find_payload,
    match_payload = match_payload,
    BIB_NONE = 0,
    BIB_CRC16_X25 = 1,
    BIB_CRC32_CASTAGNOLI = 2,
    RETX_OLDEST_BUNDLE = 0,
    RETX_SMALLEST_CID = 1
}

return package


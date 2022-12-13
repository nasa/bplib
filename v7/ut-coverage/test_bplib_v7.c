/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "test_bplib_v7.h"
#include "v7.h"
#include "bplib_os.h"
#include "unistd.h"

static void UT_V7_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint64_t retval = 0;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void Test_v7_set_eid(void)
{
    /*
        Test v7_set_eid() function
    */
    bp_endpointid_buffer_t buff;
    bp_ipn_addr_t          bp_addr;
    bp_addr.node_number    = 11;
    bp_addr.service_number = 12;
    UtAssert_VOIDCALL(v7_set_eid(&buff, &bp_addr));
    UtAssert_UINT32_EQ(buff.scheme, bp_endpointid_scheme_ipn);
    UtAssert_UINT32_EQ(buff.ssp.ipn.node_number, 11);
    UtAssert_UINT32_EQ(buff.ssp.ipn.service_number, 12);
}

void Test_v7_get_eid(void)
{
    /*
        Test v7_get_eid() function, if case
    */
    bp_endpointid_buffer_t buff;
    buff.scheme                 = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 12;
    bp_ipn_addr_t bp_addr;
    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));
    /*
        Test v7_get_eid() function, else case
    */
    buff.scheme = 0;
    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));
    UtAssert_UINT32_EQ(bp_addr.node_number, 0);
    UtAssert_UINT32_EQ(bp_addr.service_number, 0);
}

void Test_v7_compare_ipn2eid(void)
{
    /*
        Test v7_compare_ipn2eid()
    */
    bp_endpointid_buffer_t buff;
    buff.scheme                 = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 12;
    bp_ipn_addr_t bp_addr;
    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));
    /*
        Return 0 case, ipn and eid compared the same
    */
    UtAssert_UINT32_EQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    /*
        Return not equal 0, scheme not the same
    */
    buff.scheme = 0;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    /*
        Return not equal 0, node_number not the same
    */
    buff.scheme              = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number = 1;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    /*
        Return not equal 0, service_number not the same
    */
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 2;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
}

void Test_v7_compare_ipn2ipn(void)
{
    /*
        Test v7_compare_ipn2ipn() function
    */
    bp_ipn_addr_t bp_addr1;
    bp_addr1.node_number    = 11;
    bp_addr1.service_number = 12;
    bp_ipn_addr_t bp_addr2;
    bp_addr2.node_number    = 11;
    bp_addr2.service_number = 12;
    /*
        Test bp_ipn1 and ipn2 are the same case
    */
    UtAssert_UINT32_EQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
    /*
        Test node_number not same case
    */
    bp_addr1.node_number = 1;
    UtAssert_UINT32_NEQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
    /*
        Test node_number not same case
    */
    bp_addr1.node_number    = 11;
    bp_addr1.service_number = 2;
    UtAssert_UINT32_NEQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
}

void Test_v7_get_current_time(void)
{
    UT_SetHandlerFunction(UT_KEY(bplib_os_get_dtntime_ms), UT_V7_GetTime_Handler, NULL);
    UtAssert_UINT32_EQ(v7_get_current_time(), 0);
}

void TestBplibV7_Register(void)
{
    UtTest_Add(Test_v7_set_eid, NULL, NULL, "BPLIB v7_set_eid");
    UtTest_Add(Test_v7_get_eid, NULL, NULL, "BPLIB v7_get_eid");
    UtTest_Add(Test_v7_compare_ipn2eid, NULL, NULL, "BPLIB v7_compare_ipn2eid");
    UtTest_Add(Test_v7_compare_ipn2ipn, NULL, NULL, "BPLIB v7_compare_ipn2ipn");
    UtTest_Add(Test_v7_get_current_time, NULL, NULL, "BPLIB v7_get_current_time");
}

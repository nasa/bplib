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
#include "test_bplib_v7.h"

void test_v7_init_ipn_eid(void)
{
    /* Test function for:
     * void v7_init_ipn_eid(bp_endpointid_buffer_t *eid, bp_ipn_t node, bp_ipn_t service)
     */
    bp_endpointid_buffer_t eid;
    bp_ipn_t               node    = 1;
    bp_ipn_t               service = 2;

    UtAssert_VOIDCALL(v7_init_ipn_eid(&eid, node, service));
}

void test_v7_set_eid(void)
{
    /* Test function for:
     * void v7_set_eid(bp_endpointid_buffer_t *eid, const bp_ipn_addr_t *bp_addr)
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

void test_v7_get_eid(void)
{
    /* Test function for:
     * void v7_get_eid(bp_ipn_addr_t *bp_addr, const bp_endpointid_buffer_t *eid)
     */
    bp_endpointid_buffer_t buff;
    bp_ipn_addr_t          bp_addr;

    buff.scheme                 = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 12;

    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));

    buff.scheme = 0;
    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));
    UtAssert_UINT32_EQ(bp_addr.node_number, 0);
    UtAssert_UINT32_EQ(bp_addr.service_number, 0);
}

void test_v7_compare_ipn2eid(void)
{
    /* Test function for:
     * int v7_compare_ipn2eid(const bp_ipn_addr_t *ipn, const bp_endpointid_buffer_t *eid)
     */
    bp_endpointid_buffer_t buff;
    bp_ipn_addr_t          bp_addr;

    buff.scheme                 = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 12;
    UtAssert_VOIDCALL(v7_get_eid(&bp_addr, &buff));
    UtAssert_UINT32_EQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    buff.scheme = 0;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    buff.scheme              = bp_endpointid_scheme_ipn;
    buff.ssp.ipn.node_number = 1;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
    buff.ssp.ipn.node_number    = 11;
    buff.ssp.ipn.service_number = 2;
    UtAssert_UINT32_NEQ(v7_compare_ipn2eid(&bp_addr, &buff), 0);
}

void test_v7_compare_ipn2ipn(void)
{
    /* Test function for:
     * int v7_compare_ipn2ipn(const bp_ipn_addr_t *ipn1, const bp_ipn_addr_t *ipn2)
     */
    bp_ipn_addr_t bp_addr1;
    bp_ipn_addr_t bp_addr2;

    bp_addr1.node_number    = 11;
    bp_addr1.service_number = 12;
    bp_addr2.node_number    = 11;
    bp_addr2.service_number = 12;
    UtAssert_UINT32_EQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
    bp_addr1.node_number = 1;
    UtAssert_UINT32_NEQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
    bp_addr1.node_number    = 11;
    bp_addr1.service_number = 2;
    UtAssert_UINT32_NEQ(v7_compare_ipn2ipn(&bp_addr1, &bp_addr2), 0);
}

void TestBpV7_Register(void)
{
    UtTest_Add(test_v7_init_ipn_eid, NULL, NULL, "Test v7_init_ipn_eid");
    UtTest_Add(test_v7_set_eid, NULL, NULL, "Test v7_set_eid");
    UtTest_Add(test_v7_get_eid, NULL, NULL, "Test v7_get_eid");
    UtTest_Add(test_v7_compare_ipn2eid, NULL, NULL, "Test v7_compare_ipn2eid");
    UtTest_Add(test_v7_compare_ipn2ipn, NULL, NULL, "Test v7_compare_ipn2ipn");
}

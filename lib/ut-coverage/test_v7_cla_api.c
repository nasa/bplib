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
#include "test_bplib_base.h"

void test_bplib_create_cla_intf(void)
{
    /* Test function for:
     * bp_handle_t bplib_create_cla_intf(bplib_routetbl_t *rtbl)
     */
    bplib_routetbl_t    rtbl;
    bplib_mpool_block_t sblk;
    bp_handle_t         self_intf_id;
    bplib_mpool_flow_t  flow;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&self_intf_id, 0, sizeof(bp_handle_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_GT(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_cla_ingress(void)
{
    /* Test function for:
     * int bplib_cla_ingress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t
     * timeout)
     */
    bplib_routetbl_t  rtbl;
    bp_handle_t       intf_id;
    void             *bundle  = NULL;
    size_t            size    = 100;
    uint64_t          timeout = 3000;
    bplib_mpool_ref_t flow_ref;
    bplib_cla_stats_t stats;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow_ref, 0, sizeof(bplib_mpool_ref_t));
    memset(&stats, 0, sizeof(bplib_cla_stats_t));

    UtAssert_UINT32_NEQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_get_parent_pool_from_link), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_alloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_UINT32_NEQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_UINT32_EQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    timeout = 0;
    UtAssert_UINT32_EQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_cla_egress(void)
{
    /* Test function for:
     * int bplib_cla_egress(bplib_routetbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
     */
    bplib_routetbl_t  rtbl;
    bp_handle_t       intf_id;
    void             *bundle  = NULL;
    size_t            size    = 100;
    uint64_t          timeout = 0;
    bplib_mpool_ref_t flow_ref;
    bplib_cla_stats_t stats;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow_ref, 0, sizeof(bplib_mpool_ref_t));
    memset(&stats, 0, sizeof(bplib_cla_stats_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_pull), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    timeout = 3000;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_UINT32_NEQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, &stats);
    UtAssert_UINT32_EQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_cla_event_impl(void)
{
    /* Test function for:
     * int bplib_cla_event_impl(void *arg, bplib_mpool_block_t *intf_block)
     */
    bplib_mpool_flow_generic_event_t arg;
    bplib_mpool_block_t              intf_block;
    bplib_mpool_flow_t               flow;
    UtAssert_UINT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    memset(&arg, 0, sizeof(bplib_mpool_flow_generic_event_t));
    memset(&intf_block, 0, sizeof(bplib_mpool_block_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    arg.event_type = bplib_mpool_flow_event_up;
    UtAssert_UINT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, &flow);
    arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UtAssert_UINT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    arg.event_type = bplib_mpool_flow_event_down;
    UtAssert_UINT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_generic_bundle_ingress(void)
{
    /* Test function for:
     * int bplib_generic_bundle_ingress(bplib_mpool_ref_t flow_ref, const void *content, size_t size, uint64_t
     * time_limit)
     */
    bplib_mpool_block_content_t  flow_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    bplib_mpool_flow_t           flow;
    bplib_mpool_block_t          pblk;
    bplib_mpool_ref_t            refptr;
    bplib_mpool_bblock_primary_t pri_block;

    memset(&flow_ref, 0, sizeof(bplib_mpool_block_content_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));
    memset(&pblk, 0, sizeof(bplib_mpool_block_t));
    memset(&refptr, 0, sizeof(bplib_mpool_ref_t));
    memset(&pri_block, 0, sizeof(bplib_mpool_bblock_primary_t));

    UtAssert_UINT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_push), UT_lib_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_alloc), UT_lib_AltHandler_PointerReturn, &pblk);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_make_block), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_UINT32_NEQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_alloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_make_block), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_generic_bundle_egress(void)
{
    /* Test function for:
     * int bplib_generic_bundle_egress(bplib_mpool_ref_t flow_ref, void *content, size_t *size, uint64_t time_limit)
     */
    bplib_mpool_block_content_t  flow_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    bplib_mpool_flow_t           flow;
    bplib_mpool_block_t          pblk;
    bplib_mpool_ref_t            refptr;
    bplib_mpool_bblock_primary_t pri_block;

    memset(&flow_ref, 0, sizeof(bplib_mpool_block_content_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));
    memset(&pblk, 0, sizeof(bplib_mpool_block_t));
    memset(&refptr, 0, sizeof(bplib_mpool_ref_t));
    memset(&pri_block, 0, sizeof(bplib_mpool_bblock_primary_t));

    UtAssert_UINT32_EQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_compute_full_bundle_size), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_out), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_NEQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_pull), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_UINT32_NEQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UtAssert_UINT32_EQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_pull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_lib_AltHandler_PointerReturn, NULL);
}

void TestBplibBase_ClaApi_Register(void)
{
    UtTest_Add(test_bplib_create_cla_intf, NULL, NULL, "Test bplib_create_cla_intf");
    UtTest_Add(test_bplib_cla_ingress, NULL, NULL, "Test bplib_cla_ingress");
    UtTest_Add(test_bplib_cla_egress, NULL, NULL, "Test bplib_cla_egress");
    UtTest_Add(test_bplib_cla_event_impl, NULL, NULL, "Test bplib_cla_event_impl");
    UtTest_Add(test_bplib_generic_bundle_ingress, NULL, NULL, "Test bplib_generic_bundle_ingress");
    UtTest_Add(test_bplib_generic_bundle_egress, NULL, NULL, "Test bplib_generic_bundle_egress");
}

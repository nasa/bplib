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
    BPLib_MEM_block_t sblk;
    bp_handle_t         self_intf_id;
    BPLib_MEM_flow_t  flow;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&sblk, 0, sizeof(BPLib_MEM_block_t));
    memset(&self_intf_id, 0, sizeof(bp_handle_t));
    memset(&flow, 0, sizeof(BPLib_MEM_flow_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_GT(bplib_create_cla_intf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
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
    BPLib_MEM_ref_t flow_ref;
    bplib_cla_stats_t stats;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow_ref, 0, sizeof(BPLib_MEM_ref_t));
    memset(&stats, 0, sizeof(bplib_cla_stats_t));

    UtAssert_INT32_NEQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetParentPoolFromLink), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_INT32_NEQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_INT32_EQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    timeout = 0;
    UtAssert_INT32_EQ(bplib_cla_ingress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
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
    BPLib_MEM_ref_t flow_ref;
    bplib_cla_stats_t stats;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow_ref, 0, sizeof(BPLib_MEM_ref_t));
    memset(&stats, 0, sizeof(bplib_cla_stats_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_INT32_NEQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    timeout = 3000;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UtAssert_INT32_NEQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &stats);
    UtAssert_INT32_EQ(bplib_cla_egress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_cla_event_impl(void)
{
    /* Test function for:
     * int bplib_cla_event_impl(void *arg, BPLib_MEM_block_t *intf_block)
     */
    BPLib_STOR_CACHE_FlowGenericEvent_t arg;
    BPLib_MEM_block_t              intf_block;
    BPLib_MEM_flow_t               flow;

    UtAssert_INT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    memset(&arg, 0, sizeof(BPLib_STOR_CACHE_FlowGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_MEM_block_t));
    memset(&flow, 0, sizeof(BPLib_MEM_flow_t));

    arg.event_type = BPLib_STOR_CACHE_FlowEventUp;
    UtAssert_INT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UtAssert_INT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    arg.event_type = BPLib_STOR_CACHE_FlowEventDown;
    UtAssert_INT32_EQ(bplib_cla_event_impl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_generic_bundle_ingress(void)
{
    /* Test function for:
     * int bplib_generic_bundle_ingress(BPLib_MEM_ref_t flow_ref, const void *content, size_t size, uint64_t
     * time_limit)
     */
    BPLib_STOR_CACHE_BlockContent_t  flow_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    BPLib_MEM_flow_t           flow;
    BPLib_MEM_block_t          pblk;
    BPLib_MEM_ref_t            refptr;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&flow_ref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&flow, 0, sizeof(BPLib_MEM_flow_t));
    memset(&pblk, 0, sizeof(BPLib_MEM_block_t));
    memset(&refptr, 0, sizeof(BPLib_MEM_ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UtAssert_INT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_INT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), BP_ERROR);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPush), UT_lib_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, &pblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_INT32_EQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), BP_ERROR);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_INT32_NEQ(bplib_generic_bundle_ingress(&flow_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_generic_bundle_egress(void)
{
    /* Test function for:
     * int bplib_generic_bundle_egress(BPLib_MEM_ref_t flow_ref, void *content, size_t *size, uint64_t time_limit)
     */
    BPLib_STOR_CACHE_BlockContent_t  flow_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    BPLib_MEM_flow_t           flow;
    BPLib_MEM_block_t          pblk;
    BPLib_MEM_ref_t            refptr;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&flow_ref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&flow, 0, sizeof(BPLib_MEM_flow_t));
    memset(&pblk, 0, sizeof(BPLib_MEM_block_t));
    memset(&refptr, 0, sizeof(BPLib_MEM_ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UtAssert_INT32_EQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_compute_full_bundle_size), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_out), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_INT32_NEQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_INT32_NEQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UtAssert_INT32_EQ(bplib_generic_bundle_egress(&flow_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
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

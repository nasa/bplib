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

static void BPLIB__routing_GetTime_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 1000;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void test_bplib_route_ingress_to_parent(void)
{
    /* Test function for:
     * int bplib_route_ingress_to_parent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    subq_src.parent_offset = 20;
    subq_src.type          = BPLib_STOR_CACHE_BlocktypeFlow;
    subq_src.next          = calloc(1, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_route_ingress_to_parent(NULL, &subq_src), 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_route_ingress_to_parent(NULL, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_ingress_route_single_bundle(void)
{
    /* Test function for:
     * void bplib_route_ingress_route_single_bundle(bplib_routetbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk)
     */
    bplib_routetbl_t             tbl;
    bplib_routeentry_t           route_entry;
    BPLib_STOR_CACHE_Block_t          pblk;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&route_entry, 0, sizeof(bplib_routeentry_t));
    tbl.route_tbl          = &route_entry;
    tbl.route_tbl->intf_id = BPLIB_HANDLE_FLASH_STORE_BASE;
    memset(&pblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    pri_block.data.delivery.delivery_policy = bplib_policy_delivery_custody_tracking;
    tbl.registered_routes                   = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UtAssert_VOIDCALL(bplib_route_ingress_route_single_bundle((bplib_routetbl_t *)&tbl, &pblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_ingress_baseintf_forwarder(void)
{
    /* Test function for:
     * int bplib_route_ingress_baseintf_forwarder(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    bplib_routetbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UtAssert_UINT32_NEQ(bplib_route_ingress_baseintf_forwarder(&rtbl, NULL), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_baseintf_AltHandler_PointerReturn, &subq_src);
    UtAssert_UINT32_NEQ(bplib_route_ingress_baseintf_forwarder(&rtbl, NULL), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_register_generic_intf(void)
{
    /* Test function for:
     * bp_handle_t bplib_route_register_generic_intf(bplib_routetbl_t *tbl, bp_handle_t parent_intf_id,
     * BPLib_STOR_CACHE_Block_t *flow_block)
     */
    bplib_routetbl_t    tbl;
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Block_t flow_block;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&parent_intf_id, 0, sizeof(bp_handle_t));
    parent_intf_id.hdl = 101;
    memset(&flow_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    flow_block.parent_offset = 20;
    flow_block.type          = BPLib_STOR_CACHE_BlocktypeFlow;

    UtAssert_UINT32_EQ(bplib_route_register_generic_intf(&tbl, parent_intf_id, &flow_block).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow_block);
    UtAssert_UINT32_EQ(bplib_route_register_generic_intf(&tbl, parent_intf_id, &flow_block).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_register_forward_ingress_handler(void)
{
    /* Test function for:
     * int bplib_route_register_forward_ingress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
     * BPLib_STOR_CACHE_CallbackFunc_t ingress)
     */
    bplib_routetbl_t tbl;
    bp_handle_t      intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t ingress = NULL;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(bplib_route_register_forward_ingress_handler(&tbl, intf_id, ingress), 0);
}

void test_bplib_route_register_forward_egress_handler(void)
{
    /* Test function for:
     * int bplib_route_register_forward_egress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
     * BPLib_STOR_CACHE_CallbackFunc_t ingress)
     */
    bplib_routetbl_t            tbl;
    bp_handle_t                 intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t egress = NULL;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(bplib_route_register_forward_egress_handler(&tbl, intf_id, egress), 0);
}

void test_bplib_route_push_egress_bundle(void)
{
    /* Test function for:
     * int bplib_route_push_egress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
     */
    bplib_routetbl_t     tbl;
    bp_handle_t          intf_id;
    BPLib_STOR_CACHE_Block_t *cb = NULL;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(bplib_route_push_egress_bundle(&tbl, intf_id, cb), 0);
}

void test_bplib_route_maintenance_request_wait(void)
{
    /* Test function for:
     * void bplib_route_maintenance_request_wait(bplib_routetbl_t *tbl)
     */
    bplib_routetbl_t rtbl;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    rtbl.last_intf_poll = 0;

    UT_SetHandlerFunction(UT_KEY(bplib_os_get_dtntime_ms), BPLIB__routing_GetTime_Handler, NULL);
    UtAssert_VOIDCALL(bplib_route_maintenance_request_wait(&rtbl));
}

void test_bplib_route_register_event_handler(void)
{
    /* Test function for:
     * int bplib_route_register_event_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_CallbackFunc_t
     * event)
     */
    bplib_routetbl_t            tbl;
    bp_handle_t                 intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t event = NULL;
    BPLib_STOR_CACHE_Flow_t          flow;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UtAssert_UINT32_NEQ(bplib_route_register_event_handler(&tbl, intf_id, event), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_route_register_event_handler(&tbl, intf_id, event), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_del_intf(void)
{
    /* Test function for:
     * int bplib_route_del_intf(bplib_routetbl_t *tbl, bp_handle_t intf_id)
     */
    bplib_routetbl_t   tbl;
    bplib_routeentry_t route_entry;
    bp_handle_t        intf_id;
    BPLib_STOR_CACHE_Ref_t  ref;
    BPLib_STOR_CACHE_Flow_t ifp;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&route_entry, 0, sizeof(bplib_routeentry_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&ifp, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_route_del_intf((bplib_routetbl_t *)&tbl, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &ifp);
    tbl.registered_routes = 1;
    tbl.route_tbl         = &route_entry;
    UtAssert_UINT32_EQ(bplib_route_del_intf((bplib_routetbl_t *)&tbl, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_get_next_avail_intf(void)
{
    /* Test function for:
     * bp_handle_t bplib_route_get_next_avail_intf(const bplib_routetbl_t *tbl, bp_ipn_t dest)
     */
    bplib_routetbl_t tbl;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    bp_ipn_t dest = 101;
    UtAssert_UINT32_EQ(bplib_route_get_next_avail_intf(&tbl, dest).hdl, 0);
}

void test_bplib_route_push_ingress_bundle(void)
{
    /* Test function for:
     * int bplib_route_push_ingress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
     */
    bplib_routetbl_t     tbl;
    bp_handle_t          intf_id;
    BPLib_STOR_CACHE_Block_t *cb = NULL;
    BPLib_STOR_CACHE_Flow_t   flow_block;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&flow_block, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow_block);
    UtAssert_UINT32_EQ(bplib_route_push_ingress_bundle(&tbl, intf_id, cb), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_add(void)
{
    /* Test function for:
     * int bplib_route_add(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
     */
    bplib_routetbl_t   rtbl;
    bplib_routeentry_t route_entry;
    BPLib_STOR_CACHE_Pool_t      pool;
    bp_ipn_t           dest = 101;
    bp_ipn_t           mask = 100;
    bp_handle_t        intf_id;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&route_entry, 0, sizeof(bplib_routeentry_t));
    rtbl.route_tbl = &route_entry;
    memset(&pool, 0, sizeof(BPLib_STOR_CACHE_Pool_t));
    rtbl.pool = &pool;

    UT_SetHandlerFunction(UT_KEY(bplib_os_calloc), UT_lib_AltHandler_PointerReturn, &rtbl);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, &pool);
    memset(&intf_id, 0, sizeof(bp_handle_t));

    rtbl.registered_routes = 20;
    UtAssert_UINT32_NEQ(bplib_route_add((bplib_routetbl_t *)&rtbl, dest, mask, intf_id), 0);

    rtbl.registered_routes = 1;
    rtbl.max_routes        = 100;
    UtAssert_UINT32_NEQ(bplib_route_add((bplib_routetbl_t *)&rtbl, dest, mask, intf_id), 0);

    mask             = 0;
    route_entry.mask = 0;
    route_entry.dest = 101;
    UtAssert_UINT32_NEQ(bplib_route_add((bplib_routetbl_t *)&rtbl, dest, mask, intf_id), 0);

    route_entry.mask       = 1;
    rtbl.registered_routes = 2;
    UtAssert_UINT32_EQ(bplib_route_add((bplib_routetbl_t *)&rtbl, dest, mask, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_os_calloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_del(void)
{
    /* Test function for:
     * int bplib_route_del(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
     */
    bplib_routetbl_t   tbl;
    bplib_routeentry_t route_entry;
    bp_ipn_t           dest = 101;
    bp_ipn_t           mask = 100;
    bp_handle_t        intf_id;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&route_entry, 0, sizeof(bplib_routeentry_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(bplib_route_del((bplib_routetbl_t *)&tbl, dest, mask, intf_id), 0);
    tbl.registered_routes = 1;
    tbl.route_tbl         = &route_entry;
    tbl.route_tbl->dest   = 101;
    tbl.route_tbl->mask   = 100;
    UtAssert_UINT32_EQ(bplib_route_del((bplib_routetbl_t *)&tbl, dest, mask, intf_id), 0);
}

void test_bplib_route_get_next_intf_with_flags(void)
{
    /* Test function for:
     * bp_handle_t bplib_route_get_next_intf_with_flags(const bplib_routetbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
     * uint32_t flag_mask)
     */
    bplib_routetbl_t   tbl;
    bplib_routeentry_t route_entry;
    bp_ipn_t           dest      = 101;
    uint32_t           req_flags = 0;
    uint32_t           flag_mask = 0;
    BPLib_STOR_CACHE_Flow_t flow_block;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&route_entry, 0, sizeof(bplib_routeentry_t));
    tbl.route_tbl = &route_entry;
    memset(&flow_block, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    tbl.registered_routes = 1;
    UtAssert_UINT32_EQ(bplib_route_get_next_intf_with_flags((bplib_routetbl_t *)&tbl, dest, req_flags, flag_mask).hdl,
                       0);

    flag_mask = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, &flow_block);
    UtAssert_UINT32_EQ(bplib_route_get_next_intf_with_flags((bplib_routetbl_t *)&tbl, dest, req_flags, flag_mask).hdl,
                       0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_intf_set_flags(void)
{
    /* Test function for:
     * int bplib_route_intf_set_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
     */
    bplib_routetbl_t  tbl;
    bp_handle_t       intf_id = BP_INVALID_HANDLE;
    uint32_t          flags   = 0;
    BPLib_STOR_CACHE_Ref_t ref;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowModifyFlags), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_route_intf_set_flags(&tbl, intf_id, flags), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_intf_unset_flags(void)
{
    /* Test function for:
     * int bplib_route_intf_unset_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
     */
    bplib_routetbl_t  tbl;
    bp_handle_t       intf_id = BP_INVALID_HANDLE;
    uint32_t          flags   = 0;
    BPLib_STOR_CACHE_Ref_t ref;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowModifyFlags), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_route_intf_unset_flags(&tbl, intf_id, flags), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_route_maintenance_complete_wait(void)
{
    /* Test function for:
     * void bplib_route_maintenance_complete_wait(bplib_routetbl_t *tbl)
     */
    bplib_routetbl_t rtbl;
    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    UtAssert_VOIDCALL(bplib_route_maintenance_complete_wait(&rtbl));
}

void test_bplib_route_periodic_maintenance(void)
{
    /* Test function for:
     * void bplib_route_periodic_maintenance(bplib_routetbl_t *tbl)
     */
    bplib_routetbl_t rtbl;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));

    UT_SetHandlerFunction(UT_KEY(bplib_os_get_dtntime_ms), BPLIB__routing_GetTime_Handler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BP_ERROR);
    UtAssert_VOIDCALL(bplib_route_periodic_maintenance((bplib_routetbl_t *)&rtbl));

    rtbl.poll_count = 1;
    UtAssert_VOIDCALL(bplib_route_periodic_maintenance((bplib_routetbl_t *)&rtbl));
}

void test_bplib_route_alloc_table(void)
{
    /* Test function for:
     * bplib_routetbl_t *bplib_route_alloc_table(uint32_t max_routes, size_t cache_mem_size)
     */
    uint32_t         max_routes     = 0;
    size_t           cache_mem_size = 1000;
    bplib_routetbl_t tbl;
    BPLib_STOR_CACHE_Pool_t    pool;

    memset(&tbl, 0, sizeof(bplib_routetbl_t));
    memset(&pool, 0, sizeof(BPLib_STOR_CACHE_Pool_t));

    UT_SetHandlerFunction(UT_KEY(bplib_os_calloc), UT_lib_sizet_Handler, NULL);
    UtAssert_NULL(bplib_route_alloc_table(max_routes, cache_mem_size));

    max_routes = 1;
    UT_SetHandlerFunction(UT_KEY(bplib_os_calloc), UT_lib_AltHandler_PointerReturn, &tbl);
    UtAssert_NULL(bplib_route_alloc_table(max_routes, cache_mem_size));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, &pool);
    UtAssert_NOT_NULL(bplib_route_alloc_table(max_routes, cache_mem_size));

    UT_SetHandlerFunction(UT_KEY(bplib_os_calloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, NULL);
}

void TestBplibBase_Routing_Register(void)
{
    UtTest_Add(test_bplib_route_ingress_to_parent, NULL, NULL, "Test bplib_route_ingress_to_parent");
    UtTest_Add(test_bplib_route_ingress_route_single_bundle, NULL, NULL,
               "Test bplib_route_ingress_route_single_bundle");
    UtTest_Add(test_bplib_route_ingress_baseintf_forwarder, NULL, NULL, "Test bplib_route_ingress_baseintf_forwarder");
    UtTest_Add(test_bplib_route_register_forward_ingress_handler, NULL, NULL,
               "Test bplib_route_register_forward_ingress_handler");
    UtTest_Add(test_bplib_route_register_forward_egress_handler, NULL, NULL,
               "Test bplib_route_register_forward_egress_handler");
    UtTest_Add(test_bplib_route_register_generic_intf, NULL, NULL, "Test bplib_route_register_generic_intf");
    UtTest_Add(test_bplib_route_register_event_handler, NULL, NULL, "Test bplib_route_register_event_handler");
    UtTest_Add(test_bplib_route_del_intf, NULL, NULL, "Test bplib_route_del_intf");
    UtTest_Add(test_bplib_route_get_next_avail_intf, NULL, NULL, "Test bplib_route_get_next_avail_intf");
    UtTest_Add(test_bplib_route_push_ingress_bundle, NULL, NULL, "Test bplib_route_push_ingress_bundle");
    UtTest_Add(test_bplib_route_intf_set_flags, NULL, NULL, "Test bplib_route_intf_set_flags");
    UtTest_Add(test_bplib_route_intf_unset_flags, NULL, NULL, "Test bplib_route_intf_unset_flags");
    UtTest_Add(test_bplib_route_add, NULL, NULL, "Test bplib_route_add");
    UtTest_Add(test_bplib_route_del, NULL, NULL, "Test bplib_route_del");
    UtTest_Add(test_bplib_route_get_next_intf_with_flags, NULL, NULL, "Test bplib_route_get_next_intf_with_flags");
    UtTest_Add(test_bplib_route_push_egress_bundle, NULL, NULL, "Test bplib_route_push_egress_bundle");
    UtTest_Add(test_bplib_route_maintenance_complete_wait, NULL, NULL, "Test bplib_route_maintenance_complete_wait");
    UtTest_Add(test_bplib_route_periodic_maintenance, NULL, NULL, "Test bplib_route_periodic_maintenance");
    UtTest_Add(test_bplib_route_maintenance_request_wait, NULL, NULL, "Test bplib_route_maintenance_request_wait");
    UtTest_Add(test_bplib_route_alloc_table, NULL, NULL, "Test bplib_route_alloc_table");
}

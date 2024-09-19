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
#include "test_bplib_cache.h"

void test_BPLib_STOR_CACHE_Module_entry_make_pending(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_entry_make_pending(BPLib_STOR_CACHE_Module_entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags)
     */
    BPLib_STOR_CACHE_Module_entry_t store_entry;
    uint32_t            set_flags   = 1;
    uint32_t            clear_flags = 0;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_entry_make_pending(&store_entry, set_flags, clear_flags));
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_attach(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_Module_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Module_state_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_mpool), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_attach), UT_cache_valid_bphandle_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_detach(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    BPLib_STOR_CACHE_Ref_t   flow_block_ref;
    BPLib_STOR_CACHE_Module_state_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    service_addr.node_number    = 100;
    service_addr.service_number = 101;
    memset(&flow_block_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_Module_detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_AltHandler_PointerReturn, &flow_block_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_register_module_service(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_Module_register_module_service(bplib_routetbl_t *tbl, bp_handle_t cache_intf_id, const
     * BPLib_STOR_CACHE_Module_module_api_t *api, void *init_arg)
     */
    bplib_routetbl_t        *tbl           = NULL;
    bp_handle_t              cache_intf_id = BP_INVALID_HANDLE;
    BPLib_STOR_CACHE_Module_module_api_t api;
    void                    *init_arg = NULL;
    BPLib_STOR_CACHE_Module_state_t      state;
    BPLib_STOR_CACHE_Ref_t        parent_ref;
    BPLib_STOR_CACHE_Block_t      cblk;

    memset(&api, 0, sizeof(BPLib_STOR_CACHE_Module_module_api_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&parent_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_register_module_service(tbl, cache_intf_id, NULL, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &parent_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, &cblk);
    api.instantiate = test_BPLib_STOR_CACHE_Module_instantiate_stub;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_register_module_service(tbl, cache_intf_id, &api, init_arg).hdl, 0);
    api.module_type = BPLib_STOR_CACHE_Module_module_type_offload;
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_Module_register_module_service(tbl, cache_intf_id, &api, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_configure(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_configure(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key,
     * BPLib_STOR_CACHE_Module_module_valtype_t vt, const void *val)
     */
    bplib_routetbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    BPLib_STOR_CACHE_Module_module_valtype_t vt             = BPLib_STOR_CACHE_Module_module_valtype_integer;
    void                        *val            = NULL;
    BPLib_STOR_CACHE_Module_state_t          state;
    BPLib_STOR_CACHE_Block_t          blk;
    BPLib_STOR_CACHE_Module_offload_api_t    api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_Module_offload_api_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.configure = test_BPLib_STOR_CACHE_Module_configure_stub;

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_configure(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_query(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_query(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Module_module_valtype_t
     * vt, const void **val)
     */
    bplib_routetbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    BPLib_STOR_CACHE_Module_module_valtype_t vt             = BPLib_STOR_CACHE_Module_module_valtype_integer;
    void                        *val            = NULL;
    BPLib_STOR_CACHE_Module_state_t          state;
    BPLib_STOR_CACHE_Block_t          blk;
    BPLib_STOR_CACHE_Module_offload_api_t    api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_Module_offload_api_t));

    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.query     = test_BPLib_STOR_CACHE_Module_query_stub;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_query(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_start(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_start(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
     */
    bplib_routetbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    BPLib_STOR_CACHE_Module_state_t       state;
    BPLib_STOR_CACHE_Block_t       blk;
    BPLib_STOR_CACHE_Module_offload_api_t api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_Module_offload_api_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.start     = test_BPLib_STOR_CACHE_Module_startstop_stub;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_start(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_stop(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_stop(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
     */
    bplib_routetbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    BPLib_STOR_CACHE_Module_state_t       state;
    BPLib_STOR_CACHE_Block_t       blk;
    BPLib_STOR_CACHE_Module_offload_api_t api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_Module_offload_api_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.stop      = test_BPLib_STOR_CACHE_Module_startstop_stub;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_stop(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_debug_scan(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_handle_t         intf_id;
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Ref_t   flow_block_ref;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&flow_block_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_AltHandler_PointerReturn,
                          &flow_block_ref);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_egress_impl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_egress_impl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    void               *arg = NULL;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_cache_egress_AltHandler_PointerReturn, &subq_src);
    subq_src.type = BPLib_STOR_CACHE_BlocktypeRef;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowTryPull), UT_cache_sizet_Handler, NULL);
}

void test_BPLib_STOR_CACHE_Module_flush_pending(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_flush_pending(BPLib_STOR_CACHE_Module_state_t *state)
     */
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));
    flow.ingress.current_depth_limit = 2;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BP_ERROR);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_flush_pending(&state));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_do_poll(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_do_poll(BPLib_STOR_CACHE_Module_state_t *state)
     */
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_IterGotoMax), BP_ERROR);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_do_poll(&state), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_do_route_up(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_do_route_up(BPLib_STOR_CACHE_Module_state_t *state, bp_ipn_t dest, bp_ipn_t mask)
     */
    BPLib_STOR_CACHE_Module_state_t state;
    bp_ipn_t            dest;
    bp_ipn_t            mask = 0;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    dest = 1;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_IterGotoMin), BP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_GetKeyValue), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_GetKeyValue), UT_cache_uint64_Handler, NULL);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_do_route_up(&state, dest, mask), 0);
}

void test_BPLib_STOR_CACHE_Module_do_intf_statechange(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_do_intf_statechange(BPLib_STOR_CACHE_Module_state_t *state, bool is_up)
     */
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_do_intf_statechange(&state, false), 0);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_do_intf_statechange(&state, true), 0);

    flow.current_state_flags = 8;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_do_intf_statechange(&state, true), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_event_impl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_event_impl(void *event_arg, BPLib_STOR_CACHE_Block_t *intf_block)
     */
    BPLib_STOR_CACHE_FlowGenericEvent_t event_arg;
    BPLib_STOR_CACHE_Block_t              intf_block;
    BPLib_STOR_CACHE_Module_state_t              state;
    BPLib_STOR_CACHE_Flow_t               flow;

    memset(&event_arg, 0, sizeof(BPLib_STOR_CACHE_FlowGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_event_impl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    event_arg.event_type = BPLib_STOR_CACHE_FlowEventPoll;
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_IterGotoMax), BP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterGotoFirst), BP_ERROR);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_event_impl(&event_arg, &intf_block), 0);

    event_arg.event_type         = BPLib_STOR_CACHE_FlowEventUp;
    event_arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_event_impl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_process_pending(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_process_pending(void *arg, BPLib_STOR_CACHE_Block_t *job)
     */
    BPLib_STOR_CACHE_Block_t job;
    BPLib_STOR_CACHE_Flow_t  flow;

    memset(&job, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&flow, 0, sizeof(BPLib_STOR_CACHE_Flow_t));

    flow.ingress.current_depth_limit = 2;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BP_ERROR);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_process_pending(NULL, &job), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_FlowCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_destruct_state(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_destruct_state(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Block_t sblk1;
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_RBT_Root_t    index;
    BPLib_STOR_CACHE_RBT_Link_t    rlink;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&index, 0, sizeof(BPLib_STOR_CACHE_RBT_Root_t));
    memset(&rlink, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));
    index.root              = &rlink;
    state.idle_list         = sblk1;
    state.pending_list      = sblk1;
    state.idle_list.next    = &state.idle_list;
    state.pending_list.next = &state.pending_list;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_destruct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_TreeIsEmpty), UT_cache_bool_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_destruct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_construct_entry(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_construct_entry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Module_entry_t store_entry;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_construct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_construct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_destruct_entry(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_destruct_entry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Module_entry_t store_entry;
    BPLib_STOR_CACHE_Module_state_t state;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_destruct_entry(NULL, &sblk), 0);

    store_entry.parent = &state;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RBT_GetKeyValue), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_GetKeyValue), UT_cache_uint64_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_destruct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_construct_blockref(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_construct_blockref(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t    sblk;
    BPLib_STOR_CACHE_Module_blockref_t blockref;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blockref, 0, sizeof(BPLib_STOR_CACHE_Module_blockref_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_construct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &blockref);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_construct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_destruct_blockref(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_destruct_blockref(void *arg, BPLib_STOR_CACHE_Block_t *rblk)
     */
    BPLib_STOR_CACHE_Block_t    sblk;
    BPLib_STOR_CACHE_Block_t    sblk1;
    BPLib_STOR_CACHE_Module_blockref_t blockref;
    BPLib_STOR_CACHE_Module_entry_t    store_entry;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blockref, 0, sizeof(BPLib_STOR_CACHE_Module_blockref_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_destruct_blockref(NULL, &sblk), 0);

    blockref.storage_entry = &store_entry;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &blockref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk1);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_destruct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_construct_state(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_construct_state(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Module_state_t state;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Module_construct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_construct_state(NULL, &sblk), 0);
}

void test_BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg)
     */
    BPLib_STOR_CACHE_RBT_Link_t node;

    memset(&node, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted(&node, NULL));
}

void TestBplibCache_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_Module_entry_make_pending, NULL, NULL, "Test BPLib_STOR_CACHE_Module_entry_make_pending");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_attach, NULL, NULL, "Test BPLib_STOR_CACHE_Module_attach");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_detach, NULL, NULL, "Test BPLib_STOR_CACHE_Module_detach");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_register_module_service, NULL, NULL, "Test BPLib_STOR_CACHE_Module_register_module_service");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_configure, NULL, NULL, "Test BPLib_STOR_CACHE_Module_configure");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_query, NULL, NULL, "Test BPLib_STOR_CACHE_Module_query");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_start, NULL, NULL, "Test BPLib_STOR_CACHE_Module_start");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_stop, NULL, NULL, "Test BPLib_STOR_CACHE_Module_stop");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_debug_scan, NULL, NULL, "Test BPLib_STOR_CACHE_Module_debug_scan");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_egress_impl, NULL, NULL, "Test BPLib_STOR_CACHE_Module_egress_impl");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_flush_pending, NULL, NULL, "Test BPLib_STOR_CACHE_Module_flush_pending");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_do_poll, NULL, NULL, "Test BPLib_STOR_CACHE_Module_do_poll");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_do_route_up, NULL, NULL, "Test BPLib_STOR_CACHE_Module_do_route_up");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_do_intf_statechange, NULL, NULL, "Test BPLib_STOR_CACHE_Module_do_intf_statechange");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_event_impl, NULL, NULL, "Test BPLib_STOR_CACHE_Module_event_impl");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_process_pending, NULL, NULL, "Test BPLib_STOR_CACHE_Module_process_pending");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_destruct_state, NULL, NULL, "Test BPLib_STOR_CACHE_Module_destruct_state");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_construct_entry, NULL, NULL, "Test BPLib_STOR_CACHE_Module_construct_entry");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_destruct_entry, NULL, NULL, "Test BPLib_STOR_CACHE_Module_destruct_entry");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_construct_blockref, NULL, NULL, "Test BPLib_STOR_CACHE_Module_construct_blockref");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_destruct_blockref, NULL, NULL, "Test BPLib_STOR_CACHE_Module_destruct_blockref");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_construct_state, NULL, NULL, "Test BPLib_STOR_CACHE_Module_construct_state");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted, NULL, NULL, "Test BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted");
}

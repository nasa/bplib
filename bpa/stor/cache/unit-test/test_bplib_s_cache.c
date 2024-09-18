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

void test_bplib_cache_entry_make_pending(void)
{
    /* Test function for:
     * void bplib_cache_entry_make_pending(bplib_cache_entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags)
     */
    bplib_cache_entry_t store_entry;
    uint32_t            set_flags   = 1;
    uint32_t            clear_flags = 0;
    bplib_mpool_block_t sblk;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_VOIDCALL(bplib_cache_entry_make_pending(&store_entry, set_flags, clear_flags));
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_attach(void)
{
    /* Test function for:
     * bp_handle_t bplib_cache_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    bplib_mpool_block_t sblk;
    bplib_cache_state_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_mpool), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_cache_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(bplib_cache_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_attach), UT_cache_valid_bphandle_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(bplib_cache_attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_detach(void)
{
    /* Test function for:
     * int bplib_cache_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    bplib_mpool_ref_t   flow_block_ref;
    bplib_cache_state_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    service_addr.node_number    = 100;
    service_addr.service_number = 101;
    memset(&flow_block_ref, 0, sizeof(bplib_mpool_ref_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_GT(bplib_cache_detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_AltHandler_PointerReturn, &flow_block_ref);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(bplib_cache_detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_dataservice_detach), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_register_module_service(void)
{
    /* Test function for:
     * bp_handle_t bplib_cache_register_module_service(bplib_routetbl_t *tbl, bp_handle_t cache_intf_id, const
     * bplib_cache_module_api_t *api, void *init_arg)
     */
    bplib_routetbl_t        *tbl           = NULL;
    bp_handle_t              cache_intf_id = BP_INVALID_HANDLE;
    bplib_cache_module_api_t api;
    void                    *init_arg = NULL;
    bplib_cache_state_t      state;
    bplib_mpool_ref_t        parent_ref;
    bplib_mpool_block_t      cblk;

    memset(&api, 0, sizeof(bplib_cache_module_api_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&parent_ref, 0, sizeof(bplib_mpool_ref_t));
    memset(&cblk, 0, sizeof(bplib_mpool_block_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_cache_register_module_service(tbl, cache_intf_id, NULL, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_cache_AltHandler_PointerReturn, &parent_ref);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_cache_AltHandler_PointerReturn, &cblk);
    api.instantiate = test_bplib_cache_instantiate_stub;
    UtAssert_UINT32_EQ(bplib_cache_register_module_service(tbl, cache_intf_id, &api, init_arg).hdl, 0);
    api.module_type = bplib_cache_module_type_offload;
    UtAssert_UINT32_GT(bplib_cache_register_module_service(tbl, cache_intf_id, &api, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_configure(void)
{
    /* Test function for:
     * int bplib_cache_configure(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key,
     * bplib_cache_module_valtype_t vt, const void *val)
     */
    bplib_routetbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    bplib_cache_module_valtype_t vt             = bplib_cache_module_valtype_integer;
    void                        *val            = NULL;
    bplib_cache_state_t          state;
    bplib_mpool_block_t          blk;
    bplib_cache_offload_api_t    api;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&blk, 0, sizeof(bplib_mpool_block_t));
    memset(&api, 0, sizeof(bplib_cache_offload_api_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.configure = test_bplib_cache_configure_stub;

    UtAssert_UINT32_EQ(bplib_cache_configure(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_query(void)
{
    /* Test function for:
     * int bplib_cache_query(bplib_routetbl_t *tbl, bp_handle_t module_intf_id, int key, bplib_cache_module_valtype_t
     * vt, const void **val)
     */
    bplib_routetbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    bplib_cache_module_valtype_t vt             = bplib_cache_module_valtype_integer;
    void                        *val            = NULL;
    bplib_cache_state_t          state;
    bplib_mpool_block_t          blk;
    bplib_cache_offload_api_t    api;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&blk, 0, sizeof(bplib_mpool_block_t));
    memset(&api, 0, sizeof(bplib_cache_offload_api_t));

    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.query     = test_bplib_cache_query_stub;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(bplib_cache_query(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_start(void)
{
    /* Test function for:
     * int bplib_cache_start(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
     */
    bplib_routetbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    bplib_cache_state_t       state;
    bplib_mpool_block_t       blk;
    bplib_cache_offload_api_t api;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&blk, 0, sizeof(bplib_mpool_block_t));
    memset(&api, 0, sizeof(bplib_cache_offload_api_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.start     = test_bplib_cache_startstop_stub;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(bplib_cache_start(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_stop(void)
{
    /* Test function for:
     * int bplib_cache_stop(bplib_routetbl_t *tbl, bp_handle_t module_intf_id)
     */
    bplib_routetbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    bplib_cache_state_t       state;
    bplib_mpool_block_t       blk;
    bplib_cache_offload_api_t api;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&blk, 0, sizeof(bplib_mpool_block_t));
    memset(&api, 0, sizeof(bplib_cache_offload_api_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.stop      = test_bplib_cache_startstop_stub;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(bplib_cache_stop(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_debug_scan(void)
{
    /* Test function for:
     * void bplib_cache_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id)
     */
    bplib_routetbl_t   *tbl = NULL;
    bp_handle_t         intf_id;
    bplib_cache_state_t state;
    bplib_mpool_ref_t   flow_block_ref;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&flow_block_ref, 0, sizeof(bplib_mpool_ref_t));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(bplib_cache_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_AltHandler_PointerReturn,
                          &flow_block_ref);
    UtAssert_VOIDCALL(bplib_cache_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_VOIDCALL(bplib_cache_debug_scan(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(bplib_route_get_intf_controlblock), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_egress_impl(void)
{
    /* Test function for:
     * int bplib_cache_egress_impl(void *arg, bplib_mpool_block_t *subq_src)
     */
    void               *arg = NULL;
    bplib_mpool_block_t subq_src;
    bplib_cache_state_t state;
    bplib_mpool_flow_t  flow;

    memset(&subq_src, 0, sizeof(bplib_mpool_block_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    UtAssert_UINT32_NEQ(bplib_cache_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(bplib_cache_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_pull), UT_cache_egress_AltHandler_PointerReturn, &subq_src);
    subq_src.type = bplib_mpool_blocktype_ref;
    UtAssert_UINT32_NEQ(bplib_cache_egress_impl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_pull), UT_cache_sizet_Handler, NULL);
}

void test_bplib_cache_flush_pending(void)
{
    /* Test function for:
     * void bplib_cache_flush_pending(bplib_cache_state_t *state)
     */
    bplib_cache_state_t state;
    bplib_mpool_flow_t  flow;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));
    flow.ingress.current_depth_limit = 2;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_list_iter_forward), BP_ERROR);
    UtAssert_VOIDCALL(bplib_cache_flush_pending(&state));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_do_poll(void)
{
    /* Test function for:
     * int bplib_cache_do_poll(bplib_cache_state_t *state)
     */
    bplib_cache_state_t state;
    bplib_mpool_block_t sblk;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_iter_goto_max), BP_ERROR);
    UtAssert_UINT32_EQ(bplib_cache_do_poll(&state), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_do_route_up(void)
{
    /* Test function for:
     * int bplib_cache_do_route_up(bplib_cache_state_t *state, bp_ipn_t dest, bp_ipn_t mask)
     */
    bplib_cache_state_t state;
    bp_ipn_t            dest;
    bp_ipn_t            mask = 0;
    bplib_mpool_block_t sblk;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    dest = 1;

    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_iter_goto_min), BP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_get_key_value), 1);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_get_key_value), UT_cache_uint64_Handler, NULL);

    UtAssert_UINT32_EQ(bplib_cache_do_route_up(&state, dest, mask), 0);
}

void test_bplib_cache_do_intf_statechange(void)
{
    /* Test function for:
     * int bplib_cache_do_intf_statechange(bplib_cache_state_t *state, bool is_up)
     */
    bplib_cache_state_t state;
    bplib_mpool_flow_t  flow;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_cache_do_intf_statechange(&state, false), 0);

    UtAssert_UINT32_EQ(bplib_cache_do_intf_statechange(&state, true), 0);

    flow.current_state_flags = 8;
    UtAssert_UINT32_EQ(bplib_cache_do_intf_statechange(&state, true), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_event_impl(void)
{
    /* Test function for:
     * int bplib_cache_event_impl(void *event_arg, bplib_mpool_block_t *intf_block)
     */
    bplib_mpool_flow_generic_event_t event_arg;
    bplib_mpool_block_t              intf_block;
    bplib_cache_state_t              state;
    bplib_mpool_flow_t               flow;

    memset(&event_arg, 0, sizeof(bplib_mpool_flow_generic_event_t));
    memset(&intf_block, 0, sizeof(bplib_mpool_block_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    UtAssert_UINT32_NEQ(bplib_cache_event_impl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    event_arg.event_type = bplib_mpool_flow_event_poll;
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_iter_goto_max), BP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_list_iter_forward), BP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_list_iter_goto_first), BP_ERROR);
    UtAssert_UINT32_EQ(bplib_cache_event_impl(&event_arg, &intf_block), 0);

    event_arg.event_type         = bplib_mpool_flow_event_up;
    event_arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, &flow);
    UtAssert_UINT32_EQ(bplib_cache_event_impl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_process_pending(void)
{
    /* Test function for:
     * int bplib_cache_process_pending(void *arg, bplib_mpool_block_t *job)
     */
    bplib_mpool_block_t job;
    bplib_mpool_flow_t  flow;

    memset(&job, 0, sizeof(bplib_mpool_block_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    flow.ingress.current_depth_limit = 2;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, &flow);
    UT_SetDefaultReturnValue(UT_KEY(bplib_mpool_list_iter_forward), BP_ERROR);

    UtAssert_UINT32_EQ(bplib_cache_process_pending(NULL, &job), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_destruct_state(void)
{
    /* Test function for:
     * int bplib_cache_destruct_state(void *arg, bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t sblk;
    bplib_mpool_block_t sblk1;
    bplib_cache_state_t state;
    bplib_rbt_root_t    index;
    bplib_rbt_link_t    rlink;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&sblk1, 0, sizeof(bplib_mpool_block_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&index, 0, sizeof(bplib_rbt_root_t));
    memset(&rlink, 0, sizeof(bplib_rbt_link_t));
    index.root              = &rlink;
    state.idle_list         = sblk1;
    state.pending_list      = sblk1;
    state.idle_list.next    = &state.idle_list;
    state.pending_list.next = &state.pending_list;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(bplib_cache_destruct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_tree_is_empty), UT_cache_bool_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_cache_destruct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_construct_entry(void)
{
    /* Test function for:
     * int bplib_cache_construct_entry(void *arg, bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t sblk;
    bplib_cache_entry_t store_entry;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UtAssert_UINT32_NEQ(bplib_cache_construct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UtAssert_UINT32_EQ(bplib_cache_construct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_destruct_entry(void)
{
    /* Test function for:
     * int bplib_cache_destruct_entry(void *arg, bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t sblk;
    bplib_cache_entry_t store_entry;
    bplib_cache_state_t state;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));

    UtAssert_UINT32_NEQ(bplib_cache_destruct_entry(NULL, &sblk), 0);

    store_entry.parent = &state;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_get_key_value), 1);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_get_key_value), UT_cache_uint64_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_cache_destruct_entry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_construct_blockref(void)
{
    /* Test function for:
     * int bplib_cache_construct_blockref(void *arg, bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t    sblk;
    bplib_cache_blockref_t blockref;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&blockref, 0, sizeof(bplib_cache_blockref_t));

    UtAssert_UINT32_NEQ(bplib_cache_construct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &blockref);
    UtAssert_UINT32_EQ(bplib_cache_construct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_destruct_blockref(void)
{
    /* Test function for:
     * int bplib_cache_destruct_blockref(void *arg, bplib_mpool_block_t *rblk)
     */
    bplib_mpool_block_t    sblk;
    bplib_mpool_block_t    sblk1;
    bplib_cache_blockref_t blockref;
    bplib_cache_entry_t    store_entry;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&sblk1, 0, sizeof(bplib_mpool_block_t));
    memset(&blockref, 0, sizeof(bplib_cache_blockref_t));
    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UtAssert_UINT32_NEQ(bplib_cache_destruct_blockref(NULL, &sblk), 0);

    blockref.storage_entry = &store_entry;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &blockref);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, &sblk1);

    UtAssert_UINT32_EQ(bplib_cache_destruct_blockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_uncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_construct_state(void)
{
    /* Test function for:
     * int bplib_cache_construct_state(void *arg, bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t sblk;
    bplib_cache_state_t state;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));

    UtAssert_UINT32_NEQ(bplib_cache_construct_state(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(bplib_cache_construct_state(NULL, &sblk), 0);
}

void test_bplib_cache_entry_tree_insert_unsorted(void)
{
    /* Test function for:
     * int bplib_cache_entry_tree_insert_unsorted(const bplib_rbt_link_t *node, void *arg)
     */
    bplib_rbt_link_t node;

    memset(&node, 0, sizeof(bplib_rbt_link_t));

    UtAssert_VOIDCALL(bplib_cache_entry_tree_insert_unsorted(&node, NULL));
}

void TestBplibCache_Register(void)
{
    UtTest_Add(test_bplib_cache_entry_make_pending, NULL, NULL, "Test bplib_cache_entry_make_pending");
    UtTest_Add(test_bplib_cache_attach, NULL, NULL, "Test bplib_cache_attach");
    UtTest_Add(test_bplib_cache_detach, NULL, NULL, "Test bplib_cache_detach");
    UtTest_Add(test_bplib_cache_register_module_service, NULL, NULL, "Test bplib_cache_register_module_service");
    UtTest_Add(test_bplib_cache_configure, NULL, NULL, "Test bplib_cache_configure");
    UtTest_Add(test_bplib_cache_query, NULL, NULL, "Test bplib_cache_query");
    UtTest_Add(test_bplib_cache_start, NULL, NULL, "Test bplib_cache_start");
    UtTest_Add(test_bplib_cache_stop, NULL, NULL, "Test bplib_cache_stop");
    UtTest_Add(test_bplib_cache_debug_scan, NULL, NULL, "Test bplib_cache_debug_scan");
    UtTest_Add(test_bplib_cache_egress_impl, NULL, NULL, "Test bplib_cache_egress_impl");
    UtTest_Add(test_bplib_cache_flush_pending, NULL, NULL, "Test bplib_cache_flush_pending");
    UtTest_Add(test_bplib_cache_do_poll, NULL, NULL, "Test bplib_cache_do_poll");
    UtTest_Add(test_bplib_cache_do_route_up, NULL, NULL, "Test bplib_cache_do_route_up");
    UtTest_Add(test_bplib_cache_do_intf_statechange, NULL, NULL, "Test bplib_cache_do_intf_statechange");
    UtTest_Add(test_bplib_cache_event_impl, NULL, NULL, "Test bplib_cache_event_impl");
    UtTest_Add(test_bplib_cache_process_pending, NULL, NULL, "Test bplib_cache_process_pending");
    UtTest_Add(test_bplib_cache_destruct_state, NULL, NULL, "Test bplib_cache_destruct_state");
    UtTest_Add(test_bplib_cache_construct_entry, NULL, NULL, "Test bplib_cache_construct_entry");
    UtTest_Add(test_bplib_cache_destruct_entry, NULL, NULL, "Test bplib_cache_destruct_entry");
    UtTest_Add(test_bplib_cache_construct_blockref, NULL, NULL, "Test bplib_cache_construct_blockref");
    UtTest_Add(test_bplib_cache_destruct_blockref, NULL, NULL, "Test bplib_cache_destruct_blockref");
    UtTest_Add(test_bplib_cache_construct_state, NULL, NULL, "Test bplib_cache_construct_state");
    UtTest_Add(test_bplib_cache_entry_tree_insert_unsorted, NULL, NULL, "Test bplib_cache_entry_tree_insert_unsorted");
}

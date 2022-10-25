/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in bplib_routing header
 */

#include "bplib_routing.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_add()
 * ----------------------------------------------------
 */
int bplib_route_add(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_add, int);

    UT_GenStub_AddParam(bplib_route_add, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_add, bp_ipn_t, dest);
    UT_GenStub_AddParam(bplib_route_add, bp_ipn_t, mask);
    UT_GenStub_AddParam(bplib_route_add, bp_handle_t, intf_id);

    UT_GenStub_Execute(bplib_route_add, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_add, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_alloc_table()
 * ----------------------------------------------------
 */
bplib_routetbl_t *bplib_route_alloc_table(uint32_t max_routes, size_t cache_mem_size)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_alloc_table, bplib_routetbl_t *);

    UT_GenStub_AddParam(bplib_route_alloc_table, uint32_t, max_routes);
    UT_GenStub_AddParam(bplib_route_alloc_table, size_t, cache_mem_size);

    UT_GenStub_Execute(bplib_route_alloc_table, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_alloc_table, bplib_routetbl_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_bind_sub_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_route_bind_sub_intf(bplib_mpool_block_t *flow_block, bp_handle_t parent_intf_id)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_bind_sub_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_route_bind_sub_intf, bplib_mpool_block_t *, flow_block);
    UT_GenStub_AddParam(bplib_route_bind_sub_intf, bp_handle_t, parent_intf_id);

    UT_GenStub_Execute(bplib_route_bind_sub_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_bind_sub_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_del()
 * ----------------------------------------------------
 */
int bplib_route_del(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_del, int);

    UT_GenStub_AddParam(bplib_route_del, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_del, bp_ipn_t, dest);
    UT_GenStub_AddParam(bplib_route_del, bp_ipn_t, mask);
    UT_GenStub_AddParam(bplib_route_del, bp_handle_t, intf_id);

    UT_GenStub_Execute(bplib_route_del, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_del, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_del_intf()
 * ----------------------------------------------------
 */
int bplib_route_del_intf(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_del_intf, int);

    UT_GenStub_AddParam(bplib_route_del_intf, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_del_intf, bp_handle_t, intf_id);

    UT_GenStub_Execute(bplib_route_del_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_del_intf, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_forward_baseintf_bundle()
 * ----------------------------------------------------
 */
int bplib_route_forward_baseintf_bundle(bplib_mpool_block_t *flow_block, void *forward_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_forward_baseintf_bundle, int);

    UT_GenStub_AddParam(bplib_route_forward_baseintf_bundle, bplib_mpool_block_t *, flow_block);
    UT_GenStub_AddParam(bplib_route_forward_baseintf_bundle, void *, forward_arg);

    UT_GenStub_Execute(bplib_route_forward_baseintf_bundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_forward_baseintf_bundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_get_intf_controlblock()
 * ----------------------------------------------------
 */
bplib_mpool_ref_t bplib_route_get_intf_controlblock(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_get_intf_controlblock, bplib_mpool_ref_t);

    UT_GenStub_AddParam(bplib_route_get_intf_controlblock, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_get_intf_controlblock, bp_handle_t, intf_id);

    UT_GenStub_Execute(bplib_route_get_intf_controlblock, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_get_intf_controlblock, bplib_mpool_ref_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_get_mpool()
 * ----------------------------------------------------
 */
bplib_mpool_t *bplib_route_get_mpool(const bplib_routetbl_t *tbl)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_get_mpool, bplib_mpool_t *);

    UT_GenStub_AddParam(bplib_route_get_mpool, const bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_get_mpool, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_get_mpool, bplib_mpool_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_get_next_avail_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_route_get_next_avail_intf(const bplib_routetbl_t *tbl, bp_ipn_t dest)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_get_next_avail_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_route_get_next_avail_intf, const bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_get_next_avail_intf, bp_ipn_t, dest);

    UT_GenStub_Execute(bplib_route_get_next_avail_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_get_next_avail_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_get_next_intf_with_flags()
 * ----------------------------------------------------
 */
bp_handle_t bplib_route_get_next_intf_with_flags(const bplib_routetbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
                                                 uint32_t flag_mask)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_get_next_intf_with_flags, bp_handle_t);

    UT_GenStub_AddParam(bplib_route_get_next_intf_with_flags, const bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_get_next_intf_with_flags, bp_ipn_t, dest);
    UT_GenStub_AddParam(bplib_route_get_next_intf_with_flags, uint32_t, req_flags);
    UT_GenStub_AddParam(bplib_route_get_next_intf_with_flags, uint32_t, flag_mask);

    UT_GenStub_Execute(bplib_route_get_next_intf_with_flags, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_get_next_intf_with_flags, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_ingress_baseintf_forwarder()
 * ----------------------------------------------------
 */
int bplib_route_ingress_baseintf_forwarder(void *arg, bplib_mpool_block_t *subq_src)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_ingress_baseintf_forwarder, int);

    UT_GenStub_AddParam(bplib_route_ingress_baseintf_forwarder, void *, arg);
    UT_GenStub_AddParam(bplib_route_ingress_baseintf_forwarder, bplib_mpool_block_t *, subq_src);

    UT_GenStub_Execute(bplib_route_ingress_baseintf_forwarder, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_ingress_baseintf_forwarder, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_ingress_route_single_bundle()
 * ----------------------------------------------------
 */
void bplib_route_ingress_route_single_bundle(bplib_routetbl_t *tbl, bplib_mpool_block_t *pblk)
{
    UT_GenStub_AddParam(bplib_route_ingress_route_single_bundle, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_ingress_route_single_bundle, bplib_mpool_block_t *, pblk);

    UT_GenStub_Execute(bplib_route_ingress_route_single_bundle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_ingress_to_parent()
 * ----------------------------------------------------
 */
int bplib_route_ingress_to_parent(void *arg, bplib_mpool_block_t *subq_src)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_ingress_to_parent, int);

    UT_GenStub_AddParam(bplib_route_ingress_to_parent, void *, arg);
    UT_GenStub_AddParam(bplib_route_ingress_to_parent, bplib_mpool_block_t *, subq_src);

    UT_GenStub_Execute(bplib_route_ingress_to_parent, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_ingress_to_parent, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_intf_set_flags()
 * ----------------------------------------------------
 */
int bplib_route_intf_set_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_intf_set_flags, int);

    UT_GenStub_AddParam(bplib_route_intf_set_flags, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_intf_set_flags, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_intf_set_flags, uint32_t, flags);

    UT_GenStub_Execute(bplib_route_intf_set_flags, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_intf_set_flags, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_intf_unset_flags()
 * ----------------------------------------------------
 */
int bplib_route_intf_unset_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_intf_unset_flags, int);

    UT_GenStub_AddParam(bplib_route_intf_unset_flags, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_intf_unset_flags, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_intf_unset_flags, uint32_t, flags);

    UT_GenStub_Execute(bplib_route_intf_unset_flags, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_intf_unset_flags, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_maintenance_complete_wait()
 * ----------------------------------------------------
 */
void bplib_route_maintenance_complete_wait(bplib_routetbl_t *tbl)
{
    UT_GenStub_AddParam(bplib_route_maintenance_complete_wait, bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_maintenance_complete_wait, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_maintenance_request_wait()
 * ----------------------------------------------------
 */
void bplib_route_maintenance_request_wait(bplib_routetbl_t *tbl)
{
    UT_GenStub_AddParam(bplib_route_maintenance_request_wait, bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_maintenance_request_wait, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_periodic_maintenance()
 * ----------------------------------------------------
 */
void bplib_route_periodic_maintenance(bplib_routetbl_t *tbl)
{
    UT_GenStub_AddParam(bplib_route_periodic_maintenance, bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_periodic_maintenance, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_process_active_flows()
 * ----------------------------------------------------
 */
void bplib_route_process_active_flows(bplib_routetbl_t *tbl)
{
    UT_GenStub_AddParam(bplib_route_process_active_flows, bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_process_active_flows, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_push_egress_bundle()
 * ----------------------------------------------------
 */
int bplib_route_push_egress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_push_egress_bundle, int);

    UT_GenStub_AddParam(bplib_route_push_egress_bundle, const bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_push_egress_bundle, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_push_egress_bundle, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_route_push_egress_bundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_push_egress_bundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_push_ingress_bundle()
 * ----------------------------------------------------
 */
int bplib_route_push_ingress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_push_ingress_bundle, int);

    UT_GenStub_AddParam(bplib_route_push_ingress_bundle, const bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_push_ingress_bundle, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_push_ingress_bundle, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_route_push_ingress_bundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_push_ingress_bundle, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_register_event_handler()
 * ----------------------------------------------------
 */
int bplib_route_register_event_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_callback_func_t event)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_register_event_handler, int);

    UT_GenStub_AddParam(bplib_route_register_event_handler, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_register_event_handler, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_register_event_handler, bplib_mpool_callback_func_t, event);

    UT_GenStub_Execute(bplib_route_register_event_handler, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_register_event_handler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_register_forward_egress_handler()
 * ----------------------------------------------------
 */
int bplib_route_register_forward_egress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                bplib_mpool_callback_func_t egress)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_register_forward_egress_handler, int);

    UT_GenStub_AddParam(bplib_route_register_forward_egress_handler, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_register_forward_egress_handler, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_register_forward_egress_handler, bplib_mpool_callback_func_t, egress);

    UT_GenStub_Execute(bplib_route_register_forward_egress_handler, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_register_forward_egress_handler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_register_forward_ingress_handler()
 * ----------------------------------------------------
 */
int bplib_route_register_forward_ingress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                 bplib_mpool_callback_func_t ingress)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_register_forward_ingress_handler, int);

    UT_GenStub_AddParam(bplib_route_register_forward_ingress_handler, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_register_forward_ingress_handler, bp_handle_t, intf_id);
    UT_GenStub_AddParam(bplib_route_register_forward_ingress_handler, bplib_mpool_callback_func_t, ingress);

    UT_GenStub_Execute(bplib_route_register_forward_ingress_handler, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_register_forward_ingress_handler, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_register_generic_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_route_register_generic_intf(bplib_routetbl_t *tbl, bp_handle_t parent_intf_id,
                                              bplib_mpool_block_t *flow_block)
{
    UT_GenStub_SetupReturnBuffer(bplib_route_register_generic_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_route_register_generic_intf, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_register_generic_intf, bp_handle_t, parent_intf_id);
    UT_GenStub_AddParam(bplib_route_register_generic_intf, bplib_mpool_block_t *, flow_block);

    UT_GenStub_Execute(bplib_route_register_generic_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_route_register_generic_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_release_intf_controlblock()
 * ----------------------------------------------------
 */
void bplib_route_release_intf_controlblock(bplib_routetbl_t *tbl, bplib_mpool_ref_t refptr)
{
    UT_GenStub_AddParam(bplib_route_release_intf_controlblock, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_route_release_intf_controlblock, bplib_mpool_ref_t, refptr);

    UT_GenStub_Execute(bplib_route_release_intf_controlblock, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_route_set_maintenance_request()
 * ----------------------------------------------------
 */
void bplib_route_set_maintenance_request(bplib_routetbl_t *tbl)
{
    UT_GenStub_AddParam(bplib_route_set_maintenance_request, bplib_routetbl_t *, tbl);

    UT_GenStub_Execute(bplib_route_set_maintenance_request, Basic, NULL);
}

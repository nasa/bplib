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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#ifndef bplib_routing_h
#define bplib_routing_h

#include <stdint.h>
#include "bplib_api_types.h"
#include "v7_mpool.h"
#include "v7_mpool_flows.h"

#define BPLIB_INTF_STATE_ADMIN_UP BPLIB_MPOOL_FLOW_FLAGS_ADMIN_UP
#define BPLIB_INTF_STATE_OPER_UP  BPLIB_MPOOL_FLOW_FLAGS_OPER_UP
#define BPLIB_INTF_STATE_STORAGE  BPLIB_MPOOL_FLOW_FLAGS_STORAGE

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef int (*bplib_route_action_func_t)(bplib_routetbl_t *tbl, bplib_mpool_ref_t ref, void *arg);

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bplib_routetbl_t *bplib_route_alloc_table(uint32_t max_routes, size_t cache_mem_size);
bplib_mpool_t    *bplib_route_get_mpool(const bplib_routetbl_t *tbl);

bp_handle_t bplib_route_register_generic_intf(bplib_routetbl_t *tbl, bp_handle_t parent_intf_id,
                                              bplib_mpool_block_t *flow_block);

void bplib_route_ingress_route_single_bundle(bplib_routetbl_t *tbl, bplib_mpool_block_t *pblk);
int  bplib_route_ingress_baseintf_forwarder(void *arg, bplib_mpool_block_t *subq_src);
int  bplib_route_ingress_to_parent(void *arg, bplib_mpool_block_t *subq_src);

bp_handle_t       bplib_route_bind_sub_intf(bplib_mpool_block_t *flow_block, bp_handle_t parent_intf_id);
bplib_mpool_ref_t bplib_route_get_intf_controlblock(bplib_routetbl_t *tbl, bp_handle_t intf_id);
void              bplib_route_release_intf_controlblock(bplib_routetbl_t *tbl, bplib_mpool_ref_t refptr);

int bplib_route_register_forward_ingress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                 bplib_mpool_callback_func_t ingress);
int bplib_route_register_forward_egress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                bplib_mpool_callback_func_t egress);
int bplib_route_register_event_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_callback_func_t event);

int         bplib_route_del_intf(bplib_routetbl_t *tbl, bp_handle_t intf_id);
bp_handle_t bplib_route_get_next_intf_with_flags(const bplib_routetbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
                                                 uint32_t flag_mask);
bp_handle_t bplib_route_get_next_avail_intf(const bplib_routetbl_t *tbl, bp_ipn_t dest);
int         bplib_route_add(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id);
int         bplib_route_del(bplib_routetbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id);

int bplib_route_intf_set_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags);
int bplib_route_intf_unset_flags(bplib_routetbl_t *tbl, bp_handle_t intf_id, uint32_t flags);

int bplib_route_push_ingress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb);
int bplib_route_push_egress_bundle(const bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_mpool_block_t *cb);

int bplib_route_forward_baseintf_bundle(bplib_mpool_block_t *flow_block, void *forward_arg);

void bplib_route_set_maintenance_request(bplib_routetbl_t *tbl);
void bplib_route_maintenance_request_wait(bplib_routetbl_t *tbl);
void bplib_route_maintenance_complete_wait(bplib_routetbl_t *tbl);

void bplib_route_process_active_flows(bplib_routetbl_t *tbl);
void bplib_route_periodic_maintenance(bplib_routetbl_t *tbl);

#endif

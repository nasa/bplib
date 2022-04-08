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

#define BPLIB_INTF_STATE_ADMIN_UP 0x01
#define BPLIB_INTF_STATE_OPER_UP  0x02
#define BPLIB_INTF_STATE_STORAGE  0x04

#define BPLIB_INTF_STATE_ALL_FLAGS 0x07

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * Enumeration that defines the various possible routing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    bplib_event_type_undefined,
    bplib_event_type_poll_interval,
    bplib_event_type_attach_subintf,
    bplib_event_type_remove_subintf,
    bplib_event_type_interface_up,
    bplib_event_type_interface_down,
    bplib_event_type_route_up,
    bplib_event_type_route_down,
    bplib_event_type_max

} bplib_event_type_t;

typedef struct bplib_route_state_event
{
    bplib_event_type_t event_type; /* must be first */
    bp_ipn_t           dest;
    bp_ipn_t           mask;
} bplib_route_state_event_t;

typedef struct bplib_intf_state_event
{
    bplib_event_type_t event_type; /* must be first */
    bp_handle_t        intf_id;
} bplib_intf_state_event_t;

typedef union bplib_generic_event
{
    bplib_event_type_t        event_type;
    bplib_intf_state_event_t  intf_state;
    bplib_route_state_event_t route_state;
} bplib_generic_event_t;

typedef int (*bplib_route_action_func_t)(bplib_routetbl_t *tbl, bplib_mpool_block_t *cb, void *arg);

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bplib_routetbl_t *bplib_route_alloc_table(uint32_t max_routes, uint32_t max_intfs, size_t cache_mem_size);
bplib_mpool_t    *bplib_route_get_mpool(const bplib_routetbl_t *tbl);

bp_handle_t bplib_route_register_generic_intf(bplib_routetbl_t *tbl, bp_handle_t parent_intf_id,
                                              bplib_mpool_refptr_t *blkref);

void bplib_route_ingress_route_single_bundle(bplib_routetbl_t *tbl, bplib_mpool_block_t *pblk);
int  bplib_route_ingress_baseintf_forwarder(bplib_routetbl_t *tbl, bplib_mpool_block_t *flow_block, void *forward_arg);
int  bplib_route_ingress_to_parent(bplib_routetbl_t *tbl, bplib_mpool_block_t *flow_block, void *ingress_arg);

bp_handle_t           bplib_route_bind_sub_intf(bplib_mpool_block_t *flow_block, bp_handle_t parent_intf_id);
bplib_mpool_refptr_t *bplib_route_get_intf_controlblock(bplib_routetbl_t *tbl, bp_handle_t intf_id);
void                  bplib_route_release_intf_controlblock(bplib_routetbl_t *tbl, bplib_mpool_refptr_t *refptr);

int bplib_route_register_forward_ingress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                 bplib_route_action_func_t ingress, void *ingress_arg);
int bplib_route_register_forward_egress_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id,
                                                bplib_route_action_func_t egress, void *egress_arg);
int bplib_route_register_event_handler(bplib_routetbl_t *tbl, bp_handle_t intf_id, bplib_route_action_func_t event);

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

void bplib_route_do_maintenance(bplib_routetbl_t *tbl);

// bplib_mpool_baseintf_t        *bplib_mpool_cast_baseintf(bplib_mpool_block_t *cb);
// bplib_mpool_flow_t            *bplib_mpool_cast_flow(bplib_mpool_block_t *cb);

#endif

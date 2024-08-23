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

#ifndef V7_BASE_INTERNAL_H
#define V7_BASE_INTERNAL_H

#include "bplib_api_types.h"
#include "bplib_mem_bundle.h"
#include "bplib_mem_rbtree.h"

struct bp_socket
{
    bplib_mpool_block_t fblk;
};

typedef struct bplib_cla_stats
{
    uintmax_t ingress_byte_count;
    uintmax_t egress_byte_count;

} bplib_cla_stats_t;

typedef struct bplib_route_serviceintf_info
{
    bp_ipn_t          node_number;
    bplib_rbt_root_t  service_index;
    bplib_mpool_ref_t storage_service;

} bplib_route_serviceintf_info_t;

typedef struct bplib_service_endpt bplib_service_endpt_t;

struct bplib_service_endpt
{
    bplib_rbt_link_t     rbt_link; /* for storage in RB tree, must be first */
    bplib_mpool_block_t *self_ptr;
    bplib_mpool_ref_t    subflow_ref;
};

typedef struct bplib_socket_info bplib_socket_info_t;
struct bplib_socket_info
{
    bplib_routetbl_t   *parent_rtbl;
    bp_handle_t         socket_intf_id;
    bplib_connection_t  params;
    uintmax_t           ingress_byte_count;
    uintmax_t           egress_byte_count;
    bp_sequencenumber_t last_bundle_seq;
};

typedef struct bplib_routeentry
{
    bp_ipn_t    dest;
    bp_ipn_t    mask;
    bp_handle_t intf_id;
} bplib_routeentry_t;

struct bplib_routetbl
{
    uint32_t            max_routes;
    uint32_t            registered_routes;
    bp_handle_t         activity_lock;
    volatile bool       maint_request_flag;
    volatile bool       maint_active_flag;
    uint8_t             poll_count;
    uint64_t            last_intf_poll;
    uintmax_t           routing_success_count;
    uintmax_t           routing_error_count;
    bplib_mpool_t      *pool;
    bplib_mpool_block_t flow_list;
    bplib_routeentry_t *route_tbl;
};

int bplib_serviceflow_forward_ingress(void *arg, bplib_mpool_block_t *subq_src);
int bplib_serviceflow_forward_egress(void *arg, bplib_mpool_block_t *subq_src);
int bplib_dataservice_event_impl(void *arg, bplib_mpool_block_t *intf_block);
int bplib_dataservice_base_construct(void *arg, bplib_mpool_block_t *blk);
int bplib_cla_event_impl(void *arg, bplib_mpool_block_t *intf_block);
int bplib_generic_bundle_ingress(bplib_mpool_ref_t flow_ref, const void *content, size_t size, uint64_t time_limit);
int bplib_generic_bundle_egress(bplib_mpool_ref_t flow_ref, void *content, size_t *size, uint64_t time_limit);

#endif /* V7_BASE_INTERNAL_H*/
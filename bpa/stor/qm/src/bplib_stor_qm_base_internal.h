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

#ifndef BPLIB_QM_BASE_INTERNAL_H
#define BPLIB_QM_BASE_INTERNAL_H

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

#include "bplib_mem_rbtree.h"

struct BPLib_STOR_QM_ModuleApiContent
{
    BPLib_MEM_RBT_Link_t                rbt_link;
    BPLib_STOR_CACHE_BlocktypeApi_t     api;
    size_t                              user_content_size;
    BPLib_MEM_AlignedData_t             user_data_start;
};

// TODO typedef struct BPLib_STOR_CACHE_ClaStats
//{
//    uintmax_t ingress_byte_count;
//    uintmax_t egress_byte_count;
//
//} BPLib_STOR_CACHE_ClaStats_t;

typedef struct BPLib_STOR_CACHE_RouteServiceintfInfo
{
    bp_ipn_t          node_number;
    BPLib_MEM_RBT_Root_t  service_index;
    BPLib_STOR_CACHE_Ref_t storage_service;

} BPLib_STOR_CACHE_RouteServiceintfInfo_t;

typedef struct BPLib_STOR_CACHE_ServiceEndpt BPLib_STOR_CACHE_ServiceEndpt_t;

struct BPLib_STOR_CACHE_ServiceEndpt
{
    BPLib_MEM_RBT_Link_t      rbt_link; /* for storage in RB tree, must be first */
    BPLib_MEM_Block_t        *self_ptr;
    BPLib_STOR_CACHE_Ref_t    subflow_ref;
};

typedef struct BPLib_STOR_QM_QueueEntry
{
    bp_ipn_t    dest;
    bp_ipn_t    mask;
    bp_handle_t intf_id;
} BPLib_STOR_QM_QueueEntry_t;

struct BPLib_STOR_QM_QueueTbl
{
    uint32_t                      max_queues;
    uint32_t                      registered_queues;
    bp_handle_t                   activity_lock;
    volatile bool                 maint_request_flag;
    volatile bool                 maint_active_flag;
    uint8_t                       poll_count;
    BPLib_TIME_MonotonicTime_t    last_intf_poll;
    uintmax_t                     queueing_success_count;
    uintmax_t                     queueing_error_count;
    BPLib_STOR_CACHE_Pool_t      *pool;
    BPLib_STOR_CACHE_Block_t      duct_list;
    BPLib_STOR_QM_QueueEntry_t   *queue_tbl;
};

typedef struct bp_socket bp_socket_t;
struct bp_socket
{
    BPLib_MEM_Block_t fblk;
};

typedef struct BPLib_STOR_CACHE_SocketInfo BPLib_STOR_CACHE_SocketInfo_t;
struct BPLib_STOR_CACHE_SocketInfo
{
    struct BPLib_STOR_QM_QueueTbl  *parent_rtbl;
    bp_handle_t                     socket_intf_id;
    BPLib_STOR_CACHE_Connection_t   params;
    uintmax_t                       ingress_byte_count;
    uintmax_t                       egress_byte_count;
    bp_sequencenumber_t             last_bundle_seq;
};

#endif /* BPLIB_QM_BASE_INTERNAL_H*/
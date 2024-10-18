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

#ifndef BPLIB_STOR_CACHE_BASE_INTERNAL_H
#define BPLIB_STOR_CACHE_BASE_INTERNAL_H

#include "bplib_api_types.h"
#include "bplib_mem.h"
#include "bplib_mem_rbtree.h"

#include "bplib_time.h"

#include "bplib_stor_qm.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

typedef struct BPLib_STOR_CACHE_ClaStats
{
    uintmax_t ingress_byte_count;
    uintmax_t egress_byte_count;

}BPLib_STOR_CACHE_ClaStats_t;

typedef struct BPLib_STOR_QM_ServiceintfInfo
{
    BPLib_Ipn_t          node_number;
    BPLib_MEM_RBT_Root_t  service_index;
    BPLib_STOR_CACHE_Ref_t storage_service;

} BPLib_STOR_QM_ServiceintfInfo_t;

static inline BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_GetPool(const BPLib_STOR_QM_QueueTbl_t *svc)
{
    #ifdef QM_QUEUE_TABLE
    return svc->pool;
    #else // QM_QUEUE_TABLE
    return (BPLib_STOR_CACHE_Pool_t *) NULL;
    #endif // QM_QUEUE_TABLE 
}

int BPLib_STOR_CACHE_ServiceductForwardIngress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);
int BPLib_STOR_CACHE_ServiceductForwardEgress(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);
int BPLib_STOR_CACHE_DataserviceEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block);
int BPLib_STOR_CACHE_DataserviceBaseConstruct(void *arg, BPLib_STOR_CACHE_Block_t *blk);
int BPLib_STOR_CACHE_ClaEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block);
int BPLib_STOR_CACHE_GenericBundleIngress(BPLib_STOR_CACHE_Ref_t duct_ref, const void *content, size_t size, BPLib_TIME_MonotonicTime_t time_limit);
int BPLib_STOR_CACHE_GenericBundleEgress(BPLib_STOR_CACHE_Ref_t duct_ref, void *content, size_t *size, BPLib_TIME_MonotonicTime_t time_limit);

#endif /* BPLIB_STOR_CACHE_BASE_INTERNAL_H*/
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

#ifndef BPLIB_STOR_QM_TYPES_H
#define BPLIB_STOR_QM_TYPES_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/**
 * @brief Upper limit to how deep a single queue may ever be
 *
 * This value is large enough such that a queue can essentially be considered unlimited.
 * Note that it is counting bundles, not bytes.
 *
 * Note that in some circumstances a queue can become "overfilled" - such as if two queues
 * that were individually less than this limit are merged together.  Even in that event,
 * there will still never be any ambiguity or chance of overrunning the counters used
 * to track queue depth (which are currently 32 bit values).
 */
#define BPLIB_MAX_SUBQ_DEPTH   0x10000000
#define BPLIB_MPOOL_SHORT_SUBQ_DEPTH 0x10

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct BPLib_STOR_QM_ModuleApi BPLib_STOR_QM_ModuleApi_t;

typedef struct BPLib_STOR_QM_QueueTbl BPLib_STOR_QM_QueueTbl_t;

typedef int (*BPLib_STOR_QM_ActionFunc_t)(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Ref_t ref, void *arg);

typedef struct BPLib_STOR_QM_Job
{
    BPLib_STOR_CACHE_Block_t        link;
    BPLib_STOR_CACHE_CallbackFunc_t handler;
} BPLib_STOR_QM_Job_t;

typedef struct BPLib_STOR_QM_JobStateChange
{
    BPLib_STOR_QM_Job_t base_job;

    /* JPHFIX: this single event handler may be separated into per-event-type handlers */
    BPLib_STOR_CACHE_CallbackFunc_t event_handler;
} BPLib_STOR_QM_JobStateChange_t;

typedef struct BPLib_STOR_QM_SubqWorkitem
{
    BPLib_STOR_QM_Job_t               job_header;
    BPLib_STOR_CACHE_SubqBase_t  base_subq;
    unsigned int                 current_depth_limit;
} BPLib_STOR_QM_SubqWorkitem_t;

typedef struct BPLib_STOR_QM_Duct BPLib_STOR_QM_Duct_t;
struct BPLib_STOR_QM_Duct
{
    uint32_t pending_state_flags;
    uint32_t current_state_flags;

    BPLib_STOR_QM_JobStateChange_t statechange_job;
    BPLib_STOR_CACHE_Ref_t    parent;

    BPLib_STOR_QM_SubqWorkitem_t ingress;
    BPLib_STOR_QM_SubqWorkitem_t egress;
};

typedef struct BPLib_STOR_QM_DuctContent BPLib_STOR_QM_DuctContent_t;
struct BPLib_STOR_QM_DuctContent
{
    BPLib_STOR_QM_Duct_t                dblock;
    BPLib_MEM_AlignedData_t             user_data_start;
};

#endif /* BPLIB_STOR_QM_TYPES_H */

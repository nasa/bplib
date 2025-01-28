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

#ifndef BPLIB_QM_H
#define BPLIB_QM_H

#include "bplib_api_types.h"
#include "bplib_bundle.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_mem.h"

#define QM_NO_WAIT       WAITQUEUE_NO_WAIT
#define QM_WAIT_FOREVER  WAITQUEUE_WAIT_FOREVER

typedef enum BPLib_JobState
{
    CONTACT_IN_CLA_TO_BI = 0,
    CONTACT_IN_BI_TO_EBP,
    CONTACT_IN_EBP_TO_CT,
    CONTACT_IN_CT_TO_STOR,
    CONTACT_OUT_STOR_TO_CT,
    CONTACT_OUT_CT_TO_EBP,
    CONTACT_OUT_EBP_TO_BI,
    CONTACT_OUT_BI_TO_CLA,
    BUNDLE_STOR,
    CHANNEL_IN_ADU_TO_PI,
    CHANNEL_IN_PI_TO_EBP,
    CHANNEL_IN_EBP_TO_CT,
    CHANNEL_IN_CT_TO_STOR,
    CHANNEL_OUT_STOR_TO_CT,
    CHANNEL_OUT_CT_TO_EBP,
    CHANNEL_OUT_EBP_TO_PI,
    CHANNEL_OUT_PI_TO_ADU,
    /* Must be last */
    NUM_JOB_STATES
} BPLib_QM_JobState_t;

typedef enum BPLib_QM_Priority
{
    QM_PRI_NORMAL = 1
} BPLib_QM_Priority_t;

typedef struct BPLib_Instance
{
    BPLib_MEM_Pool_t pool;

    /* In a future ticket, the state below can go inside a TBDefined QueueTable_t */
    void* job_mem;
    BPLib_QM_WaitQueue_t GenericWorkerJobs;
    void* unsorted_job_mem;
    BPLib_QM_WaitQueue_t UnsortedJobs;
    void* contact_egress_mem;
    BPLib_QM_WaitQueue_t ContactEgressJobs;
    /* A secret note for Sara
     * void* channel_egress_mem;
     * BPLib_QM_WaitQueue_t ChannelEgressJobs;
    */
    /* End QueueTable_t */
} BPLib_Instance_t;

typedef BPLib_QM_JobState_t (*BPLib_QM_JobFunc_t)(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle);

typedef struct BPLib_QM_Job
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_JobState_t state;
    BPLib_QM_JobFunc_t job_func;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Job_t;

typedef struct BPLib_QM_UnsortedJob
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_JobState_t next_state;
    BPLib_QM_Priority_t priority;
} BPLib_QM_UnsortedJob_t;

BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t max_jobs);

void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst);

void BPLib_QM_SortJobs(BPLib_Instance_t* inst, size_t num_jobs);

void BPLib_QM_RunJob(BPLib_Instance_t* inst, int timeout_ms);

BPLib_Status_t BPLib_QM_AddUnsortedJob(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms);

#endif /* BPLIB_QM_H */

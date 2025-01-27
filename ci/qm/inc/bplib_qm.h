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

#include "bplib_bundle.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_mem.h"

#define QM_NO_WAIT       WAITQUEUE_NO_WAIT
#define QM_WAIT_FOREVER  WAITQUEUE_WAIT_FOREVER

typedef enum BPLib_JobState
{
    /* Ingress States */
    STATE_BI_IN,
    STATE_EBP_IN,
    STATE_CT_IN,
    STATE_CACHE_IN,
    STATE_PI_IN,
    /* Egress States */
    STATE_CLA_OUT,
    STATE_BI_OUT,
    STATE_EBP_OUT,
    STATE_CT_OUT,
    STATE_CACHE_OUT,
    STATE_PI_OUT,
    STATE_ADU_OUT,
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
    /* In a future ticket, the state below can become QueueTable_t */
    void* job_mem;
    BPLib_QM_WaitQueue_t jobs;
    void* event_mem;
    BPLib_QM_WaitQueue_t events;
    void* cla_out_mem;
    BPLib_QM_WaitQueue_t cla_out;
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

typedef struct BPLib_QM_Event
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_JobState_t next_state;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Event_t;

bool BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t max_jobs);

void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst);

void BPLib_QM_EventLoopAdvance(BPLib_Instance_t* inst, size_t num_jobs);

void BPLib_QM_RunJob(BPLib_Instance_t* inst, int timeout_ms);

bool BPLib_QM_PostEvent(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms);

#endif /* BPLIB_QM_H */

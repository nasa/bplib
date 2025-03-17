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

#ifndef BPLIB_QM_WORKERSTATE_H
#define BPLIB_QM_WORKERSTATE_H

#include "bplib_api_types.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_qm_job.h"

#include <pthread.h>

typedef struct BPLib_QM_WorkerState
{
    pthread_mutex_t Lock;
    pthread_cond_t Cond;
    bool HasJob;
    BPLib_QM_Job_t Job;
    BPLib_QM_JobState_t JobResult;
    int WorkerID;
    BPLib_QM_IntegerQueue_t* FreeWorkerQueue;
} BPLib_QM_WorkerState_t;

BPLib_Status_t BPLib_QM_WorkerState_Init(BPLib_QM_WorkerState_t* WorkerState, int WorkerID,
    BPLib_QM_IntegerQueue_t* FreeWorkerQueue);

BPLib_Status_t BPLib_QM_WorkerState_MarkJobDone(BPLib_QM_WorkerState_t* WorkerState,
    BPLib_QM_JobState_t JobResult);

BPLib_Status_t BPLib_QM_WorkerState_GiveNewJob(BPLib_QM_WorkerState_t* WorkerState,
    BPLib_QM_Job_t* Job);

/* The eventual goal is to have this return BPLib_Status_t.
** The complication is coalescing OS errors into status codes meaningfully.
** While it's not great to truncate everything into "true" or "false", it makes our
** calling logic simpler. Eventually it needs to be [SUCCESS, TIMEOUT, ...at least one error code]
*/
bool BPLib_QM_WorkerState_WaitForNewJob(BPLib_QM_WorkerState_t* WorkerState, int TimeoutMs);

#endif /* BPLIB_QM_WORKERSTATE_H */

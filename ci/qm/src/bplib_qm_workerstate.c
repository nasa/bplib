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
#include "bplib_qm_workerstate.h"
#include "bplib_qm.h"

#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

static void ms_to_abstimeout(uint32_t ms, struct timespec *ts)
{
    if (ts == NULL)
    {
        return;
    }

    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += ms / 1000;
    /* TODO: Can this math overflow ? I think so... */
    ts->tv_nsec += (ms % 1000 * 1000000);
    ts->tv_sec += ts->tv_nsec / 1000000000;
    ts->tv_nsec %= 1000000000;
}

BPLib_Status_t BPLib_QM_WorkerState_Init(BPLib_QM_WorkerState_t* WorkerState, int WorkerID,
    BPLib_QM_IntegerQueue_t* FreeWorkerQueue)
{
    if ((WorkerState == NULL) || (FreeWorkerQueue == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_init(&WorkerState->Lock, NULL);
    pthread_cond_init(&WorkerState->Cond, NULL);
    WorkerState->HasJob = false;
    WorkerState->WorkerID = WorkerID;
    WorkerState->FreeWorkerQueue = FreeWorkerQueue;
    memset(&WorkerState->Job, 0, sizeof(BPLib_QM_Job_t));
    WorkerState->JobResult = NO_NEXT_STATE;

    if (!BPLib_QM_IntegerQueueTryPush(WorkerState->FreeWorkerQueue, WorkerState->WorkerID, QM_NO_WAIT))
    {
        return BPLIB_QM_PUSH_ERROR;
    }
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QM_WorkerState_GiveNewJob(BPLib_QM_WorkerState_t* WorkerState,
    BPLib_QM_Job_t* Job)
{
    if ((WorkerState == NULL) || (Job == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_lock(&WorkerState->Lock);
    memcpy(&WorkerState->Job, Job, sizeof(BPLib_QM_Job_t));
    WorkerState->HasJob = true;
    pthread_cond_signal(&WorkerState->Cond);
    pthread_mutex_unlock(&WorkerState->Lock);

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QM_WorkerState_MarkJobDone(BPLib_QM_WorkerState_t* WorkerState,
    BPLib_QM_JobState_t JobResult)
{
    if (WorkerState == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_lock(&WorkerState->Lock);
    WorkerState->HasJob = false;
    WorkerState->JobResult = JobResult;
    pthread_mutex_unlock(&WorkerState->Lock);

    if (!BPLib_QM_IntegerQueueTryPush(WorkerState->FreeWorkerQueue, WorkerState->WorkerID, QM_NO_WAIT))
    {
        return BPLIB_QM_PUSH_ERROR;
    }
    return BPLIB_SUCCESS;
}

bool BPLib_QM_WorkerState_WaitForNewJob(BPLib_QM_WorkerState_t* WorkerState, int TimeoutMs)
{
    struct timespec Deadline;
    int rc;

    if (WorkerState == NULL)
    {
        return false;
    }

    ms_to_abstimeout((uint32_t)TimeoutMs, &Deadline);
    pthread_mutex_lock(&WorkerState->Lock);
    while (WorkerState->HasJob == false)
    {
        rc = pthread_cond_timedwait(&WorkerState->Cond, &WorkerState->Lock, &Deadline);
        if (rc != 0)
        {
            if (rc != ETIMEDOUT)
            {
                fprintf(stderr, "BPLib_QM_Worker_WaitForNewJob NON-TIMEOUT ERROR: %s\n",
                    strerror(rc));
            }
            pthread_mutex_unlock(&WorkerState->Lock);
            return false;
        }
    }
    pthread_mutex_unlock(&WorkerState->Lock);
    return true;
}

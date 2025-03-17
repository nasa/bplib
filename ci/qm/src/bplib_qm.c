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

#include "bplib_qm.h"
#include "bplib_qm_job.h"
#include "bplib_pl.h"
#include "bplib_qm_workerstate.h"

#include <stdio.h>
#include <assert.h>

#define BPLIB_QM_RUNJOB_PERF_ID  0x7F
#define BPLIB_QM_JOBWAIT_TIMEOUT 1L

BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t MaxJobs)
{
    bool QueueInit;
    uint16_t i;

    if (inst == NULL)
    {
        return BPLIB_ERROR;
    }

    inst->NumWorkers = 0;

    /* Initialize the jobs and unsorted jobs queue */
    QueueInit = true;
    if (!BPLib_QM_WaitQueueInit(&(inst->GenericWorkerJobs), sizeof(BPLib_QM_Job_t), MaxJobs))
    {
        QueueInit = false;
    }

    /* TODO: This is a BundleQueue, but is kept as WaitQueueInit in expectation this code is
    ** soon to be removed. I feel it's best to just leave this alone for now.
    */
    if (!BPLib_QM_WaitQueueInit(&(inst->BundleCacheList), sizeof(BPLib_Bundle_t*), MaxJobs))
    {
        QueueInit = false;
    }

    for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
    {
        if (!BPLib_QM_WaitQueueInit(&(inst->ChannelEgressJobs[i]), sizeof(BPLib_Bundle_t*), MaxJobs))
        {
            QueueInit = false;
        }
    }

    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        if (!BPLib_QM_WaitQueueInit(&(inst->ContactEgressJobs[i]), sizeof(BPLib_Bundle_t*), MaxJobs))
        {
            QueueInit = false;
        }
    }

    if (QueueInit == false)
    {
        BPLib_QM_QueueTableDestroy(inst);
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst)
{
    uint16_t i;

    if (inst == NULL)
    {
        return;
    }

    BPLib_QM_WaitQueueDestroy(&(inst->GenericWorkerJobs));
    BPLib_QM_WaitQueueDestroy(&(inst->BundleCacheList));

    for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
    {
        BPLib_QM_WaitQueueDestroy(&(inst->ChannelEgressJobs[i]));
    }

    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        BPLib_QM_WaitQueueDestroy(&(inst->ContactEgressJobs[i]));
    }
}

BPLib_Status_t BPLib_QM_RegisterWorker(BPLib_Instance_t* inst, int* WorkerID)
{
    BPLib_Status_t Status;
    int NewWorkerID;

    if ((inst == NULL) || (WorkerID == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (inst->NumWorkers == QM_MAX_GEN_WORKERS)
    {
        return BPLIB_ERROR;
    }

    NewWorkerID = inst->NumWorkers;
    Status = BPLib_QM_WorkerState_Init(&inst->RegisteredWorkers[NewWorkerID], NewWorkerID,
        &inst->FreeWorkers);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    inst->NumWorkers++;
    *WorkerID = NewWorkerID;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QM_CreateJob(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int TimeoutMs)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_QM_Job_t NewJob;

    NewJob.Bundle = bundle;
    NewJob.NextState = state;
    NewJob.Priority = priority;
    
    if (!BPLib_QM_WaitQueueTryPush(&(inst->GenericWorkerJobs), &NewJob, TimeoutMs))
    {
        Status = BPLIB_QM_PUSH_ERROR;
    }

    return Status;
}

void BPLib_QM_WorkerRunJob(BPLib_Instance_t* inst, int WorkerID, int TimeoutMs)
{
    BPLib_QM_WorkerState_t* WorkerState;
    BPLib_QM_JobState_t JobResult;
    BPLib_QM_JobFunc_t JobFunc;
    BPLib_Status_t Status;

    if (inst == NULL)
    {
        return;
    }
    if ((WorkerID < 0) || (WorkerID >= inst->NumWorkers))
    {
        printf("Invalid Worker ID\n"); // REMOVE BEFORE MERGE
        return;
    }

    WorkerState = &inst->RegisteredWorkers[WorkerID];
    if (BPLib_QM_WorkerState_WaitForNewJob(WorkerState, TimeoutMs))
    {
        JobFunc = BPLib_QM_JobLookup(WorkerState->Job.NextState);
        assert(JobFunc != NULL); // REMOVE BEFORE MERGE
        JobResult = JobFunc(inst, WorkerState->Job.Bundle);
        Status = BPLib_QM_WorkerState_MarkJobDone(WorkerState, JobResult);
        if (Status != BPLIB_SUCCESS)
        {
            fprintf(stderr, "Worker Failed to MarkJobDone\n");
        }
    }
}

void BPLib_QM_ScheduleJobs(BPLib_Instance_t* inst, size_t MaxJobs)
{
    size_t JobsScheduled;
    int CurrWorkerID;
    BPLib_QM_Job_t CurrJob;
    BPLib_QM_WorkerState_t* WorkerState;
    BPLib_Status_t Status;

    if (inst == NULL)
    {
        return;
    }

    JobsScheduled = 0;
    while (JobsScheduled < MaxJobs)
    {
        /* Wait for a free worker */
        if (BPLib_QM_IntegerQueueTryPull(&(inst->FreeWorkers), &CurrWorkerID, BPLIB_QM_JOBWAIT_TIMEOUT))
        {
            /* Lookup the worker state associated with the CurrWorkerID */
            WorkerState = &inst->RegisteredWorkers[CurrWorkerID];
            assert(WorkerState->HasJob == false); // REMOVE BEFORE MERGE
            if (WorkerState->JobResult == NO_NEXT_STATE)
            {
                /* The worker has NO_NEXT_STATE, it can be given a new job */
                if (BPLib_QM_WaitQueueTryPull(&(inst->GenericWorkerJobs), &CurrJob, BPLIB_QM_JOBWAIT_TIMEOUT))
                {
                    Status = BPLib_QM_WorkerState_GiveNewJob(WorkerState, &CurrJob);
                    assert(Status == BPLIB_SUCCESS); // REMOVE BEFORE MERGE
                    JobsScheduled++;
                }
                else
                {
                    /* No job was available, return the worker ID to the free worker queue */
                    if (BPLib_QM_IntegerQueueTryPush(&(inst->FreeWorkers), CurrWorkerID, QM_NO_WAIT) == false)
                    {
                        /* This is a fatal condition: The worker queue should always be able to
                        ** hold a worker because it's been sized for MAX_WORKERS
                        */
                        fprintf(stderr, "Failed to return idle worker to free queue\n");
                        return;
                    }
                }
            }
            else
            {
                /* The worker has a next state, update the job with the next hop. */
                /* Give the worker the same job with the next state advanced */
                WorkerState->Job.NextState = WorkerState->JobResult;
                Status = BPLib_QM_WorkerState_GiveNewJob(WorkerState, &WorkerState->Job);
                assert(Status == BPLIB_SUCCESS); // REMOVE BEFORE MERGE
                JobsScheduled++;
            }
        }
    }

    /* Consider returning JobsScheduled here */
}

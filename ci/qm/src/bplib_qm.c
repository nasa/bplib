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
#include "bplib_stor.h"
#include "bplib_cla.h"
#include "bplib_nc.h"

#include <stdio.h>

#define BPLIB_QM_RUNJOB_PERF_ID  0x7F

BPLib_Status_t BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t MaxJobs)
{
    bool QueueInit;
    int i;
    BPLib_Status_t Status;

    if (inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Initialize worker registration state */
    if (pthread_mutex_init(&inst->RegisteredWorkersLock, NULL) != 0)
    {
        return BPLIB_QM_INIT_ERROR;
    }

    /* Init Cache */
    Status = BPLib_STOR_Init(inst);
    if (Status != BPLIB_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize Cache\n");
        return BPLIB_QM_INIT_ERROR;
    }

    /* Setup Worker State */
    for (i = 0; i < QM_MAX_GEN_WORKERS; i++)
    {
        memset(&inst->RegisteredWorkers[i].CurrJob, 0, sizeof(BPLib_QM_Job_t));
        inst->RegisteredWorkers[i].CurrJob.NextState = NO_NEXT_STATE;
    }

    QueueInit = true;

    /* Initialize the job queue */
    if (!BPLib_QM_WaitQueueInit(&(inst->GenericWorkerJobs), sizeof(BPLib_QM_Job_t), MaxJobs))
    {
        QueueInit = false;
    }

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

    /* Worker State Cleanup */
    pthread_mutex_destroy(&inst->RegisteredWorkersLock);
    inst->NumWorkers = 0;

    /* Queue Cleanup */
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

BPLib_Status_t BPLib_QM_RegisterWorker(BPLib_Instance_t* inst, int32_t* WorkerID)
{
    int NewWorkerID;
    BPLib_Status_t Status;

    if ((inst == NULL) || (WorkerID == NULL))
    {
        *WorkerID = -1;
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_lock(&inst->RegisteredWorkersLock);
    if (inst->NumWorkers == QM_MAX_GEN_WORKERS)
    {
        *WorkerID = -1;
        Status = BPLIB_QM_WRKR_REGISTER_ERROR;
    }
    else
    {
        NewWorkerID = inst->NumWorkers;
        inst->NumWorkers++;
        *WorkerID = NewWorkerID;
        Status = BPLIB_SUCCESS;
    }
    pthread_mutex_unlock(&inst->RegisteredWorkersLock);

    return Status;
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

BPLib_Status_t BPLib_QM_WorkerRunJob(BPLib_Instance_t* inst, int32_t WorkerID, int TimeoutMs)
{
    BPLib_QM_WorkerState_t* WorkerState;
    BPLib_QM_JobFunc_t JobFunc;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if (inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if ((WorkerID < 0) || (WorkerID >= inst->NumWorkers))
    {
        return BPLIB_QM_BAD_WRKR_ID;
    }

    /* Note: WorkerState is a resource shared amongst threads, but each worker state
    ** is separated by the 'WorkerID' parameter.  This is is given at startup and
    ** will not be changed for the duration of the run, nor will other threads be accessing
    ** the specific WorkerID's WorkerState. This state is not locked because it would introduce
    ** significant loss of concurrency as the worker threads compete for the array, just to
    ** access their own state within it.
    */
    WorkerState = &inst->RegisteredWorkers[WorkerID];
    if (WorkerState->CurrJob.NextState == NO_NEXT_STATE)
    {
        if (BPLib_QM_WaitQueueTryPull(&(inst->GenericWorkerJobs), &WorkerState->CurrJob, TimeoutMs))
        {
            JobFunc = BPLib_QM_JobLookup(WorkerState->CurrJob.NextState);
            WorkerState->CurrJob.NextState = JobFunc(inst, WorkerState->CurrJob.Bundle);
            Status = BPLIB_SUCCESS;
        }
        else
        {
            Status = BPLIB_TIMEOUT;
        }
    }
    else
    {
        JobFunc = BPLib_QM_JobLookup(WorkerState->CurrJob.NextState);
        WorkerState->CurrJob.NextState = JobFunc(inst, WorkerState->CurrJob.Bundle);
        Status = BPLIB_SUCCESS;
    }

    return Status;
}

bool BPLib_QM_IsIngressIdle(BPLib_Instance_t* Inst)
{
    if (Inst == NULL)
    {
        return true;
    }

    return BPLib_QM_WaitQueueIsEmpty(&(Inst->GenericWorkerJobs));
}

bool BPLib_QM_IsDuctEmpty(BPLib_Instance_t* Inst, int EgressID, bool LocalDelivery)
{
    BPLib_QM_WaitQueue_t* DuctQueue;

    if (Inst == NULL)
    {
        /* NULL PTR ERROR will be caught in DuctPull */
        return true;
    }

    /* Determine which queue to pull from */
    if (LocalDelivery == true)
    {
        DuctQueue = &(Inst->ChannelEgressJobs[EgressID]);
    }
    else
    {
        DuctQueue = &(Inst->ContactEgressJobs[EgressID]);
    }

    return BPLib_QM_WaitQueueIsEmpty(DuctQueue);
}

BPLib_Status_t BPLib_QM_DuctPull(BPLib_Instance_t* Inst, int EgressID, bool LocalDelivery,
    int TimeoutMs, BPLib_Bundle_t** RetBundle)
{
    BPLib_Bundle_t* Bundle;
    BPLib_QM_JobState_t CurrState;
    BPLib_QM_JobFunc_t JobFunc;
    BPLib_QM_WaitQueue_t* DuctQueue;
    BPLib_CLA_ContactRunState_t ContactState;
    bool DuctActive = false;
    BPLib_Status_t Status = BPLIB_SUCCESS;
    size_t NumStoredEgressed = 0;

    if (RetBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    *RetBundle = NULL;

    /* Determine which queue to pull from */
    if (LocalDelivery == true)
    {
        CurrState = CHANNEL_OUT_STOR_TO_CT;
        DuctQueue = &(Inst->ChannelEgressJobs[EgressID]);
        DuctActive = (BPLib_NC_GetAppState(EgressID) == BPLIB_NC_APP_STATE_STARTED);
    }
    else
    {
        CurrState = CONTACT_OUT_STOR_TO_CT;
        DuctQueue = &(Inst->ContactEgressJobs[EgressID]);
        (void) BPLib_CLA_GetContactRunState(EgressID, &ContactState);
        DuctActive = (ContactState == BPLIB_CLA_STARTED);
    }

    /* If the duct is empty, try to load more from storage */
    if ((BPLib_QM_IsDuctEmpty(Inst, EgressID, LocalDelivery) == true) && DuctActive)
    {
        Status = BPLib_STOR_EgressForID(Inst, EgressID, false, &NumStoredEgressed);
        if (Status != BPLIB_SUCCESS)
        {
            return Status;
        }
    }

    /* Pull the bundle from the queue and push it to the 'edge' of BPA 
    ** Note: There's no check for DuctActive here to support the case where bundles
    ** remain in the queue after a Channel or Contact is stopped.
    */
    if (BPLib_QM_WaitQueueTryPull(DuctQueue, &Bundle, TimeoutMs))
    {
        /* Take this bundle all the way to NO_NEXT_STATE */
        while (CurrState != NO_NEXT_STATE)
        {
            JobFunc = BPLib_QM_JobLookup(CurrState);
            CurrState = JobFunc(Inst, Bundle);
        }
        *RetBundle = Bundle;
        return BPLIB_SUCCESS;
    }
    else
    {
        return BPLIB_TIMEOUT;
    }
}

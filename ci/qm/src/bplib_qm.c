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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BPLIB_QM_RUNJOB_PERF_ID 0x7F
#define BPLIB_QM_JOBWAIT_TIMEOUT 1L

bool BPLib_QM_QueueTableInit(BPLib_Instance_t* inst, size_t max_jobs)
{
    bool queue_init;

    if (inst == NULL)
    {
        return false;
    }

    /* This is a one-time allocation when BPLib is initialized
    ** Note: We have decided to modify this so that job_mem and unsorted_job_mem are passed
    **  in as parameters this function. This will be done in a future ticket.
    */
    inst->job_mem = calloc(max_jobs, sizeof(BPLib_QM_Job_t));
    inst->unsorted_job_mem = calloc(max_jobs, sizeof(BPLib_QM_UnsortedJob_t));
    inst->contact_egress_mem = calloc(max_jobs, sizeof(BPLib_Bundle_t *));
    if (inst->unsorted_job_mem == NULL)
    {
        free(inst->job_mem);
        free(inst->unsorted_job_mem);
        free(inst->contact_egress_mem);
        return false;
    }

    /* Initialize the jobs and unsorted jobs queue */
    queue_init = true;
    if (!BPLib_QM_WaitQueueInit(&(inst->GenericWorkerJobs), inst->job_mem, sizeof(BPLib_QM_Job_t), max_jobs))
    {
        queue_init = false;
    }
    if (!BPLib_QM_WaitQueueInit(&(inst->UnsortedJobs), inst->unsorted_job_mem, sizeof(BPLib_QM_UnsortedJob_t), max_jobs))
    {
        queue_init = false;
    }
    if (!BPLib_QM_WaitQueueInit(&(inst->ContactEgressJobs), inst->contact_egress_mem, sizeof(BPLib_Bundle_t*), max_jobs))
    {
        queue_init = false;
    }
    if (!queue_init)
    {
        free(inst->job_mem);
        free(inst->unsorted_job_mem);
        free(inst->contact_egress_mem);
        return false;
    }

    return true;
}

void BPLib_QM_QueueTableDestroy(BPLib_Instance_t* inst)
{
    if (inst == NULL)
    {
        return;
    }

    BPLib_QM_WaitQueueDestroy(&(inst->GenericWorkerJobs));
    BPLib_QM_WaitQueueDestroy(&(inst->UnsortedJobs));
    BPLib_QM_WaitQueueDestroy(&(inst->ContactEgressJobs));
    free(inst->job_mem);
    free(inst->unsorted_job_mem);
    free(inst->contact_egress_mem);
}

bool BPLib_QM_AddUnsortedJob(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms)
{
    BPLib_QM_UnsortedJob_t unsorted_job;
    unsorted_job.bundle = bundle;
    unsorted_job.next_state = state;
    unsorted_job.priority = priority;
    return BPLib_QM_WaitQueueTryPush(&(inst->UnsortedJobs), &unsorted_job, timeout_ms);
}

void BPLib_QM_RunJob(BPLib_Instance_t* inst, int timeout_ms)
{
    BPLib_QM_Job_t curr_job;
    BPLib_QM_JobState_t next_state;

    if (BPLib_QM_WaitQueueTryPull(&(inst->GenericWorkerJobs), &curr_job, timeout_ms))
    {
        /* Run the job and get back the next state */
        BPLib_PL_PerfLogEntry(BPLIB_QM_RUNJOB_PERF_ID);
        next_state = curr_job.job_func(inst, curr_job.bundle);
        BPLib_PL_PerfLogExit(BPLIB_QM_RUNJOB_PERF_ID);

        /* Create a new unsorted job with the next state and place it in the unsorted jobs queue
        ** Important note here:
        **  Should a worker fail to place the job in the unsorted jobs queue, this
        **  may indicate a system that's over-tasked. There are several ways to define what 
        **  our node's behavior should be here:
        **      1) we could put this jobs in an overflow queue, then create a notificatino that the system degraded.
        **      2) we could selectively drop "less important" UnsortedJob_t based on their next_state or priority.
        **         This means bundles would be dropped, which I'm guessing we dont want
        **      3) we could block and wait until this job can be pushed.
        **
        **  I think it could be a good idea to make the unsorted jobs queue larger than the jobs queue so that this
        **  case is infrequent.
        */
        BPLib_QM_AddUnsortedJob(inst, curr_job.bundle, next_state, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }
}


void BPLib_QM_SortJobs(BPLib_Instance_t* inst, size_t num_jobs)
{
    size_t jobs_scheduled;
    BPLib_QM_Job_t curr_job;
    BPLib_QM_UnsortedJob_t unsorted_job;

    jobs_scheduled = 0;
    while (jobs_scheduled < num_jobs)
    {
        if (BPLib_QM_WaitQueueTryPull(&(inst->UnsortedJobs), &unsorted_job, BPLIB_QM_JOBWAIT_TIMEOUT))
        {
            if (unsorted_job.next_state == STATE_CLA_OUT)
            {
                BPLib_QM_WaitQueueTryPush(&(inst->ContactEgressJobs), &unsorted_job.bundle, WAITQUEUE_WAIT_FOREVER);
            }
            else if (unsorted_job.next_state == STATE_ADU_OUT)
            {
                /* Implement something similar to CLA Out State */
            }
            else
            {
                /* Create a new job for the unsorted job and place it in the generic worker jobs queue */
                curr_job.bundle = unsorted_job.bundle;
                curr_job.state = unsorted_job.next_state;
                curr_job.job_func = BPLib_QM_Job_Lookup(unsorted_job.next_state);
                assert(curr_job.job_func != NULL);
                curr_job.priority = unsorted_job.priority;

                /* Add the job to the job queue so a worker can discover it 
                ** Note: There is no backpressuring logic right now so this can block indefintely.
                **  This will only do so if the system is clogged or misconfigured. 
                **  We will have to remove the 'WAITQUEUE_WAIT_FOREVER` and replace it 
                **  with a backpressuring strategy, but it should be done in a future ticket.
                */
                BPLib_QM_WaitQueueTryPush(&(inst->GenericWorkerJobs), &curr_job, WAITQUEUE_WAIT_FOREVER);
                jobs_scheduled++;
            }
        }
        else
        {
            break;
        }
    }

    return;
}

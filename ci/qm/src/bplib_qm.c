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
#define BPLIB_QM_EVT_TIMEOUT_MAX_MS 1L

bool BPLib_QM_QueueTableInit(BPLib_QM_QueueTable_t* tbl, size_t max_jobs)
{
    bool queue_init;

    if (tbl == NULL)
    {
        return false;
    }

    /* Belongs in BPLib_Init(), but we don't have that function. */
    BPLib_QM_Job_TableInit();

    /* This is a one-time allocation when BPLib is initialized
    ** Note: We have decided to modify this so that job_mem and event_mem are passed
    **  in as parameters this function. This will be done in a future ticket.
    */
    tbl->job_mem = calloc(max_jobs, sizeof(BPLib_QM_Job_t));
    tbl->event_mem = calloc(max_jobs, sizeof(BPLib_QM_Event_t));
    tbl->cla_out_mem = calloc(max_jobs, sizeof(BPLib_Bundle_t *));
    if (tbl->event_mem == NULL)
    {
        free(tbl->job_mem);
        free(tbl->event_mem);
        free(tbl->cla_out_mem);
        return false;
    }

    /* Initialize the job and event queue  */
    queue_init = true;
    if (!BPLib_QM_WaitQueueInit(&(tbl->jobs), tbl->job_mem, sizeof(BPLib_QM_Job_t), max_jobs))
    {
        queue_init = false;
    }
    if (!BPLib_QM_WaitQueueInit(&(tbl->events), tbl->event_mem, sizeof(BPLib_QM_Event_t), max_jobs))
    {
        queue_init = false;
    }
    if (!BPLib_QM_WaitQueueInit(&(tbl->cla_out), tbl->cla_out_mem, sizeof(BPLib_Bundle_t*), max_jobs))
    {
        queue_init = false;
    }
    if (!queue_init)
    {
        free(tbl->job_mem);
        free(tbl->event_mem);
        free(tbl->cla_out_mem);
        return false;
    }

    return true;
}

void BPLib_QM_QueueTableDestroy(BPLib_QM_QueueTable_t* tbl)
{
    if (tbl == NULL)
    {
        return;
    }

    BPLib_QM_WaitQueueDestroy(&(tbl->jobs));
    BPLib_QM_WaitQueueDestroy(&(tbl->events));
    BPLib_QM_WaitQueueDestroy(&(tbl->cla_out));
    free(tbl->job_mem);
    free(tbl->event_mem);
    free(tbl->cla_out_mem);
}

bool BPLib_QM_PostEvent(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms)
{
    BPLib_QM_Event_t event;
    event.bundle = bundle;
    event.next_state = state;
    event.priority = priority;
    return BPLib_QM_WaitQueueTryPush(&(tbl->events), &event, timeout_ms);
}

void BPLib_QM_RunJob(BPLib_QM_QueueTable_t* tbl, int timeout_ms)
{
    BPLib_QM_Job_t curr_job;
    BPLib_QM_JobState_t next_state;

    if (BPLib_QM_WaitQueueTryPull(&(tbl->jobs), &curr_job, timeout_ms))
    {
        /* Run the job and get back the next state */
        BPLib_PL_PerfLogEntry(BPLIB_QM_RUNJOB_PERF_ID);
        next_state = curr_job.job_func(tbl, curr_job.bundle);
        BPLib_PL_PerfLogExit(BPLIB_QM_RUNJOB_PERF_ID);

        /* Create a new event with the next state and post it to the event loop 
        ** Important note here:
        **  Should a worker fail to post an event back to the event queue, this
        **  may indicate a system that's over-tasked. There are several ways to define what 
        **  our node's behavior should be here:
        **      1) we could post these event to an overflow queue, then notify the event loop that the system degraded.
        **      2) we could selectively drop "less important" events based on their next_state or priority.
        **         This means bundles would be dropped, which I'm guessing we dont want
        **      3) we could block and wait until this event can be pushed.
        **
        **  I think it could be a good idea to make the event queue larger than the jobs queue so that this
        **  case is infrequent.
        */
        BPLib_QM_PostEvent(tbl, curr_job.bundle, next_state, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }
}


void BPLib_QM_EventLoopAdvance(BPLib_QM_QueueTable_t* tbl, size_t num_jobs)
{
    size_t jobs_scheduled;
    BPLib_QM_Job_t curr_job;
    BPLib_QM_Event_t curr_event;

    jobs_scheduled = 0;
    while (jobs_scheduled < num_jobs)
    {
        if (BPLib_QM_WaitQueueTryPull(&(tbl->events), &curr_event, BPLIB_QM_EVT_TIMEOUT_MAX_MS))
        {
            if (curr_event.next_state == STATE_CLA_OUT)
            {
                BPLib_QM_WaitQueueTryPush(&(tbl->cla_out), &curr_event.bundle, WAITQUEUE_WAIT_FOREVER);
            }
            else if (curr_event.next_state == STATE_ADU_OUT)
            {
                /* Implement something similar to CLA Out State */
            }
            else
            {
                /* Construct a new job for this event */
                curr_job.bundle = curr_event.bundle;
                curr_job.state = curr_event.next_state;
                curr_job.job_func = BPLib_QM_Job_Lookup(curr_event.next_state);
                assert(curr_job.job_func != NULL);
                curr_job.priority = curr_event.priority;

                /* Add the job to the job queue so a worker can discover it 
                ** Note: There is no backpressuring logic right now so this can block indefintely.
                **  This will only do so if the system is clogged or misconfigured. 
                **  We will have to remove the 'WAITQUEUE_WAIT_FOREVER` and replace it 
                **  with a backpressuring strategy, but it should be done in a future ticket.
                */
                BPLib_QM_WaitQueueTryPush(&(tbl->jobs), &curr_job, WAITQUEUE_WAIT_FOREVER);
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

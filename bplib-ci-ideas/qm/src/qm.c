#include "qm.h"
#include "qm_jobs.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct BPLib_QM_Event
{
    BPLib_Bundle_t* bundle;
    BPLib_QM_JobState_t next_state;
    BPLib_QM_Priority_t priority;
} BPLib_QM_Event_t;

bool BPLib_QM_QueueTableInit(BPLib_QM_QueueTable_t* tbl, size_t max_jobs)
{
    bool queue_init;

    if (tbl == NULL)
    {
        return false;
    }

    /* Belongs in BPLib_Init(), but this demo doesn't have that function */
    BPLib_QM_JobTableInit();

    /* This is one-time allocation when BPLib is initialized
    ** Note:
    **   Because the underlying waitqueues accept a void* for underlying storage,
    **   this function can be modified to allow the caller to pass in queue storage
    **   if this one-time calloc() call is undesirable. calloc() could also be moved
    **   within the waitqueue init function (my preference).
    */
    tbl->job_mem = calloc(max_jobs, sizeof(BPLib_QM_Job_t));
    if (tbl->job_mem == NULL)
    {
        return false;
    }
    tbl->event_mem = calloc(max_jobs, sizeof(BPLib_QM_Event_t));
    if (tbl->event_mem == NULL)
    {
        free(tbl->job_mem);
        return false;
    }

    /* Initialize the job and event queue  */
    queue_init = true;
    if (!BPLib_CI_WaitQueueInit(&(tbl->jobs), tbl->job_mem, sizeof(BPLib_QM_Job_t), max_jobs))
    {
        queue_init = false;
    }
    if (!BPLib_CI_WaitQueueInit(&(tbl->events), tbl->event_mem, sizeof(BPLib_QM_Event_t), max_jobs))
    {
        queue_init = false;
    }
    if (!queue_init)
    {
        free(tbl->job_mem);
        free(tbl->event_mem);
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

    BPLib_CI_WaitQueueDestroy(&(tbl->jobs));
    BPLib_CI_WaitQueueDestroy(&(tbl->events));
    free(tbl->job_mem);
    free(tbl->event_mem);
}

bool BPLib_QM_PostEvent(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle,
    BPLib_QM_JobState_t state, BPLib_QM_Priority_t priority, int timeout_ms)
{
    BPLib_QM_Event_t event;
    event.bundle = bundle;
    event.next_state = state;
    event.priority = priority;
    return BPLib_CI_WaitQueueTryPush(&(tbl->events), &event, timeout_ms);
}

void* BPLib_QM_EventLoop(BPLib_QM_QueueTable_t* tbl, bool* exit_flag)
{
    int queue_timeout_ms;
    BPLib_QM_Job_t curr_job;
    BPLib_QM_Event_t curr_event;

    queue_timeout_ms = 100;

    /* Right now this loop makes the assumption that if an event can be pulled, 
    ** then the job queue has open space.  That may not be true in practice and
    ** this logic should be refactored to represent that.  This simplification
    ** was done for demo purposes.
    */
    while ((*exit_flag) == true)
    {
        if (BPLib_CI_WaitQueueTryPull(&(tbl->events), &curr_event, queue_timeout_ms))
        {
            /* Construct a new job for this event:
            ** Herein, lies the beauty of separate event and job queues. You can
            ** 'manage' events that were posted from a single observer.  This allows
            ** for future implementation of QOS, backpresure, load balancing, NON-generic tasks,
            ** BPAccel, etc.
            */
            curr_job.bundle = curr_event.bundle;
            curr_job.state = curr_event.next_state;
            curr_job.job_func = job_funcs[curr_event.next_state];
            assert(curr_job.job_func != NULL);
            curr_job.priority = curr_event.priority;

            /* Add the job to the job queue so a worker can discover it */
            BPLib_CI_WaitQueueTryPush(&(tbl->jobs), &curr_job, WAITQUEUE_BLOCK_FOREVER);
        }
    }

    printf("Exiting event loop\n");
    return NULL;
}

void BPLib_QM_RunJob(BPLib_QM_QueueTable_t* tbl, int timeout_ms)
{
    BPLib_QM_Job_t curr_job;
    BPLib_QM_Event_t new_event;
    BPLib_QM_JobState_t next_state;

    if (BPLib_CI_WaitQueueTryPull(&(tbl->jobs), &curr_job, timeout_ms))
    {
        /* Run the job and get back the next state */
        next_state = curr_job.job_func(tbl, curr_job.bundle);

        /* Create a new event with the next state and post it to the event loop 
        ** Important note here:
        **  Should a worker fail to post an event back to the event queue, this
        **  may foreshadow a system that's over-tasked.  You 
        **    
        */
        if (BPLib_QM_PostEvent(tbl, curr_job.bundle, next_state, QM_PRI_NORMAL, QM_NOWAIT) == false)
        {
            /* Send an event message at the least */
            printf("System clogged\n");
        }
    }
}

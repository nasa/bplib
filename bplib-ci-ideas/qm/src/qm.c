#include "qm.h"

#include <stdio.h>
#include <stdlib.h>

bool all_kinds_of_jobs(BPLib_Bundle_t* bundle)
{
    printf("Sure I'm good at all kinds of jobs\n");

    // Post Event.
}

bool BPLib_QM_QueueTableInit(BPLib_QM_QueueTable_t* tbl, size_t max_jobs)
{
    bool queue_init;

    if (tbl == NULL)
    {
        return false;
    }

    /* This is one-time allocation when BPLib is initialized
    ** Note:
    **   Because the underlying waitqueues accept a void* for underlying storage,
    **   this function can be modified to allow the caller to pass in queue storage
    **   incase this one-time calloc() call is undesirable. calloc could also be moved
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
    while ((*exit_flag) != false)
    {
        printf("Waiting for an event to be posted\n");
        if (BPLib_CI_WaitQueueTryPull(&(tbl->events), &curr_event, queue_timeout_ms))
        {
            /* Construct a new job for this event:
            ** Herein, lies the beauty of separate event and job queues: You can
            ** 'manage' events that were posted from a single observer.  This allows
            ** for future implementation of QOS or other random system-engineery 
            ** features we can't possibly account for.
            */
            curr_job.bundle = curr_event.bundle;
            curr_job.priority = curr_event.priority;
            curr_job.job_func = all_kinds_of_jobs;

            /* Note this blocks forever, this is unlikely to be the right solution
            ** long term.
            */
            BPLib_CI_WaitQueueTryPush(&(tbl->jobs), &curr_job, WAITQUEUE_BLOCK_FOREVER);
        }
    }

    printf("Exiting event loop\n");
    return NULL;
}

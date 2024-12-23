#ifndef __GSFC_BPLIB_JOB_H__
#define __GSFC_BPLIB_JOB_H__

#include "bundle.h"

typedef struct BPLib_JobNotif
{
    // A job is a queue reference
    // and eventually a priority
} BPLib_JobNotif_t;

typedef BPLib_JobFunc_t;

bool BPLib_CI_Job_Run(BPLib_Job_t* job)
{
    Bundle* b;

    if (job == NULL)
    {
        return false;
    }

    /* Queue TryPullNoWait */
    // If you can't immediatley pull from the queue, somethings wrong
    // with our design. The assumption is that there's one job per queue push
    // return false;

    b->job_func();

}

#endif /* __GSFC_BPLIB_JOB_H__ */

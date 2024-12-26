#include "qm_jobs.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

static bool jobs_table_init_done = false;

BPLib_QM_JobState_t BPJob_QM_ClaToBi(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CLA TO BI\n");
    sleep(1);
    return STATE_BI_TO_CLA;
}

BPLib_QM_JobState_t BPJob_QM_BiToCla(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("BI TO CLA\n");
    sleep(1);
    return STATE_CLA_TO_BI;
}

void BPLib_QM_JobTableInit()
{
    if (jobs_table_init_done)
    {
        return;
    }

    memset(job_funcs, 0, sizeof(job_funcs));

    job_funcs[STATE_CLA_TO_BI] = BPJob_QM_ClaToBi;
    job_funcs[STATE_BI_TO_CLA] = BPJob_QM_BiToCla;

    jobs_table_init_done = true;
}

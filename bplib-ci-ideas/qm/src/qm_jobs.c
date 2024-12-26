#include "qm_jobs.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

static bool jobs_table_init_done = false;

BPLib_QM_JobState_t BPLib_Job_CLA_TO_BI(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CLA TO BI\n");
    sleep(1);
    return STATE_BI_TO_EBP;
}

BPLib_QM_JobState_t BPLib_Job_BI_TO_EBP(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("BI TO EBP\n");
    sleep(1);
    return STATE_EBP_TO_CT;
}

BPLib_QM_JobState_t BPLib_Job_EBP_TO_CT(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("EBP TO CT\n");
    sleep(1);
    return STATE_CT_TO_CACHE;
}

BPLib_QM_JobState_t BPLib_Job_CT_TO_CACHE(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CT TO CACHE\n");
    sleep(1);
    return STATE_CACHE_TO_CT;
}

BPLib_QM_JobState_t BPLib_Job_CACHE_TO_CT(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CACHE TO CT\n");
    sleep(1);
    return STATE_CT_TO_EBP;
}

BPLib_QM_JobState_t BPLib_Job_CT_TO_EBP(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CT TO EBP\n");
    sleep(1);
    return STATE_EBP_TO_BI;
}

BPLib_QM_JobState_t BPLib_Job_EBP_TO_BI(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("EBP TO BI\n");
    sleep(1);
    return STATE_BI_TO_CLA;
}

BPLib_QM_JobState_t BPLib_Job_BI_TO_CLA(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
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
    job_funcs[STATE_CLA_TO_BI] = BPLib_Job_CLA_TO_BI;
    job_funcs[STATE_BI_TO_EBP] = BPLib_Job_BI_TO_EBP;
    job_funcs[STATE_EBP_TO_CT] = BPLib_Job_EBP_TO_CT;
    job_funcs[STATE_CT_TO_CACHE] = BPLib_Job_CT_TO_CACHE;
    job_funcs[STATE_CACHE_TO_CT] = BPLib_Job_CACHE_TO_CT;
    job_funcs[STATE_CT_TO_EBP] = BPLib_Job_CT_TO_EBP;
    job_funcs[STATE_EBP_TO_BI] = BPLib_Job_EBP_TO_BI;
    job_funcs[STATE_BI_TO_CLA] = BPLib_Job_BI_TO_CLA;

    /* In a codebase with BPLib_Init(), we don't need this bool. */
    jobs_table_init_done = true;
}

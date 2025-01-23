#include "bplib_qm_job.h"

#include <stdio.h>
#include <string.h>

static bool jobs_table_init_done = false;
BPLib_QM_JobFunc_t job_funcs[NUM_JOB_STATES];

/* 
** Job Functions - These are the entry points to jobs being run in QM.
*/
BPLib_QM_JobState_t BPLib_Job_BI_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_EBP_IN;
}

BPLib_QM_JobState_t BPLib_Job_EBP_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_CT_IN;
}

BPLib_QM_JobState_t BPLib_Job_CT_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_CACHE_IN;
}

BPLib_QM_JobState_t BPLib_Job_CACHE_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_PI_OUT;
}

BPLib_QM_JobState_t BPLib_Job_PI_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_CACHE_IN;
}

BPLib_QM_JobState_t BPLib_Job_BI_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_CLA_OUT;
}

BPLib_QM_JobState_t BPLib_Job_EBP_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_BI_OUT;
}

BPLib_QM_JobState_t BPLib_Job_CT_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_EBP_OUT;
}

BPLib_QM_JobState_t BPLib_Job_CACHE_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    return STATE_CT_OUT;
}

BPLib_QM_JobState_t BPLib_Job_PI_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("PI EGRESS REACHED\n");
    return STATE_ADU_OUT;
}

void BPLib_QM_JobTableInit()
{
    if (jobs_table_init_done)
    {
        return;
    }

    memset(job_funcs, 0, sizeof(job_funcs));
    job_funcs[STATE_BI_IN] = BPLib_Job_BI_Ingress;
    job_funcs[STATE_EBP_IN] = BPLib_Job_EBP_Ingress;
    job_funcs[STATE_CT_IN] = BPLib_Job_CT_Ingress;
    job_funcs[STATE_CACHE_IN] = BPLib_Job_CACHE_Ingress;
    job_funcs[STATE_PI_IN] = BPLib_Job_PI_Ingress;
    job_funcs[STATE_BI_OUT] = BPLib_Job_BI_Egress;
    job_funcs[STATE_EBP_OUT] = BPLib_Job_EBP_Egress;
    job_funcs[STATE_CT_OUT] = BPLib_Job_CT_Egress;
    job_funcs[STATE_CACHE_OUT] = BPLib_Job_CACHE_Egress;
    job_funcs[STATE_PI_OUT] = BPLib_Job_PI_Egress;

    /* In a codebase with BPLib_Init(), we would just call JobTableInit() */
    jobs_table_init_done = true;
}

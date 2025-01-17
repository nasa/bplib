#include "qm_jobs.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

static bool jobs_table_init_done = false;

/* 
** Sandwiches - These are the entry points to jobs being run in QM.
*/
BPLib_QM_JobState_t BPLib_Job_BI_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CLA TO BI %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_BI_TO_EBP;
}

BPLib_QM_JobState_t BPLib_Job_EBP_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("BI TO EBP %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_EBP_TO_CT;
}

BPLib_QM_JobState_t BPLib_Job_CT_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("EBP TO CT %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_CT_TO_CACHE;
}

BPLib_QM_JobState_t BPLib_Job_CACHE_Ingress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CT TO CACHE %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_CACHE_TO_CT;
}

BPLib_QM_JobState_t BPLib_Job_CACHE_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CACHE TO CT %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_CT_TO_EBP;
}

BPLib_QM_JobState_t BPLib_Job_CT_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("CT TO EBP %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_EBP_TO_BI;
}

BPLib_QM_JobState_t BPLib_Job_EBP_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("EBP TO BI %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_BI_TO_CLA;
}

BPLib_QM_JobState_t BPLib_Job_BI_Egress(BPLib_QM_QueueTable_t* tbl, BPLib_Bundle_t* bundle)
{
    printf("BI TO CLA %lu\n", bundle->blocks.pri_blk.src_eid.node_number);
    sleep(1);
    return STATE_CLA_TO_BI;
}

void BPLib_QM_JobTableInit()
{
    if (jobs_table_init_done)
    {
        return;
    }

    /* job_funcs is an array of sandwhich-pointers */
    memset(job_funcs, 0, sizeof(job_funcs));
    job_funcs[STATE_CLA_TO_BI] = BPLib_Job_BI_Ingress;
    job_funcs[STATE_BI_TO_EBP] = BPLib_Job_EBP_Ingress;
    job_funcs[STATE_EBP_TO_CT] = BPLib_Job_CT_Ingress;
    job_funcs[STATE_CT_TO_CACHE] = BPLib_Job_CACHE_Ingress;
    job_funcs[STATE_CACHE_TO_CT] = BPLib_Job_CACHE_Egress;
    job_funcs[STATE_CT_TO_EBP] = BPLib_Job_CT_Egress;
    job_funcs[STATE_EBP_TO_BI] = BPLib_Job_EBP_Egress;
    job_funcs[STATE_BI_TO_CLA] = BPLib_Job_BI_Egress;

    /* In a codebase with BPLib_Init(), we would just call JobTableInit() */
    jobs_table_init_done = true;
}

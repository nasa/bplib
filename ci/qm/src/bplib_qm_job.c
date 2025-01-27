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
    //return STATE_PI_OUT; // Go back to CLA here
    return STATE_EBP_OUT;
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

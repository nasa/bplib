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

/* 
** Job Functions - These are the entry points to jobs being run in QM.
*/
static BPLib_QM_JobState_t BPLib_Job_BI_Ingress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_EBP_IN;
}

static BPLib_QM_JobState_t BPLib_Job_EBP_Ingress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_CT_IN;
}

static BPLib_QM_JobState_t BPLib_Job_CT_Ingress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_CACHE_IN;
}

static BPLib_QM_JobState_t BPLib_Job_CACHE_Ingress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    //return STATE_PI_OUT; // Go back to CLA here
    return STATE_EBP_OUT;
}

static BPLib_QM_JobState_t BPLib_Job_PI_Ingress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_CACHE_IN;
}

static BPLib_QM_JobState_t BPLib_Job_BI_Egress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_CLA_OUT;
}

static BPLib_QM_JobState_t BPLib_Job_EBP_Egress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_BI_OUT;
}

static BPLib_QM_JobState_t BPLib_Job_CT_Egress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_EBP_OUT;
}

static BPLib_QM_JobState_t BPLib_Job_CACHE_Egress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return STATE_CT_OUT;
}

static BPLib_QM_JobState_t BPLib_Job_PI_Egress(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    printf("PI EGRESS REACHED\n");
    return STATE_ADU_OUT;
}

static const BPLib_QM_JobFunc_t job_funcs[NUM_JOB_STATES] =
{
    [STATE_BI_IN] = BPLib_Job_BI_Ingress,
    [STATE_EBP_IN] = BPLib_Job_EBP_Ingress,
    [STATE_CT_IN] = BPLib_Job_CT_Ingress,
    [STATE_CACHE_IN] = BPLib_Job_CACHE_Ingress,
    [STATE_PI_IN] = BPLib_Job_PI_Ingress,
    [STATE_BI_OUT] = BPLib_Job_BI_Egress,
    [STATE_EBP_OUT] = BPLib_Job_EBP_Egress,
    [STATE_CT_OUT] = BPLib_Job_CT_Egress,
    [STATE_CACHE_OUT] = BPLib_Job_CACHE_Egress,
    [STATE_PI_OUT] = BPLib_Job_PI_Egress
};

BPLib_QM_JobFunc_t BPLib_QM_Job_Lookup(BPLib_QM_JobState_t job_state)
{
    if ((job_state >= 0) && (job_state < NUM_JOB_STATES))
    {
        return job_funcs[job_state];
    }
    return NULL;
}

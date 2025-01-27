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
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Job Functions - These are the entry points to jobs being run within QM.
*/
static BPLib_QM_JobState_t ContactIn_CLA_TO_BI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_IN_BI_TO_EBP;
}

static BPLib_QM_JobState_t ContactIn_BI_TO_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ContactIn_EBP_TO_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_IN_CT_TO_STOR;
}

static BPLib_QM_JobState_t ContactIn_CT_TO_STOR(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Note: Looping back to ADU Out Path */
    return CONTACT_OUT_STOR_TO_CT;
}

static BPLib_QM_JobState_t ContactOut_STOR_TO_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_OUT_CT_TO_EBP;
}

static BPLib_QM_JobState_t ContactOut_CT_TO_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_OUT_EBP_TO_BI;
}

static BPLib_QM_JobState_t ContactOut_EBP_TO_BI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_OUT_BI_TO_CLA;
}

static BPLib_QM_JobState_t ContactOut_BI_TO_CLA(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* FIXME: Our loop doesn't currently use this state. */
    abort();
    return 0;
}

static BPLib_QM_JobState_t BundleStorage(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Nothing should be hooked up to Bundle Stor/Cache yet */
    abort();
    return 0;
}

static BPLib_QM_JobState_t ChannelIn_ADU_TO_PI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_IN_PI_TO_EBP;
}

static BPLib_QM_JobState_t ChannelIn_PI_TO_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ChannelIn_EBP_TO_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_IN_CT_TO_STOR;
}

static BPLib_QM_JobState_t ChannelIn_CT_TO_STOR(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Note: Looping back to ADU Out Path */
    return CHANNEL_OUT_STOR_TO_CT;
}

static BPLib_QM_JobState_t ChanneOut_STOR_TO_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_OUT_CT_TO_EBP;
}

static BPLib_QM_JobState_t ChanneOut_CT_TO_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_OUT_EBP_TO_PI;
}

static BPLib_QM_JobState_t ChanneOut_EBP_TO_PI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_OUT_PI_TO_ADU;
}

static BPLib_QM_JobState_t ChanneOut_PI_TO_ADU(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* FIXME: Our loop doesn't currently use this state. */
    abort();
    return 0;
}

/*******************************************************************************
 * JobState_t to JobFunc_t mapping
*/
static const BPLib_QM_JobFunc_t job_funcs[NUM_JOB_STATES] =
{
    [CONTACT_IN_CLA_TO_BI] = ContactIn_CLA_TO_BI,
    [CONTACT_IN_BI_TO_EBP] = ContactIn_BI_TO_EBP,
    [CONTACT_IN_EBP_TO_CT] = ContactIn_EBP_TO_CT,
    [CONTACT_IN_CT_TO_STOR] = ContactIn_CT_TO_STOR,
    [CONTACT_OUT_STOR_TO_CT] = ContactOut_STOR_TO_CT,
    [CONTACT_OUT_CT_TO_EBP] = ContactOut_CT_TO_EBP,
    [CONTACT_OUT_EBP_TO_BI] = ContactOut_EBP_TO_BI,
    [CONTACT_OUT_BI_TO_CLA] = ContactOut_BI_TO_CLA,
    [BUNDLE_STOR] = BundleStorage,
    [CHANNEL_IN_ADU_TO_PI] = ChannelIn_ADU_TO_PI,
    [CHANNEL_IN_PI_TO_EBP] = ChannelIn_PI_TO_EBP,
    [CHANNEL_IN_EBP_TO_CT] = ChannelIn_EBP_TO_CT,
    [CHANNEL_IN_CT_TO_STOR] = ChannelIn_CT_TO_STOR,
    [CHANNEL_OUT_STOR_TO_CT] = ChanneOut_STOR_TO_CT,
    [CHANNEL_OUT_CT_TO_EBP] = ChanneOut_CT_TO_EBP,
    [CHANNEL_OUT_EBP_TO_PI] = ChanneOut_EBP_TO_PI,
    [CHANNEL_OUT_PI_TO_ADU] = ChanneOut_PI_TO_ADU,
};

BPLib_QM_JobFunc_t BPLib_QM_Job_Lookup(BPLib_QM_JobState_t job_state)
{
    if ((job_state >= 0) && (job_state < NUM_JOB_STATES))
    {
        return job_funcs[job_state];
    }
    return NULL;
}

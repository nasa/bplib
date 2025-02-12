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
#include "bplib_bi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Job Functions - These are the entry points to jobs being run within QM.
*/
static BPLib_QM_JobState_t ContactIn_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ContactIn_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Note: Looping back to Contact Out Path */
    return CONTACT_IN_CT_TO_STOR;
    // return CONTACT_OUT_STOR_TO_CT;
}

static BPLib_QM_JobState_t ContactOut_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_OUT_CT_TO_EBP;
}

static BPLib_QM_JobState_t ContactOut_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CONTACT_OUT_EBP_TO_BI;
}

static BPLib_QM_JobState_t ContactOut_BI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    BPLib_QM_WaitQueueTryPush(&(inst->ContactEgressJobs), &bundle, QM_WAIT_FOREVER);
    return NO_NEXT_STATE;
}

static BPLib_QM_JobState_t ChannelIn_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ChannelIn_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Note: Looping back to ADU Out Path */
    // return CHANNEL_OUT_STOR_TO_CT;
    return CHANNEL_IN_CT_TO_STOR;
}

static BPLib_QM_JobState_t ChannelOut_CT(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_OUT_CT_TO_EBP;
}


static BPLib_QM_JobState_t ChannelOut_EBP(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    return CHANNEL_OUT_EBP_TO_PI;
}

static BPLib_QM_JobState_t ChannelOut_PI(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Hacky attempt at routing just to prove concept, FIX ME */
    if (bundle->blocks.pri_blk.dest_eid.service_number == 0x42)
    {
        BPLib_QM_WaitQueueTryPush(&(inst->ChannelEgressJobs[0]), &bundle, QM_WAIT_FOREVER);
    }
    else
    {
        BPLib_QM_WaitQueueTryPush(&(inst->ChannelEgressJobs[1]), &bundle, QM_WAIT_FOREVER);
    }
    
    return NO_NEXT_STATE;
}

static BPLib_QM_JobState_t STOR_Cache(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    bool QueuePushReturnStatus;
    printf("STOR_Cache received bundle with Dest EID: \"ipn:%lu.%lu\".\n",
        bundle->blocks.pri_blk.dest_eid.node_number,
        bundle->blocks.pri_blk.dest_eid.service_number);

    QueuePushReturnStatus = BPLib_QM_WaitQueueTryPush(&(inst->BundleCacheList), &bundle, QM_WAIT_FOREVER);
    if (QueuePushReturnStatus == false)
    {
        printf("STOR_Cache failed BPLib_QM_WaitQueueTryPush\n");
    }

    return NO_NEXT_STATE;
}

/*******************************************************************************
 * JobState_t to JobFunc_t mapping
*/
static const BPLib_QM_JobFunc_t job_funcs[NUM_GENWORKER_STATES] =
{
    [CONTACT_IN_BI_TO_EBP] = ContactIn_EBP,
    [CONTACT_IN_EBP_TO_CT] = ContactIn_CT,
    [CONTACT_IN_CT_TO_STOR] = STOR_Cache,
    [CONTACT_OUT_STOR_TO_CT] = ContactOut_CT,
    [CONTACT_OUT_CT_TO_EBP] = ContactOut_EBP,
    [CONTACT_OUT_EBP_TO_BI] = ContactOut_BI,
    [CHANNEL_IN_PI_TO_EBP] = ChannelIn_EBP,
    [CHANNEL_IN_EBP_TO_CT] = ChannelIn_CT,
    [CHANNEL_IN_CT_TO_STOR] = STOR_Cache,
    [CHANNEL_OUT_STOR_TO_CT] = ChannelOut_CT,
    [CHANNEL_OUT_CT_TO_EBP] = ChannelOut_EBP,
    [CHANNEL_OUT_EBP_TO_PI] = ChannelOut_PI,
};

BPLib_QM_JobFunc_t BPLib_QM_Job_Lookup(BPLib_QM_JobState_t job_state)
{
    if ((job_state >= 0) && (job_state < NUM_GENWORKER_STATES))
    {
        return job_funcs[job_state];
    }
    return NULL;
}

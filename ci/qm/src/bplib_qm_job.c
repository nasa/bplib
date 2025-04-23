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
#include "bplib_qm.h"
#include "bplib_qm_waitqueue.h"
#include "bplib_bi.h"
#include "bplib_as.h"
#include "bplib_eid.h"
#include "bplib_nc.h"
#include "bplib_ebp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Job Functions - These are the entry points to jobs being run within QM.
*/
static BPLib_QM_JobState_t ContactIn_EBP(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CONTACT_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ContactIn_CT(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CONTACT_IN_CT_TO_STOR;
}

static BPLib_QM_JobState_t ContactOut_CT(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CONTACT_OUT_CT_TO_EBP;
}

static BPLib_QM_JobState_t ContactOut_EBP(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_Status_t Status;
    
    Status = BPLib_EBP_UpdateExtensionBlocks(Bundle);

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);
        BPLib_EM_SendEvent(BPLIB_QM_EBP_OUT_ERR_EID, BPLib_EM_EventType_ERROR, 
                "Error updating extension blocks, Status = %d.", Status);

        return NO_NEXT_STATE;
    }

    return CONTACT_OUT_EBP_TO_BI;
}

static BPLib_QM_JobState_t ContactOut_BI(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_QM_WaitQueueTryPush(&(Inst->ContactEgressJobs[Bundle->Meta.EgressID]), &Bundle, QM_WAIT_FOREVER);
    return NO_NEXT_STATE;
}

static BPLib_QM_JobState_t ChannelIn_EBP(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CHANNEL_IN_EBP_TO_CT;
}

static BPLib_QM_JobState_t ChannelIn_CT(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CHANNEL_IN_CT_TO_STOR;
}

static BPLib_QM_JobState_t ChannelOut_CT(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CHANNEL_OUT_CT_TO_EBP;
}

static BPLib_QM_JobState_t ChannelOut_EBP(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    return CHANNEL_OUT_EBP_TO_PI;
}

static BPLib_QM_JobState_t ChannelOut_PI(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_QM_WaitQueueTryPush(&(Inst->ChannelEgressJobs[Bundle->Meta.EgressID]), &Bundle, QM_WAIT_FOREVER);
    return NO_NEXT_STATE;
}

static BPLib_QM_JobState_t STOR_Router(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    int i, j;
    BPLib_EID_t* DestEID;

    BPLib_NC_ReaderLock();

    /* For build 7.0 our ingress route strategy is as follows:
    ** - If the bundle is local, forward to the channel immediatley
    ** - If the bundle is for an available contact, deliver without storing
    ** - If the bundle if for an un-available contact or channel, store
    */
    DestEID = &Bundle->blocks.PrimaryBlock.DestEID;
    if (BPLib_EID_NodeIsMatch(DestEID, &BPLIB_EID_INSTANCE))
    {
        /* Go through each channel and find a match */
        for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
        {
            if (BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[i].LocalServiceNumber == DestEID->Service)
            {
                if (BPLib_NC_GetAppState(i) == BPLIB_NC_APP_STATE_STARTED)
                {
                    /* We have a channel we can deliver to: forward without storing */
                    Bundle->Meta.EgressID = i;
                    BPLib_NC_ReaderUnlock();
                    return CHANNEL_OUT_STOR_TO_CT;
                }
            }
        }
    }
    else
    {
        /* Iterate through the contacts: this is very slow. */
        for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
        {
            for (j = 0; j < BPLIB_MAX_CONTACT_DEST_EIDS; j++)
            {
                /* Code not available: BPLib_NC_GetContactState(i) to check if contact active */
                if (BPLib_EID_PatternIsMatch(DestEID, &BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[i].DestEIDs[j]))
                {
                    Bundle->Meta.EgressID = i;
                    BPLib_NC_ReaderUnlock();
                    return CONTACT_OUT_STOR_TO_CT;
                }
            }
        }
    }

    /* We never found an active channel or contact: store this bundle */
    BPLib_STOR_StoreBundle(Inst, Bundle);

    BPLib_NC_ReaderUnlock();
    return NO_NEXT_STATE;
}

/*******************************************************************************
 * JobState_t to JobFunc_t mapping
*/
static const BPLib_QM_JobFunc_t job_funcs[NUM_GENWORKER_STATES] =
{
    [CONTACT_IN_BI_TO_EBP] = ContactIn_EBP,
    [CONTACT_IN_EBP_TO_CT] = ContactIn_CT,
    [CONTACT_IN_CT_TO_STOR] = STOR_Router,
    [CONTACT_OUT_STOR_TO_CT] = ContactOut_CT,
    [CONTACT_OUT_CT_TO_EBP] = ContactOut_EBP,
    [CONTACT_OUT_EBP_TO_BI] = ContactOut_BI,
    [CHANNEL_IN_PI_TO_EBP] = ChannelIn_EBP,
    [CHANNEL_IN_EBP_TO_CT] = ChannelIn_CT,
    [CHANNEL_IN_CT_TO_STOR] = STOR_Router,
    [CHANNEL_OUT_STOR_TO_CT] = ChannelOut_CT,
    [CHANNEL_OUT_CT_TO_EBP] = ChannelOut_EBP,
    [CHANNEL_OUT_EBP_TO_PI] = ChannelOut_PI,
};

BPLib_QM_JobFunc_t BPLib_QM_JobLookup(BPLib_QM_JobState_t job_state)
{
    if ((job_state >= 0) && (job_state < NUM_GENWORKER_STATES))
    {
        return job_funcs[job_state];
    }
    /* Placing this here avoids performing the check for each job */
    fprintf(stderr, "Warning: Returning NULL from BPLib_QM_JobLookup\n");
    return NULL;
}

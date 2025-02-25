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

/*
** Include
*/
#include "bplib_bi.h"
#include "bplib_cbor.h"
#include "bplib_qm.h"
#include "bplib_bblocks.h"

#include <stdio.h>

/*
** Function Definitions
*/

int BPLib_BI_Init(void) {
    return BPLIB_SUCCESS;
}

/* Receive candidate bundle from CLA, CBOR decode it, then place it to EBP In Queue */
BPLib_Status_t BPLib_BI_RecvFullBundleIn(BPLib_Instance_t* inst, const void *BundleIn, size_t Size)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_Bundle_t* CandidateBundle;

    // TODO the implementation example for "bundle" has it on the stack.
    BPLib_Bundle_t bundle;

    if ((inst == NULL) || (BundleIn == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Create the bundle from the incoming blob */
    CandidateBundle = BPLib_MEM_BundleAlloc(&inst->pool, BundleIn, Size);
    if (CandidateBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* TODO: CBOR Decode the bundle and return the deserialized bundle pointer */
    /* TODO: fully fill out primary block fields from decoded bundle */
    BPLib_CBOR_DecodeBundle(CandidateBundle, Size, &bundle);

    bundle.blocks.PrimaryBlock.BundleProcFlags = BPLIB_BUNDLE_PROTOCOL_VERSION;

    if (Size == BPLIB_TEMPORARY_BUNDLE_SIZE_FOR_CHAN_DELIVERY_HACK_0)
    {
        /* this will route it to channel 0 egress, after cache */
        bundle.blocks.PrimaryBlock.DestEID.Node = BPLIB_TEMPORARY_EID_NODE_NUM_FOR_CHANNEL_ROUTES;
        bundle.blocks.PrimaryBlock.DestEID.Service = BPLIB_TEMPORARY_EID_SERVICE_NUM_FOR_CHANNEL_0_ROUTES;
    }
    else if (Size == BPLIB_TEMPORARY_BUNDLE_SIZE_FOR_CHAN_DELIVERY_HACK_1)
    {
        /* this will route it to channel 1 egress, after cache */
        bundle.blocks.PrimaryBlock.DestEID.Node = BPLIB_TEMPORARY_EID_NODE_NUM_FOR_CHANNEL_ROUTES;
        bundle.blocks.PrimaryBlock.DestEID.Service = BPLIB_TEMPORARY_EID_SERVICE_NUM_FOR_CHANNEL_1_ROUTES;
    }
    else
    {
        /* this will route it back to the CLA egress, after cache */
        bundle.blocks.PrimaryBlock.DestEID.Node = BPLIB_TEMPORARY_EID_NODE_NUM_FOR_CONTACT_ROUTES;
        bundle.blocks.PrimaryBlock.DestEID.Service = BPLIB_TEMPORARY_EID_SERVICE_NUM_FOR_CONTACT_ROUTES;
    }

    /* Validate the deserialized bundle (this does nothing right now) */
    Status = BPLib_BI_ValidateBundle();

    printf("Ingressing packet of %lu bytes from CLA, jamming deserialized Dest EID to %lu.%lu\n",
        (unsigned long)Size,
        bundle.blocks.PrimaryBlock.DestEID.Node,
        bundle.blocks.PrimaryBlock.DestEID.Service);

    // TODO Allocate and copy bundle off the stack?
    BPLib_QM_AddUnsortedJob(inst, &bundle, CONTACT_IN_BI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    return Status;
}

/* Receive Control Messages from CLA, pass them to CT*/
BPLib_Status_t BPLib_BI_RecvCtrlMsg(BPLib_CLA_CtrlMsg_t* MsgPtr)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Pass the control message to CT */
    /* Call a CT function here */
    /* BPLib_CT_RecvCtrlMsg() */
    
    return Status;
}

/* Validate deserialized bundle after CBOR decoding*/
BPLib_Status_t BPLib_BI_ValidateBundle(void)
{
    /* Check bundle version = 7 */
    
    /* Check against Policy Database for authorized source EID */
    
    /* Check for block number, duplicate extension block, like Age, Hop Count, Previous Node */
    
    return BPLIB_SUCCESS;
}

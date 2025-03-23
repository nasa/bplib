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
#include "bplib_as.h"
#include "bplib_eid.h"
#include "bplib_bblocks.h"

#include <stdio.h>

/*
** Function Definitions
*/

/* Receive candidate bundle from CLA, CBOR decode it, then place it to EBP In Queue */
BPLib_Status_t BPLib_BI_RecvFullBundleIn(BPLib_Instance_t* inst, const void *BundleIn, size_t Size)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_Status_t DecodeStatus;
    BPLib_Bundle_t* CandidateBundle;

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

    DecodeStatus = BPLib_CBOR_DecodeBundle(BundleIn, Size, CandidateBundle);
    if (DecodeStatus != BPLIB_SUCCESS)
    {
        /* cease bundle processing and free the memory */
        BPLib_MEM_BundleFree(&inst->pool, CandidateBundle);

        BPLib_EM_SendEvent(BPLIB_BI_INGRESS_CBOR_DECODE_ERR_EID,
            BPLib_EM_EventType_ERROR,
            "Error decoding bundle, RC = %d", DecodeStatus);

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_UNINTELLIGIBLE, 1);

        return DecodeStatus;
    }

    /* Validate the deserialized bundle (this does nothing right now) */
    Status = BPLib_BI_ValidateBundle();
    /* TODO check this return once validate is actually implemented */

    BPLib_EM_SendEvent(BPLIB_BI_INGRESS_DBG_EID, BPLib_EM_EventType_DEBUG,
                "Ingressing %lu-byte bundle from CLA, with Dest EID: %lu.%lu, and Src EID: %lu.%lu.",
                (unsigned long) Size,
                CandidateBundle->blocks.PrimaryBlock.DestEID.Node,
                CandidateBundle->blocks.PrimaryBlock.DestEID.Service,
                CandidateBundle->blocks.PrimaryBlock.SrcEID.Node,
                CandidateBundle->blocks.PrimaryBlock.SrcEID.Service);

    Status = BPLib_QM_AddUnsortedJob(inst, CandidateBundle, CONTACT_IN_BI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
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


BPLib_Status_t BPLib_BI_BlobCopyOut(BPLib_Bundle_t* StoredBundle,
                                    void* OutputBuffer,
                                    size_t OutputBufferSize,
                                    size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;

    if ((StoredBundle == NULL) || (StoredBundle->blob == NULL) || (OutputBuffer == NULL) || (NumBytesCopied == NULL))
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        ReturnStatus = BPLib_CBOR_EncodeBundle(StoredBundle, OutputBuffer, OutputBufferSize, NumBytesCopied);
    }

    return ReturnStatus;
}

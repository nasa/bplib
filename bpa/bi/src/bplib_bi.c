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
    BPLib_Status_t Status;
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

    Status = BPLib_CBOR_DecodeBundle(BundleIn, Size, CandidateBundle);
    if (Status != BPLIB_SUCCESS)
    {
        /* cease bundle processing and free the memory */
        BPLib_MEM_BundleFree(&inst->pool, CandidateBundle);

        BPLib_EM_SendEvent(BPLIB_BI_INGRESS_CBOR_DECODE_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Error decoding bundle, RC = %d", Status);

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_UNINTELLIGIBLE, 1);

        return Status;
    }

    /* Validate the deserialized bundle */
    Status = BPLib_BI_ValidateBundle(CandidateBundle);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_MEM_BundleFree(&inst->pool, CandidateBundle);

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_UNINTELLIGIBLE, 1);
        
        return Status;
    }

    /* Get reception time of bundle */
    BPLib_TIME_GetMonotonicTime(&(NewBundle->Meta.MonoTime));
    
    BPLib_EM_SendEvent(BPLIB_BI_INGRESS_DBG_EID, BPLib_EM_EventType_DEBUG,
                "Ingressing %lu-byte bundle from CLA, with Dest EID: %lu.%lu, and Src EID: %lu.%lu.",
                (unsigned long) Size,
                CandidateBundle->blocks.PrimaryBlock.DestEID.Node,
                CandidateBundle->blocks.PrimaryBlock.DestEID.Service,
                CandidateBundle->blocks.PrimaryBlock.SrcEID.Node,
                CandidateBundle->blocks.PrimaryBlock.SrcEID.Service);

    Status = BPLib_QM_CreateJob(inst, CandidateBundle, CONTACT_IN_BI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_MEM_BundleFree(&inst->pool, CandidateBundle);
    }

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

/* Validate deserialized bundle after CBOR decoding */
BPLib_Status_t BPLib_BI_ValidateBundle(BPLib_Bundle_t *CandidateBundle)
{
    uint32_t ExtBlkIdx       = 0;
    bool     PrevNodePresent = false;
    bool     AgeBlockPresent = false;
    bool     HopCountPresent = false;

    /* 
    ** Most validation is done in CBOR as fields are being decoded. The remaining
    ** validation done here is for anything that requires a fully decoded bundle to
    ** determine.
    */

    /* Verify a payload is present */
    if (bundle->blocks.PayloadHeader.BlockType != BPLib_BlockType_Payload)
    {
        return BPLIB_BI_INVALID_BUNDLE_ERR;
    }

    /* Verify no extension block is duplicated */
    for (ExtBlkIdx = 0; ExtBlkIdx < BPLIB_MAX_NUM_EXTENSION_BLOCKS; ExtBlkIdx++)
    {
        if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_HopCount)
        {
            if (HopCountPresent)
            {
                return BPLIB_BI_INVALID_BUNDLE_ERR;
            }
            else
            {
                HopCountPresent = true;
            }
        }
        if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_Age)
        {
            if (AgeBlockPresent)
            {
                return BPLIB_BI_INVALID_BUNDLE_ERR;
            }
            else
            {
                AgeBlockPresent = true;
            }
        }
        if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_HopCount)
        {
            if (HopCountPresent)
            {
                return BPLIB_BI_INVALID_BUNDLE_ERR;
            }
            else
            {
                HopCountPresent = true;
            }
        }
    }

    /* Verify that there is either an age block or a valid DTN timestamp */
    if (AgeBlockPresent == false && CandidateBundle->blocks.PrimaryBlock.Timestamp.CreateTime == 0)
    {
        return BPLIB_BI_INVALID_BUNDLE_ERR;
    }

    /* TODO Check against Policy Database for authorized source EID, etc */
    
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

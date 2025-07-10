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
BPLib_Status_t BPLib_BI_RecvFullBundleIn(BPLib_Instance_t* Inst, const void *BundleIn, 
                                            size_t Size, uint32_t ContId)
{
    BPLib_Status_t Status;
    BPLib_Bundle_t* CandidateBundle;

    if ((Inst == NULL) || (BundleIn == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (ContId >= BPLIB_MAX_NUM_CONTACTS)
    {
        return BPLIB_INVALID_CONT_ID_ERR;
    }

    /* Create the bundle from the incoming blob */
    CandidateBundle = BPLib_MEM_BundleAlloc(&Inst->pool, BundleIn, Size);
    if (CandidateBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Get reception time of bundle */
    CandidateBundle->blocks.PrimaryBlock.MonoTime.Time = BPLib_TIME_GetMonotonicTime();
    CandidateBundle->blocks.PrimaryBlock.MonoTime.BootEra = BPLib_TIME_GetBootEra();

    /* Decode the bundle */
    Status = BPLib_CBOR_DecodeBundle(BundleIn, Size, CandidateBundle);

    /* If decode was successful, try validating the bundle */
    if (Status == BPLIB_SUCCESS)
    {
        Status = BPLib_BI_ValidateBundle(CandidateBundle);
    }

    /* Increment the case-specific counter for the failure of either decode or validation */
    if (Status == BPLIB_CBOR_DEC_BUNDLE_TOO_LONG_DEC_ERR)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_TOO_LONG, 1);
    }
    else if (Status == BPLIB_CBOR_DEC_HOP_BLOCK_EXCEEDED_ERR)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_HOP_EXCEEDED, 1);
    }
    else if (Status == BPLIB_CBOR_DEC_UNKNOWN_BLOCK_DEC_ERR)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_UNSUPPORTED_BLOCK, 1);
    }
    else if (Status == BPLIB_BI_EXPIRED_BUNDLE_ERR)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_EXPIRED, 1);
    }
    else if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_UNINTELLIGIBLE, 1);
    }

    /* If decode and validation were successful, create the job to ingress bundle */
    if (Status == BPLIB_SUCCESS)
    {
        Status = BPLib_QM_CreateJob(Inst, CandidateBundle, CONTACT_IN_BI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }
    
    /* If something failed, cease bundle processing and free memory */
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_MEM_BundleFree(&Inst->pool, CandidateBundle);

        BPLib_EM_SendEvent(BPLIB_BI_INGRESS_CBOR_DECODE_INF_EID, BPLib_EM_EventType_INFORMATION,
                            "[CLA In #%d]: Error ingressing bundle, RC = %d", ContId, Status);

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_RECEIVED, 1);
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
    uint64_t AgeBlkTime      = 0;
    uint32_t ExtBlkIdx       = 0;
    uint32_t i;
    bool     PrevNodePresent = false;
    bool     AgeBlockPresent = false;
    bool     HopCountPresent = false;

    if (CandidateBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* 
    ** Most validation is done in CBOR as fields are being decoded. The remaining
    ** validation done here is for anything that requires a fully decoded bundle to
    ** determine.
    */

    /* Verify a payload is present */
    if (CandidateBundle->blocks.PayloadHeader.BlockType != BPLib_BlockType_Payload)
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
                AgeBlkTime = CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].BlockData.AgeBlockData.Age;
            }
        }
        if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_PrevNode)
        {
            if (PrevNodePresent)
            {
                return BPLIB_BI_INVALID_BUNDLE_ERR;
            }
            else
            {
                PrevNodePresent = true;
            }
        }

        /* Verify that no block number is duplicated */
        if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType != BPLib_BlockType_Reserved)
        {
            if (CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockNum == 
                CandidateBundle->blocks.PayloadHeader.BlockNum)
            {
                return BPLIB_BI_INVALID_BUNDLE_ERR;
            }
            
            for (i = 0; i < ExtBlkIdx; i++)
            {
                if (CandidateBundle->blocks.ExtBlocks[i].Header.BlockNum == 
                    CandidateBundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockNum)
                {
                    return BPLIB_BI_INVALID_BUNDLE_ERR;
                }
            }
        }
    }

    /* If an age block is present, make sure the bundle is not expired */
    if (AgeBlockPresent && CandidateBundle->blocks.PrimaryBlock.Timestamp.CreateTime == 0)
    {
        if ((BPLib_TIME_GetMonotonicTime() - CandidateBundle->blocks.PrimaryBlock.MonoTime.Time + AgeBlkTime) >= 
             CandidateBundle->blocks.PrimaryBlock.Lifetime)
        {
            return BPLIB_BI_EXPIRED_BUNDLE_ERR;
        }
    }
    else
    {
        /* If there is neither an age block or a valid creation time, return error */
        if (CandidateBundle->blocks.PrimaryBlock.Timestamp.CreateTime == 0)
        {
            return BPLIB_BI_INVALID_BUNDLE_ERR; 
        }
        /* If there's a valid creation time, make sure bundle is not expired */
        else
        {
            /* If the current DTN time is 0 (implying it's invalid), bundle won't expire */
            if ((CandidateBundle->blocks.PrimaryBlock.Timestamp.CreateTime + 
                 CandidateBundle->blocks.PrimaryBlock.Lifetime) <= BPLib_TIME_GetCurrentDtnTime())
            {
                return BPLIB_BI_EXPIRED_BUNDLE_ERR;
            }
        }
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

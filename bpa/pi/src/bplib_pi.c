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

/**
 * @file
 *
 * Public APIs for Payload Interface
 */

/*
** Include
*/

#include "bplib_pi.h"
#include "bplib_mem.h"
#include "bplib_fwp.h"
#include "bplib_nc.h"
#include "bplib_ebp.h"
#include <stdio.h>


/*
** Function Definitions
*/

/* Add application configurations */
BPLib_Status_t BPLib_PI_AddApplication(uint8_t ChanId)
{
    return BPLIB_SUCCESS;
}

/* Remove application configurations */
BPLib_Status_t BPLib_PI_RemoveApplication(uint8_t ChanId)
{
    return BPLIB_SUCCESS;
}

/* Validate channel configuration parameters */
BPLib_Status_t BPLib_PI_ValidateConfigs(void *TblData)
{
    BPLib_PI_ChannelTable_t *TblDataPtr = (BPLib_PI_ChannelTable_t *)TblData;

    /* Temporary check, implement full validation TODO */
    if (TblDataPtr->Configs[0].MaxBundlePayloadSize == -1)
    {
        return BPLIB_PI_INVALID_CONFIG_ERROR;
    }

    return BPLIB_SUCCESS;
}

/* Ingress an ADU */
BPLib_Status_t BPLib_PI_Ingress(BPLib_Instance_t* Inst, uint8_t ChanId, 
                                                            void *AduPtr, size_t AduSize)
{
    BPLib_Bundle_t *NewBundle;
    BPLib_PI_Config_t *CurrCanonConfig;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if ((Inst == NULL) || (AduPtr == NULL) || (BPLib_NC_ConfigPtrs.ChanConfigPtr == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Channel ID must be within array index limits */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    /* Indicate ADU reception */
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, ADU_COUNT_RECEIVED, 1);

    /* Allocate Bundle based on AduSize */
    NewBundle = BPLib_MEM_BundleAlloc(&Inst->pool, (const void*)AduPtr, AduSize);
    if (NewBundle == NULL)
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        /* Mark the primary block as "dirty" */
        NewBundle->blocks.PrimaryBlock.RequiresEncode = true;

        BPLib_NC_ReaderLock();

        CurrCanonConfig = &BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId];

        /* Set primary block based on channel table configurations */
        BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.DestEID), CurrCanonConfig->DestEID);
        BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.ReportToEID), CurrCanonConfig->ReportToEID);
        BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.SrcEID), BPLIB_EID_INSTANCE);
        NewBundle->blocks.PrimaryBlock.SrcEID.Service = CurrCanonConfig->LocalServiceNumber;

        NewBundle->blocks.PrimaryBlock.BundleProcFlags = CurrCanonConfig->BundleProcFlags;
        NewBundle->blocks.PrimaryBlock.CrcType = CurrCanonConfig->CrcType;
        NewBundle->blocks.PrimaryBlock.Lifetime = CurrCanonConfig->Lifetime;

        /* 
        ** Try to set creation timestamp. If no valid DTN time can be found, the CreateTime
        ** will be set to 0 and the MonoTime will be used for age block calculations and 
        ** other time calculations later on
        */
        BPLib_TIME_GetMonotonicTime(&(NewBundle->Meta.MonoTime));
        NewBundle->blocks.PrimaryBlock.Timestamp.CreateTime = BPLib_TIME_GetDtnTime(NewBundle->Meta.MonoTime);

        /* Initialize payload block */
        NewBundle->blocks.PayloadHeader.BlockType = BPLib_BlockType_Payload;
        NewBundle->blocks.PayloadHeader.CrcType = CurrCanonConfig->PayloadBlkConfig.CrcType;
        NewBundle->blocks.PayloadHeader.BlockNum = 1;
        NewBundle->blocks.PayloadHeader.BlockProcFlags = CurrCanonConfig->PayloadBlkConfig.BlockProcFlags;    

        /* Fill out the rest of the payload block metadata */
        NewBundle->blocks.PayloadHeader.RequiresEncode = true;
        NewBundle->blocks.PayloadHeader.DataOffsetStart = 0;
        NewBundle->blocks.PayloadHeader.DataSize = AduSize;

        BPLib_NC_ReaderUnlock();

        /* Initialize the extension block data - parameters have been validated, ignore return code */
        (void) BPLib_EBP_InitializeExtensionBlocks(NewBundle, ChanId);

        Status = BPLib_QM_CreateJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_GENERATED_ACCEPTED, 1);
        BPLib_EM_SendEvent(BPLIB_PI_INGRESS_DBG_EID, BPLib_EM_EventType_DEBUG,
            "[ADU In #%d]: Ingressed ADU of %lu bytes.", ChanId, AduSize);
    }
    else 
    {
        BPLib_MEM_BundleFree(&Inst->pool, NewBundle);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_GENERATED_REJECTED, 1);
        BPLib_EM_SendEvent(BPLIB_PI_INGRESS_ERR_EID, BPLib_EM_EventType_ERROR,
            "[ADU In #%d]: Failed to ingress an ADU. Error = %d.",
            ChanId, Status);
    }

    return Status;
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout)
{
    BPLib_Bundle_t    *Bundle = NULL;
    BPLib_Status_t     Status;

    /* Null checks */
    if ((Inst == NULL) || (AduPtr == NULL) || (AduSize == NULL))
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }
    else if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        *AduSize = 0;
        Status = BPLIB_INVALID_CHAN_ID_ERR;
    }
    /* Get the next bundle in the channel egress queue */
    else if (BPLib_QM_WaitQueueTryPull(&Inst->ChannelEgressJobs[ChanId], &Bundle, Timeout))
    {
        /* Copy out the contents of the bundle payload to the return pointer */
        Status = BPLib_MEM_CopyOutFromOffset(Bundle,
                                Bundle->blocks.PayloadHeader.DataOffsetStart,
                                Bundle->blocks.PayloadHeader.DataSize, AduPtr, BufLen);

        if (Status != BPLIB_SUCCESS)
        {
            *AduSize = 0;
            BPLib_EM_SendEvent(BPLIB_PI_EGRESS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "[ADU Out #%d]: Error copying ADU out for egress, Status = %d.", 
                            ChanId, Status); 
        }
        else
        {
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, ADU_COUNT_DELIVERED, 1);
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELIVERED, 1);

            *AduSize = Bundle->blocks.PayloadHeader.DataSize;
            BPLib_EM_SendEvent(BPLIB_PI_EGRESS_DBG_EID, BPLib_EM_EventType_DEBUG,
                    "[ADU Out #%d]: Egressing ADU of %lu bytes", 
                    ChanId, *AduSize);
        }

        /* Free the bundle */
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);
    }
    /* No packet was pulled, presumably queue is empty */
    else 
    {
        *AduSize = 0;
        Status = BPLIB_PI_TIMEOUT;
    }

    return Status;
}

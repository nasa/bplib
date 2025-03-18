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
    uint16_t CanonBlockIndex;
    uint16_t NextExtensionBlockIndex;
    BPLib_PI_CanBlkConfig_t* CurrCanonConfigSrc;
    BPLib_CanBlockHeader_t* CurrCanonConfigDest;

    if ((Inst == NULL) || (AduPtr == NULL) || (BPLib_NC_ConfigPtrs.ChanConfigPtr == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Channel ID must be within array index limits */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        return BPLIB_PI_CHAN_ID_INPUT_ERR;
    }

    /* Allocate Bundle based on AduSize */
    NewBundle = BPLib_MEM_BundleAlloc(&Inst->pool, (const void*)AduPtr, AduSize);
    if (NewBundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Mark the primary block as "dirty" */
    NewBundle->blocks.PrimaryBlock.RequiresEncode = true;

    /* Set primary block based on channel table configurations */
    BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.DestEID),
                        BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].DestEID);
    BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.ReportToEID),
                        BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].ReportToEID);
    BPLib_EID_CopyEids(&(NewBundle->blocks.PrimaryBlock.SrcEID), BPLIB_EID_INSTANCE);
    NewBundle->blocks.PrimaryBlock.SrcEID.Service =
                        BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].LocalServiceNumber;

    NewBundle->blocks.PrimaryBlock.BundleProcFlags =
                BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].BundleProcFlags;
    NewBundle->blocks.PrimaryBlock.CrcType =
                BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].CrcType;
    NewBundle->blocks.PrimaryBlock.Lifetime =
                BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].Lifetime;

    /* 
    ** Try to set creation timestamp. If no valid DTN time can be found, the CreateTime
    ** will be set to 0 and the MonoTime will be used for age block calculations and 
    ** other time calculations later on
    */
    BPLib_TIME_GetMonotonicTime(&(NewBundle->Meta.MonoTime));
    NewBundle->blocks.PrimaryBlock.Timestamp.CreateTime = BPLib_TIME_GetDtnTime(NewBundle->Meta.MonoTime);


    /* Fill out the canonical block configs */
    NextExtensionBlockIndex = 0;
    for (CanonBlockIndex = 0; CanonBlockIndex < BPLIB_MAX_NUM_CANONICAL_BLOCKS; CanonBlockIndex++)
    {
        /* Set the source (table) config pointer */
        CurrCanonConfigSrc = &BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].CanBlkConfig[CanonBlockIndex];

        /* Set the destination (bundle metadata) config pointer */
        if (CurrCanonConfigSrc->BlockType == BPLib_BlockType_Payload)
        {
            CurrCanonConfigDest = &NewBundle->blocks.PayloadHeader;
        }
        else
        {
            /* quick array index sanity check (this could happen if the src config didn't have a payload config) */
            if (NextExtensionBlockIndex > BPLIB_MAX_NUM_EXTENSION_BLOCKS)
            {
                break;
            }
            else
            {
                CurrCanonConfigDest = &NewBundle->blocks.ExtBlocks[NextExtensionBlockIndex].Header;
                NextExtensionBlockIndex++;
            }
        }

        /* Copy the configs from source to destination */
        CurrCanonConfigDest->BlockType = CurrCanonConfigSrc->BlockType;
        CurrCanonConfigDest->CrcType = CurrCanonConfigSrc->CrcType;
        CurrCanonConfigDest->BlockNum = CurrCanonConfigSrc->BlockNum;
        CurrCanonConfigDest->BlockProcFlags = CurrCanonConfigSrc->BlockProcFlags;
    }

    /* Fill out the rest of the payload block fields */
    NewBundle->blocks.PayloadHeader.RequiresEncode = true;
    NewBundle->blocks.PayloadHeader.DataOffsetStart = 0;
    NewBundle->blocks.PayloadHeader.DataSize = AduSize;

    printf("Ingressing packet of %lu bytes from ADU via channel #%d\n", (unsigned long)AduSize, ChanId);

    return BPLib_QM_AddUnsortedJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout)
{
    BPLib_Bundle_t    *Bundle;
    BPLib_Status_t     Status;

    /* Null checks */
    if ((Inst == NULL) || (AduPtr == NULL) || (AduSize == NULL))
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }
    /* Get the next bundle in the channel egress queue */
    else if (BPLib_QM_WaitQueueTryPull(&Inst->ChannelEgressJobs[ChanId], &Bundle, Timeout))
    {
        /* Copy out the contents of the bundle payload to the return pointer */
        Status = BPLib_MEM_CopyOutFromOffset(Bundle,
            Bundle->blocks.PayloadHeader.DataOffsetStart,
            Bundle->blocks.PayloadHeader.DataSize,
            AduPtr,
            BufLen);

        if (Status != BPLIB_SUCCESS)
        {
            printf("BPLib_PI_Egress hit BPLib_MEM_CopyOutFromOffset error: %d\n", Status);
        }
        else
        {
            *AduSize = Bundle->blocks.PayloadHeader.DataSize;
            printf("Egressing packet of %lu bytes to ADU via channel #%d\n", *AduSize, ChanId);
        }

        /* Free the bundle */
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);
    }
    /* No packet was pulled, presumably queue is empty */
    else 
    {
        Status = BPLIB_PI_TIMEOUT;
    }

    return Status;
}

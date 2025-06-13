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
#include "bplib_stor.h"
#include "bplib_cbor.h"
#include <stdio.h>

/* 
** Global Data 
*/

uint64_t      BPLib_PI_SequenceNums[BPLIB_MAX_NUM_CHANNELS];
volatile bool BPLib_PI_AutoEgressEnabled[BPLIB_MAX_NUM_CHANNELS];

/*
** Internal Function Definitions
*/

/* Validate general canonical block configurations */
BPLib_Status_t BPLib_PI_ValidateCanBlkConfig(BPLib_PI_CanBlkConfig_t *CanBlkConfig, 
                                                uint32_t *BlockNums, uint8_t *BlockNumsInArr)
{
    uint8_t i;

    /* Skip validation if block isn't included */
    if (CanBlkConfig->IncludeBlock == false)
    {
        return BPLIB_SUCCESS;
    }

    /* Validate that the CRC is a supported type */
    if (CanBlkConfig->CrcType != BPLib_CRC_Type_None && 
        CanBlkConfig->CrcType != BPLib_CRC_Type_CRC16 &&
        CanBlkConfig->CrcType != BPLib_CRC_Type_CRC32C)
    {
        return BPLIB_INVALID_CONFIG_ERR;
    }

    /* Validate that the block processing flags are all supported */
    if ((CanBlkConfig->BlockProcFlags | BPLIB_VALID_BLOCK_PROC_FLAG_MASK) != BPLIB_VALID_BLOCK_PROC_FLAG_MASK)
    {
        return BPLIB_INVALID_CONFIG_ERR;
    }

    if (CanBlkConfig->BlockNum == 0)
    {
        return BPLIB_INVALID_CONFIG_ERR;
    }

    /* Validate that this block number is unique */
    for (i = 0; i < *BlockNumsInArr; i++)
    {
        if (CanBlkConfig->BlockNum == BlockNums[i])
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }
    }

    /* Add block number to array */
    BlockNums[*BlockNumsInArr] = CanBlkConfig->BlockNum;
    (*BlockNumsInArr)++;

    return BPLIB_SUCCESS;
}

/*
** External Function Definitions
*/

/* Add application configurations */
BPLib_Status_t BPLib_PI_AddApplication(uint32_t ChanId)
{
    BPLib_NC_ApplicationState_t AppState;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Check for channel ID validity */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        BPLib_EM_SendEvent(BPLIB_PI_ADD_ID_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with add-application directive, invalid ChanId=%d",
                            ChanId);

        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    /* App state must be either added or removed */
    AppState = BPLib_NC_GetAppState(ChanId);
    if (AppState != BPLIB_NC_APP_STATE_REMOVED && AppState != BPLIB_NC_APP_STATE_ADDED)
    {
        BPLib_EM_SendEvent(BPLIB_PI_ADD_STATE_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with add-application directive, invalid AppState=%d for ChanId=%d",
                            AppState, ChanId);

        return BPLIB_APP_STATE_ERR;
    }

    /* Initialize sequence number */
    BPLib_PI_SequenceNums[ChanId] = 0;

    /* Do any framework-specific operations */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication(ChanId);
    if (Status == BPLIB_SUCCESS)
    {
        /* Set app state to added */
        BPLib_NC_SetAppState(ChanId, BPLIB_NC_APP_STATE_ADDED);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PI_ADD_FWP_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with add-application directive, framework specific error code = %d",
                            Status);
    }

    return Status;
}

BPLib_Status_t BPLib_PI_StartApplication(uint32_t ChanId)
{
    BPLib_NC_ApplicationState_t AppState;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Check for channel ID validity */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        BPLib_EM_SendEvent(BPLIB_PI_START_ID_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with start-application directive, invalid ChanId=%d",
                            ChanId);

        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    /* App state must be added or stopped */
    AppState = BPLib_NC_GetAppState(ChanId);
    if (AppState != BPLIB_NC_APP_STATE_ADDED && AppState != BPLIB_NC_APP_STATE_STOPPED)
    {
        BPLib_EM_SendEvent(BPLIB_PI_START_STATE_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with start-application directive, invalid AppState=%d for ChanId=%d",
                            AppState, ChanId);

        return BPLIB_APP_STATE_ERR;
    }

    /* Do any framework-specific operations */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication(ChanId);
    if (Status == BPLIB_SUCCESS)
    {        
        /* Set app state to started */
        BPLib_NC_SetAppState(ChanId, BPLIB_NC_APP_STATE_STARTED);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PI_START_FWP_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with start-application directive, framework specific error code = %d",
                            Status);
    }

    return Status;    
}

BPLib_Status_t BPLib_PI_StopApplication(uint32_t ChanId)
{
    BPLib_NC_ApplicationState_t AppState;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Check for channel ID validity */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        BPLib_EM_SendEvent(BPLIB_PI_STOP_ID_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with stop-application directive, invalid ChanId=%d",
                            ChanId);

        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    /* App state must be started */
    AppState = BPLib_NC_GetAppState(ChanId);
    if (AppState != BPLIB_NC_APP_STATE_STARTED)
    {
        BPLib_EM_SendEvent(BPLIB_PI_STOP_STATE_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with stop-application directive, invalid AppState=%d for ChanId=%d",
                            AppState, ChanId);

        return BPLIB_APP_STATE_ERR;
    }

    /* Do any framework-specific operations */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication(ChanId);
    if (Status == BPLIB_SUCCESS)
    {        
        /* Set app state to stopped */
        BPLib_NC_SetAppState(ChanId, BPLIB_NC_APP_STATE_STOPPED);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PI_STOP_FWP_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with stop-application directive, framework specific error code = %d",
                            Status);
    }

    return Status;  
}

/* Remove application configurations */
BPLib_Status_t BPLib_PI_RemoveApplication(BPLib_Instance_t *Inst, uint32_t ChanId)
{
    BPLib_Bundle_t *Bundle;
    BPLib_NC_ApplicationState_t AppState;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if (Inst == NULL)
    {
        /* Not a normal ops error, this can be reported by the general NC error event */
        return BPLIB_NULL_PTR_ERROR;
    }

    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        BPLib_EM_SendEvent(BPLIB_PI_REMOVE_ID_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with remove-application directive, invalid ChanId=%d",
                            ChanId);

        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    /* App state must be added, stopped, or removed */
    AppState = BPLib_NC_GetAppState(ChanId);
    if (AppState == BPLIB_NC_APP_STATE_STARTED)
    {
        BPLib_EM_SendEvent(BPLIB_PI_REMOVE_STATE_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with remove-application directive, invalid AppState=%d for ChanId=%d",
                            AppState, ChanId);

        return BPLIB_APP_STATE_ERR;
    }

    /* Push any bundles waiting for egress back into storage */
    while (BPLib_QM_WaitQueueTryPull(&Inst->ChannelEgressJobs[ChanId], &Bundle, QM_NO_WAIT))
    {
        Status = BPLib_STOR_StoreBundle(Inst, Bundle);

        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_PI_REMOVE_QUEUE_FLUSH_DBG_EID, BPLib_EM_EventType_DEBUG,
                                "Error with remove-application directive pushing a bundle back to storage, Status=%d for ChanId=%d",
                                Status, ChanId);

            /* Bundle is effectively getting dropped */
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);

            /* This is still considered a successful directive just with some bundle loss */
            Status = BPLIB_SUCCESS;
        }
    }

    /* Reset sequence number */
    BPLib_PI_SequenceNums[ChanId] = 0;
    
    /* Do any framework-specific operations */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_RemoveApplication(ChanId);
    if (Status == BPLIB_SUCCESS)
    {
        /* Set app state to removed */
        BPLib_NC_SetAppState(ChanId, BPLIB_NC_APP_STATE_REMOVED);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PI_REMOVE_FWP_DBG_EID, BPLib_EM_EventType_DEBUG,
                            "Error with remove-application directive, framework specific error code = %d",
                            Status);
    }

    return Status;
}

/* Validate channel configuration parameters */
BPLib_Status_t BPLib_PI_ValidateConfigs(void *TblData)
{
    BPLib_PI_ChannelTable_t *TblDataPtr = (BPLib_PI_ChannelTable_t *)TblData;
    uint32_t ChanId;
    uint32_t BlockNums[4];
    uint8_t  BlockNumsInArr;
    uint32_t i;

    for (ChanId = 0; ChanId < BPLIB_MAX_NUM_CHANNELS; ChanId++)
    {
        /* Validate registration state */
        if (TblDataPtr->Configs[ChanId].RegState != BPLIB_PI_ACTIVE &&
            TblDataPtr->Configs[ChanId].RegState != BPLIB_PI_PASSIVE_DEFER &&
            TblDataPtr->Configs[ChanId].RegState != BPLIB_PI_PASSIVE_ABANDON)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate hop limit is between 1 and 255 */
        if (TblDataPtr->Configs[ChanId].HopLimit == 0)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate primary block CRC is either 16 or 32 (not NONE) */
        if (TblDataPtr->Configs[ChanId].CrcType != BPLib_CRC_Type_CRC16 && 
            TblDataPtr->Configs[ChanId].CrcType != BPLib_CRC_Type_CRC32C)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate uniqueness of service numbers */
        for (i = 0; i < ChanId; i++)
        {
            if (TblDataPtr->Configs[i].LocalServiceNumber == 
                TblDataPtr->Configs[ChanId].LocalServiceNumber)
            {
                return BPLIB_INVALID_CONFIG_ERR;
            }
        }

        /* Validate bundle proc flags are all supported */
        if (BPLib_CBOR_VerifyBundleProcFlags(TblDataPtr->Configs[ChanId].BundleProcFlags) != BPLIB_SUCCESS)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate that the destination ID is valid and doesn't use DTN scheme */
        if (TblDataPtr->Configs[ChanId].DestEID.Scheme == BPLIB_EID_SCHEME_DTN ||
            !BPLib_EID_IsValid(&TblDataPtr->Configs[ChanId].DestEID))
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate report-to EID */
        if (!BPLib_EID_IsValid(&TblDataPtr->Configs[ChanId].ReportToEID))
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate that the maximum bundle payload size doesn't exceed the system limit */
        if (TblDataPtr->Configs[ChanId].MaxBundlePayloadSize > BPLIB_MAX_PAYLOAD_SIZE)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate that the bundle lifetime doesn't exceed the system limit */
        if (TblDataPtr->Configs[ChanId].Lifetime > BPLIB_MAX_LIFETIME_ALLOWED)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }

        /* Validate that the payload block is included and its block number is 1 */
        if (TblDataPtr->Configs[ChanId].PayloadBlkConfig.IncludeBlock == false ||
            TblDataPtr->Configs[ChanId].PayloadBlkConfig.BlockNum != 1)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        
        BlockNumsInArr = 0;

        /* Validate all canonical blocks */
        if (BPLib_PI_ValidateCanBlkConfig(&(TblDataPtr->Configs[ChanId].PrevNodeBlkConfig), BlockNums, &BlockNumsInArr) != BPLIB_SUCCESS ||
            BPLib_PI_ValidateCanBlkConfig(&(TblDataPtr->Configs[ChanId].AgeBlkConfig), BlockNums, &BlockNumsInArr) != BPLIB_SUCCESS ||
            BPLib_PI_ValidateCanBlkConfig(&(TblDataPtr->Configs[ChanId].HopCountBlkConfig), BlockNums, &BlockNumsInArr) != BPLIB_SUCCESS ||
            BPLib_PI_ValidateCanBlkConfig(&(TblDataPtr->Configs[ChanId].PayloadBlkConfig), BlockNums, &BlockNumsInArr) != BPLIB_SUCCESS)
        {
            return BPLIB_INVALID_CONFIG_ERR;
        }
    }

    return BPLIB_SUCCESS;
}

/* Ingress an ADU */
BPLib_Status_t BPLib_PI_Ingress(BPLib_Instance_t* Inst, uint32_t ChanId, 
                                                            void *AduPtr, size_t AduSize)
{
    BPLib_Bundle_t *NewBundle;
    BPLib_PI_Config_t *CurrCanonConfig;
    BPLib_Status_t Status = BPLIB_SUCCESS;

    /* Channel ID must be within array index limits */
    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    BPLib_NC_ReaderLock();

    if ((Inst == NULL) || (AduPtr == NULL) || (BPLib_NC_ConfigPtrs.ChanConfigPtr == NULL))
    {
        BPLib_NC_ReaderUnlock();
        return BPLIB_NULL_PTR_ERROR;
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
        NewBundle->blocks.PrimaryBlock.Timestamp.SequenceNumber = BPLib_PI_SequenceNums[ChanId];

        /* Update sequence number */
        BPLib_PI_SequenceNums[ChanId]++;
        if (BPLib_PI_SequenceNums[ChanId] > BPLib_NC_ConfigPtrs.MibPnConfigPtr->Configs[PARAM_SET_MAX_SEQUENCE_NUM])
        {
            BPLib_PI_SequenceNums[ChanId] = 0;
        }

        /* Initialize payload block */
        NewBundle->blocks.PayloadHeader.BlockType = BPLib_BlockType_Payload;
        NewBundle->blocks.PayloadHeader.CrcType = CurrCanonConfig->PayloadBlkConfig.CrcType;
        NewBundle->blocks.PayloadHeader.BlockNum = 1;
        NewBundle->blocks.PayloadHeader.BlockProcFlags = CurrCanonConfig->PayloadBlkConfig.BlockProcFlags;    

        /* Fill out the rest of the payload block metadata */
        NewBundle->blocks.PayloadHeader.RequiresEncode = true;
        NewBundle->blocks.PayloadHeader.DataOffsetStart = 0;
        NewBundle->blocks.PayloadHeader.DataSize = AduSize;

        /* Initialize the extension block data - parameters have been validated, ignore return code */
        (void) BPLib_EBP_InitializeExtensionBlocks(NewBundle, ChanId);

        Status = BPLib_QM_CreateJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_GENERATED_ACCEPTED, 1);
    }
    else 
    {
        BPLib_MEM_BundleFree(&Inst->pool, NewBundle);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_GENERATED_REJECTED, 1);
        BPLib_EM_SendEvent(BPLIB_PI_INGRESS_ERR_EID, BPLib_EM_EventType_ERROR,
            "[ADU In #%d]: Failed to ingress an ADU. Error = %d.",
            ChanId, Status);
    }

    BPLib_NC_ReaderUnlock();

    return Status;
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint32_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout)
{
    BPLib_Bundle_t    *Bundle = NULL;
    BPLib_Status_t     Status;

    /* Null checks */
    if ((Inst == NULL) || (AduPtr == NULL) || (AduSize == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    else if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        *AduSize = 0;
        return BPLIB_INVALID_CHAN_ID_ERR;
    }
    *AduSize = 0;

    /* Get the next bundle in the channel egress queue */
    Status = BPLib_QM_DuctPull(Inst, ChanId, true, Timeout, &Bundle);
    if (Status == BPLIB_SUCCESS)
    {
        /* Copy out the contents of the bundle payload to the return pointer */
        Status = BPLib_MEM_CopyOutFromOffset(Bundle,
                                Bundle->blocks.PayloadHeader.DataOffsetStart,
                                Bundle->blocks.PayloadHeader.DataSize, AduPtr, BufLen);
        if (Status == BPLIB_SUCCESS)
        {
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, ADU_COUNT_DELIVERED, 1);
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELIVERED, 1);

            *AduSize = Bundle->blocks.PayloadHeader.DataSize;
        }
        else
        {
            BPLib_EM_SendEvent(BPLIB_PI_EGRESS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "[ADU Out #%d]: Error copying ADU out for egress, Status = %d.",
                            ChanId, Status);
        }

        /* Free the bundle */
        BPLib_MEM_BundleFree(&Inst->pool, Bundle);
    }

    if (Status == BPLIB_TIMEOUT)
    {
        Status = BPLIB_PI_TIMEOUT;
    }

    return Status;
}

BPLib_Status_t BPLib_PI_SetAutoEgress(uint32_t ChanId, bool AutoEgressEnabled)
{
    if (ChanId < BPLIB_MAX_NUM_CHANNELS)
    {
        BPLib_PI_AutoEgressEnabled[ChanId] = AutoEgressEnabled;
        return BPLIB_SUCCESS;
    }
    
    return BPLIB_INVALID_CHAN_ID_ERR;
}

bool BPLib_PI_GetAutoEgress(uint32_t ChanId)
{
    if (ChanId < BPLIB_MAX_NUM_CHANNELS)
    {
        return BPLib_PI_AutoEgressEnabled[ChanId];
    }

    return false;
}
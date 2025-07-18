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

#include "bplib_ebp.h"
#include "bplib_pi.h"
#include "bplib_nc.h"

/*
** Function Definitions
*/

BPLib_Status_t BPLib_EBP_InitializeExtensionBlocks(BPLib_Bundle_t *Bundle, uint32_t ChanId)
{
    BPLib_PI_Config_t *CurrCanonConfig;
    uint8_t CurrExtBlkIdx = 0;

    if (Bundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (ChanId >= BPLIB_MAX_NUM_CHANNELS)
    {
        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    BPLib_NC_ReaderLock();

    CurrCanonConfig = &BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId];

    /* Initialize previous node block */
    if (CurrCanonConfig->PrevNodeBlkConfig.IncludeBlock)
    {
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockType = BPLib_BlockType_PrevNode;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.CrcType = CurrCanonConfig->PrevNodeBlkConfig.CrcType;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockNum = CurrCanonConfig->PrevNodeBlkConfig.BlockNum;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockProcFlags = CurrCanonConfig->PrevNodeBlkConfig.BlockProcFlags;        

        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.RequiresEncode = true;

        CurrExtBlkIdx++;
    }

    /* Initialize age block */
    if (CurrCanonConfig->AgeBlkConfig.IncludeBlock || Bundle->blocks.PrimaryBlock.Timestamp.CreateTime == 0)
    {
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockType = BPLib_BlockType_Age;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.CrcType = CurrCanonConfig->AgeBlkConfig.CrcType;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockNum = CurrCanonConfig->AgeBlkConfig.BlockNum;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockProcFlags = CurrCanonConfig->AgeBlkConfig.BlockProcFlags;          

        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.RequiresEncode = true;

        CurrExtBlkIdx++;
    }

    /* Initialize hop count block */
    if (CurrCanonConfig->HopCountBlkConfig.IncludeBlock)
    {
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockType = BPLib_BlockType_HopCount;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.CrcType = CurrCanonConfig->HopCountBlkConfig.CrcType;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockNum = CurrCanonConfig->HopCountBlkConfig.BlockNum;
        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.BlockProcFlags = CurrCanonConfig->HopCountBlkConfig.BlockProcFlags;

        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].Header.RequiresEncode = true;

        Bundle->blocks.ExtBlocks[CurrExtBlkIdx].BlockData.HopCountData.HopLimit = CurrCanonConfig->HopLimit;

        CurrExtBlkIdx++;
    }

    BPLib_NC_ReaderUnlock();

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_EBP_UpdateExtensionBlocks(BPLib_Bundle_t *Bundle)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    uint32_t ExtBlkIdx = 0;
    BPLib_TIME_MonotonicTime_t CurrTime;
    int64_t TimeOnNode = 0;

    if (Bundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    for (ExtBlkIdx = 0; ExtBlkIdx < BPLIB_MAX_NUM_EXTENSION_BLOCKS; ExtBlkIdx++)
    {
        /* Increment hop count */
        if (Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_HopCount)
        {
            Bundle->blocks.ExtBlocks[ExtBlkIdx].BlockData.HopCountData.HopCount++;
            Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.RequiresEncode = true;
        }

        /* Add time on node to age block */
        if (Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_Age)
        {
            CurrTime.Time = BPLib_TIME_GetMonotonicTime();
            CurrTime.BootEra = BPLib_TIME_GetBootEra();
            
            Status = BPLib_TIME_GetTimeDelta(CurrTime, Bundle->blocks.PrimaryBlock.MonoTime, &TimeOnNode);

            /* Can't do anything about a time error here, just return the status */

            Bundle->blocks.ExtBlocks[ExtBlkIdx].BlockData.AgeBlockData.Age += TimeOnNode;
            Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.RequiresEncode = true;
        }

        /* Set previous node to this node */
        if (Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.BlockType == BPLib_BlockType_PrevNode)
        {
            BPLib_EID_CopyEids(&Bundle->blocks.ExtBlocks[ExtBlkIdx].BlockData.PrevNodeBlockData.PrevNodeId,
                                BPLIB_EID_INSTANCE);
            Bundle->blocks.ExtBlocks[ExtBlkIdx].Header.RequiresEncode = true;
        }
    }
    
    return Status;
}

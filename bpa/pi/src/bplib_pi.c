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
    BPLib_MEM_Block_t* CurrBlock;
    size_t ChunkLen, BytesCopied;
    BPLib_Bundle_t* NewBundle;

    if ((Inst == NULL) || (AduPtr == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Allocate bundle header */
    CurrBlock = BPLib_MEM_BlockAlloc(&Inst->pool);
    if (CurrBlock == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    NewBundle = (BPLib_Bundle_t *)(CurrBlock);
    NewBundle->blocks.pri_blk.version = BPLIB_BUNDLE_PROTOCOL_VERSION;
    NewBundle->blocks.pri_blk.src_eid.node_number = 0x42;

    /* Allocate a blob */
    CurrBlock = BPLib_MEM_BlockListAlloc(&Inst->pool, AduSize);
    if (CurrBlock == NULL)
    {
        BPLib_MEM_BlockFree(&Inst->pool, (BPLib_MEM_Block_t *) NewBundle);
        return BPLIB_NULL_PTR_ERROR;
    }

    BytesCopied = 0;
    NewBundle->blob = CurrBlock;

    /* Copy ADU into blob */
    while (CurrBlock != NULL)
    {
        ChunkLen = BPLIB_MEM_CHUNKSIZE;

        if (AduSize - BytesCopied < ChunkLen)
        {
            ChunkLen = AduSize - BytesCopied;
        }

        memcpy(CurrBlock->chunk, (uint8_t *)(AduPtr) + BytesCopied, ChunkLen);
        CurrBlock->chunk_len = ChunkLen;

        /* Go to the next block */
        BytesCopied += ChunkLen;
        CurrBlock = CurrBlock->next;
    }

    printf("Ingressing packet of %lu bytes from ADU\n", BytesCopied);

    return BPLib_QM_AddUnsortedJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, 
                                                        QM_PRI_NORMAL, QM_WAIT_FOREVER);
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t* Inst, uint8_t ChanId, void *BundlePtr, size_t BundleSize)
{
    return BPLIB_SUCCESS;
}

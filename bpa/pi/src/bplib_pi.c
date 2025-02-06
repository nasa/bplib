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
    BPLib_MEM_Block_t *CurrBlock;
    BPLib_Bundle_t    *NewBundle;
    size_t             ChunkLen, BytesCopied;

    if ((Inst == NULL) || (AduPtr == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Allocate Bundle header */
    CurrBlock = BPLib_MEM_BlockAlloc(&Inst->pool);
    if (CurrBlock == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    memset(CurrBlock, 0, sizeof(BPLib_Bundle_t));

    NewBundle = (BPLib_Bundle_t *)(CurrBlock);
    NewBundle->blocks.pri_blk.version = BPLIB_BUNDLE_PROTOCOL_VERSION;

    /* Temporary code to allow for routing between chan 0 and 1, will be replaced */
    if (ChanId == 0)
    {
        NewBundle->blocks.pri_blk.dest_eid.service_number = 0x42;
    }

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

    printf("Ingressing packet of %lu bytes from ADU via channel #%d\n", BytesCopied, ChanId);

    return BPLib_QM_AddUnsortedJob(Inst, NewBundle, CHANNEL_IN_PI_TO_EBP, 
                                                        QM_PRI_NORMAL, QM_WAIT_FOREVER);
}

/* Egress an ADU */
BPLib_Status_t BPLib_PI_Egress(BPLib_Instance_t *Inst, uint8_t ChanId, void *AduPtr, 
                                    size_t *AduSize, size_t BufLen, uint32_t Timeout)
{
    BPLib_Bundle_t    *Bundle;
    BPLib_MEM_Block_t *CurrBlock;
    BPLib_Status_t     Status = BPLIB_SUCCESS;
    size_t             BytesCopied;

    /* Null checks */
    if ((Inst == NULL) || (AduPtr == NULL) || (AduSize == NULL))
    {
        Status = BPLIB_NULL_PTR_ERROR;
    }
    /* Get the next bundle in the channel egress queue */
    else if (BPLib_QM_WaitQueueTryPull(&Inst->ChannelEgressJobs[ChanId], &Bundle, Timeout))
    {
        BytesCopied = 0;
        CurrBlock = Bundle->blob;

        /* Copy out the contents of the bundle payload to the return pointer */
        while (CurrBlock != NULL && Status == BPLIB_SUCCESS)
        {
            /* Error, trying to copy a bundle that's too big into a buffer that's too small */
            if (BytesCopied + CurrBlock->chunk_len >= BufLen)
            {
                Status = BPLIB_BUF_LEN_ERROR;
            }
            else
            {
                memcpy((uint8_t *)AduPtr + BytesCopied, CurrBlock->chunk, CurrBlock->chunk_len);
                BytesCopied += CurrBlock->chunk_len;
                CurrBlock = CurrBlock->next;
            }
        }

        /* Free the bundle blocks */
        BPLib_MEM_BlockListFree(&Inst->pool, Bundle->blob);
        BPLib_MEM_BlockFree(&Inst->pool, (BPLib_MEM_Block_t*) Bundle);

        if (Status == BPLIB_SUCCESS)
        {
            *AduSize = BytesCopied;

            printf("Egressing packet of %lu bytes to ADU via channel #%d\n", *AduSize, ChanId);
        }
    }
    /* No packet was pulled, presumably queue is empty */
    else 
    {
        Status = BPLIB_PI_TIMEOUT;
    }

    return Status;
}

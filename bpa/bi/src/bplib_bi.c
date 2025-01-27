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
#include <stdio.h>
#include "bplib_bi.h"
#include "bplib_qm.h"

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
    BPLib_MEM_Block_t* curr_block;
    size_t n_copy, bytes_copied;
    BPLib_Bundle_t* bundle;

    if (inst == NULL)
    {
        return BPLIB_ERROR;
    }

    /* 
     * CBOR Decode the bundle and return the deserialized bundle pointer 
    */

    /* Validate the deserialized bundle*/
    Status = BPLib_BI_ValidateBundle();
    
    /* 
     * Pass the deserialized bundle into EBP In Queue 
    */

    /* Presently, there is no bundle deserialization. We're just going to create a faux bundle and attach BundleIn
    ** as the Blob
    */
    /* Bundle Header */
    curr_block = BPLib_MEM_BlockAlloc(&inst->pool);
    if (curr_block == NULL)
    {
        return BPLIB_ERROR;
    }
    bundle = (BPLib_Bundle_t*)(curr_block);
    bundle->blocks.pri_blk.version = 7;
    bundle->blocks.pri_blk.src_eid.node_number = 0x42;

    /* Blob */
    curr_block = BPLib_MEM_BlockListAlloc(&inst->pool, Size);
    if (curr_block == NULL)
    {
        BPLib_MEM_BlockFree(&inst->pool, (BPLib_MEM_Block_t*)bundle);
        return BPLIB_ERROR;
    }

    bytes_copied = 0;
    bundle->blob = curr_block;
    while (curr_block != NULL)
    {
        n_copy = BPLIB_MEM_CHUNKSIZE;
        if (Size - bytes_copied < n_copy)
        {
            n_copy = Size - bytes_copied;
        }
        memcpy(curr_block->chunk, (uint8_t*)(BundleIn) + bytes_copied, n_copy);
        curr_block->chunk_len = n_copy;

        /* Go to the next block */
        bytes_copied += n_copy;
        curr_block = curr_block->next;
    }
    printf("Ingressing packet of %lu bytes from CLA\n", bytes_copied);

    BPLib_QM_AddUnsortedJob(inst, bundle, STATE_BI_IN, QM_PRI_NORMAL, QM_WAIT_FOREVER);
    return Status;
}

/* Pull deserialized bundle from BI Out Queue, CBOR encode it, then send it to CLA */
BPLib_Status_t BPLib_BI_SendFullBundleOut(BPLib_Instance_t* inst, void *BundleOut, size_t* Size)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;

    if (inst == NULL)
    {
        return BPLIB_ERROR;
    }

    /* 
     * Pull the deserialized bundle from BI Out Queue 
     */
    
    /* 
     * CBOR encode the deserialized bundle if needed, return the bundle pointer 
     */
    
    /* 
     * Pass the bundle out to CLA 
     */
    
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

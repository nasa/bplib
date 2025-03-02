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

#include "bplib_mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_MEM_PoolInit(BPLib_MEM_Pool_t* pool, void* init_mem, size_t init_size)
{
    if (pool == NULL)
    {
        return BPLIB_ERROR;
    }

    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
    pthread_mutex_init(&pool->lock, NULL);
    return BPLib_MEM_PoolImplInit(&pool->impl, init_mem, init_size,
        sizeof(BPLib_MEM_Block_t));

}

void BPLib_MEM_PoolDestroy(BPLib_MEM_Pool_t* pool)
{
    if (pool == NULL)
    {
        return;
    }

    pthread_mutex_destroy(&pool->lock);
    BPLib_MEM_PoolImplDestroy(&pool->impl);
    memset(pool, 0, sizeof(BPLib_MEM_Pool_t));
}

BPLib_MEM_Block_t* BPLib_MEM_BlockAlloc(BPLib_MEM_Pool_t* pool)
{
    BPLib_MEM_Block_t* block;

    if (pool == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&pool->lock);
    block = (BPLib_MEM_Block_t*)(BPLib_MEM_PoolImplAlloc(&pool->impl));
    pthread_mutex_unlock(&pool->lock);
    block->used_len = 0;
    block->next = NULL;

    return block;
}

void BPLib_MEM_BlockFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* block)
{
    if (pool == NULL || block == NULL)
    {
        return;
    }

    pthread_mutex_lock(&pool->lock);
    BPLib_MEM_PoolImplFree(&pool->impl, (void*)block);
    pthread_mutex_unlock(&pool->lock);
}

BPLib_MEM_Block_t* BPLib_MEM_BlockListAlloc(BPLib_MEM_Pool_t* pool, size_t byte_len)
{
    size_t bytes_alloc, blocks_alloc;
    BPLib_MEM_Block_t* head;
    BPLib_MEM_Block_t* curr_tail;
    BPLib_MEM_Block_t* new_block;

    if (pool == NULL || byte_len == 0)
    {
        return 0;
    }

    head = NULL;
    bytes_alloc = 0;
    blocks_alloc = 0;
    do
    {
        new_block = BPLib_MEM_BlockAlloc(pool);
        if (new_block == NULL)
        {
            BPLib_MEM_BlockListFree(pool, head);
            return NULL;
        }
        blocks_alloc++;
        bytes_alloc += BPLIB_MEM_CHUNKSIZE;

        if (head == NULL)
        {
            head = new_block;
            curr_tail = head;
        }
        else
        {
            curr_tail->next = new_block;
            curr_tail = new_block;
        }
    } while (bytes_alloc < byte_len);

    return head;
}

void BPLib_MEM_BlockListFree(BPLib_MEM_Pool_t* pool, BPLib_MEM_Block_t* head)
{
    BPLib_MEM_Block_t* to_free;
    BPLib_MEM_Block_t* curr;

    if (pool == NULL || head == NULL)
    {
        return;
    }

    curr = head;
    while (curr != NULL)
    {
        to_free = curr;
        curr = curr->next;
        BPLib_MEM_BlockFree(pool, to_free);
    }
}

BPLib_Bundle_t* BPLib_MEM_BundleAlloc(BPLib_MEM_Pool_t* pool, const void* blob_data, size_t data_len)
{
    BPLib_Bundle_t    *bundle;
    BPLib_MEM_Block_t *curr_block;
    size_t copy_len, bytes_copied, bytes_remaining;

    /* NULL Checks */
    if ((pool == NULL) || (blob_data == NULL))
    {
        return NULL;
    }

    /* Allocate a MEM_Block_t for the Bundle Metadata (bblocks) */
    curr_block = BPLib_MEM_BlockAlloc(pool);
    if (curr_block == NULL)
    {
        return NULL;
    }
    memset(&curr_block->user_data.bundle, 0, sizeof(BPLib_Bundle_t));
    curr_block->used_len = sizeof(BPLib_Bundle_t);
    bundle = (BPLib_Bundle_t *)(&curr_block->user_data.bundle);

    /* Allocate a blob */
    curr_block = BPLib_MEM_BlockListAlloc(pool, data_len);
    if (curr_block == NULL)
    {
        BPLib_MEM_BlockFree(pool, (BPLib_MEM_Block_t *) bundle);
        return NULL;
    }
    bundle->blob = curr_block;

    /* Copy the blob_data in to the bundle's blob MEM_Block_t list */
    bytes_copied = 0;
    while (curr_block != NULL)
    {
        bytes_remaining = data_len - bytes_copied;
        copy_len = (bytes_remaining < BPLIB_MEM_CHUNKSIZE) ? bytes_remaining : BPLIB_MEM_CHUNKSIZE;

        memcpy(curr_block->user_data.raw_bytes,
            (void*)((uintptr_t)(blob_data) + bytes_copied),
            copy_len);
        curr_block->used_len = copy_len;

        /* Go to the next block */
        bytes_copied += copy_len;
        curr_block = curr_block->next;
    }
    return bundle;
}

void BPLib_MEM_BundleFree(BPLib_MEM_Pool_t* pool, BPLib_Bundle_t* bundle)
{
    /* NULL Checks */
    if ((pool == NULL) || (bundle == NULL))
    {
        return;
    }

    /* Note: The cast here from bundle back to MEM_Block_t is relying
    ** on the user being absolutely sure they obtained this bundle from
    ** the BundleAlloc function. Passing in a bundle_t from somewhere else
    ** is undefined behavior, and is noted in the docstring.
    */
    BPLib_MEM_BlockListFree(pool, bundle->blob);
    BPLib_MEM_BlockFree(pool, (BPLib_MEM_Block_t*)bundle);
}

BPLib_Status_t BPLib_MEM_BlobCopyOut(BPLib_Bundle_t* bundle, void* out_buffer, size_t max_len, size_t* out_size)
{
    BPLib_MEM_Block_t* curr_block;
    size_t bytes_copied;

    if ((bundle == NULL) || (out_buffer == NULL) || (out_size == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    curr_block = bundle->blob;
    bytes_copied = 0;
    while (curr_block != NULL)
    {
        if ((bytes_copied + curr_block->used_len) > max_len)
        {
            *out_size = 0;
            return BPLIB_BUF_LEN_ERROR;
        }

        memcpy((void*)((uintptr_t)out_buffer + bytes_copied),
            (void*)curr_block->user_data.raw_bytes,
            curr_block->used_len);

        bytes_copied += curr_block->used_len;
        curr_block = curr_block->next;
    }

    *out_size = bytes_copied;
    return BPLIB_SUCCESS;
}


BPLib_Status_t BPLib_MEM_CopyOutFromOffset(BPLib_Bundle_t* Bundle,
    uint64_t Offset,
    uint64_t NumBytesToCopy,
    void* OutputBuffer,
    size_t OutputBufferSize)
{
    BPLib_Status_t ReturnStatus = BPLIB_SUCCESS;
    BPLib_MEM_Block_t *CurrentBlock;
    uint64_t NumBytesLeftToSkip;
    uint64_t BytesLeftInThisBlock;
    uint64_t BytesToCopyInThisBlock;
    uint64_t RemainingBytesToCopy;
    uint64_t TotalBytesCopied;
    uintptr_t CurrentInputOffset;
    uintptr_t CurrentOutputPointer;
    uint64_t ExpectedMemBlockNumber;
    uint64_t CurrentMemBlockNumber;

    if ((Bundle == NULL) || (Bundle->blob == NULL) || (OutputBuffer == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* find the first blob that contains data after the offset */
    CurrentBlock = Bundle->blob;
    NumBytesLeftToSkip = Offset;
    ExpectedMemBlockNumber = Offset / BPLIB_MEM_CHUNKSIZE;
    for (CurrentMemBlockNumber = 0; CurrentMemBlockNumber < ExpectedMemBlockNumber; CurrentMemBlockNumber++)
    {
        NumBytesLeftToSkip -= BPLIB_MEM_CHUNKSIZE;
        CurrentBlock = CurrentBlock->next;
        if (CurrentBlock == NULL)
        {
            return BPLIB_ERROR;
        }
    }

    /* Start copying from the first block */
    CurrentOutputPointer = (uintptr_t) OutputBuffer;
    CurrentInputOffset = (uintptr_t) &CurrentBlock->user_data.raw_bytes[NumBytesLeftToSkip];
    BytesLeftInThisBlock = BPLIB_MEM_CHUNKSIZE - NumBytesLeftToSkip;
    if (NumBytesToCopy <= BytesLeftInThisBlock)
    {
        memcpy((void*)CurrentOutputPointer, (void*)CurrentInputOffset, NumBytesToCopy);
        TotalBytesCopied = NumBytesToCopy;
    }
    else
    {
        memcpy((void*)CurrentOutputPointer, (void*)CurrentInputOffset, BytesLeftInThisBlock);
        TotalBytesCopied = BytesLeftInThisBlock;
    }

    /* if we need to copy more data, loop through each next block */
    while ((TotalBytesCopied < NumBytesToCopy) && (CurrentBlock->next != NULL))
    {
        CurrentBlock = CurrentBlock->next;
        CurrentInputOffset = (uintptr_t) &CurrentBlock->user_data.raw_bytes[0];
        CurrentOutputPointer = (uintptr_t)(OutputBuffer) + TotalBytesCopied;

        RemainingBytesToCopy = NumBytesToCopy - TotalBytesCopied;
        if (RemainingBytesToCopy >= BPLIB_MEM_CHUNKSIZE)
        {
            BytesToCopyInThisBlock = BPLIB_MEM_CHUNKSIZE;
        }
        else
        {
            BytesToCopyInThisBlock = RemainingBytesToCopy;
        }
        memcpy((void*)CurrentOutputPointer, (void*)CurrentInputOffset, BytesToCopyInThisBlock);
        TotalBytesCopied += BytesToCopyInThisBlock;
    }

    if (TotalBytesCopied != NumBytesToCopy)
    {
        ReturnStatus = BPLIB_ERROR;
    }
    else
    {
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}

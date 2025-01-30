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
    block->chunk_len = 0;
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
    BPLib_MEM_PoolImplFree(&pool->impl, (void*)block->chunk);
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

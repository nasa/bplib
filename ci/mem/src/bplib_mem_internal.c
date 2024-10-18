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

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <string.h>
#include <assert.h>

#include "bplib.h"
#include "bplib_api_types.h"

#include "bplib_mem.h"
#include "bplib_mem_internal.h"

#include "osapi.h"

/**
 * Lacking bplib_os.h and bplib/os, improvised MEM-local BPLib_MEM_OS_* calls.
 */

#define OSAL_LOCALTIME_SECS_AT_2000 946684800

static OS_time_t BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV;

static osal_id_t BPLib_MEM_OS_FileDataLock;

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Lock -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Lock(BPLib_Handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(BPLib_HandleToSerial(h, BPLIB_HANDLE_MPOOL_BASE));

    OS_CondVarLock(id);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Unlock -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Unlock(BPLib_Handle_t h)
{
    osal_id_t id;

    id = OS_ObjectIdFromInteger(BPLib_HandleToSerial(h, BPLIB_HANDLE_MPOOL_BASE));

    OS_CondVarUnlock(id);
}


/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_Init -
 *-------------------------------------------------------------------------------------*/
void BPLib_MEM_OS_Init(void)
{
    OS_MutSemCreate(&BPLib_MEM_OS_FileDataLock, "bplib_mem_osal", 0);
    BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV = OS_TimeAssembleFromMilliseconds(OSAL_LOCALTIME_SECS_AT_2000, 0);
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_CreateLock -
 *-------------------------------------------------------------------------------------*/
BPLib_Handle_t BPLib_MEM_OS_CreateLock(void)
{
    char      lock_name[OS_MAX_API_NAME];
    int32     status;
    osal_id_t id;

    snprintf(lock_name, sizeof(lock_name), "bpl_mem%02u", BPLib_MEM_OS_NextSerial());
    status = OS_CondVarCreate(&id, lock_name, 0);
    if (status != OS_SUCCESS)
    {
        return BPLIB_INVALID_HANDLE;
    }

    return BPLib_HandleFromSerial(OS_ObjectIdToInteger(id), BPLIB_HANDLE_OS_BASE);
}

unsigned int BPLib_MEM_OS_NextSerial(void)
{
    static unsigned int last_serial_num = 0;
    unsigned int        next_serial;

    OS_MutSemTake(BPLib_MEM_OS_FileDataLock);
    next_serial = ++last_serial_num;
    OS_MutSemGive(BPLib_MEM_OS_FileDataLock);

    return next_serial;
}

int BPLib_MEM_OS_WaitUntilMs(BPLib_Handle_t h, uint64_t abs_dtntime_ms)
{
    osal_id_t id;
    OS_time_t until_time;
    int32     status;

    id = OS_ObjectIdFromInteger(BPLib_HandleToSerial(h, BPLIB_HANDLE_OS_BASE));

    if (abs_dtntime_ms == BPLIB_MEM_DTNTIME_INFINITE)
    {
        /* Block Forever until Success */
        status = OS_CondVarWait(id);
    }
    else
    {
        until_time = OS_TimeFromTotalMilliseconds(abs_dtntime_ms);

        /* change the epoch from DTN (2000) to OSAL */
        until_time = OS_TimeAdd(until_time, BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV);

        /* Block on Timed Wait and Update Timeout */
        status = OS_CondVarTimedWait(id, &until_time);
    }

    /* check for timeout error explicitly and translate to BPLIB_TIMEOUT */
    if (status == OS_ERROR_TIMEOUT)
    {
        return BPLIB_TIMEOUT;
    }

    /* other unexpected/unhandled errors become BPLIB_ERROR */
    if (status != OS_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* status of 0 indicates success */
    return BPLIB_SUCCESS;
}

uint64_t BPLib_MEM_OS_GetDtnTimeMs(void)
{
    OS_time_t ref_tm;
    OS_GetLocalTime(&ref_tm);

    /* Convert to DTN epoch */
    ref_tm = OS_TimeSubtract(ref_tm, BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV);

    /* Convert to milliseconds */
    return OS_TimeGetTotalMilliseconds(ref_tm);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_AllocBlockInternal
 *
 * NOTE: this must be invoked with the lock already held
 *-----------------------------------------------------------------*/
BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool,
    BPLib_MEM_Blocktype_t blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority)
{
    BPLib_MEM_Block_t                 *node;
    BPLib_MEM_BlockContent_t          *block;
    #ifdef QM_MODULE_API_AVAILABLE
    BPLib_STOR_QM_ModuleApiContent_t  *api_block;
    #endif //QM_MODULE_API_AVAILABLE
    size_t                             data_offset;
    uint32_t                           alloc_threshold;
    uint32_t                           block_count;

    BPLib_MEM_BlockAdminContent_t *admin;

    printf("%s:%d blocktype is %d\n", __FILE__, __LINE__, blocktype);

    admin = BPLib_MEM_GetAdmin(pool);

    printf("%s:%d admin is %ld\n", __FILE__, __LINE__, (uint64_t)admin);

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype == BPLib_MEM_BlocktypeUndefined || blocktype >= BPLib_MEM_BlocktypeMax)
    {
        printf("%s:%d Bad blocktype. Return null.\n", __FILE__, __LINE__);
        return NULL;
    }

    /*
     * Check free block threshold: Note that it may take additional pool blocks (refs, cbor, etc)
     * in order to forward stored bundles along when the time comes.
     *
     * Without a sufficient buffer/threshold, if the entire pool is consumed by bundles, it will
     * prevent any blocks from being forwarded (and eventually freed) as well, creating deadlock.
     *
     * This soft limit only applies for actual bundle blocks, not for refs.
     */
    alloc_threshold = (admin->bblock_alloc_threshold * priority) / 255;

    block_count = BPLib_MEM_SubqGetDepth(&admin->free_blocks);

    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {
        printf("%s:%d No free blocks. Return null.\n", __FILE__, __LINE__);
        /* no free blocks available for the requested type */
        return NULL;
    }

    #ifdef QM_MODULE_API_AVAILABLE
    /* Determine how to initialize this block by looking up the content type */
    api_block = (BPLib_STOR_QM_ModuleApiContent_t *)(void *)BPLib_MEM_RBT_SearchUnique(content_type_signature,
                                                                                   &admin->blocktype_registry);
    if (api_block == NULL)
    {
        printf("%s:%d No constructor. Return null.\n", __FILE__, __LINE__);
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = BPLib_MEM_GetUserDataOffsetByBlocktype(blocktype);
    if (data_offset > sizeof(BPLib_MEM_BlockBuffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(BPLib_MEM_BlockBuffer_t))
    {
        printf("%s:%d User content won't fit. Return null.\n", __FILE__, __LINE__);
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }
    #endif // QM_MODULE_API_AVAILABLE

    /* get a block */
    node = BPLib_MEM_SubqPullSingle(&admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

    printf("%s:%d Got block.\n", __FILE__, __LINE__);

    /*
     * Convert from blocks free to blocks used, and update high watermark if necessary.
     * This is +1 to include the block that was just pulled (that is, a call to
     * BPLib_MEM_SubqGetDepth() on the free list now will return 1 fewer than it
     * did earlier in this function).
     */
    block_count = 1 + admin->num_bufs_total - block_count;
    if (block_count > admin->max_alloc_watermark)
    {
        admin->max_alloc_watermark = block_count;
    }

    node->type = blocktype;
    block      = BPLib_MEM_GetBlockContent(node);

    /*
     * zero fill the content part first, this ensures that this is always done,
     * and avoids the need for the module to supply a dedicated constructor just to zero it
     */
    #ifndef QM_MODULE_API_AVAILABLE
    data_offset = BPLib_MEM_GetUserDataOffsetByBlocktype(blocktype);
    memset(&block->u, 0, data_offset);
    #else // QM_MODULE_API_AVAILABLE
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    BPLib_MEM_InitBaseObject(&block->header, api_block->user_content_size, content_type_signature);

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api.construct(init_arg, node) != BPLIB_SUCCESS)
        {
            // TODO Use bplog?
            // bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "Constructor failed for block type %d, signature %lx\n", blocktype,
            //      (unsigned long)content_type_signature);
        }
    }
    #endif // QM_MODULE_API_AVAILABLE

    return block;
}

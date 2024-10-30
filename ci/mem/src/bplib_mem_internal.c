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

#include <assert.h>

#include "bplib_api_types.h"

#include "bplib_mem.h"
#include "bplib_mem_internal.h"

#include "osapi.h"

/**
 * Lacking bplib_os.h and bplib/os, improvised MEM-local BPLib_MEM_OS_* calls.
 */

#define OSAL_LOCALTIME_SECS_AT_2000 946684800

static OS_time_t BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV;

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
BPLib_Status_t BPLib_MEM_OS_Init(void)
{
    int32 status;

    BPLib_MEM_OS_FileDataLock = (osal_id_t) 0xFF; // The lock ID from OSAL can be zero.

    status = OS_MutSemCreate(&BPLib_MEM_OS_FileDataLock, "bplib_mem_osal", 0);
    /**
     * Check return status of OS_MutSemCreate
     *   0 #OS_SUCCESS OS_SUCCESS
     *  -1 #OS_ERROR OS_ERROR
     *  -2 #OS_INVALID_POINTER if sem_id or sem_name are NULL
     * -13 #OS_ERR_NAME_TOO_LONG name length including null terminator greater than      * #OS_MAX_API_NAME
     * -14 #OS_ERR_NO_FREE_IDS if there are no more free mutex Ids
     * -15 #OS_ERR_NAME_TAKEN if there is already a mutex with the same name
     *  -6 #OS_SEM_FAILURE if the OS call failed @covtest
     * 
     * See osapi_error.h for other values.
     * 
     */
    if (status != OS_SUCCESS)
    {
        // TODO Send an event if OS_MutSemCreate fails.
        return BPLIB_ERROR;
    }
    BPLIB_MEM_OSAL_LOCALTIME_DTN_CONV = OS_TimeAssembleFromMilliseconds(OSAL_LOCALTIME_SECS_AT_2000, 0);
    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_CreateLock -
 *-------------------------------------------------------------------------------------*/
BPLib_Handle_t BPLib_MEM_OS_CreateLock(void)
{
    char      lock_name[OS_MAX_API_NAME];
    osal_id_t id;
    int32     status;
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
    BPLib_MEM_ApiContent_t            *api_block;
    size_t                             data_offset;
    uint32_t                           alloc_threshold;
    uint32_t                           block_count;

    BPLib_MEM_BlockAdminContent_t *admin;

    admin = BPLib_MEM_GetAdmin(pool);

    /* Only real blocks are allocated here - not secondary links nor head nodes,
     * as those are embedded within the blocks themselves. */
    if (blocktype == BPLib_MEM_BlocktypeUndefined || blocktype >= BPLib_MEM_BlocktypeMax)
    {
        return NULL;
    }

    block_count = BPLib_MEM_SubqGetDepth(&admin->free_blocks);

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

    if (block_count <= (admin->bblock_alloc_threshold - alloc_threshold))
    {
        /* no free blocks available for the requested type */
        return NULL;
    }

    /* Determine how to initialize this block by looking up the content type */
    api_block = (BPLib_MEM_ApiContent_t *)(void *)BPLib_RBT_SearchUnique(content_type_signature,
                                                                         &admin->blocktype_registry);
    if (api_block == NULL)
    {
        /* no constructor, cannot create the block! */
        return NULL;
    }

    /* sanity check that the user content will fit in the block */
    data_offset = BPLib_MEM_GetUserDataOffsetByBlocktype(blocktype);
    if (data_offset > sizeof(BPLib_MEM_BlockBuffer_t) ||
        (data_offset + api_block->user_content_size) > sizeof(BPLib_MEM_BlockBuffer_t))
    {
        /* User content will not fit in the block - cannot create an instance of this type combo */
        return NULL;
    }

    /* get a block */
    node = BPLib_MEM_SubqPullSingle(&admin->free_blocks);
    if (node == NULL)
    {
        /* this should never happen, because depth was already checked */
        return NULL;
    }

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
    memset(&block->u, 0, data_offset + api_block->user_content_size);

    BPLib_MEM_InitBaseObject(&block->header, api_block->user_content_size, content_type_signature);

    switch (blocktype)
    {
        case BPLib_MEM_BlocktypePrimary:
            BPLib_MEM_BblockPrimaryInit(node, &block->u.primary.pblock);
            break;
        case BPLib_MEM_BlocktypeCanonical:
            BPLib_MEM_BblockCanonicalInit(node, &block->u.canonical.cblock);
            break;
        case BPLib_MEM_BlocktypeDuct:
            BPLib_MEM_DuctInit(node, &block->u.duct.dblock);
            break;
        default:
            /* nothing more for this node type (this catches cbor_data)  */
            break;
    }

    /* If the module did supply a constructor, invoke it now */
    if (api_block->api.construct != NULL)
    {
        /* A constructor really should never fail nominally, if it does there is probably a bug */
        if (api_block->api.construct(init_arg, node) != BPLIB_SUCCESS)
        {
            return NULL;
        }
    }

    return block;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_BblockPrimaryInit
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_BblockPrimaryInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockPrimary_t *pblk)
{
    BPLib_MEM_InitListHead(base_block, &pblk->cblock_list);
    BPLib_MEM_InitListHead(base_block, &pblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_BblockCanonicalInit
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_BblockCanonicalInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockCanonical_t *cblk)
{
    BPLib_MEM_InitListHead(base_block, &cblk->chunk_list);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_BblockPrimaryAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_BblockPrimaryAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number,
                                                void *init_arg, uint8_t priority, BPLib_TIME_MonotonicTime_t timeout)
{
    BPLib_MEM_BlockContent_t *result;
    BPLib_MEM_Lock_t         *lock;
    bool                      within_timeout;

    lock           = BPLib_MEM_LockResource(pool);
    within_timeout = true;
    while (true)
    {
        result = BPLib_MEM_AllocBlockInternal(pool, BPLib_MEM_BlocktypePrimary, magic_number, init_arg, priority);
        if (result != NULL || !within_timeout)
        {
            break;
        }

        within_timeout = BPLib_MEM_LockWait(lock, timeout);
    }
    BPLib_MEM_LockRelease(lock);

    return (BPLib_MEM_Block_t *)result;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_BblockCanonicalAlloc
 *
 *-----------------------------------------------------------------*/
BPLib_MEM_Block_t *BPLib_MEM_BblockCanonicalAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg)
{
    BPLib_MEM_BlockContent_t *result;
    BPLib_MEM_Lock_t         *lock;

    lock   = BPLib_MEM_LockResource(pool);
    result = BPLib_MEM_AllocBlockInternal(pool, BPLib_MEM_BlocktypeCanonical, magic_number, init_arg,
                                              BPLIB_MEM_ALLOC_PRI_MED);
    BPLib_MEM_LockRelease(lock);

    return (BPLib_MEM_Block_t *)result;
}

static BPLib_Status_t BPLib_MEM_DuctEventHandler(void *arg, BPLib_MEM_Block_t *jblk)
{
    BPLib_MEM_Block_t                *fblk;
    BPLib_MEM_Duct_t                 *duct;
    uint32_t                          changed_flags;
    BPLib_MEM_DuctGenericEvent_t      event;
    bool                              was_running;
    bool                              is_running;

    fblk = BPLib_MEM_GetBlockFromLink(jblk);
    duct = BPLib_MEM_DuctCast(fblk);
    if (duct == NULL)
    {
        return BPLIB_ERROR;
    }

    was_running   = BPLib_MEM_DuctIsUp(duct);
    changed_flags = duct->pending_state_flags ^ duct->current_state_flags;
    duct->current_state_flags ^= changed_flags;
    is_running = BPLib_MEM_DuctIsUp(duct);

    /* detect changes from up->down or vice versa */
    /* this is the combination of several flags, so its not simply checking changed_flags */
    if (was_running != is_running)
    {
        if (is_running)
        {
            event.intf_state.event_type = BPLib_MEM_DuctEventUp;
        }
        else
        {
            event.intf_state.event_type = BPLib_MEM_DuctEventDown;
        }

        event.intf_state.intf_id = BPLib_MEM_GetExternalId(fblk);
        duct->statechange_job.event_handler(&event, fblk);
    }

    if (changed_flags & BPLIB_CACHE_STATE_FLAG_POLL)
    {
        event.event_type = BPLib_MEM_DuctEventPoll;
        duct->statechange_job.event_handler(&event, fblk);
    }

    return BPLIB_SUCCESS;
}

void BPLib_MEM_SubqWorkitemInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqWorkitem_t *wblk)
{
    BPLib_MEM_JobInit(base_block, &wblk->job_header);
    BPLib_MEM_SubqInit(base_block, &wblk->base_subq);
    wblk->current_depth_limit = 0;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_DuctInit
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_DuctInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Duct_t *fblk)
{
    /* now init the link structs */
    BPLib_MEM_JobInit(base_block, &fblk->statechange_job.base_job);
    fblk->statechange_job.base_job.handler = BPLib_MEM_DuctEventHandler;

    BPLib_MEM_SubqWorkitemInit(base_block, &fblk->ingress);
    BPLib_MEM_SubqWorkitemInit(base_block, &fblk->egress);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_JobInit
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_JobInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Job_t *jblk)
{
    BPLib_MEM_InitSecondaryLink(base_block, &jblk->link, BPLib_MEM_BlocktypeJob);
}

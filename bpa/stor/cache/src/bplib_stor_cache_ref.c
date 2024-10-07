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
#include "bplib_mem.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"
#include "bplib_stor_cache_internal.h"

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RefDuplicate
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefDuplicate(BPLib_STOR_CACHE_Ref_t refptr)
{
    BPLib_MEM_Lock_t *lock;

    /*
     * If the refcount is 0, that means this is still a regular (non-refcounted) object,
     * or it should have been garbage-collected already, so something is broken.
     */

    lock = BPLib_MEM_LockResource(&refptr->header.base_link);
    assert(refptr->header.refcount > 0);
    ++refptr->header.refcount;
    BPLib_MEM_LockRelease(lock);

    return refptr;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RefCreate
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefCreate(BPLib_STOR_CACHE_Block_t *blk)
{
    BPLib_STOR_CACHE_BlockContent_t *content;
    BPLib_MEM_Lock_t          *lock;

    /*
     * This drills down to the actual base object (the "root" so to speak), so that the
     * refcount is incremented on the original object.  As a result this does not currently
     * allow for a double-ref situation - although the data model does allow for that, this
     * does not create a "ref-to-ref" at the moment.  TBD if that is useful or not...
     */
    content = BPLib_STOR_CACHE_BlockDereferenceContent(blk);
    if (content == NULL)
    {
        return NULL;
    }

    lock = BPLib_MEM_LockResource(content);
    ++content->header.refcount;
    BPLib_MEM_LockRelease(lock);

    return content;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RefMakeBlock
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_RefMakeBlock(BPLib_STOR_CACHE_Ref_t refptr, uint32_t magic_number, void *init_arg)
{
    BPLib_STOR_CACHE_BlockContent_t *rblk;
    BPLib_STOR_CACHE_BlockContent_t *bblk;
    BPLib_MEM_Lock_t          *lock;
    BPLib_STOR_CACHE_Pool_t               *pool;

    bblk = BPLib_STOR_CACHE_BlockDereferenceContent(BPLib_STOR_CACHE_Dereference(refptr));
    pool = BPLib_STOR_CACHE_GetParentPoolFromLink(&bblk->header.base_link);

    lock = BPLib_MEM_LockResource(pool);
    rblk = BPLib_STOR_CACHE_AllocBlockInternal(pool, BPLib_STOR_CACHE_BlocktypeRef, magic_number, init_arg,
                                            BPLIB_MEM_ALLOC_PRI_MHI);
    BPLib_MEM_LockRelease(lock);

    if (rblk == NULL)
    {
        return NULL;
    }

    rblk->u.ref.pref_target = BPLib_STOR_CACHE_RefDuplicate(bblk);

    return (BPLib_STOR_CACHE_Block_t *)rblk;
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RefFromBlock
 *
 *-----------------------------------------------------------------*/
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_RefFromBlock(BPLib_STOR_CACHE_Block_t *rblk)
{
    BPLib_STOR_CACHE_BlockContent_t *content;

    if (rblk == NULL || rblk->type != BPLib_STOR_CACHE_BlocktypeRef)
    {
        return NULL;
    }

    content = BPLib_STOR_CACHE_GetBlockContent(rblk);

    return BPLib_STOR_CACHE_RefDuplicate(content->u.ref.pref_target);
}

/*----------------------------------------------------------------
 *
 * Function: BPLib_STOR_CACHE_RefRelease
 *
 *-----------------------------------------------------------------*/
void BPLib_STOR_CACHE_RefRelease(BPLib_STOR_CACHE_Ref_t refptr)
{
    BPLib_STOR_CACHE_BlockHeader_t *block_hdr;
    BPLib_MEM_Lock_t         *lock;
    bool                        needs_recycle;

    if (refptr != NULL)
    {
        block_hdr = &refptr->header;

        /*
         * Refcount decrement must be done under lock, but it can be
         * a fine-grained lock.
         */
        lock = BPLib_MEM_LockResource(refptr);
        if (block_hdr->refcount > 0)
        {
            --block_hdr->refcount;
        }
        needs_recycle = (block_hdr->refcount == 0);
        BPLib_MEM_LockRelease(lock);

        if (needs_recycle)
        {
            /* it should not be part of a list at this point */
            assert(BPLib_STOR_CACHE_IsLinkUnattached(&block_hdr->base_link));
            BPLib_STOR_CACHE_RecycleBlock(&block_hdr->base_link);
        }
    }
}

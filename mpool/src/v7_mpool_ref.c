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
#include "bplib_os.h"
#include "v7_types.h"

#include "v7_mpool_internal.h"

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_ref_duplicate
 *
 *-----------------------------------------------------------------*/
bplib_mpool_ref_t bplib_mpool_ref_duplicate(bplib_mpool_ref_t refptr)
{
    /*
     * If the refcount is 0, that means this is still a regular (non-refcounted) object,
     * or it should have been garbage-collected already, so something is broken.
     */
    assert(refptr->header.refcount > 0);
    ++refptr->header.refcount;

    return (bplib_mpool_ref_t)refptr;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_ref_create
 *
 *-----------------------------------------------------------------*/
bplib_mpool_ref_t bplib_mpool_ref_create(bplib_mpool_t *pool, bplib_mpool_block_t *blk)
{
    bplib_mpool_block_content_t *content;

    /*
     * This drills down to the actual base object (the "root" so to speak), so that the
     * refcount is incremented on the original object.  As a result this does not currently
     * allow for a double-ref situation - although the data model does allow for that, this
     * does not create a "ref-to-ref" at the moment.  TBD if that is useful or not...
     */
    content = (bplib_mpool_block_content_t *)bplib_mpool_obtain_base_block(blk);
    if (content == NULL)
    {
        return NULL;
    }

    /*
     * If the refcount is 0, that means this is still a regular (non-refcounted) object.
     * If nonzero then it is already a recounted object
     */
    if (content->header.refcount == 0)
    {
        /* The original object goes into the active list in the pool, and becomes "owned"
         * by the pool after this.  It should no longer be directly used by the caller. */
        bplib_mpool_insert_before(&pool->ref_managed_blocks, blk);
    }

    ++content->header.refcount;
    return content;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_ref_make_block
 *
 *-----------------------------------------------------------------*/
bplib_mpool_block_t *bplib_mpool_ref_make_block(bplib_mpool_t *pool, bplib_mpool_ref_t refptr, uint32_t magic_number,
                                                void *init_arg)
{
    bplib_mpool_block_content_t *rblk;

    rblk = bplib_mpool_alloc_block_internal(pool, bplib_mpool_blocktype_ref, magic_number, init_arg);
    if (rblk == NULL)
    {
        return NULL;
    }

    rblk->u.ref.pref_target = bplib_mpool_ref_duplicate(refptr);

    return (bplib_mpool_block_t *)rblk;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_ref_from_block
 *
 *-----------------------------------------------------------------*/
bplib_mpool_ref_t bplib_mpool_ref_from_block(bplib_mpool_block_t *rblk)
{
    bplib_mpool_block_content_t *content;

    if (rblk == NULL || rblk->type != bplib_mpool_blocktype_ref)
    {
        return NULL;
    }

    content = (bplib_mpool_block_content_t *)rblk;

    return bplib_mpool_ref_duplicate(content->u.ref.pref_target);
}

/*----------------------------------------------------------------
 *
 * Function: bplib_mpool_ref_release
 *
 *-----------------------------------------------------------------*/
void bplib_mpool_ref_release(bplib_mpool_t *pool, bplib_mpool_ref_t refptr)
{
    bplib_mpool_block_header_t *block_hdr;

    block_hdr = &refptr->header;
    if (block_hdr->refcount > 0)
    {
        --block_hdr->refcount;
    }
    if (block_hdr->refcount == 0)
    {
        bplib_mpool_recycle_block(pool, &block_hdr->base_link);
    }
}

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

#ifndef V7_MPOOL_REF_H
#define V7_MPOOL_REF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "bplib_api_types.h"
#include "v7_mpool.h"

/**
 * @brief Gets the actual block from a reference pointer (dereference)
 *
 * @param refptr
 * @return bplib_mpool_block_t*
 */
static inline bplib_mpool_block_t *bplib_mpool_dereference(bplib_mpool_ref_t refptr)
{
    return (bplib_mpool_block_t *)refptr;
}

/**
 * @brief Creates a lightweight reference to the data block
 *
 * This creates an opaque pointer that can be stored into another user object
 * A light reference increses the reference count on the target object, but does
 * not consume any pool memory directly itself.  It will prevent the target
 * object from being garbage collected.
 *
 * References must be explicitly tracked and released by the user when
 * no longer needed, using bplib_mpool_ref_release()
 *
 * References may also be duplicated via bplib_mpool_ref_duplicate()
 *
 * After this call, the passed in blk becomes managed by the pool.
 *
 * @note If this function returns non-NULL, the calling application should no longer directly
 * use the blk pointer that was passed in.  It should only use the reference pointers.
 *
 * @param blk
 * @return bplib_mpool_ref_t*
 */
bplib_mpool_ref_t bplib_mpool_ref_create(bplib_mpool_block_t *blk);

bplib_mpool_ref_t bplib_mpool_ref_duplicate(bplib_mpool_ref_t refptr);
bplib_mpool_ref_t bplib_mpool_ref_from_block(bplib_mpool_block_t *rblk);

/**
 * @brief Release a lightweight reference
 *
 * This must be invoked once a reference that was previously created by bplib_mpool_ref_create()
 * or bplib_mpool_ref_duplicate() is no longer needed by the software application.
 *
 * This decrements the reference count, and if the reference count reaches 0, it also recycles the
 * original object.
 *
 * @param refptr
 */
void bplib_mpool_ref_release(bplib_mpool_ref_t refptr);

/**
 * @brief Creates a separate block reference to the data block
 *
 * A block is allocated from the pool which refers to the original data block, and can stand in place
 * of the actual data block wherever a data block is expected, such as for queuing and storage.
 *
 * A reference of this type does not need to be explicitly released by the user, as it
 * will be automatically  released when the block is recycled via bplib_mpool_recycle_block()
 * or bplib_mpool_recycle_all_blocks_in_list()
 *
 * @note This increments the refcount, so the calling application should call
 * bplib_mpool_ref_release() on the original ref if it does not keep it.
 *
 * @param refptr
 * @param magic_number
 * @param init_arg Opaque pointer to pass to initializer (may be NULL)
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_ref_make_block(bplib_mpool_ref_t refptr, uint32_t magic_number, void *init_arg);

#ifdef __cplusplus
}
#endif

#endif /* V7_MPOOL_REF_H */

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

#ifndef BPLIB_MEM_REF_H
#define BPLIB_MEM_REF_H

#include <string.h>

#include "bplib_mem.h"
#include "bplib_api_types.h"

/**
 * @brief Gets the actual block from a reference pointer (dereference)
 *
 * @param refptr
 * @return BPLib_MEM_block_t*
 */
static inline BPLib_MEM_block_t *BPLib_MEM_dereference(BPLib_MEM_ref_t refptr)
{
    return (BPLib_MEM_block_t *)refptr;
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
 * no longer needed, using BPLib_MEM_RefRelease()
 *
 * References may also be duplicated via BPLib_MEM_RefDuplicate()
 *
 * After this call, the passed in blk becomes managed by the pool.
 *
 * @note If this function returns non-NULL, the calling application should no longer directly
 * use the blk pointer that was passed in.  It should only use the reference pointers.
 *
 * @param blk
 * @return BPLib_MEM_ref_t*
 */
BPLib_MEM_ref_t BPLib_MEM_RefCreate(BPLib_MEM_block_t *blk);

BPLib_MEM_ref_t BPLib_MEM_RefDuplicate(BPLib_MEM_ref_t refptr);
BPLib_MEM_ref_t BPLib_MEM_RefFromBlock(BPLib_MEM_block_t *rblk);

/**
 * @brief Release a lightweight reference
 *
 * This must be invoked once a reference that was previously created by BPLib_MEM_RefCreate()
 * or BPLib_MEM_RefDuplicate() is no longer needed by the software application.
 *
 * This decrements the reference count, and if the reference count reaches 0, it also recycles the
 * original object.
 *
 * @param refptr
 */
void BPLib_MEM_RefRelease(BPLib_MEM_ref_t refptr);

/**
 * @brief Creates a separate block reference to the data block
 *
 * A block is allocated from the pool which refers to the original data block, and can stand in place
 * of the actual data block wherever a data block is expected, such as for queuing and storage.
 *
 * A reference of this type does not need to be explicitly released by the user, as it
 * will be automatically  released when the block is recycled via BPLib_MEM_RecycleBlock()
 * or BPLib_MEM_RecycleAllBlocksInList()
 *
 * @note This increments the refcount, so the calling application should call
 * BPLib_MEM_RefRelease() on the original ref if it does not keep it.
 *
 * @param refptr
 * @param magic_number
 * @param init_arg Opaque pointer to pass to initializer (may be NULL)
 * @return BPLib_MEM_block_t*
 */
BPLib_MEM_block_t *BPLib_MEM_RefMakeBlock(BPLib_MEM_ref_t refptr, uint32_t magic_number, void *init_arg);

#endif /* BPLIB_MEM_REF_H */

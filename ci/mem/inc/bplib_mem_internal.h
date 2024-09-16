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

#ifndef BPLIB_MEM_INTERNAL_H
#define BPLIB_MEM_INTERNAL_H

#include <string.h>

#include "bplib.h"
#include "bplib_api_types.h"
// #include "bplib_os.h" // TODO Unstub bplib_os functions.
#include "bplib_mem.h"
#include "bplib_rbtree.h"
#include "bplib_mem_bblocks.h"
#include "bplib_mem_ref.h"

/*
 * Minimum size of a generic data block
 */
#define BP_MPOOL_MIN_USER_BLOCK_SIZE 480

#define MPOOL_CACHE_CBOR_DATA_SIGNATURE 0x6b243e33

typedef struct bplib_mpool_lock
{
    bp_handle_t lock_id;
} bplib_mpool_lock_t;

typedef struct bplib_mpool_block_header
{
    bplib_mpool_block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} bplib_mpool_block_header_t;

/*
 * This union is just to make sure that the "content_start" field
 * in a generic data block is suitably aligned to hold any data type, mainly
 * the max-sized integers, max-sized floating point values, or a pointer.
 */
typedef union bplib_mpool_aligned_data
{
    uint8_t     first_octet;
    uintmax_t   align_int;
    void       *align_ptr;
    long double align_float;
} bplib_mpool_aligned_data_t;

typedef struct bplib_mpool_api_content
{
    bplib_rbt_link_t            rbt_link;
    bplib_mpool_blocktype_api_t api;
    size_t                      user_content_size;
    bplib_mpool_aligned_data_t  user_data_start;
} bplib_mpool_api_content_t;

typedef struct bplib_mpool_generic_data_content
{
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_generic_data_content_t;

typedef struct bplib_mpool_bblock_primary_content
{
    bplib_mpool_bblock_primary_t pblock;
    bplib_mpool_aligned_data_t   user_data_start;
} bplib_mpool_bblock_primary_content_t;

typedef struct bplib_mpool_bblock_canonical_content
{
    bplib_mpool_bblock_canonical_t cblock;
    bplib_mpool_aligned_data_t     user_data_start;
} bplib_mpool_bblock_canonical_content_t;

#ifdef STOR // duct
typedef struct bplib_mpool_flow_content
{
    bplib_mpool_flow_t         fblock;
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_flow_content_t;
#endif // STOR

typedef struct bplib_mpool_block_ref_content
{
    bplib_mpool_ref_t          pref_target;
    bplib_mpool_aligned_data_t user_data_start;
} bplib_mpool_block_ref_content_t;

typedef struct bplib_mpool_block_admin_content
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t bblock_alloc_threshold;   /**< threshold at which new bundles will no longer be allocatable */
    uint32_t internal_alloc_threshold; /**< threshold at which internal blocks will no longer be allocatable */
    uint32_t max_alloc_watermark;

    bplib_rbt_root_t          blocktype_registry; /**< registry of block signature values */
    bplib_mpool_api_content_t blocktype_basic;    /**< a fixed entity in the registry for type 0 */
    bplib_mpool_api_content_t blocktype_cbor;     /**< a fixed entity in the registry for CBOR blocks */

    bplib_mpool_subq_base_t *free_blocks;    /**< blocks which are available for use */
    bplib_mpool_subq_base_t *recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    bplib_mpool_block_t *active_list; /**< a list of flows/queues that need processing */

} bplib_mpool_block_admin_content_t;

typedef union bplib_mpool_block_buffer
{
    bplib_mpool_generic_data_content_t     generic_data;
    bplib_mpool_api_content_t              api;
    bplib_mpool_bblock_primary_content_t   primary;
    bplib_mpool_bblock_canonical_content_t canonical;
    // TODO fix STOR duct
    // STOR bplib_mpool_flow_content_t             flow;
    bplib_mpool_block_ref_content_t        ref;
    bplib_mpool_block_admin_content_t      admin;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BP_MPOOL_MIN_USER_BLOCK_SIZE];
} bplib_mpool_block_buffer_t;

typedef struct bplib_mpool_block_content
{
    bplib_mpool_block_header_t header; /* must be first */
    bplib_mpool_block_buffer_t u;
} bplib_mpool_block_content_t;

struct bplib_mpool
{
    bplib_mpool_block_content_t admin_block; /**< Start of first real block (see num_bufs_total) */
};

#define MPOOL_GET_BUFFER_USER_START_OFFSET(m) (offsetof(bplib_mpool_block_buffer_t, m.user_data_start))

#define MPOOL_GET_BLOCK_USER_CAPACITY(m) (sizeof(bplib_mpool_block_buffer_t) - MPOOL_GET_BUFFER_USER_START_OFFSET(m))

/**
 * @brief Gets the administrative block for the given pool
 *
 * This is always the first block in the pool.
 *
 * @param pool
 * @return bplib_mpool_block_admin_content_t*
 */
static inline bplib_mpool_block_admin_content_t *bplib_mpool_get_admin(bplib_mpool_t *pool)
{
    /* this just confirms that the passed-in pointer looks OK */
    return &pool->admin_block.u.admin;
}

/**
 * @brief Acquires a given lock
 *
 * The lock should be identified via bplib_mpool_lock_prepare() or this
 * can re-acquire the same lock again after releasing it with
 * bplib_mpool_lock_release().
 *
 * @param lock
 */
static inline void bplib_mpool_lock_acquire(bplib_mpool_lock_t *lock)
{
    // bplib_os_lock(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Release a given lock (simple)
 *
 * This simply unlocks a resource.
 *
 * This API will NOT automatically wake any task that might be waiting for a
 * state change of the underlying resource
 *
 * @param lock
 */
static inline void bplib_mpool_lock_release(bplib_mpool_lock_t *lock)
{
    // bplib_os_unlock(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Indicate state change of the resource held by the lock
 *
 * Signals to other threads that state of the resource has changed
 *
 * Other threads waiting on the same lock are unblocked and should all re-check the condition
 * they are waiting on.
 *
 * @param lock
 */
static inline void bplib_mpool_lock_broadcast_signal(bplib_mpool_lock_t *lock)
{
    // bplib_os_broadcast_signal(lock->lock_id); // TODO Unstub.
}

/**
 * @brief Prepares for resource-based locking
 *
 * Locates the correct lock to use for the given resource, but does not acquire the lock
 *
 * @note  it is imperative that all calls use the same referece address (such as the head
 * of the list) when referring to the same resource for locking to work correctly.
 *
 * @param resource_addr
 * @return bplib_mpool_lock_t*
 */
bplib_mpool_lock_t *bplib_mpool_lock_prepare(void *resource_addr);

/**
 * @brief Lock a given resource
 *
 * Locates the correct lock to use for a given resource address, and also acquires it
 *
 * @note  it is imperative that all calls use the same referece address (such as the head
 * of the list) when referring to the same resource for locking to work correctly.
 *
 * @param resource_addr
 * @return bplib_mpool_lock_t*
 */
bplib_mpool_lock_t *bplib_mpool_lock_resource(void *resource_addr);

/**
 * @brief Waits for a state change related to the given lock
 *
 * @note The resource must be locked when called.
 *
 * @param lock
 * @param until_dtntime
 * @return true
 * @return false
 */
bool bplib_mpool_lock_wait(bplib_mpool_lock_t *lock, uint64_t until_dtntime);

void bplib_mpool_bblock_primary_init(bplib_mpool_block_t *base_block, bplib_mpool_bblock_primary_t *pblk);
void bplib_mpool_bblock_canonical_init(bplib_mpool_block_t *base_block, bplib_mpool_bblock_canonical_t *cblk);
void bplib_mpool_subq_init(bplib_mpool_block_t *base_block, bplib_mpool_subq_base_t *qblk);
void bplib_mpool_flow_init(bplib_mpool_block_t *base_block, bplib_mpool_flow_t *fblk);

/**
 * @brief Append a single bundle to the given queue (flow)
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @param cpb
 */
void bplib_mpool_subq_push_single(bplib_mpool_subq_base_t *subq, bplib_mpool_block_t *cpb);

/**
 * @brief Get the next bundle from the given queue (flow)
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @return bplib_mpool_block_t*
 */
bplib_mpool_block_t *bplib_mpool_subq_pull_single(bplib_mpool_subq_base_t *subq);

/**
 * @brief Counts the number of blocks in a list
 *
 * For record-keeping or stats-reporting purposes, this counts the number of blocks in a list
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param list the subject list head
 * @return uint32_t number of blocks
 */
uint32_t bplib_mpool_list_count_blocks(bplib_mpool_block_t *list);

/**
 * @brief Pushes an entire list of blocks into a subq
 *
 * Similar to bplib_mpool_subq_move_all() but the source is just a simple list rather
 * than a subq FIFO object.
 *
 * Because simple lists do not track the number of entries, this counts the blocks in the
 * list prior to moving the items, so the dest queue push stats will remain correct.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst the destination queue
 * @param list  the source list
 * @return uint32_t number of blocks moved
 */
uint32_t bplib_mpool_subq_merge_list(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_block_t *list);

/**
 * @brief Merges the entire contents of the source subq into the destination
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst
 * @param subq_src
 * @return The number of queue entries moved
 */
uint32_t bplib_mpool_subq_move_all(bplib_mpool_subq_base_t *subq_dst, bplib_mpool_subq_base_t *subq_src);

/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param pool
 * @param subq
 * @return uint32_t The number of queue entries dropped
 */
uint32_t bplib_mpool_subq_drop_all(bplib_mpool_t *pool, bplib_mpool_subq_base_t *subq);

#ifdef STOR // job
void bplib_mpool_job_cancel_internal(bplib_mpool_job_t *job);
void bplib_mpool_job_mark_active_internal(bplib_mpool_block_t *active_list, bplib_mpool_job_t *job);
#endif //STOR

/* gets to the underlying block content (which may be a ref block) */
bplib_mpool_block_content_t       *bplib_mpool_get_block_content(bplib_mpool_block_t *cb);
const bplib_mpool_block_content_t *bplib_mpool_get_block_content_const(const bplib_mpool_block_t *cb);

/* similar to bplib_mpool_get_block_content() but also dereferences any ref blocks */
bplib_mpool_block_content_t *bplib_mpool_block_dereference_content(bplib_mpool_block_t *cb);

void bplib_mpool_init_base_object(bplib_mpool_block_header_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);

bplib_mpool_block_content_t *bplib_mpool_alloc_block_internal(bplib_mpool_t *pool, bplib_mpool_blocktype_t blocktype,
                                                              uint32_t content_type_signature, void *init_arg,
                                                              uint8_t priority);

#endif /* BPLIB_MEM_INTERNAL_H */

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

#ifndef V7_CACHE_INTERNAL_H
#define V7_CACHE_INTERNAL_H

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"
#include "bplib_dataservice.h"
#include "v7_cache.h"
#include "v7.h"
#include "v7_rbtree.h"
#include "v7_codec.h"

#include "v7_mpool.h"
#include "v7_mpool_bblocks.h"
#include "v7_mpool_flows.h"
#include "v7_mpool_ref.h"

/*
 * Randomly-chosen 32-bit static values that can be put into
 * data structures to help positively identify those structs later.
 */
#define BPLIB_STORE_SIGNATURE_STATE    0x683359a7
#define BPLIB_STORE_SIGNATURE_ENTRY    0xf223fff9
#define BPLIB_STORE_SIGNATURE_QUEUE    0x30241224
#define BPLIB_STORE_SIGNATURE_BLOCKREF 0x77e96b11

#define BPLIB_STORE_FLAG_ACTIVITY         0x01
#define BPLIB_STORE_FLAG_LOCAL_CUSTODY    0x02
#define BPLIB_STORE_FLAG_ACTION_TIME_WAIT 0x04
#define BPLIB_STORE_FLAG_LOCALLY_QUEUED   0x08

/* the set of flags for which retention is required - all are typically set for valid entries
 * if any of these becomes UN-set, retention of the entry is NOT required */
//#define BPLIB_STORE_FLAGS_RETENTION_REQUIRED  (BPLIB_STORE_FLAG_WITHIN_LIFETIME | BPLIB_STORE_FLAG_AWAITING_CUSTODY)
#define BPLIB_STORE_FLAGS_ACTION_WAIT_STATE (BPLIB_STORE_FLAG_ACTION_TIME_WAIT | BPLIB_STORE_FLAG_LOCALLY_QUEUED)

#define BP_CACHE_DACS_LIFETIME   86400000 /* 24 hrs */
#define BP_CACHE_DACS_OPEN_TIME  10000    /* 10 sec */
#define BP_CACHE_FAST_RETRY_TIME 3000     /* 3 sec */
#define BP_CACHE_IDLE_RETRY_TIME 3600000  /* 1 hour */
#define BP_CACHE_AGE_OUT_TIME    60000    /* 1 minute */

/*
 * The size of the time "buckets" stored in the time index
 *
 * as every entry in the time index tree may consume an extra block (required for duplicate handling),
 * this will batch together a group of times.  There shouldn't be a real need for millisecond-
 * level precision here, and this should hopefully reduce the block usage by consolidating
 * all entries that are within a range of times.
 *
 * This is a number of LSBs of the seconds to be grouped
 */
#define BP_CACHE_TIME_BUCKET_SHIFT 10 /* approx 1 second */

#define BP_CACHE_TIME_INFINITE BP_DTNTIME_INFINITE

typedef struct bplib_cache_state
{
    bp_ipn_addr_t self_addr;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    bplib_mpool_block_t pending_list;

    uint64_t action_time; /**< DTN time when the pending_list was last checked */

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    bplib_mpool_block_t idle_list;

    bplib_rbt_root_t hash_index;
    bplib_rbt_root_t dest_eid_index;
    bplib_rbt_root_t time_index;

    uint32_t generated_dacs_seq;

} bplib_cache_state_t;

/*
 * There are generally two ways for a bundle to be pulled from storage:
 * 1. The outgoing CLA comes up, and the bundle is to be sent through it
 *     -> this is based on destination EID, and matching routes in the route tbl.
 *        it is triggered based on a state change event from a CLA interface
 * 2. The bundle has been sitting for some amount of time
 *     -> this is triggered based on a periodic polling/maintenance call
 *        may be based on the expiration time or retransmit time
 *
 * Both of these tasks need reasonably efficient lookups - cannot be sequential
 * searches through a list.  A dedicated index is created for each, using the
 * R-B tree facility.
 *
 * Note that the R-B tree mechanism does not allow for duplicate entries, but
 * there absolutely can be multiple entries in this use-case, so we need to
 * use an intermediate queue here.
 */
typedef struct bplib_cache_queue
{
    bplib_rbt_link_t    rbt_link;    /* must be first */
    bplib_mpool_block_t bundle_list; /* jphfix - subq? */

} bplib_cache_queue_t;

typedef enum bplib_cache_entry_state
{
    bplib_cache_entry_state_undefined,
    bplib_cache_entry_state_idle,
    bplib_cache_entry_state_queue,
    bplib_cache_entry_state_delete,
    bplib_cache_entry_state_generate_dacs,
    bplib_cache_entry_state_max
} bplib_cache_entry_state_t;

typedef struct bplib_cache_dacs_pending
{
    bp_ipn_addr_t                      prev_custodian_id;
    bp_custody_accept_payload_block_t *payload_ref;

} bplib_cache_dacs_pending_t;

typedef union bplib_cache_entry_data
{
    bplib_cache_dacs_pending_t dacs;
} bplib_cache_entry_data_t;

typedef struct bplib_cache_entry
{
    bplib_cache_state_t      *parent;
    bplib_cache_entry_state_t state;
    uint32_t                  flags;
    bplib_mpool_ref_t         refptr;
    uint64_t                  action_time;    /**< DTN time when entity is due to have some action (e.g. transmit) */
    uint64_t                  next_eval_time; /* jphfix - this should be removed */
    bplib_mpool_block_t       hash_link;
    bplib_mpool_block_t       time_link;
    bplib_mpool_block_t       destination_link;
    bplib_cache_entry_data_t  data;
} bplib_cache_entry_t;

typedef struct bplib_cache_blockref
{
    bplib_mpool_block_t *storage_entry_block;
} bplib_cache_blockref_t;

typedef struct bplib_cache_custodian_info
{
    bp_ipn_addr_t        flow_id;
    bp_ipn_addr_t        custodian_id;
    bplib_mpool_block_t *cblk;
    bp_val_t             eid_hash;
    bp_sequencenumber_t  sequence_num;
    bp_ipn_t             final_dest_node;
    bplib_cache_entry_t *store_entry;
} bplib_cache_custodian_info_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline bplib_mpool_block_t *bplib_cache_state_self_block(bplib_cache_state_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&state->pending_list);
}

static inline bplib_mpool_block_t *bplib_cache_entry_self_block(bplib_cache_entry_t *entry)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&entry->hash_link);
}

static inline bplib_mpool_block_t *bplib_cache_queue_self_block(bplib_cache_queue_t *queue)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&queue->bundle_list);
}

/* Allows reconstitution of the parent pool from a cache state pointer */
static inline bplib_mpool_t *bplib_cache_parent_pool(bplib_cache_state_t *state)
{
    return bplib_mpool_get_parent_pool_from_link(bplib_cache_state_self_block(state));
}

/* Allows reconstitution of the flow object from a cache state pointer */
static inline bplib_mpool_flow_t *bplib_cache_get_flow(bplib_cache_state_t *state)
{
    return bplib_mpool_flow_cast(bplib_cache_state_self_block(state));
}

/* Allows reconstitution of the queue struct from an RBT link pointer */
static inline bplib_cache_queue_t *bplib_cache_queue_from_rbt_link(const bplib_rbt_link_t *link)
{
    /* this relies on "rbt_link" being the first entry in the struct */
    return (bplib_cache_queue_t *)((void *)link);
}

/* Allows reconstitution of the queue struct from a bundle_list pointer */
static inline bplib_cache_queue_t *bplib_cache_queue_from_bundle_list(bplib_mpool_block_t *list)
{
    return bplib_mpool_generic_data_cast(bplib_mpool_get_block_from_link(list), BPLIB_STORE_SIGNATURE_QUEUE);
}

void bplib_cache_custody_finalize_dacs(bplib_cache_state_t *state, bplib_cache_entry_t *store_entry);
void bplib_cache_custody_store_bundle(bplib_cache_state_t *state, bplib_mpool_block_t *qblk);
bool bplib_cache_custody_check_dacs(bplib_cache_state_t *state, bplib_mpool_block_t *qblk);

void bplib_cache_fsm_execute(bplib_mpool_block_t *sblk);

void bplib_cache_remove_from_subindex(bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link);
void bplib_cache_add_to_subindex(bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link, bp_val_t index_val);

void bplib_cache_entry_make_pending(bplib_mpool_block_t *qblk, uint32_t set_flags, uint32_t clear_flags);

#endif /* V7_CACHE_INTERNAL_H */

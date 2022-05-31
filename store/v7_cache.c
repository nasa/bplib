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

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"
#include "bplib_dataservice.h"
#include "v7_cache.h"
#include "v7.h"
#include "v7_rbtree.h"
#include "v7_codec.h"
#include "crc.h"

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

#define BPLIB_STORE_HASH_SALT_DACS   0x3126c0cf
#define BPLIB_STORE_HASH_SALT_BUNDLE 0x7739ae76

#define BPLIB_STORE_FLAG_WITHIN_LIFETIME   0x01
#define BPLIB_STORE_FLAG_AWAITING_CUSTODY  0x02
#define BPLIB_STORE_FLAG_AWAITING_TRANSMIT 0x04
#define BPLIB_STORE_FLAG_LOCALLY_QUEUED    0x08

/* the set of flags for which retention is required - all are typically set for valid entries
 * if any of these becomes UN-set, retention of the entry is NOT required */
#define BPLIB_STORE_FLAGS_RETENTION_REQUIRED  (BPLIB_STORE_FLAG_WITHIN_LIFETIME | BPLIB_STORE_FLAG_AWAITING_CUSTODY)
#define BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE (BPLIB_STORE_FLAG_LOCALLY_QUEUED | BPLIB_STORE_FLAG_AWAITING_TRANSMIT)

#define BP_CACHE_DACS_LIFETIME  86400000 /* 24 hrs */
#define BP_CACHE_DACS_OPEN_TIME 10000

#define BP_CACHE_IDLE_RETRY_TIME 3600000
#define BP_CACHE_FAST_RETRY_TIME 3000

/*
 * The size of the time "buckets" stored in the time index
 *
 * as every entry in the time index tree may consume an extra block (required for duplicate handling),
 * this will batch together a group of times.  There shouldn't be a real need for millisecond-
 * level precision here, and this should hopefully reduce the block usage by consolidating
 * all entries that are within a range of times.
 *
 * This should be a bitwise mask of the seconds to be grouped
 */
#define BP_CACHE_TIME_BUCKET_SIZE 0x3FF

#define BP_CACHE_TIME_INFINITE BP_DTNTIME_INFINITE

typedef struct bplib_store_cache_state
{
    bp_ipn_addr_t self_addr;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    bplib_mpool_block_t pending_list;

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

} bplib_store_cache_state_t;

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
typedef struct bplib_store_cache_queue
{
    bplib_rbt_link_t    rbt_link;    /* must be first */
    bplib_mpool_block_t bundle_list; /* jphfix - subq? */

} bplib_store_cache_queue_t;

typedef enum bplib_store_cache_entry_type
{
    bplib_store_cache_entry_type_none          = 0,
    bplib_store_cache_entry_type_normal_bundle = 1,
    bplib_store_cache_entry_type_pending_dacs  = 2
} bplib_store_cache_entry_type_t;

typedef struct bplib_store_cache_dacs_pending
{
    const bp_endpointid_buffer_t      *prev_custodian_ref;
    bp_custody_accept_payload_block_t *payload_ref;

} bplib_store_cache_dacs_pending_t;

typedef union bplib_store_cache_entry_data
{
    bplib_store_cache_dacs_pending_t dacs;
} bplib_store_cache_entry_data_t;

typedef struct bplib_store_cache_entry
{
    bplib_store_cache_state_t     *parent_state;
    bplib_store_cache_entry_type_t entry_type;
    uint32_t                       flags;
    bplib_mpool_ref_t              refptr;
    uint64_t                       last_eval_time; /**< DTN time when status of this entry was last checked */
    uint64_t                       next_eval_time; /**< DTN time when status of this entry should be checked */
    uint64_t                       expire_time;    /**< DTN time when entity is no longer useful */
    uint64_t                       transmit_time;  /**< DTN time when entity is due to be [re]transmitted */
    bplib_mpool_block_t            hash_link;
    bplib_mpool_block_t            time_link;
    bplib_mpool_block_t            destination_link;
    bplib_store_cache_entry_data_t data;
} bplib_store_cache_entry_t;

typedef struct bplib_store_cache_blockref
{
    bplib_mpool_block_t *storage_entry_block;
} bplib_store_cache_blockref_t;

typedef struct bplib_store_cache_custodian_info
{
    const bp_endpointid_buffer_t *flow_source_eid;
    bplib_mpool_block_t          *cblk;
    bp_val_t                      eid_hash;
    bp_sequencenumber_t           sequence_num;
    bp_endpointid_buffer_t       *custodian_eid;
    bp_ipn_t                      final_dest_node;
    bplib_store_cache_entry_t    *store_entry;
} bplib_store_cache_custodian_info_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline bplib_mpool_block_t *bplib_store_cache_state_self_block(bplib_store_cache_state_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&state->pending_list);
}

static inline bplib_mpool_block_t *bplib_store_cache_entry_self_block(bplib_store_cache_entry_t *entry)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&entry->hash_link);
}

static inline bplib_mpool_block_t *bplib_store_cache_queue_self_block(bplib_store_cache_queue_t *queue)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return bplib_mpool_get_block_from_link(&queue->bundle_list);
}

/* Allows reconstitution of the parent pool from a cache state pointer */
static inline bplib_mpool_t *bplib_store_cache_parent_pool(bplib_store_cache_state_t *state)
{
    return bplib_mpool_get_parent_pool_from_link(bplib_store_cache_state_self_block(state));
}

/* Allows reconstitution of the flow object from a cache state pointer */
static inline bplib_mpool_flow_t *bplib_store_cache_get_flow(bplib_store_cache_state_t *state)
{
    return bplib_mpool_flow_cast(bplib_store_cache_state_self_block(state));
}

/* Allows reconstitution of the queue struct from an RBT link pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_rbt_link(const bplib_rbt_link_t *link)
{
    /* this relies on "rbt_link" being the first entry in the struct */
    return (bplib_store_cache_queue_t *)((void *)link);
}

/* Allows reconstitution of the queue struct from a bundle_list pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_bundle_list(bplib_mpool_block_t *list)
{
    return bplib_mpool_generic_data_cast(bplib_mpool_get_block_from_link(list), BPLIB_STORE_SIGNATURE_QUEUE);
}

static bplib_store_cache_state_t *bplib_store_cache_get_state(bplib_mpool_block_t *intf_block)
{
    bplib_store_cache_state_t *state;

    state = bplib_mpool_generic_data_cast(intf_block, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): storage_block incorrect for bplib_store_cache_state_t\n", __func__);
        return NULL;
    }

    return state;
}

void bplib_store_cache_entry_make_pending(bplib_mpool_block_t *qblk, uint32_t set_flags, uint32_t clear_flags)
{
    bplib_store_cache_entry_t *store_entry;
    bplib_mpool_block_t       *sblk;

    sblk        = bplib_mpool_get_block_from_link(qblk);
    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        /* update the flags based on the event */
        store_entry->flags |= set_flags;
        store_entry->flags &= ~clear_flags;

        bplib_mpool_extract_node(sblk);
        bplib_mpool_insert_before(&store_entry->parent_state->pending_list, sblk);
    }
}

int bplib_store_cache_handle_ref_recycle(void *arg, bplib_mpool_block_t *rblk)
{
    bplib_store_cache_blockref_t *block_ref;

    block_ref = bplib_mpool_generic_data_cast(rblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (block_ref == NULL)
    {
        return BP_ERROR;
    }

    assert(block_ref->storage_entry_block != NULL);

    /*
     * always put back into pending_list, this will re-evalute its current
     * state and reclassify it as appropriate.  This also clears the BPLIB_STORE_FLAG_LOCALLY_QUEUED
     * flag.
     */
    bplib_store_cache_entry_make_pending(block_ref->storage_entry_block, 0, BPLIB_STORE_FLAG_LOCALLY_QUEUED);

    return BP_SUCCESS;
}

void bplib_store_cache_remove_from_subindex(bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link)
{
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t       *list_ptr;
    bplib_mpool_block_t       *self_block;

    /* grab the list ptr before removal (in case it becomes empty by this) */
    list_ptr = bplib_mpool_get_next_block(index_link);
    bplib_mpool_extract_node(index_link);

    /* If this arrived back at an empty head node, that means this was the last entry in that subq,
     * which then needs to be removed from its parent index tree */
    if (list_ptr != index_link && bplib_mpool_is_empty_list_head(list_ptr))
    {
        self_block  = bplib_mpool_get_block_from_link(list_ptr);
        store_queue = bplib_mpool_generic_data_cast(self_block, BPLIB_STORE_SIGNATURE_QUEUE);

        /* if node was already extracted/not in the tree, this has no effect */
        bplib_rbt_extract_node(index_root, &store_queue->rbt_link);
        bplib_mpool_recycle_block(self_block);
    }
}

int bplib_store_cache_construct_queue(void *arg, bplib_mpool_block_t *tblk)
{
    bplib_store_cache_queue_t *store_queue;

    store_queue = bplib_mpool_generic_data_cast(tblk, BPLIB_STORE_SIGNATURE_QUEUE);
    if (store_queue == NULL)
    {
        return BP_ERROR;
    }

    bplib_mpool_init_list_head(tblk, &store_queue->bundle_list);

    return BP_SUCCESS;
}

int bplib_store_cache_destruct_queue(void *arg, bplib_mpool_block_t *qblk)
{
    bplib_store_cache_queue_t *store_queue;

    store_queue = bplib_mpool_generic_data_cast(qblk, BPLIB_STORE_SIGNATURE_QUEUE);
    if (store_queue == NULL)
    {
        return BP_ERROR;
    }

    assert(bplib_mpool_is_empty_list_head(&store_queue->bundle_list));

    return BP_SUCCESS;
}

void bplib_store_cache_add_to_subindex(bplib_rbt_root_t *index_root, bplib_mpool_block_t *index_link,
                                       bp_val_t index_val)
{
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t       *tblk;
    bplib_rbt_link_t          *tlink;

    tlink = bplib_rbt_search(index_val, index_root);
    if (tlink != NULL)
    {
        /* not the first time this was seen, add to the existing subq */
        store_queue = bplib_store_cache_queue_from_rbt_link(tlink);
    }
    else
    {
        /* first occurrance of this particular index, need to create a subq block */
        tblk        = bplib_mpool_generic_data_alloc(bplib_mpool_get_parent_pool_from_link(index_link),
                                                     BPLIB_STORE_SIGNATURE_QUEUE, NULL);
        store_queue = bplib_mpool_generic_data_cast(tblk, BPLIB_STORE_SIGNATURE_QUEUE);
        if (store_queue != NULL)
        {
            /* This should always work, because it was already known _not_ to be a duplicate */
            bplib_rbt_insert_value(index_val, index_root, &store_queue->rbt_link);
        }
        else
        {
            /* must be out of memory */
            store_queue = NULL;
        }
    }

    if (store_queue != NULL)
    {
        bplib_mpool_insert_before(&store_queue->bundle_list, index_link);
    }
}

void bplib_store_cache_compute_custody_hash(bp_val_t salt, bplib_store_cache_custodian_info_t *custody_info)
{
    bp_crcval_t                   hash;
    bplib_crc_parameters_t *const EID_HASH_ALGORITHM = &BPLIB_CRC32_CASTAGNOLI;

    /* use a CRC as a hash function */
    hash = bplib_crc_initial_value(EID_HASH_ALGORITHM) ^ (bp_crcval_t)salt;
    hash = bplib_crc_update(EID_HASH_ALGORITHM, hash, custody_info->flow_source_eid,
                            sizeof(*custody_info->flow_source_eid));
    hash =
        bplib_crc_update(EID_HASH_ALGORITHM, hash, custody_info->custodian_eid, sizeof(*custody_info->custodian_eid));

    custody_info->eid_hash = (bp_val_t)bplib_crc_finalize(EID_HASH_ALGORITHM, hash);
}

bplib_mpool_ref_t bplib_store_cache_create_dacs_bundle(bplib_store_cache_state_t          *state,
                                                       bplib_mpool_bblock_primary_t      **pri_block_out,
                                                       bp_custody_accept_payload_block_t **pay_out)
{
    int                  status;
    bplib_mpool_block_t *pblk;
    bplib_mpool_block_t *cblk;
    bplib_mpool_t       *ppool;

    bplib_mpool_bblock_primary_t   *pri_block;
    bp_primary_block_t             *pri;
    bplib_mpool_bblock_canonical_t *c_block;
    bp_canonical_block_buffer_t    *pay;

    /* This needs to turn the DACS information in the temporary object into a full bundle */
    /* Allocate Blocks */
    cblk   = NULL;
    pblk   = NULL;
    status = BP_ERROR;
    ppool  = bplib_store_cache_parent_pool(state);

    do
    {
        pblk      = bplib_mpool_bblock_primary_alloc(ppool);
        pri_block = bplib_mpool_bblock_primary_cast(pblk);
        if (pri_block == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate primary block\n");
            break;
        }

        pri = bplib_mpool_bblock_primary_get_logical(pri_block);

        /* Initialize Primary Block */
        pri->version = 7;

        v7_set_eid(&pri->sourceEID, &state->self_addr);
        v7_set_eid(&pri->reportEID, &state->self_addr);

        pri->creationTimeStamp.sequence_num = state->generated_dacs_seq;
        ++state->generated_dacs_seq;
        pri->creationTimeStamp.time = v7_get_current_time();

        pri->lifetime                     = BP_CACHE_DACS_LIFETIME;
        pri->controlFlags.isAdminRecord   = true;
        pri->controlFlags.mustNotFragment = true;
        pri->crctype                      = bp_crctype_CRC16;

        /* Add Payload Block */
        cblk    = bplib_mpool_bblock_canonical_alloc(ppool);
        c_block = bplib_mpool_bblock_canonical_cast(cblk);
        if (c_block == NULL)
        {
            bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "Failed to allocate payload block\n");
            break;
        }

        pay = bplib_mpool_bblock_canonical_get_logical(c_block);

        pay->canonical_block.blockNum  = bp_blocktype_custodyAcceptPayloadBlock;
        pay->canonical_block.blockType = bp_blocktype_custodyAcceptPayloadBlock;
        pay->canonical_block.crctype   = bp_crctype_CRC16;

        bplib_mpool_bblock_primary_append(pri_block, cblk);
        cblk   = NULL; /* do not need now that it is stored */
        status = BP_SUCCESS;

        *pri_block_out = pri_block;
        *pay_out       = &pay->data.custody_accept_payload_block;
    }
    while (false);

    /* clean up, if anything did not work, recycle the blocks now */
    if (cblk != NULL)
    {
        bplib_mpool_recycle_block(cblk);
        cblk = NULL;
    }

    if (pblk != NULL && status != BP_SUCCESS)
    {
        bplib_mpool_recycle_block(pblk);
        pblk = NULL;
    }

    return bplib_mpool_ref_create(pblk);
}

void bplib_store_cache_open_dacs(bplib_store_cache_state_t *state, bplib_store_cache_custodian_info_t *custody_info)
{
    bplib_mpool_block_t               *sblk;
    bplib_store_cache_entry_t         *store_entry;
    bplib_store_cache_dacs_pending_t  *dacs_pending;
    bplib_mpool_ref_t                  pending_bundle;
    bplib_mpool_bblock_primary_t      *pri_block;
    bp_custody_accept_payload_block_t *ack_content;
    bp_handle_t                        self_intf_id;

    /* Create the storage-specific data block for keeping local refs  */
    sblk = bplib_mpool_generic_data_alloc(bplib_store_cache_parent_pool(state), BPLIB_STORE_SIGNATURE_ENTRY, state);
    pending_bundle = bplib_store_cache_create_dacs_bundle(state, &pri_block, &ack_content);
    store_entry    = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL && pending_bundle != NULL)
    {
        store_entry->entry_type = bplib_store_cache_entry_type_pending_dacs;

        store_entry->refptr = bplib_mpool_ref_duplicate(pending_bundle);

        /* the "expire_time" here is for the local open/pending entry, not for the eventual
         * bundle (the actual DACS bundle itself doesn't even exist yet). */
        store_entry->expire_time   = store_entry->last_eval_time + BP_CACHE_DACS_LIFETIME;
        store_entry->transmit_time = store_entry->last_eval_time + BP_CACHE_DACS_OPEN_TIME;

        /* need to fill out the delivery_data so this will look like a regular bundle when sent */
        self_intf_id = bplib_mpool_get_external_id(bplib_store_cache_state_self_block(state));
        pri_block->delivery_data.delivery_policy      = bplib_policy_delivery_local_ack;
        pri_block->delivery_data.local_retx_interval  = BP_CACHE_FAST_RETRY_TIME;
        pri_block->delivery_data.ingress_intf_id      = self_intf_id;
        pri_block->delivery_data.ingress_time         = store_entry->last_eval_time;
        pri_block->delivery_data.storage_intf_id      = self_intf_id;
        pri_block->delivery_data.committed_storage_id = (bp_sid_t)sblk;

        /* the ack will be sent to the previous custodian of record */
        pri_block->pri_logical_data.destinationEID = *custody_info->custodian_eid;
        ack_content->flow_source_eid               = *custody_info->flow_source_eid;

        /* set convenience pointers in the dacs_pending extension data - this is mainly
         * so these don't need to be re-found when they are needed again later */
        dacs_pending                     = &store_entry->data.dacs;
        dacs_pending->prev_custodian_ref = &pri_block->pri_logical_data.destinationEID;
        dacs_pending->payload_ref        = ack_content;

        bplib_store_cache_add_to_subindex(&state->hash_index, &store_entry->hash_link, custody_info->eid_hash);
        bplib_store_cache_entry_make_pending(
            sblk, BPLIB_STORE_FLAGS_RETENTION_REQUIRED | BPLIB_STORE_FLAG_AWAITING_TRANSMIT, 0);

        custody_info->store_entry = store_entry;
    }
    else
    {
        if (sblk != NULL)
        {
            bplib_mpool_recycle_block(sblk);
            sblk = NULL;
        }

        store_entry = NULL;
    }

    bplib_mpool_ref_release(pending_bundle);
}

void bplib_store_cache_dacs_finalize(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry)
{
    /* after this point, the entry becomes a normal bundle, it is removed from EID hash
     * so future appends are also prevented */
    store_entry->entry_type = bplib_store_cache_entry_type_normal_bundle;
    bplib_store_cache_remove_from_subindex(&state->hash_index, &store_entry->hash_link);
}

void bplib_store_cache_append_dacs(bplib_store_cache_state_t *state, bplib_store_cache_custodian_info_t *custody_info)
{
    bp_custody_accept_payload_block_t *payload;

    if (custody_info->store_entry != NULL)
    {
        payload = custody_info->store_entry->data.dacs.payload_ref;
        if (payload->num_entries < BP_DACS_MAX_SEQ_PER_PAYLOAD)
        {
            payload->sequence_nums[payload->num_entries] = custody_info->sequence_num;
            ++payload->num_entries;
        }

        /* if DACS bundle is full now, mark it as "done" */
        if (payload->num_entries == BP_DACS_MAX_SEQ_PER_PAYLOAD)
        {
            bplib_store_cache_dacs_finalize(state, custody_info->store_entry);
            bplib_store_cache_entry_make_pending(&custody_info->store_entry->hash_link, 0,
                                                 BPLIB_STORE_FLAG_AWAITING_TRANSMIT);
        }
    }
}

void bplib_store_cache_evaluate_bundle_status(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry)
{
    bplib_mpool_block_t          *rblk;
    bplib_mpool_block_t          *eblk;
    bplib_mpool_bblock_primary_t *pri_block;
    bplib_mpool_flow_t           *self_flow;

    rblk      = NULL;
    eblk      = bplib_mpool_dereference(store_entry->refptr);
    pri_block = bplib_mpool_bblock_primary_cast(eblk);
    self_flow = bplib_store_cache_get_flow(state);

    if (pri_block != NULL)
    {
        /* flags indicate things that should _block_ [re]transmission - if they are all clear, send it */
        if ((store_entry->flags & BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE) == 0)
        {
            /* bundle is due for [re]transmit */

            /* Set egress intf to invalid - which can be used as a marker for items which are
             * blocked from egress (it will be set valid again when it actually goes out, so if
             * it stays invalid, that means there is no egress intf available) */
            pri_block->delivery_data.egress_intf_id = BP_INVALID_HANDLE;
            pri_block->delivery_data.egress_time    = 0;

            rblk = bplib_mpool_ref_make_block(store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF, store_entry);
        }
        else if ((store_entry->flags & BPLIB_STORE_FLAG_LOCALLY_QUEUED) == 0 &&
                 bp_handle_is_valid(pri_block->delivery_data.egress_intf_id))
        {
            /* bundle is no longer locally queued here, and confirmed it was fetched by a CLA */
            if (pri_block->delivery_data.delivery_policy != bplib_policy_delivery_custody_tracking)
            {
                /* If not doing full custody tracking, then the egress CLA is now considered the implicit custodian */
                store_entry->flags &= ~BPLIB_STORE_FLAG_AWAITING_CUSTODY;
            }

            /* reschedule the next retransmit time based on the anticipted round-trip time of the egress intf */
            /* JPHFIX: this is fixed for now, but should be configurable/specific to the egress intf */
            store_entry->transmit_time =
                pri_block->delivery_data.egress_time + pri_block->delivery_data.local_retx_interval;
        }
    }

    if (rblk != NULL)
    {
        if (bplib_mpool_flow_try_push(&self_flow->ingress, rblk, 0))
        {
            /*
             * set both flags to indicate this is now in transit ...
             *  - item is locally queued
             *  - item should not be sent again until retransmit time
             */
            store_entry->flags |= BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE;
        }
        else
        {
            bplib_mpool_recycle_block(rblk);
        }

        rblk = NULL;
    }
}

void bplib_store_cache_evaluate_pending_dacs_status(bplib_store_cache_state_t *state,
                                                    bplib_store_cache_entry_t *store_entry)
{
    /* check if dacs is due for transmit */
    if ((store_entry->flags & BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE) == 0)
    {
        bplib_store_cache_dacs_finalize(state, store_entry);
        bplib_store_cache_evaluate_bundle_status(state, store_entry);
    }
}

void bplib_store_cache_schedule_next_visit(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry)
{
    uint64_t ref_time;

    ref_time = store_entry->last_eval_time;

    if ((store_entry->flags & BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE) == 0)
    {
        /* item is pending transmit but blocked for some temporary external reason, so retry more aggressively */
        ref_time += BP_CACHE_FAST_RETRY_TIME;
    }
    else
    {
        /* item is idle for now, but don't want to leave it in a state where it is never checked again at all */
        ref_time += BP_CACHE_IDLE_RETRY_TIME;
    }

    /* calculate the time for the _next_ retransmit event, or expire time (whichever comes first) */
    if (ref_time > store_entry->transmit_time)
    {
        ref_time = store_entry->transmit_time;
    }
    if (ref_time > store_entry->expire_time)
    {
        ref_time = store_entry->expire_time;
    }

    /*
     * determine the batch/bucket that this should go into
     */
    ref_time |= BP_CACHE_TIME_BUCKET_SIZE;

    if (ref_time != store_entry->next_eval_time)
    {
        /*
         * If this was in the time index, the old entry needs to be removed first,
         * then it needs to be added in the new spot
         */
        bplib_store_cache_remove_from_subindex(&state->time_index, &store_entry->time_link);
        bplib_store_cache_add_to_subindex(&state->time_index, &store_entry->time_link, ref_time);
        store_entry->next_eval_time = ref_time;
    }
}

void bplib_store_cache_evaluate_pending_entry(bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_store_cache_entry_t *store_entry;
    uint64_t                   now_time;
    bool                       retention_required;

    /* This cast should always work, unless there is a bug */
    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        state = store_entry->parent_state;

        /* as a catch-all/backup, items will be sent to the expired list, unless
         * a better list is identified (such as idle_list) */
        now_time                    = bplib_os_get_dtntime_ms();
        store_entry->last_eval_time = now_time;

        /*
         * Check various conditions that may indicate action is needed on this bundle
         */
        if (now_time >= store_entry->expire_time)
        {
            store_entry->flags &= ~BPLIB_STORE_FLAG_WITHIN_LIFETIME;
            store_entry->expire_time = BP_CACHE_TIME_INFINITE;
        }

        if (now_time >= store_entry->transmit_time)
        {
            /*
             * Note, there is also a possibility that this bundle is gets sent to a CLA egress queue awaiting
             * the external software entity to call cla_egress() to get it, but it never does.  This should not
             * generate duplicate/multiple retransmit copies if that is the case.
             *
             * To avoid this, the "transmit_time" is set to infinite for now, and shouldn't be reset to something
             * real until it is confirmed that there is no bundle copy in pending output anymore (that is,
             * bplib_store_cache_handle_ref_recycle() was called and it was cleaned up).
             *
             * This is mainly to avoid any potential snowball effects from a misbehaving CLA.  If the CLA was
             * not able to fetch data, it should declare itself DOWN and then all is OK (because existing entries
             * in the egress queue get purged when an intf goes down, and we don't deliver anything new to it).
             */
            store_entry->flags &= ~BPLIB_STORE_FLAG_AWAITING_TRANSMIT;
            store_entry->transmit_time = BP_CACHE_TIME_INFINITE; /* for now... */
        }

        /*
         * Only evaluate entries which are still within their useful lifetime.  Everything
         * else can fall through and get put into the expired list.
         *
         * This checks which flags are NOT set, which is easier by inverting the bits.
         */
        retention_required = !(~store_entry->flags & BPLIB_STORE_FLAGS_RETENTION_REQUIRED);

        if (retention_required)
        {
            switch (store_entry->entry_type)
            {
                case bplib_store_cache_entry_type_normal_bundle:
                    bplib_store_cache_evaluate_bundle_status(state, store_entry);
                    break;
                case bplib_store_cache_entry_type_pending_dacs:
                    bplib_store_cache_evaluate_pending_dacs_status(state, store_entry);
                    break;
                default:
                    /* nothing to do for this entry type */
                    break;
            }

            /* re-evaluate after the above call */
            retention_required = !(~store_entry->flags & BPLIB_STORE_FLAGS_RETENTION_REQUIRED);
        }

        if (retention_required)
        {
            /*
             * The entry is going into the a non-expired list, so we should also ensure
             * that a time-based next visit is also scheduled on it.  This could
             * be far in the future, e.g. for a young bundle where the egress intf
             * is not currently available.
             *
             * The main objective here is to always periodically revisit items in the
             * idle list at some point, don't let them linger indefinitely without
             * at least looking at them.
             */
            bplib_store_cache_schedule_next_visit(state, store_entry);
            bplib_mpool_insert_before(&state->idle_list, sblk);
        }
        else
        {
            /* note - all entries in expired list will be removed from time_index and destination_index */
            bplib_mpool_recycle_block(sblk);
        }
    }
}

void bplib_store_cache_insert_custody_tracking_block(bplib_store_cache_state_t          *state,
                                                     bplib_mpool_bblock_primary_t       *pri_block,
                                                     bplib_store_cache_custodian_info_t *custody_info)
{
    bplib_mpool_bblock_canonical_t *custody_block;

    custody_info->cblk = bplib_mpool_bblock_canonical_alloc(bplib_store_cache_parent_pool(state));
    if (custody_info->cblk != NULL)
    {
        bplib_mpool_bblock_primary_append(pri_block, custody_info->cblk);

        custody_block = bplib_mpool_bblock_canonical_cast(custody_info->cblk);
        if (custody_block != NULL)
        {
            custody_block->canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyTrackingBlock;
            custody_block->canonical_logical_data.canonical_block.blockNum  = bp_blocktype_custodyTrackingBlock;
            custody_block->canonical_logical_data.canonical_block.crctype   = pri_block->pri_logical_data.crctype;

            custody_info->custodian_eid =
                &custody_block->canonical_logical_data.data.custody_tracking_block.current_custodian;
        }
    }
}

int bplib_store_cache_find_dacs_match(void *arg, bplib_mpool_block_t *lblk)
{
    bplib_store_cache_entry_t          *store_entry;
    bplib_store_cache_dacs_pending_t   *dacs_pending;
    bplib_store_cache_custodian_info_t *custody_info;
    int                                 result;

    custody_info = arg;

    /* possible match, but need to verify */
    store_entry = bplib_mpool_generic_data_cast(lblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL && store_entry->entry_type == bplib_store_cache_entry_type_pending_dacs)
    {
        dacs_pending = &store_entry->data.dacs;

        result = memcmp(dacs_pending->prev_custodian_ref, custody_info->custodian_eid, sizeof(bp_endpointid_buffer_t));

        if (result == 0)
        {
            result = memcmp(&dacs_pending->payload_ref->flow_source_eid, custody_info->flow_source_eid,
                            sizeof(bp_endpointid_buffer_t));
        }

        if (result == 0)
        {
            custody_info->store_entry = store_entry;
        }
    }
    else
    {
        result = -1;
    }

    return result;
}

bool bplib_store_cache_find_pending_dacs(bplib_store_cache_state_t          *state,
                                         bplib_store_cache_custodian_info_t *custody_info)
{
    bplib_rbt_link_t          *custody_rbt_link;
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t       *sblk;

    custody_rbt_link = bplib_rbt_search(custody_info->eid_hash, &state->hash_index);
    if (custody_rbt_link != NULL)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(custody_rbt_link);

        /*
         * to handle possible hash collision/overlap, there is a list at this entry,
         * which needs to be searched.  Collisions should be unlikely enough that
         * the lists, if they ever become more than one entry, stay short enough
         * such that sequential search is not a burden.
         */
        sblk = bplib_mpool_search_list(&store_queue->bundle_list, bplib_store_cache_find_dacs_match, custody_info);
    }
    else
    {
        sblk = NULL;
    }

    return (sblk != NULL);
}

void bplib_store_cache_get_custodian_info(bplib_store_cache_custodian_info_t *custody_info,
                                          bplib_mpool_bblock_primary_t       *pri_block)
{
    bplib_mpool_bblock_canonical_t *custody_block;
    bp_ipn_addr_t                   final_dest_addr;

    memset(custody_info, 0, sizeof(*custody_info));

    custody_info->flow_source_eid = &pri_block->pri_logical_data.sourceEID;
    custody_info->sequence_num    = pri_block->pri_logical_data.creationTimeStamp.sequence_num;

    v7_get_eid(&final_dest_addr, &pri_block->pri_logical_data.destinationEID);
    custody_info->final_dest_node = final_dest_addr.node_number;

    custody_info->cblk = bplib_mpool_bblock_primary_locate_canonical(pri_block, bp_blocktype_custodyTrackingBlock);
    if (custody_info->cblk != NULL)
    {
        custody_block = bplib_mpool_bblock_canonical_cast(custody_info->cblk);
        if (custody_block != NULL)
        {
            /* need to generate a DACS back to the previous custodian indicated in the custody block */
            custody_info->custodian_eid =
                &custody_block->canonical_logical_data.data.custody_tracking_block.current_custodian;
        }
    }
}

void bplib_store_cache_ack_custody_tracking_block(bplib_store_cache_state_t          *state,
                                                  bplib_store_cache_custodian_info_t *custody_info)
{
    bplib_store_cache_compute_custody_hash(BPLIB_STORE_HASH_SALT_DACS, custody_info);

    if (!bplib_store_cache_find_pending_dacs(state, custody_info))
    {
        /* open DACS bundle did not exist - make an empty one now */
        bplib_store_cache_open_dacs(state, custody_info);
    }

    bplib_store_cache_append_dacs(state, custody_info);
}

void bplib_store_cache_update_custody_tracking_block(bplib_store_cache_state_t          *state,
                                                     bplib_store_cache_entry_t          *bundle_store_entry,
                                                     bplib_store_cache_custodian_info_t *custody_info)
{
    v7_set_eid(custody_info->custodian_eid, &state->self_addr);

    /* when the custody ACK for this block comes in, this block needs to be found again,
     * so make an entry in the hash index for it */
    bplib_store_cache_compute_custody_hash(BPLIB_STORE_HASH_SALT_BUNDLE ^ custody_info->sequence_num, custody_info);
    bplib_store_cache_add_to_subindex(&state->hash_index, &bundle_store_entry->hash_link, custody_info->eid_hash);
}

void bplib_store_cache_do_custody_tracking(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry,
                                           bplib_mpool_bblock_primary_t *pri_block)
{
    bplib_store_cache_custodian_info_t custody_info;
    bool                               is_local;

    bplib_store_cache_get_custodian_info(&custody_info, pri_block);

    /* check if this is the last stop on the custody train */
    is_local = (custody_info.final_dest_node == state->self_addr.node_number);

    if (custody_info.custodian_eid)
    {
        /* Acknowledge the block in the bundle */
        bplib_store_cache_ack_custody_tracking_block(state, &custody_info);

        if (is_local)
        {
            bplib_mpool_recycle_block(custody_info.cblk);
            custody_info.cblk          = NULL;
            custody_info.custodian_eid = NULL;

            /* this only needs acceptance by the local delivery agent, do not expect an ack bundle */
            pri_block->delivery_data.delivery_policy = bplib_policy_delivery_local_ack;
        }
    }
    else if (!is_local)
    {
        /* There is no previous custodian, but the custody block needs to be added (because this
         * function is only invoked where full custody tracking is enabled).  This is the case when
         * this storage entity is the first custodian on locally generated bundles */
        bplib_store_cache_insert_custody_tracking_block(state, pri_block, &custody_info);
    }

    if (custody_info.custodian_eid)
    {
        /* update the custody block to reflect the new custodian (this service) -
         * whenever the bundle is finally forwarded, this tells the recipient to notify us */
        bplib_store_cache_update_custody_tracking_block(state, store_entry, &custody_info);
    }
}

int bplib_store_cache_find_bundle_match(void *arg, bplib_mpool_block_t *lblk)
{
    bplib_store_cache_entry_t          *store_entry;
    bplib_mpool_bblock_primary_t       *pri_block;
    bplib_store_cache_custodian_info_t *custody_info;
    int                                 result;

    /* possible match, but need to verify */
    custody_info = arg;
    result       = -1;

    store_entry = bplib_mpool_generic_data_cast(lblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL && store_entry->entry_type == bplib_store_cache_entry_type_normal_bundle)
    {
        pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

        if (pri_block != NULL &&
            pri_block->pri_logical_data.creationTimeStamp.sequence_num == custody_info->sequence_num)
        {
            result = memcmp(&pri_block->pri_logical_data.sourceEID, custody_info->flow_source_eid,
                            sizeof(bp_endpointid_buffer_t));
        }
    }

    if (result == 0)
    {
        custody_info->store_entry = store_entry;
    }

    return result;
}

void bplib_store_cache_do_ack_bundle(bplib_store_cache_state_t *state, bplib_store_cache_custodian_info_t *custody_info)
{
    bplib_rbt_link_t          *custody_rbt_link;
    bplib_store_cache_queue_t *store_queue;

    bplib_store_cache_compute_custody_hash(BPLIB_STORE_HASH_SALT_BUNDLE ^ custody_info->sequence_num, custody_info);

    custody_rbt_link = bplib_rbt_search(custody_info->eid_hash, &state->hash_index);
    if (custody_rbt_link != NULL)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(custody_rbt_link);

        /*
         * to handle possible hash collision/overlap, there is a list at this entry,
         * which needs to be searched.  Collisions should be unlikely enough that
         * the lists, if they ever become more than one entry, stay short enough
         * such that sequential search is not a burden.
         */
        if (bplib_mpool_search_list(&store_queue->bundle_list, bplib_store_cache_find_bundle_match, custody_info))
        {
            /* found it ! */
            printf("%s(): Got custody ACK for seq %lu\n", __func__, (unsigned long)custody_info->sequence_num);

            /* can clear the flag that says it needed another custodian, and reevaluate */
            bplib_store_cache_entry_make_pending(&custody_info->store_entry->hash_link, 0,
                                                 BPLIB_STORE_FLAG_AWAITING_CUSTODY);
        }
    }
}

void bplib_store_cache_process_dacs(bplib_store_cache_state_t *state, bplib_mpool_bblock_primary_t *pri_block,
                                    const bp_custody_accept_payload_block_t *ack_payload)
{
    bp_integer_t                       i;
    bplib_store_cache_custodian_info_t custody_info;

    memset(&custody_info, 0, sizeof(custody_info));

    custody_info.custodian_eid   = &pri_block->pri_logical_data.destinationEID;
    custody_info.flow_source_eid = &ack_payload->flow_source_eid;

    for (i = 0; i < ack_payload->num_entries; ++i)
    {
        custody_info.sequence_num = ack_payload->sequence_nums[i];
        bplib_store_cache_do_ack_bundle(state, &custody_info);
    }
}

bool bplib_store_cache_check_dacs(bplib_store_cache_state_t *state, bplib_mpool_block_t *qblk)
{
    bplib_mpool_bblock_primary_t   *pri_block;
    bplib_mpool_bblock_canonical_t *c_block;

    c_block   = NULL;
    pri_block = bplib_mpool_bblock_primary_cast(qblk);
    if (pri_block != NULL)
    {
        /* check if it has a custody_ack payload type */
        c_block = bplib_mpool_bblock_canonical_cast(
            bplib_mpool_bblock_primary_locate_canonical(pri_block, bp_blocktype_custodyAcceptPayloadBlock));
        if (c_block != NULL)
        {
            /* it is an acceptance block (dacs) */
            bplib_store_cache_process_dacs(state, pri_block,
                                           &c_block->canonical_logical_data.data.custody_accept_payload_block);
        }
    }

    return (c_block != NULL);
}

int bplib_store_cache_construct_entry(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_entry_t *store_entry;

    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry == NULL)
    {
        return BP_ERROR;
    }

    store_entry->parent_state   = arg;
    store_entry->last_eval_time = bplib_os_get_dtntime_ms();
    bplib_mpool_init_secondary_link(sblk, &store_entry->hash_link, bplib_mpool_blocktype_secondary_generic);
    bplib_mpool_init_secondary_link(sblk, &store_entry->time_link, bplib_mpool_blocktype_secondary_generic);
    bplib_mpool_init_secondary_link(sblk, &store_entry->destination_link, bplib_mpool_blocktype_secondary_generic);

    return BP_SUCCESS;
}

int bplib_store_cache_destruct_entry(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_entry_t *store_entry;
    bplib_store_cache_state_t *state;

    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry == NULL)
    {
        return BP_ERROR;
    }

    state = store_entry->parent_state;

    /* need to make sure this is removed from all index trees */
    bplib_store_cache_remove_from_subindex(&state->hash_index, &store_entry->hash_link);
    bplib_store_cache_remove_from_subindex(&state->time_index, &store_entry->time_link);
    bplib_store_cache_remove_from_subindex(&state->dest_eid_index, &store_entry->destination_link);

    /* release the refptr */
    bplib_mpool_ref_release(store_entry->refptr);
    store_entry->refptr = NULL;

    return BP_SUCCESS;
}

int bplib_store_cache_egress_impl(void *arg, bplib_mpool_block_t *subq_src)
{
    bplib_mpool_flow_t           *flow;
    bplib_mpool_block_t          *qblk;
    bplib_mpool_block_t          *sblk;
    bplib_mpool_block_t          *intf_block;
    bplib_store_cache_state_t    *state;
    bplib_mpool_bblock_primary_t *pri_block;
    bplib_store_cache_entry_t    *store_entry;
    bp_primary_block_t           *pri;
    bp_ipn_addr_t                 dest_addr;
    int                           forward_count;

    intf_block = bplib_mpool_get_block_from_link(subq_src);
    state      = bplib_store_cache_get_state(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    flow = bplib_mpool_flow_cast(intf_block);
    if (flow == NULL)
    {
        return -1;
    }

    forward_count = 0;
    while (true)
    {
        qblk = bplib_mpool_flow_try_pull(&flow->egress, 0);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        ++forward_count;

        /* Is this a data bundle that needs to be stored, or is this a custody ack? */
        if (bplib_store_cache_check_dacs(state, qblk))
        {
            /* it is an ACK; these do not need to be stored */
            sblk = NULL;
        }
        else
        {
            /* Create the storage-specific data block for keeping local refs  */
            sblk = bplib_mpool_generic_data_alloc(bplib_store_cache_parent_pool(state), BPLIB_STORE_SIGNATURE_ENTRY,
                                                  state);
        }

        if (sblk != NULL)
        {
            /* These 2 are dynamic casts, could return NULL, but should not unless there are bugs */
            pri_block   = bplib_mpool_bblock_primary_cast(qblk);
            store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);

            if (pri_block != NULL && store_entry != NULL)
            {
                /* this is a static cast, cannot return NULL */
                pri = bplib_mpool_bblock_primary_get_logical(pri_block);

                store_entry->entry_type = bplib_store_cache_entry_type_normal_bundle;

                /* this keeps a copy of the ref here, after qblk is recycled */
                store_entry->refptr = bplib_mpool_ref_from_block(qblk);

                v7_get_eid(&dest_addr, &pri->destinationEID);
                bplib_store_cache_add_to_subindex(&state->dest_eid_index, &store_entry->destination_link,
                                                  dest_addr.node_number);

                if (pri_block->delivery_data.delivery_policy != bplib_policy_delivery_none)
                {
                    store_entry->flags |= BPLIB_STORE_FLAG_AWAITING_CUSTODY;
                }

                /*
                 * Determine expire_time - this should reflect the absolute DTN time at which
                 * this bundle is no longer useful.  This serves as the index into the time tree.
                 */
                /* for now, assume its within lifetime, first timed poll will find out if its not */
                /* TBD: may not want to even store it if this is not true */
                store_entry->expire_time = pri->creationTimeStamp.time + pri->lifetime;
                store_entry->flags |= BPLIB_STORE_FLAG_WITHIN_LIFETIME;

                pri_block->delivery_data.storage_intf_id      = bplib_mpool_get_external_id(intf_block);
                pri_block->delivery_data.committed_storage_id = (bp_sid_t)sblk;

                /* NOTE:
                 * This may also be the right place to generate a custody signal, if the bundle
                 * has the full custody tracking/transfer service level.
                 */
                if (pri_block->delivery_data.delivery_policy == bplib_policy_delivery_custody_tracking)
                {
                    bplib_store_cache_do_custody_tracking(state, store_entry, pri_block);
                }

                /* This puts it into the right spot for future holding */
                bplib_store_cache_evaluate_pending_entry(sblk);

                /* done with the bundle and the storage entry block (do not recycle it) */
                sblk = NULL;
            }
        }

        /*
         * If anything is left pointing to a block, those blocks should be recycled.
         * (or else this would be a leak).
         *
         * The original/input ref to the bundle can be removed without issue, a copy
         * was stored, so this should not trigger a zero refcount unless the storage
         * failed.
         */
        if (qblk)
        {
            bplib_mpool_recycle_block(qblk);
        }
        if (sblk)
        {
            bplib_mpool_recycle_block(sblk);
        }
    }

    return forward_count;
}

void bplib_store_cache_flush_pending(bplib_store_cache_state_t *state)
{
    bplib_mpool_list_iter_t list_it;
    int                     status;
    bplib_mpool_flow_t     *self_flow;

    self_flow = bplib_store_cache_get_flow(state);

    /* Attempt to re-route all bundles in the pending list */
    /* In some cases the bundle can get re-added to the pending list, so this is done in a loop */
    status = bplib_mpool_list_iter_goto_first(&state->pending_list, &list_it);
    while (status == BP_SUCCESS && bplib_mpool_subq_workitem_may_push(&self_flow->ingress))
    {
        /* removal of an iterator node is allowed */
        bplib_mpool_extract_node(list_it.position);
        bplib_store_cache_evaluate_pending_entry(list_it.position);
        status = bplib_mpool_list_iter_forward(&list_it);
    }
}

int bplib_store_cache_do_poll(bplib_store_cache_state_t *state)
{
    bplib_rbt_iter_t           rbt_it;
    bplib_mpool_list_iter_t    list_it;
    bplib_store_cache_queue_t *store_queue;
    int                        rbt_status;
    int                        list_status;

    rbt_status = bplib_rbt_iter_goto_max(bplib_os_get_dtntime_ms(), &state->time_index, &rbt_it);
    while (rbt_status == BP_SUCCESS)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(rbt_it.position);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        rbt_status = bplib_rbt_iter_prev(&rbt_it);

        /* move the entire set of nodes on this tree entry to the pending_list */
        /* remove everything from the time index because its time has passed and will be rescheduled */
        list_status = bplib_mpool_list_iter_goto_first(&store_queue->bundle_list, &list_it);
        while (list_status == BP_SUCCESS)
        {
            /* removal of an iterator node is allowed */
            bplib_mpool_extract_node(list_it.position);
            bplib_store_cache_entry_make_pending(list_it.position, 0, 0);
            list_status = bplib_mpool_list_iter_forward(&list_it);
        }

        /* done with this entry in the time index (will be re-added when pending_list is processed) */
        bplib_rbt_extract_node(&state->time_index, &store_queue->rbt_link);

        bplib_mpool_recycle_block(bplib_mpool_get_block_from_link(&store_queue->bundle_list));
    }

    return BP_SUCCESS;
}

int bplib_store_cache_do_route_up(bplib_store_cache_state_t *state, bp_ipn_t dest, bp_ipn_t mask)
{
    bplib_rbt_iter_t           rbt_it;
    bplib_mpool_list_iter_t    list_it;
    bplib_store_cache_queue_t *store_queue;
    bp_ipn_t                   curr_ipn;
    int                        rbt_status;
    int                        list_status;

    rbt_status = bplib_rbt_iter_goto_min(dest, &state->dest_eid_index, &rbt_it);
    while (rbt_status == BP_SUCCESS)
    {
        curr_ipn = bplib_rbt_get_key_value(rbt_it.position);
        if ((curr_ipn & mask) != dest)
        {
            /* no longer a route match, all done */
            break;
        }

        rbt_status  = bplib_rbt_iter_next(&rbt_it);
        store_queue = bplib_store_cache_queue_from_rbt_link(rbt_it.position);

        /* put everything on the bundle list here onto the pending_list, but
         * do not remove from the bundle list at this time */
        list_status = bplib_mpool_list_iter_goto_first(&store_queue->bundle_list, &list_it);
        while (list_status == BP_SUCCESS)
        {
            bplib_store_cache_entry_make_pending(list_it.position, 0, 0);
            list_status = bplib_mpool_list_iter_forward(&list_it);
        }
    }

    return BP_SUCCESS;
}

int bplib_store_cache_do_intf_statechange(bplib_store_cache_state_t *state, bool is_up)
{
    bplib_mpool_flow_t *self_flow;

    self_flow = bplib_store_cache_get_flow(state);
    if (is_up)
    {
        self_flow->ingress.current_depth_limit = BP_MPOOL_MAX_SUBQ_DEPTH;
        self_flow->egress.current_depth_limit  = BP_MPOOL_MAX_SUBQ_DEPTH;
    }
    else
    {
        self_flow->ingress.current_depth_limit = 0;
        self_flow->egress.current_depth_limit  = 0;
    }
    return BP_SUCCESS;
}

int bplib_store_cache_event_impl(void *event_arg, bplib_mpool_block_t *intf_block)
{
    bplib_store_cache_state_t        *state;
    bplib_mpool_flow_generic_event_t *event;
    bp_handle_t                       self_intf_id;

    event        = event_arg;
    self_intf_id = bplib_mpool_get_external_id(intf_block);
    state        = bplib_store_cache_get_state(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    if (event->event_type == bplib_mpool_flow_event_poll)
    {
        bplib_store_cache_do_poll(state);
    }
    else if ((event->event_type == bplib_mpool_flow_event_up || event->event_type == bplib_mpool_flow_event_down) &&
             bp_handle_equal(self_intf_id, event->intf_state.intf_id))
    {
        bplib_store_cache_do_intf_statechange(state, event->event_type == bplib_mpool_flow_event_up);
    }

    /* any sort of action may have put bundles in the pending queue, so flush it now */
    bplib_store_cache_flush_pending(state);

    return BP_SUCCESS;
}

int bplib_store_cache_construct_state(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;

    state = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        return BP_ERROR;
    }

    bplib_mpool_init_list_head(sblk, &state->pending_list);
    bplib_mpool_init_list_head(sblk, &state->idle_list);

    bplib_rbt_init_root(&state->hash_index);
    bplib_rbt_init_root(&state->dest_eid_index);
    bplib_rbt_init_root(&state->time_index);

    return BP_SUCCESS;
}

int bplib_store_cache_destruct_state(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;

    state = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        return BP_ERROR;
    }

    /* At this point, all the sub-indices and lists should be empty.  The application
     * should have made this so before attempting to delete the intf.
     * If not so, they cannot be cleaned up now, because the state object is no longer valid,
     * the desctructors for these objects will not work correctly */
    assert(bplib_rbt_tree_is_empty(&state->time_index));
    assert(bplib_rbt_tree_is_empty(&state->dest_eid_index));
    assert(bplib_rbt_tree_is_empty(&state->hash_index));
    assert(bplib_mpool_is_link_unattached(&state->idle_list));
    assert(bplib_mpool_is_link_unattached(&state->pending_list));

    return BP_SUCCESS;
}

int bplib_store_cache_construct_blockref(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_blockref_t *blockref;
    bplib_store_cache_entry_t    *store_entry;

    store_entry = arg;
    blockref    = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (blockref == NULL)
    {
        return BP_ERROR;
    }

    /*
     * note, this needs a ref back to the block itself, not the store_entry object.
     */
    blockref->storage_entry_block = bplib_store_cache_entry_self_block(store_entry);
    return BP_SUCCESS;
}

void bplib_store_cache_init(bplib_mpool_t *pool)
{
    const bplib_mpool_blocktype_api_t state_api = (bplib_mpool_blocktype_api_t) {
        .construct = bplib_store_cache_construct_state,
        .destruct  = bplib_store_cache_destruct_state,
    };

    const bplib_mpool_blocktype_api_t entry_api = (bplib_mpool_blocktype_api_t) {
        .construct = bplib_store_cache_construct_entry,
        .destruct  = bplib_store_cache_destruct_entry,
    };

    const bplib_mpool_blocktype_api_t queue_api = (bplib_mpool_blocktype_api_t) {
        .construct = bplib_store_cache_construct_queue,
        .destruct  = bplib_store_cache_destruct_queue,
    };

    const bplib_mpool_blocktype_api_t blockref_api = (bplib_mpool_blocktype_api_t) {
        .construct = bplib_store_cache_construct_blockref,
        .destruct  = bplib_store_cache_handle_ref_recycle,
    };

    bplib_mpool_register_blocktype(pool, BPLIB_STORE_SIGNATURE_STATE, &state_api, sizeof(bplib_store_cache_state_t));
    bplib_mpool_register_blocktype(pool, BPLIB_STORE_SIGNATURE_ENTRY, &entry_api, sizeof(bplib_store_cache_entry_t));
    bplib_mpool_register_blocktype(pool, BPLIB_STORE_SIGNATURE_QUEUE, &queue_api, sizeof(bplib_store_cache_queue_t));
    bplib_mpool_register_blocktype(pool, BPLIB_STORE_SIGNATURE_BLOCKREF, &blockref_api,
                                   sizeof(bplib_store_cache_blockref_t));
}

bp_handle_t bplib_store_cache_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_block_t       *sblk;
    bplib_mpool_t             *pool;
    bplib_mpool_ref_t          flow_block_ref;
    bp_handle_t                storage_intf_id;

    pool = bplib_route_get_mpool(tbl);

    /* register Mem Cache storage module */
    bplib_store_cache_init(pool);

    sblk = bplib_mpool_flow_alloc(pool, BPLIB_STORE_SIGNATURE_STATE, pool);
    if (sblk == NULL)
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "%s(): Insufficient memory to create file storage\n", __func__);
        return BP_INVALID_HANDLE;
    }

    /* this must always work, it was just created above */
    flow_block_ref = bplib_mpool_ref_create(sblk);
    state          = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);

    storage_intf_id = bplib_dataservice_attach(tbl, service_addr, bplib_dataservice_type_storage, flow_block_ref);
    if (!bp_handle_is_valid(storage_intf_id))
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot attach - service addr invalid?\n", __func__);
        bplib_mpool_ref_release(flow_block_ref);
    }
    else
    {
        /* there should be no reason for any of these reg calls to fail */
        bplib_route_register_forward_egress_handler(tbl, storage_intf_id, bplib_store_cache_egress_impl);
        bplib_route_register_forward_ingress_handler(tbl, storage_intf_id, bplib_route_ingress_to_parent);
        bplib_route_register_event_handler(tbl, storage_intf_id, bplib_store_cache_event_impl);

        /* This will keep the ref to itself inside of the state struct, this
         * creates a circular reference and prevents the refcount from ever becoming 0
         */
        state->self_addr = *service_addr;
    }

    return storage_intf_id;
}

int bplib_store_cache_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_ref_t          flow_block_ref;
    int                        status;

    flow_block_ref = bplib_dataservice_detach(tbl, service_addr);
    if (flow_block_ref != NULL)
    {
        state = bplib_mpool_generic_data_cast(bplib_mpool_dereference(flow_block_ref), BPLIB_STORE_SIGNATURE_STATE);
    }
    else
    {
        state = NULL;
    }

    if (state == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot detach - service addr invalid?\n", __func__);
        status = BP_ERROR;
    }
    else
    {
        /* Release the local ref - this should cause the refcount to become 0 */
        bplib_mpool_ref_release(flow_block_ref);
        status = BP_SUCCESS;
    }

    return status;
}

void bplib_store_cache_debug_scan(bplib_routetbl_t *tbl, bp_handle_t intf_id)
{
    bplib_mpool_ref_t          intf_block_ref;
    bplib_store_cache_state_t *state;

    intf_block_ref = bplib_route_get_intf_controlblock(tbl, intf_id);
    if (intf_block_ref == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf invalid\n", __func__);
        return;
    }

    state = bplib_store_cache_get_state(bplib_mpool_dereference(intf_block_ref));
    if (state == NULL)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): Parent intf is not a storage cache\n", __func__);
        return;
    }

    printf("DEBUG: %s() intf_id=%d\n", __func__, bp_handle_printable(intf_id));

    bplib_mpool_debug_print_list_stats(&state->pending_list, "pending_list");
    bplib_mpool_debug_print_list_stats(&state->idle_list, "idle_list");

    bplib_route_release_intf_controlblock(tbl, intf_block_ref);
}

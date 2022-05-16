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
#define BP_CACHE_DACS_OPEN_TIME 2500

#define BP_CACHE_IDLE_RETRY_TIME 3600000
#define BP_CACHE_FAST_RETRY_TIME 2000

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
    bplib_mpool_ref_t   self_fblk_ref;
    bp_ipn_addr_t       self_addr;
    bplib_mpool_flow_t *self_flow;
    bplib_mpool_t      *parent_pool;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    bplib_mpool_block_t pending_list;

    /*
     * expired_list holds bundle refs that have reached expiration time and
     * may be recycled, pending its removal from the other secondary indices.
     */
    bplib_mpool_block_t expired_list;

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
    bplib_rbt_link_t     rbt_link; /* must be first */
    bplib_mpool_block_t *self;
    bplib_mpool_block_t  bundle_list; /* jphfix - subq? */

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

/* Allows reconsitition of the queue struct from an RBT link pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_rbt_link(const bplib_rbt_link_t *link)
{
    /* this relies on "rbt_link" being the first entry in the struct */
    return (bplib_store_cache_queue_t *)((void *)link);
}

/* Allows reconsitition of the entry struct from an time tree link pointer */
static inline bplib_store_cache_queue_t *bplib_store_cache_queue_from_bundle_list(const bplib_mpool_block_t *list)
{
    return (bplib_store_cache_queue_t *)((uint8_t *)list - offsetof(bplib_store_cache_queue_t, bundle_list));
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

    sblk        = bplib_mpool_obtain_base_block(qblk);
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

void bplib_store_cache_handle_ref_recycle(void *arg, bplib_mpool_block_t *rblk)
{
    bplib_store_cache_blockref_t *block_ref;

    block_ref = bplib_mpool_generic_data_cast(rblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (block_ref != NULL)
    {
        assert(block_ref->storage_entry_block != NULL);

        /*
         * always put back into pending_list, this will re-evalute its current
         * state and reclassify it as appropriate.  This also clears the BPLIB_STORE_FLAG_LOCALLY_QUEUED
         * flag.
         */
        bplib_store_cache_entry_make_pending(block_ref->storage_entry_block, 0, BPLIB_STORE_FLAG_LOCALLY_QUEUED);
    }
}

void bplib_store_cache_remove_from_subindex(bplib_mpool_t *pool, bplib_rbt_root_t *index_root,
                                            bplib_mpool_block_t *index_link)
{
    bplib_store_cache_queue_t *store_queue;
    bplib_mpool_block_t       *list_ptr;

    /* grab the list ptr before removal (in case it becomes empty by this) */
    list_ptr = bplib_mpool_get_next_block(index_link);
    bplib_mpool_extract_node(index_link);

    /* If this arrived back at an empty head node, that means this was the last entry in that subq,
     * which then needs to be removed from its parent index tree */
    if (list_ptr != index_link && bplib_mpool_is_empty_list_head(list_ptr))
    {
        store_queue = bplib_store_cache_queue_from_bundle_list(list_ptr);

        if (bplib_rbt_get_key_value(&store_queue->rbt_link) != 0)
        {
            bplib_rbt_extract_node(index_root, &store_queue->rbt_link);
        }

        bplib_mpool_recycle_block(pool, store_queue->self);
    }
}

void bplib_store_cache_construct_queue(void *arg, bplib_mpool_block_t *tblk)
{
    bplib_store_cache_queue_t *store_queue;

    store_queue = bplib_mpool_generic_data_cast(tblk, BPLIB_STORE_SIGNATURE_QUEUE);
    if (store_queue != NULL)
    {
        bplib_mpool_init_list_head(&store_queue->bundle_list);
        store_queue->self = tblk;
    }
}

void bplib_store_cache_add_to_subindex(bplib_mpool_t *pool, bplib_rbt_root_t *index_root,
                                       bplib_mpool_block_t *index_link, bp_val_t index_val)
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
        tblk        = bplib_mpool_generic_data_alloc(pool, BPLIB_STORE_SIGNATURE_QUEUE, NULL);
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

bp_val_t bplib_store_cache_compute_custody_hash(bp_val_t salt, const bp_endpointid_buffer_t *flow_source_eid,
                                                const bp_endpointid_buffer_t *custodian_eid)
{
    bp_crcval_t                   hash;
    bplib_crc_parameters_t *const EID_HASH_ALGORITHM = &BPLIB_CRC32_CASTAGNOLI;

    /* use a CRC as a hash function */
    hash = bplib_crc_initial_value(EID_HASH_ALGORITHM) ^ (bp_crcval_t)salt;
    hash = bplib_crc_update(EID_HASH_ALGORITHM, hash, flow_source_eid, sizeof(*flow_source_eid));
    hash = bplib_crc_update(EID_HASH_ALGORITHM, hash, custodian_eid, sizeof(*custodian_eid));

    return (bp_val_t)bplib_crc_finalize(EID_HASH_ALGORITHM, hash);
}

bplib_mpool_ref_t bplib_store_cache_create_dacs_bundle(bplib_store_cache_state_t          *state,
                                                       bplib_mpool_bblock_primary_t      **pri_block_out,
                                                       bp_custody_accept_payload_block_t **pay_out)
{
    int                  status;
    bplib_mpool_block_t *pblk;
    bplib_mpool_block_t *cblk;

    bplib_mpool_bblock_primary_t   *pri_block;
    bp_primary_block_t             *pri;
    bplib_mpool_bblock_canonical_t *c_block;
    bp_canonical_block_buffer_t    *pay;

    /* This needs to turn the DACS information in the temporary object into a full bundle */
    /* Allocate Blocks */
    cblk   = NULL;
    pblk   = NULL;
    status = BP_ERROR;

    do
    {
        pblk      = bplib_mpool_bblock_primary_alloc(state->parent_pool);
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
        cblk    = bplib_mpool_bblock_canonical_alloc(state->parent_pool);
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
        bplib_mpool_recycle_block(state->parent_pool, cblk);
        cblk = NULL;
    }

    if (pblk != NULL && status != BP_SUCCESS)
    {
        bplib_mpool_recycle_block(state->parent_pool, pblk);
        pblk = NULL;
    }

    return bplib_mpool_ref_create(state->parent_pool, pblk);
}

bplib_store_cache_entry_t *bplib_store_cache_open_dacs(bplib_store_cache_state_t *state, bp_val_t eid_hash,
                                                       const bp_endpointid_buffer_t *prev_custodian,
                                                       const bp_endpointid_buffer_t *flow_source_eid)
{
    bplib_mpool_block_t              *sblk;
    bplib_store_cache_entry_t        *store_entry;
    bplib_store_cache_dacs_pending_t *dacs_pending;
    bplib_mpool_ref_t                 pending_bundle;

    bplib_mpool_bblock_primary_t      *pri_block;
    bp_custody_accept_payload_block_t *ack_content;

    /* Create the storage-specific data block for keeping local refs  */
    sblk           = bplib_mpool_generic_data_alloc(state->parent_pool, BPLIB_STORE_SIGNATURE_ENTRY, state);
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
        store_entry->flags         = BPLIB_STORE_FLAG_WITHIN_LIFETIME | BPLIB_STORE_FLAG_AWAITING_TRANSMIT;

        /* need to fill out the delivery_data so this will look like a regular bundle when sent */
        pri_block->delivery_data.delivery_policy      = bplib_policy_delivery_local_ack;
        pri_block->delivery_data.local_retx_interval  = BP_CACHE_FAST_RETRY_TIME;
        pri_block->delivery_data.ingress_intf_id      = state->self_flow->external_id;
        pri_block->delivery_data.ingress_time         = store_entry->last_eval_time;
        pri_block->delivery_data.storage_intf_id      = state->self_flow->external_id;
        pri_block->delivery_data.committed_storage_id = (bp_sid_t)sblk;

        /* the ack will be sent to the previous custodian of record */
        pri_block->pri_logical_data.destinationEID = *prev_custodian;
        ack_content->flow_source_eid               = *flow_source_eid;

        /* set convenience pointers in the dacs_pending extension data - this is mainly
         * so these don't need to be re-found when they are needed again later */
        dacs_pending                     = &store_entry->data.dacs;
        dacs_pending->prev_custodian_ref = &pri_block->pri_logical_data.destinationEID;
        dacs_pending->payload_ref        = ack_content;

        bplib_store_cache_add_to_subindex(state->parent_pool, &state->hash_index, &store_entry->hash_link, eid_hash);
        bplib_store_cache_entry_make_pending(sblk, BPLIB_STORE_FLAGS_RETENTION_REQUIRED, 0);
    }
    else
    {
        if (sblk != NULL)
        {
            bplib_mpool_recycle_block(state->parent_pool, sblk);
            sblk = NULL;
        }

        store_entry = NULL;
    }

    bplib_mpool_ref_release(state->parent_pool, pending_bundle);

    return store_entry;
}

void bplib_store_cache_append_dacs(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *dacs_pending,
                                   bp_integer_t sequence)
{
    bp_custody_accept_payload_block_t *payload;

    payload = dacs_pending->data.dacs.payload_ref;
    if (payload->num_entries < BP_DACS_MAX_SEQ_PER_PAYLOAD)
    {
        payload->sequence_nums[payload->num_entries] = sequence;
        ++payload->num_entries;
    }

    /* if DACS bundle is full now, mark it as "done" */
    if (payload->num_entries == BP_DACS_MAX_SEQ_PER_PAYLOAD)
    {
        bplib_store_cache_entry_make_pending(bplib_mpool_obtain_base_block(&dacs_pending->hash_link), 0,
                                             BPLIB_STORE_FLAG_AWAITING_TRANSMIT);
    }
}

bplib_mpool_block_t *bplib_store_cache_evaluate_bundle_status(bplib_store_cache_state_t *state,
                                                              bplib_store_cache_entry_t *store_entry)
{
    bplib_mpool_block_t          *rblk;
    bplib_mpool_block_t          *eblk;
    bplib_mpool_bblock_primary_t *pri_block;

    rblk      = NULL;
    eblk      = bplib_mpool_dereference(store_entry->refptr);
    pri_block = bplib_mpool_bblock_primary_cast(eblk);

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

            rblk = bplib_mpool_ref_make_block(state->parent_pool, store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF,
                                              store_entry);
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

    return rblk;
}

bplib_mpool_block_t *bplib_store_cache_evaluate_pending_dacs_status(bplib_store_cache_state_t *state,
                                                                    bplib_store_cache_entry_t *store_entry)
{
    bplib_mpool_block_t          *rblk;
    bplib_mpool_block_t          *eblk;
    bplib_mpool_bblock_primary_t *pri_block;

    rblk      = NULL;
    eblk      = bplib_mpool_dereference(store_entry->refptr);
    pri_block = bplib_mpool_bblock_primary_cast(eblk);

    /* check if bundle is due for [re]transmit */
    if (pri_block != NULL && (store_entry->flags & BPLIB_STORE_FLAGS_TRANSMIT_WAIT_STATE) == 0)
    {
        /* Set egress intf to invalid - which can be used as a marker for items which are
         * blocked from egress (it will be set valid again when it actually goes out, so if
         * it stays invalid, that means there is no egress intf available) */
        pri_block->delivery_data.egress_intf_id = BP_INVALID_HANDLE;
        pri_block->delivery_data.egress_time    = 0;

        /* after this point, the entry becomes a normal bundle, it is removed from EID hash
         * so future appends are also prevented */
        store_entry->entry_type = bplib_store_cache_entry_type_normal_bundle;
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->hash_index, &store_entry->hash_link);

        /* Check if the dest queue has any space, before creating the wrapper block */
        rblk = bplib_mpool_ref_make_block(state->parent_pool, store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF,
                                          store_entry);
    }

    return rblk;
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
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->time_index, &store_entry->time_link);
        bplib_store_cache_add_to_subindex(state->parent_pool, &state->time_index, &store_entry->time_link, ref_time);
        store_entry->next_eval_time = ref_time;
    }
}

void bplib_store_cache_evaluate_pending_entry(bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_store_cache_entry_t *store_entry;
    uint64_t                   now_time;
    bool                       retention_required;
    bplib_mpool_block_t       *rblk;

    rblk = NULL;
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
                    rblk = bplib_store_cache_evaluate_bundle_status(state, store_entry);
                    break;
                case bplib_store_cache_entry_type_pending_dacs:
                    rblk = bplib_store_cache_evaluate_pending_dacs_status(state, store_entry);
                    break;
                default:
                    /* nothing to do for this entry type */
                    break;
            }

            if (rblk != NULL)
            {
                if (bplib_mpool_subq_depthlimit_try_push(state->parent_pool, state->self_fblk_ref,
                                                         &state->self_flow->ingress, rblk, 0))
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
                    bplib_mpool_recycle_block(state->parent_pool, rblk);
                }

                rblk = NULL;
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
            bplib_mpool_insert_before(&state->expired_list, sblk);
        }
    }
}

bplib_mpool_block_t *bplib_store_cache_insert_custody_tracking_block(bplib_store_cache_state_t    *state,
                                                                     bplib_mpool_bblock_primary_t *pri_block)
{
    bplib_mpool_block_t            *cblk;
    bplib_mpool_bblock_canonical_t *custody_block;

    cblk = bplib_mpool_bblock_canonical_alloc(state->parent_pool);
    if (cblk != NULL)
    {
        bplib_mpool_bblock_primary_append(pri_block, cblk);

        custody_block = bplib_mpool_bblock_canonical_cast(cblk);
        if (custody_block != NULL)
        {
            custody_block->canonical_logical_data.canonical_block.blockType = bp_blocktype_custodyTrackingBlock;
            custody_block->canonical_logical_data.canonical_block.blockNum  = bp_blocktype_custodyTrackingBlock;
            custody_block->canonical_logical_data.canonical_block.crctype   = pri_block->pri_logical_data.crctype;
        }
    }

    return cblk;
}

bplib_store_cache_entry_t *bplib_store_cache_find_pending_dacs(bplib_store_cache_state_t *state, bp_val_t eid_hash,
                                                               const bp_endpointid_buffer_t *prev_custodian,
                                                               const bp_endpointid_buffer_t *flow_source_eid)
{
    bplib_rbt_link_t                 *custody_rbt_link;
    bplib_store_cache_dacs_pending_t *dacs_pending;
    bplib_store_cache_queue_t        *store_queue;
    bplib_store_cache_entry_t        *store_entry;
    bplib_mpool_list_iter_t           iter;
    int                               status;

    store_entry      = NULL;
    custody_rbt_link = bplib_rbt_search(eid_hash, &state->hash_index);
    if (custody_rbt_link != NULL)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(custody_rbt_link);

        /*
         * to handle possible hash collision/overlap, there is a list at this entry,
         * which needs to be searched.  Collisions should be unlikely enough that
         * the lists, if they ever become more than one entry, stay short enough
         * such that sequential search is not a burden.
         */
        status = bplib_mpool_list_iter_goto_first(&store_queue->bundle_list, &iter);
        while (true)
        {
            if (status != BP_SUCCESS)
            {
                /* no match found */
                store_entry = NULL;
                break;
            }

            /* possible match, but need to verify */
            store_entry = bplib_mpool_generic_data_cast(iter.position, BPLIB_STORE_SIGNATURE_ENTRY);
            if (store_entry != NULL && store_entry->entry_type == bplib_store_cache_entry_type_pending_dacs)
            {
                dacs_pending = &store_entry->data.dacs;

                if (memcmp(dacs_pending->prev_custodian_ref, prev_custodian, sizeof(bp_endpointid_buffer_t)) == 0 &&
                    memcmp(&dacs_pending->payload_ref->flow_source_eid, flow_source_eid,
                           sizeof(bp_endpointid_buffer_t)) == 0)
                {
                    break;
                }
            }

            status = bplib_mpool_list_iter_forward(&iter);
        }
    }

    return store_entry;
}

void bplib_store_cache_ack_custody_tracking_block(bplib_store_cache_state_t    *state,
                                                  bplib_mpool_bblock_primary_t *pri_block, bplib_mpool_block_t *cblk)
{
    bplib_mpool_bblock_canonical_t *custody_block;
    bp_custody_tracking_block_t    *custody_content;
    bplib_store_cache_entry_t      *store_entry;
    bp_val_t                        eid_hash;

    custody_block = bplib_mpool_bblock_canonical_cast(cblk);
    if (custody_block != NULL)
    {
        /* need to generate a DACS back to the previous custodian indicated in the custody block */
        custody_content = &custody_block->canonical_logical_data.data.custody_tracking_block;

        if (custody_content != NULL)
        {
            eid_hash = bplib_store_cache_compute_custody_hash(BPLIB_STORE_HASH_SALT_DACS,
                                                              &pri_block->pri_logical_data.sourceEID,
                                                              &custody_content->current_custodian);

            store_entry = bplib_store_cache_find_pending_dacs(state, eid_hash, &custody_content->current_custodian,
                                                              &pri_block->pri_logical_data.sourceEID);
            if (store_entry == NULL)
            {
                /* open DACS bundle did not exist - make an empty one now */
                store_entry = bplib_store_cache_open_dacs(state, eid_hash, &custody_content->current_custodian,
                                                          &pri_block->pri_logical_data.sourceEID);
            }

            if (store_entry != NULL)
            {
                bplib_store_cache_append_dacs(state, store_entry,
                                              pri_block->pri_logical_data.creationTimeStamp.sequence_num);
            }
        }
    }
}

void bplib_store_cache_update_custody_tracking_block(bplib_store_cache_state_t    *state,
                                                     bplib_store_cache_entry_t    *store_entry,
                                                     bplib_mpool_bblock_primary_t *pri_block, bplib_mpool_block_t *cblk)
{
    bplib_mpool_bblock_canonical_t *custody_block;
    bp_custody_tracking_block_t    *custody_content;
    bp_val_t                        eid_hash;

    custody_block = bplib_mpool_bblock_canonical_cast(cblk);
    if (custody_block != NULL)
    {
        custody_content = &custody_block->canonical_logical_data.data.custody_tracking_block;
        v7_set_eid(&custody_content->current_custodian, &state->self_addr);

        /* when the custody ACK for this block comes in, this block needs to be found again,
         * so make an entry in the hash index for it */
        eid_hash = bplib_store_cache_compute_custody_hash(
            BPLIB_STORE_HASH_SALT_BUNDLE ^ pri_block->pri_logical_data.creationTimeStamp.sequence_num,
            &pri_block->pri_logical_data.sourceEID, &custody_content->current_custodian);

        bplib_store_cache_add_to_subindex(state->parent_pool, &state->hash_index, &store_entry->hash_link, eid_hash);
    }
}

void bplib_store_cache_do_custody_tracking(bplib_store_cache_state_t *state, bplib_store_cache_entry_t *store_entry,
                                           bplib_mpool_bblock_primary_t *pri_block)
{
    bplib_mpool_block_t *cblk;

    cblk = bplib_mpool_bblock_primary_locate_canonical(pri_block, bp_blocktype_custodyTrackingBlock);
    if (cblk != NULL)
    {
        /* Acknowledge the block in the bundle */
        bplib_store_cache_ack_custody_tracking_block(state, pri_block, cblk);
    }
    else
    {
        /* There is no previous custodian, but the custody block needs to be added (because this
         * function is only invoked where full custody tracking is enabled).  This is the case when
         * this storage entity is the first custodian on locally generated  */
        cblk = bplib_store_cache_insert_custody_tracking_block(state, pri_block);
    }

    if (cblk != NULL)
    {
        /* update the custody block to reflect the new custodian (this service) -
         * whenever the bundle is finally forwarded, this tells the recipient to notify us */
        bplib_store_cache_update_custody_tracking_block(state, store_entry, pri_block, cblk);
    }
}

void bplib_store_cache_do_ack_bundle(bplib_store_cache_state_t *state, const bp_endpointid_buffer_t *prev_custodian,
                                     const bp_endpointid_buffer_t *flow_source_eid, bp_integer_t sequence_num)
{
    bplib_rbt_link_t             *custody_rbt_link;
    bplib_store_cache_queue_t    *store_queue;
    bplib_store_cache_entry_t    *store_entry;
    bplib_mpool_bblock_primary_t *pri_block;
    bplib_mpool_list_iter_t       iter;
    int                           status;
    bp_val_t                      eid_hash;

    eid_hash = bplib_store_cache_compute_custody_hash(BPLIB_STORE_HASH_SALT_BUNDLE ^ sequence_num, flow_source_eid,
                                                      prev_custodian);

    store_entry      = NULL;
    custody_rbt_link = bplib_rbt_search(eid_hash, &state->hash_index);
    if (custody_rbt_link != NULL)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(custody_rbt_link);

        /*
         * to handle possible hash collision/overlap, there is a list at this entry,
         * which needs to be searched.  Collisions should be unlikely enough that
         * the lists, if they ever become more than one entry, stay short enough
         * such that sequential search is not a burden.
         */
        status = bplib_mpool_list_iter_goto_first(&store_queue->bundle_list, &iter);
        while (true)
        {
            if (status != BP_SUCCESS)
            {
                /* no match found */
                store_entry = NULL;
                break;
            }

            /* possible match, but need to verify */
            store_entry = bplib_mpool_generic_data_cast(iter.position, BPLIB_STORE_SIGNATURE_ENTRY);
            if (store_entry != NULL && store_entry->entry_type == bplib_store_cache_entry_type_normal_bundle)
            {
                pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

                if (pri_block != NULL && pri_block->pri_logical_data.creationTimeStamp.sequence_num == sequence_num &&
                    memcmp(&pri_block->pri_logical_data.sourceEID, flow_source_eid, sizeof(bp_endpointid_buffer_t)) ==
                        0)
                {
                    /* found it ! */
                    printf("%s(): Got custody ACK for seq %lu\n", __func__, (unsigned long)sequence_num);

                    /* can clear the flag that says it needed another custodian, and reevaluate */
                    bplib_store_cache_entry_make_pending(&store_entry->hash_link, 0, BPLIB_STORE_FLAG_AWAITING_CUSTODY);
                    break;
                }
            }

            status = bplib_mpool_list_iter_forward(&iter);
        }
    }
}

void bplib_store_cache_process_dacs(bplib_store_cache_state_t *state, const bplib_mpool_bblock_primary_t *pri_block,
                                    const bp_custody_accept_payload_block_t *ack_payload)
{
    bp_integer_t i;

    for (i = 0; i < ack_payload->num_entries; ++i)
    {
        bplib_store_cache_do_ack_bundle(state, &pri_block->pri_logical_data.destinationEID,
                                        &ack_payload->flow_source_eid, ack_payload->sequence_nums[i]);
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

void bplib_store_cache_construct_entry(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_entry_t *store_entry;

    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        store_entry->parent_state   = arg;
        store_entry->last_eval_time = bplib_os_get_dtntime_ms();
        bplib_mpool_init_secondary_link(sblk, &store_entry->hash_link);
        bplib_mpool_init_secondary_link(sblk, &store_entry->time_link);
        bplib_mpool_init_secondary_link(sblk, &store_entry->destination_link);
    }
}

void bplib_store_cache_destruct_entry(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_entry_t *store_entry;
    bplib_store_cache_state_t *state;

    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        state = store_entry->parent_state;

        /* need to make sure this is removed from all index trees */
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->hash_index, &store_entry->hash_link);
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->time_index, &store_entry->time_link);
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->dest_eid_index,
                                               &store_entry->destination_link);
    }
}

int bplib_store_cache_egress_impl(bplib_routetbl_t *rtbl, bplib_mpool_ref_t intf_ref, void *egress_arg)
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

    intf_block = bplib_mpool_dereference(intf_ref);
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
        qblk = bplib_mpool_subq_depthlimit_try_pull(state->parent_pool, state->self_fblk_ref, &flow->egress, 0);
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
            sblk = bplib_mpool_generic_data_alloc(state->parent_pool, BPLIB_STORE_SIGNATURE_ENTRY, state);
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
                bplib_store_cache_add_to_subindex(state->parent_pool, &state->dest_eid_index,
                                                  &store_entry->destination_link, dest_addr.node_number);

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

                pri_block->delivery_data.storage_intf_id      = state->self_flow->external_id;
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
            bplib_mpool_recycle_block(state->parent_pool, qblk);
        }
        if (sblk)
        {
            bplib_mpool_recycle_block(state->parent_pool, sblk);
        }
    }

    return forward_count;
}

void bplib_store_cache_cleanup_exipred_entry(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_store_cache_entry_t *store_entry;

    /* This cast should always work, unless there is a bug */
    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        state = store_entry->parent_state;

        /* ensure this has been removed from the various indices */
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->hash_index, &store_entry->hash_link);
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->time_index, &store_entry->time_link);
        bplib_store_cache_remove_from_subindex(state->parent_pool, &state->dest_eid_index,
                                               &store_entry->destination_link);

        /* release the refptr */
        bplib_mpool_ref_release(state->parent_pool, store_entry->refptr);
        store_entry->refptr = NULL;
    }
}

void bplib_store_cache_handle_bundle_pending_list_entry(void *arg, bplib_mpool_block_t *sblk)
{
    /* this is a wrapper to make it compatible with bplib_mpool_foreach_item_in_list() */
    bplib_store_cache_evaluate_pending_entry(sblk);
}

void bplib_store_cache_flush_pending(bplib_store_cache_state_t *state)
{
    /* Attempt to re-route all bundles in the pending list */
    /* In some cases the bundle can get re-added to the pending list, so this is done in a loop */
    do
    {
        bplib_mpool_foreach_item_in_list(&state->pending_list, true, bplib_store_cache_handle_bundle_pending_list_entry,
                                         state);
        bplib_mpool_foreach_item_in_list(&state->expired_list, false, bplib_store_cache_cleanup_exipred_entry, state);
        bplib_mpool_recycle_all_blocks_in_list(state->parent_pool, &state->expired_list);
    }
    while (!bplib_mpool_is_empty_list_head(&state->pending_list));
}

void bplib_store_cache_entry_make_pending_wrapper(void *arg, bplib_mpool_block_t *qblk)
{
    bplib_store_cache_entry_make_pending(qblk, 0, 0);
}

int bplib_store_cache_do_poll(bplib_store_cache_state_t *state)
{
    bplib_rbt_iter_t           it;
    bplib_store_cache_queue_t *store_queue;
    int                        iter_status;

    iter_status = bplib_rbt_iter_goto_max(bplib_os_get_dtntime_ms(), &state->time_index, &it);
    while (iter_status == BP_SUCCESS)
    {
        store_queue = bplib_store_cache_queue_from_rbt_link(it.position);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        iter_status = bplib_rbt_iter_prev(&it);

        /* move the entire set of nodes on this tree entry to the pending_list */
        bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true, bplib_store_cache_entry_make_pending_wrapper,
                                         state);

        /* done with this entry in the time index (will be re-added when pending_list is processed) */
        bplib_rbt_extract_node(&state->time_index, &store_queue->rbt_link);

        bplib_mpool_recycle_block(state->parent_pool, store_queue->self);
    }

    return BP_SUCCESS;
}

int bplib_store_cache_do_route_up(bplib_store_cache_state_t *state, bp_ipn_t dest, bp_ipn_t mask)
{
    bplib_rbt_iter_t           it;
    bplib_store_cache_queue_t *store_queue;
    bp_ipn_t                   curr_ipn;
    int                        iter_status;

    iter_status = bplib_rbt_iter_goto_min(dest, &state->dest_eid_index, &it);
    while (iter_status == BP_SUCCESS)
    {
        curr_ipn = bplib_rbt_get_key_value(it.position);
        if ((curr_ipn & mask) != dest)
        {
            /* no longer a route match, all done */
            break;
        }

        store_queue = bplib_store_cache_queue_from_rbt_link(it.position);

        /* move the entire set of nodes on this tree entry to the pending_list */
        bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, false, bplib_store_cache_entry_make_pending_wrapper,
                                         state);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        iter_status = bplib_rbt_iter_next(&it);
    }

    return BP_SUCCESS;
}

int bplib_store_cache_do_intf_statechange(bplib_store_cache_state_t *state, bool is_up)
{
    if (is_up)
    {
        state->self_flow->ingress.current_depth_limit = BP_MPOOL_MAX_SUBQ_DEPTH;
        state->self_flow->egress.current_depth_limit  = BP_MPOOL_MAX_SUBQ_DEPTH;
    }
    else
    {
        state->self_flow->ingress.current_depth_limit = 0;
        state->self_flow->egress.current_depth_limit  = 0;
    }
    return BP_SUCCESS;
}

int bplib_store_cache_event_impl(bplib_routetbl_t *rtbl, bplib_mpool_ref_t intf_ref, void *event_arg)
{
    bplib_store_cache_state_t *state;
    bplib_generic_event_t     *event;

    event = event_arg;
    state = bplib_store_cache_get_state(bplib_mpool_dereference(intf_ref));
    if (state == NULL)
    {
        return -1;
    }

    if (event->event_type == bplib_event_type_poll_interval)
    {
        bplib_store_cache_do_poll(state);
    }
    else if (event->event_type == bplib_event_type_route_up)
    {
        bplib_store_cache_do_route_up(state, event->route_state.dest, event->route_state.mask);
    }
    else if ((event->event_type == bplib_event_type_interface_up ||
              event->event_type == bplib_event_type_interface_down) &&
             bp_handle_equal(state->self_flow->external_id, event->intf_state.intf_id))
    {
        bplib_store_cache_do_intf_statechange(state, event->event_type == bplib_event_type_interface_up);
    }

    /* any sort of action may have put bundles in the pending queue, so flush it now */
    bplib_store_cache_flush_pending(state);

    return 0;
}

void bplib_store_cache_construct_state(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_t             *pool;

    pool  = arg;
    state = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        state->parent_pool = pool;
        state->self_flow   = bplib_mpool_flow_cast(sblk);

        bplib_mpool_init_list_head(&state->pending_list);
        bplib_mpool_init_list_head(&state->expired_list);
        bplib_mpool_init_list_head(&state->idle_list);

        bplib_rbt_init_root(&state->hash_index);
        bplib_rbt_init_root(&state->dest_eid_index);
        bplib_rbt_init_root(&state->time_index);
    }
}

void bplib_store_cache_destruct_state(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_state_t *state;
    bplib_rbt_iter_t           it;
    bplib_store_cache_queue_t *store_queue;
    int                        status;

    state = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        /* make sure all lists and indices are empty, blocks will be leaked if not */
        /* first tackle the time index */
        status = bplib_rbt_iter_goto_min(0, &state->time_index, &it);
        while (status == BP_SUCCESS)
        {
            store_queue = bplib_store_cache_queue_from_rbt_link(it.position);
            status      = bplib_rbt_iter_next(&it);

            /* move the entire set of nodes on this tree entry to the pending_list */
            bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true,
                                             bplib_store_cache_entry_make_pending_wrapper, state);

            /* done with this entry in the time index */
            bplib_rbt_extract_node(&state->time_index, &store_queue->rbt_link);
            bplib_mpool_recycle_block(state->parent_pool, store_queue->self);
        }

        /* next tackle the destination EID index */
        status = bplib_rbt_iter_goto_min(0, &state->dest_eid_index, &it);
        while (status == BP_SUCCESS)
        {
            store_queue = bplib_store_cache_queue_from_rbt_link(it.position);
            status      = bplib_rbt_iter_next(&it);

            /* move the entire set of nodes on this tree entry to the pending_list */
            bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true,
                                             bplib_store_cache_entry_make_pending_wrapper, state);

            /* done with this entry in the EID index */
            bplib_rbt_extract_node(&state->dest_eid_index, &store_queue->rbt_link);
            bplib_mpool_recycle_block(state->parent_pool, store_queue->self);
        }

        status = bplib_rbt_iter_goto_min(0, &state->hash_index, &it);
        while (status == BP_SUCCESS)
        {
            store_queue = bplib_store_cache_queue_from_rbt_link(it.position);
            status      = bplib_rbt_iter_next(&it);

            /* move the entire set of nodes on this tree entry to the pending_list */
            bplib_mpool_foreach_item_in_list(&store_queue->bundle_list, true,
                                             bplib_store_cache_entry_make_pending_wrapper, state);

            /* done with this entry in the time index */
            bplib_rbt_extract_node(&state->hash_index, &store_queue->rbt_link);
            bplib_mpool_recycle_block(state->parent_pool, store_queue->self);
        }

        bplib_mpool_recycle_all_blocks_in_list(state->parent_pool, &state->idle_list);
        bplib_mpool_recycle_all_blocks_in_list(state->parent_pool, &state->pending_list);
        bplib_mpool_recycle_all_blocks_in_list(state->parent_pool, &state->expired_list);

        state->parent_pool = NULL;
    }
}

void bplib_store_cache_construct_blockref(void *arg, bplib_mpool_block_t *sblk)
{
    bplib_store_cache_blockref_t *blockref;
    bplib_store_cache_entry_t    *store_entry;

    store_entry = arg;
    blockref    = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (blockref != NULL)
    {
        /*
         * note, this needs a ref back to the block itself, not the store_entry object.
         * but because this has two secondary links in it (time/dest index) either of
         * these can be used to reconstitute the original block pointer.
         */
        blockref->storage_entry_block = bplib_mpool_obtain_base_block(&store_entry->time_link);
    }
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
        .destruct  = NULL,
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
    flow_block_ref = bplib_mpool_ref_create(pool, sblk);
    state          = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_STATE);

    storage_intf_id = bplib_dataservice_attach(tbl, service_addr, bplib_dataservice_type_storage, flow_block_ref);
    if (!bp_handle_is_valid(storage_intf_id))
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "%s(): cannot attach - service addr invalid?\n", __func__);
        bplib_mpool_ref_release(pool, flow_block_ref);
    }
    else
    {
        /* there should be no reason for any of these reg calls to fail */
        bplib_route_register_forward_egress_handler(tbl, storage_intf_id, bplib_store_cache_egress_impl, NULL);
        bplib_route_register_forward_ingress_handler(tbl, storage_intf_id, bplib_route_ingress_to_parent, NULL);
        bplib_route_register_event_handler(tbl, storage_intf_id, bplib_store_cache_event_impl);

        /* This will keep the ref to itself inside of the state struct, this
         * creates a circular reference and prevents the refcount from ever becoming 0
         */
        state->self_fblk_ref = flow_block_ref;
        state->self_addr     = *service_addr;
    }

    return storage_intf_id;
}

int bplib_store_cache_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    bplib_store_cache_state_t *state;
    bplib_mpool_ref_t          flow_block_ref;
    int                        status;
    bplib_mpool_t             *pool;

    pool           = bplib_route_get_mpool(tbl);
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
        /* Break the internal circular reference first, this will permit the refcount to become 0 */
        bplib_mpool_ref_release(pool, state->self_fblk_ref);
        state->self_fblk_ref = NULL;

        /* Release the local ref - this should cause the refcount to become 0 */
        bplib_mpool_ref_release(pool, flow_block_ref);
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
    bplib_mpool_debug_print_list_stats(&state->expired_list, "expired_list");
    bplib_mpool_debug_print_list_stats(&state->idle_list, "idle_list");

    bplib_route_release_intf_controlblock(tbl, intf_block_ref);
}

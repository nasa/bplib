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
#define BPLIB_STORE_SIGNATURE_BLOCKREF 0x77e96b11

#define BPLIB_STORE_FLAG_ACTIVITY         0x01
#define BPLIB_STORE_FLAG_LOCAL_CUSTODY    0x02
#define BPLIB_STORE_FLAG_ACTION_TIME_WAIT 0x04
#define BPLIB_STORE_FLAG_LOCALLY_QUEUED   0x08
#define BPLIB_STORE_FLAG_PENDING_FORWARD  0x10

/* the set of flags for which retention is required - all are typically set for valid entries
 * if any of these becomes UN-set, retention of the entry is NOT required */
#define BPLIB_STORE_FLAGS_ACTION_WAIT_STATE (BPLIB_STORE_FLAG_ACTION_TIME_WAIT | BPLIB_STORE_FLAG_LOCALLY_QUEUED)

#define BP_CACHE_DACS_LIFETIME   86400000 /* 24 hrs */
#define BP_CACHE_DACS_OPEN_TIME  10000    /* 10 sec */
#define BP_CACHE_FAST_RETRY_TIME 3000     /* 3 sec */
#define BP_CACHE_IDLE_RETRY_TIME 3600000  /* 1 hour */
#define BP_CACHE_AGE_OUT_TIME    5000     /* 5 seconds */

#define BP_CACHE_TIME_INFINITE BP_DTNTIME_INFINITE

typedef enum bplib_cache_entry_state
{
    bplib_cache_entry_state_undefined,
    bplib_cache_entry_state_idle,
    bplib_cache_entry_state_queue,
    bplib_cache_entry_state_delete,
    bplib_cache_entry_state_generate_dacs,
    bplib_cache_entry_state_max
} bplib_cache_entry_state_t;

typedef struct bplib_cache_state
{
    bp_ipn_addr_t self_addr;

    bplib_mpool_job_t pending_job;

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

    bplib_rbt_root_t bundle_index;
    bplib_rbt_root_t dacs_index;
    bplib_rbt_root_t dest_eid_jphfix_index;
    bplib_rbt_root_t time_jphfix_index;

    const bplib_cache_offload_api_t *offload_api;
    bplib_mpool_block_t             *offload_blk;

    uint32_t generated_dacs_seq;

    uint32_t fsm_state_enter_count[bplib_cache_entry_state_max];
    uint32_t fsm_state_exit_count[bplib_cache_entry_state_max];
    uint32_t discard_count;
} bplib_cache_state_t;

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
    bp_ipn_addr_t             flow_id_copy;
    bp_sequencenumber_t       flow_seq_copy;
    bplib_mpool_ref_t         refptr;
    bp_sid_t                  offload_sid;
    uint64_t                  action_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    uint64_t                  expire_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    bplib_rbt_link_t          hash_rbt_link;
    bplib_rbt_link_t          time_rbt_link;
    bplib_rbt_link_t          dest_eid_rbt_link;
    bplib_cache_entry_data_t  data;
} bplib_cache_entry_t;

typedef struct bplib_cache_blockref
{
    bplib_cache_entry_t *storage_entry;
} bplib_cache_blockref_t;

typedef struct bplib_cache_custodian_info
{
    bool                 match_dacs;
    bp_ipn_addr_t        flow_id;
    bp_ipn_addr_t        custodian_id;
    bplib_mpool_block_t *this_cblk;
    bplib_mpool_block_t *prev_cblk;
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
#define bplib_cache_entry_from_link(ptr, member) \
    bplib_cache_entry_get_container(ptr, offsetof(bplib_cache_entry_t, member))
static inline bplib_cache_entry_t *bplib_cache_entry_get_container(const bplib_rbt_link_t *link, size_t offset)
{
    return (bplib_cache_entry_t *)(void *)((uint8_t *)link - offset);
}

void bplib_cache_custody_finalize_dacs(bplib_cache_state_t *state, bplib_cache_entry_t *store_entry);
void bplib_cache_custody_store_bundle(bplib_cache_state_t *state, bplib_mpool_block_t *qblk);
bool bplib_cache_custody_check_dacs(bplib_cache_state_t *state, bplib_mpool_block_t *qblk);

void bplib_cache_fsm_execute(bplib_mpool_block_t *sblk);

int bplib_cache_entry_tree_insert_unsorted(const bplib_rbt_link_t *node, void *arg);

void bplib_cache_entry_make_pending(bplib_cache_entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags);

int  bplib_cache_egress_impl(void *arg, bplib_mpool_block_t *subq_src);
void bplib_cache_flush_pending(bplib_cache_state_t *state);
int  bplib_cache_do_poll(bplib_cache_state_t *state);
int  bplib_cache_do_route_up(bplib_cache_state_t *state, bp_ipn_t dest, bp_ipn_t mask);
int  bplib_cache_do_intf_statechange(bplib_cache_state_t *state, bool is_up);
int  bplib_cache_event_impl(void *event_arg, bplib_mpool_block_t *intf_block);
int  bplib_cache_process_pending(void *arg, bplib_mpool_block_t *job);
int  bplib_cache_destruct_state(void *arg, bplib_mpool_block_t *sblk);
int  bplib_cache_construct_entry(void *arg, bplib_mpool_block_t *sblk);
int  bplib_cache_destruct_entry(void *arg, bplib_mpool_block_t *sblk);
int  bplib_cache_construct_blockref(void *arg, bplib_mpool_block_t *sblk);
int  bplib_cache_destruct_blockref(void *arg, bplib_mpool_block_t *rblk);
int  bplib_cache_construct_state(void *arg, bplib_mpool_block_t *sblk);

void bplib_cache_custody_insert_tracking_block(bplib_cache_state_t *state, bplib_mpool_bblock_primary_t *pri_block,
                                               bplib_cache_custodian_info_t *custody_info);
int  bplib_cache_custody_find_dacs_match(const bplib_rbt_link_t *node, void *arg);
bool bplib_cache_custody_find_pending_dacs(bplib_cache_state_t *state, bplib_cache_custodian_info_t *dacs_info);
bplib_mpool_ref_t bplib_cache_custody_create_dacs(bplib_cache_state_t                *state,
                                                  bplib_mpool_bblock_primary_t      **pri_block_out,
                                                  bp_custody_accept_payload_block_t **pay_out);
void              bplib_cache_custody_open_dacs(bplib_cache_state_t *state, bplib_cache_custodian_info_t *custody_info);
void bplib_cache_custody_append_dacs(bplib_cache_state_t *state, bplib_cache_custodian_info_t *custody_info);
void bplib_cache_custody_update_tracking_block(bplib_cache_state_t *state, bplib_cache_custodian_info_t *custody_info);
int  bplib_cache_custody_find_bundle_match(const bplib_rbt_link_t *node, void *arg);
void bplib_cache_custody_process_bundle(bplib_cache_state_t *state, bplib_mpool_bblock_primary_t *pri_block,
                                        bplib_cache_custodian_info_t *custody_info);
void bplib_cache_custody_process_remote_dacs_bundle(bplib_cache_state_t *state, bplib_mpool_bblock_primary_t *pri_block,
                                                    const bp_custody_accept_payload_block_t *ack_payload);
void bplib_cache_custody_init_info_from_pblock(bplib_cache_custodian_info_t *custody_info,
                                               bplib_mpool_bblock_primary_t *pri_block);
void bplib_cache_custody_ack_tracking_block(bplib_cache_state_t                *state,
                                            const bplib_cache_custodian_info_t *custody_info);

bplib_cache_entry_state_t bplib_cache_fsm_state_idle_eval(bplib_cache_entry_t *store_entry);
bplib_cache_entry_state_t bplib_cache_fsm_state_queue_eval(bplib_cache_entry_t *store_entry);
void                      bplib_cache_fsm_state_queue_enter(bplib_cache_entry_t *store_entry);
void                      bplib_cache_fsm_state_queue_exit(bplib_cache_entry_t *store_entry);
bplib_cache_entry_state_t bplib_cache_fsm_state_delete_eval(bplib_cache_entry_t *store_entry);
void                      bplib_cache_fsm_state_delete_enter(bplib_cache_entry_t *store_entry);
void                      bplib_cache_fsm_reschedule(bplib_cache_state_t *state, bplib_cache_entry_t *store_entry);
bplib_cache_entry_state_t bplib_cache_fsm_state_generate_dacs_eval(bplib_cache_entry_t *store_entry);
void                      bplib_cache_fsm_state_generate_dacs_exit(bplib_cache_entry_t *store_entry);
void bplib_cache_fsm_transition_state(bplib_cache_entry_t *entry, bplib_cache_entry_state_t next_state);
bplib_cache_entry_state_t bplib_cache_fsm_get_next_state(bplib_cache_entry_t *entry);
#endif /* V7_CACHE_INTERNAL_H */

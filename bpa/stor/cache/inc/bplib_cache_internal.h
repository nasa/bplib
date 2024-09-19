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

#ifndef BPLIB_CACHE_INTERNAL_H
#define BPLIB_CACHE_INTERNAL_H

#ifdef STOR

#include "bplib.h"
#include "bplib_os.h"
#include "bplib_routing.h"
#include "bplib_dataservice.h"
#include "bplib_mem.h"
#include "bplib_cache.h"
#include "v7.h"
#include "bplib_rbtree.h"
#include "BPLib_STOR_CACHE_Module_codec.h"
#include "bplib_mem.h"
#include "bplib_bc_bundle.h"
#include "bplib_s_qm_ducts.h"
#include "bplib_mem_ref.h"
#endif // STOR

struct bplib_cache
{
    BPLib_STOR_CACHE_Block_content_t admin_block; /**< Start of first real block (see num_bufs_total) */
};

typedef struct BPLib_STOR_CACHE_BblockPrimaryContent
{
    BPLib_STOR_CACHE_BblockPrimary_t pblock;
    BPLib_STOR_CACHE_AlignedData_t   user_data_start;
} BPLib_STOR_CACHE_BblockPrimaryContent_t;

typedef struct BPLib_STOR_CACHE_BblockCanonicalContent
{
    BPLib_STOR_CACHE_BblockCanonical_t cblock;
    BPLib_STOR_CACHE_AlignedData_t     user_data_start;
} BPLib_STOR_CACHE_BblockCanonicalContent_t;

struct BPLib_STOR_CACHE_Module_subq_base
{
    BPLib_STOR_CACHE_Block_t block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
};

typedef struct BPLib_STOR_CACHE_Module_subq_workitem
{
    #ifdef STOR
    BPLib_STOR_CACHE_Job_t       job_header;
    #endif // STOR
    BPLib_STOR_CACHE_Module_subq_base_t base_subq;
    unsigned int            current_depth_limit;
} BPLib_STOR_CACHE_Module_subq_workitem_t;

#ifdef STOR // duct
typedef struct BPLib_STOR_CACHE_FlowContent
{
    BPLib_STOR_CACHE_Flow_t         fblock;
    BPLib_STOR_CACHE_AlignedData_t user_data_start;
} BPLib_STOR_CACHE_FlowContent_t;
#endif // STOR

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

typedef enum BPLib_STOR_CACHE_Module_entry_state
{
    BPLib_STOR_CACHE_Module_entry_state_undefined,
    BPLib_STOR_CACHE_Module_entry_state_idle,
    BPLib_STOR_CACHE_Module_entry_state_queue,
    BPLib_STOR_CACHE_Module_entry_state_delete,
    BPLib_STOR_CACHE_Module_entry_state_generate_dacs,
    BPLib_STOR_CACHE_Module_entry_state_max
} BPLib_STOR_CACHE_Module_entry_state_t;

#endif // STOR

typedef struct BPLib_STOR_CACHE_Module_state
{
    int placeholder;

    #ifdef STOR
    bp_ipn_addr_t self_addr;

    BPLib_STOR_CACHE_Job_t pending_job;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    BPLib_STOR_CACHE_Block_t pending_list;

    uint64_t action_time; /**< DTN time when the pending_list was last checked */

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    BPLib_STOR_CACHE_Block_t idle_list;

// TODO Either share the RBT code from MEM or add the rest of the code to STORE_CACHE.
    BPLib_STOR_CACHE_RBT_Root_t bundle_index;
    BPLib_STOR_CACHE_RBT_Root_t dacs_index;
    BPLib_STOR_CACHE_RBT_Root_t dest_eid_jphfix_index;
    BPLib_STOR_CACHE_RBT_Root_t time_jphfix_index;

    const BPLib_STOR_CACHE_Module_offload_api_t *offload_api;
    BPLib_STOR_CACHE_Block_t             *offload_blk;

    uint32_t generated_dacs_seq;

    uint32_t fsm_state_enter_count[BPLib_STOR_CACHE_Module_entry_state_max];
    uint32_t fsm_state_exit_count[BPLib_STOR_CACHE_Module_entry_state_max];
    uint32_t discard_count;
    #endif // STOR
    
} BPLib_STOR_CACHE_Module_state_t;

#ifdef STOR

typedef struct BPLib_STOR_CACHE_Module_dacs_pending
{
    bp_ipn_addr_t                      prev_custodian_id;
    bp_custody_accept_payload_block_t *payload_ref;

} BPLib_STOR_CACHE_Module_dacs_pending_t;

typedef union BPLib_STOR_CACHE_Module_entry_data
{
    BPLib_STOR_CACHE_Module_dacs_pending_t dacs;
} BPLib_STOR_CACHE_Module_entry_data_t;

typedef struct BPLib_STOR_CACHE_Module_entry
{
    BPLib_STOR_CACHE_Module_state_t      *parent;
    BPLib_STOR_CACHE_Module_entry_state_t state;
    uint32_t                  flags;
    bp_ipn_addr_t             flow_id_copy;
    bp_sequencenumber_t       flow_seq_copy;
    BPLib_STOR_CACHE_Ref_t         refptr;
    bp_sid_t                  offload_sid;
    uint64_t                  action_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    uint64_t                  expire_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    BPLib_STOR_CACHE_RBT_Link_t          hash_rbt_link;
    BPLib_STOR_CACHE_RBT_Link_t          time_rbt_link;
    BPLib_STOR_CACHE_RBT_Link_t          dest_eid_rbt_link;
    BPLib_STOR_CACHE_Module_entry_data_t  data;
} BPLib_STOR_CACHE_Module_entry_t;

typedef struct BPLib_STOR_CACHE_Module_blockref
{
    BPLib_STOR_CACHE_Module_entry_t *storage_entry;
} BPLib_STOR_CACHE_Module_blockref_t;

typedef struct BPLib_STOR_CACHE_Module_custodian_info
{
    bool                 match_dacs;
    bp_ipn_addr_t        flow_id;
    bp_ipn_addr_t        custodian_id;
    BPLib_STOR_CACHE_Block_t *this_cblk;
    BPLib_STOR_CACHE_Block_t *prev_cblk;
    bp_val_t             eid_hash;
    bp_sequencenumber_t  sequence_num;
    bp_ipn_t             final_dest_node;
    BPLib_STOR_CACHE_Module_entry_t *store_entry;
} BPLib_STOR_CACHE_Module_custodian_info_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Module_state_self_block(BPLib_STOR_CACHE_Module_state_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return BPLib_STOR_CACHE_GetBlockFromLink(&state->pending_list);
}

/* Allows reconstitution of the parent pool from a cache state pointer */
static inline BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_Module_parent_pool(BPLib_STOR_CACHE_Module_state_t *state)
{
    return BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_Module_state_self_block(state));
}

/* Allows reconstitution of the flow object from a cache state pointer */
static inline BPLib_STOR_CACHE_Flow_t *BPLib_STOR_CACHE_Module_get_flow(BPLib_STOR_CACHE_Module_state_t *state)
{
    return BPLib_STOR_CACHE_FlowCast(BPLib_STOR_CACHE_Module_state_self_block(state));
}

/* Allows reconstitution of the queue struct from an RBT link pointer */
#define BPLib_STOR_CACHE_Module_entry_from_link(ptr, member) \
    BPLib_STOR_CACHE_Module_entry_get_container(ptr, offsetof(BPLib_STOR_CACHE_Module_entry_t, member))
static inline BPLib_STOR_CACHE_Module_entry_t *BPLib_STOR_CACHE_Module_entry_get_container(const BPLib_STOR_CACHE_RBT_Link_t *link, size_t offset)
{
    return (BPLib_STOR_CACHE_Module_entry_t *)(void *)((uint8_t *)link - offset);
}

void BPLib_STOR_CACHE_Module_custody_finalize_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_entry_t *store_entry);
void BPLib_STOR_CACHE_Module_custody_store_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Block_t *qblk);
bool BPLib_STOR_CACHE_Module_custody_check_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Block_t *qblk);

void BPLib_STOR_CACHE_Module_fsm_execute(BPLib_STOR_CACHE_Block_t *sblk);

int BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg);

void BPLib_STOR_CACHE_Module_entry_make_pending(BPLib_STOR_CACHE_Module_entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags);

int  BPLib_STOR_CACHE_Module_egress_impl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);
void BPLib_STOR_CACHE_Module_flush_pending(BPLib_STOR_CACHE_Module_state_t *state);
int  BPLib_STOR_CACHE_Module_do_poll(BPLib_STOR_CACHE_Module_state_t *state);
int  BPLib_STOR_CACHE_Module_do_route_up(BPLib_STOR_CACHE_Module_state_t *state, bp_ipn_t dest, bp_ipn_t mask);
int  BPLib_STOR_CACHE_Module_do_intf_statechange(BPLib_STOR_CACHE_Module_state_t *state, bool is_up);
int  BPLib_STOR_CACHE_Module_event_impl(void *event_arg, BPLib_STOR_CACHE_Block_t *intf_block);
int  BPLib_STOR_CACHE_Module_process_pending(void *arg, BPLib_STOR_CACHE_Block_t *job);
int  BPLib_STOR_CACHE_Module_destruct_state(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_Module_construct_entry(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_Module_destruct_entry(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_Module_construct_blockref(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_Module_destruct_blockref(void *arg, BPLib_STOR_CACHE_Block_t *rblk);
int  BPLib_STOR_CACHE_Module_construct_state(void *arg, BPLib_STOR_CACHE_Block_t *sblk);

void BPLib_STOR_CACHE_Module_custody_insert_tracking_block(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                               BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);
int  BPLib_STOR_CACHE_Module_custody_find_dacs_match(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg);
bool BPLib_STOR_CACHE_Module_custody_find_pending_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *dacs_info);
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_Module_custody_create_dacs(BPLib_STOR_CACHE_Module_state_t                *state,
                                                  BPLib_STOR_CACHE_BblockPrimary_t      **pri_block_out,
                                                  bp_custody_accept_payload_block_t **pay_out);
void              BPLib_STOR_CACHE_Module_custody_open_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);
void BPLib_STOR_CACHE_Module_custody_append_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);
void BPLib_STOR_CACHE_Module_custody_update_tracking_block(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);
int  BPLib_STOR_CACHE_Module_custody_find_bundle_match(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg);
void BPLib_STOR_CACHE_Module_custody_process_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                        BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);
void BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                                    const bp_custody_accept_payload_block_t *ack_payload);
void BPLib_STOR_CACHE_Module_custody_init_info_from_pblock(BPLib_STOR_CACHE_Module_custodian_info_t *custody_info,
                                               BPLib_STOR_CACHE_BblockPrimary_t *pri_block);
void BPLib_STOR_CACHE_Module_custody_ack_tracking_block(BPLib_STOR_CACHE_Module_state_t                *state,
                                            const BPLib_STOR_CACHE_Module_custodian_info_t *custody_info);

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_idle_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry);
BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_queue_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void                      BPLib_STOR_CACHE_Module_fsm_state_queue_enter(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void                      BPLib_STOR_CACHE_Module_fsm_state_queue_exit(BPLib_STOR_CACHE_Module_entry_t *store_entry);
BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_delete_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void                      BPLib_STOR_CACHE_Module_fsm_state_delete_enter(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void                      BPLib_STOR_CACHE_Module_fsm_reschedule(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_entry_t *store_entry);
BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void                      BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_exit(BPLib_STOR_CACHE_Module_entry_t *store_entry);
void BPLib_STOR_CACHE_Module_fsm_transition_state(BPLib_STOR_CACHE_Module_entry_t *entry, BPLib_STOR_CACHE_Module_entry_state_t next_state);
BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_get_next_state(BPLib_STOR_CACHE_Module_entry_t *entry);
#endif // STOR

int BPLib_STOR_CACHE_placeholder(void);

#endif /* BPLIB_CACHE_INTERNAL_H */

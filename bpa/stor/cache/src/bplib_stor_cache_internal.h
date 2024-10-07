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

#ifndef BPLIB_STOR_CACHE_INTERNAL_H
#define BPLIB_STOR_CACHE_INTERNAL_H

#include <stdarg.h>

#include "bplib_time.h"

#include "bplib_stor_cache_block.h"

// TODO OSAL #define bplog(flags, evt, ...) BPLIB_MEM_OS_Log(__FILE__, __LINE__, flags, evt, __VA_ARGS__)
int bplog(uint32_t *flags, uint32_t event, ...);

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

// #define BP_CACHE_TIME_INFINITE BP_MONOTIME_INFINITE
#define BP_CACHE_TIME_INFINITE BPLIB_DTNTIME_INFINITE

typedef enum
{
    BPLib_STOR_CACHE_EntryStateUndefined,
    BPLib_STOR_CACHE_EntryStateIdle,
    BPLib_STOR_CACHE_EntryStateQueue,
    BPLib_STOR_CACHE_EntryStateDelete,
    BPLib_STOR_CACHE_EntryStateGenerateDacs,
    BPLib_STOR_CACHE_EntryStateMax
} BPLib_STOR_CACHE_EntryState_t;

#include "bplib_stor_cache_job.h"

typedef struct BPLib_STOR_CACHE_State
{
    int placeholder;

    bp_ipn_addr_t self_addr;

    BPLib_STOR_CACHE_Job_t pending_job;

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    BPLib_STOR_CACHE_Block_t pending_list;

    BPLib_TIME_MonotonicTime_t action_time; /**< Monotonic time when the pending_list was last checked */

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    BPLib_STOR_CACHE_Block_t idle_list;

    BPLib_MEM_RBT_Root_t bundle_index;
    BPLib_MEM_RBT_Root_t dacs_index;
    #ifdef jphfix
    BPLib_MEM_RBT_Root_t dest_eid_jphfix_index;
    BPLib_MEM_RBT_Root_t time_jphfix_index;
    #else // jphfix
    BPLib_MEM_RBT_Root_t dest_eid_index;
    BPLib_MEM_RBT_Root_t time_index;
    #endif // jphfix

    const BPLib_STOR_CACHE_OffloadApi_t  *offload_api;
    BPLib_STOR_CACHE_Block_t             *offload_blk;


    uint32_t generated_dacs_seq;

    uint32_t fsm_state_enter_count[BPLib_STOR_CACHE_EntryStateMax];
    uint32_t fsm_state_exit_count[BPLib_STOR_CACHE_EntryStateMax];
    uint32_t discard_count;

} BPLib_STOR_CACHE_State_t;

// #include "bplib_cache.h"  // For bp_ipn_addr_t?
typedef struct BPLib_STOR_CACHE_DacsPending
{
    bp_ipn_addr_t                      prev_custodian_id;
    BPLIB_CT_AcceptPayloadBlock_t *payload_ref;

} BPLib_STOR_CACHE_DacsPending_t;

typedef union BPLib_STOR_CACHE_EntryData
{
    BPLib_STOR_CACHE_DacsPending_t dacs;
} BPLib_STOR_CACHE_EntryData_t;

struct BPLib_STOR_CACHE_Entry
{
    BPLib_STOR_CACHE_State_t      *parent;
    BPLib_STOR_CACHE_EntryState_t state;
    uint32_t                      flags;
    bp_ipn_addr_t                 duct_id_copy;
    bp_sequencenumber_t           duct_seq_copy;
    BPLib_STOR_CACHE_Ref_t        refptr;
    bp_sid_t                      offload_sid;
    BPLib_TIME_MonotonicTime_t    action_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    BPLib_TIME_MonotonicTime_t    expire_time; /**< DTN time when entity is due to have some action (e.g. transmit) */
    BPLib_MEM_RBT_Link_t          hash_rbt_link;
    BPLib_MEM_RBT_Link_t          time_rbt_link;
    BPLib_MEM_RBT_Link_t          dest_eid_rbt_link;
    BPLib_STOR_CACHE_EntryData_t  data;
};

typedef struct BPLib_STOR_CACHE_Blockref
{
    BPLib_STOR_CACHE_Entry_t *storage_entry;
} BPLib_STOR_CACHE_Blockref_t;

typedef struct BPLib_STOR_CACHE_CustodianInfo
{
    bool                 match_dacs;
    bp_ipn_addr_t        duct_id;
    bp_ipn_addr_t        custodian_id;
    BPLib_STOR_CACHE_Block_t *this_cblk;
    BPLib_STOR_CACHE_Block_t *prev_cblk;
    bp_val_t             eid_hash;
    bp_sequencenumber_t  sequence_num;
    bp_ipn_t             final_dest_node;
    BPLib_STOR_CACHE_Entry_t *store_entry;
} BPLib_STOR_CACHE_CustodianInfo_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_StateSeldblock(BPLib_STOR_CACHE_State_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return BPLib_STOR_CACHE_GetBlockFromLink(&state->pending_list);
}

/* Allows reconstitution of the parent pool from a cache state pointer */
static inline BPLib_STOR_CACHE_Pool_t *BPLib_STOR_CACHE_ParentPool(BPLib_STOR_CACHE_State_t *state)
{
    return BPLib_STOR_CACHE_GetParentPoolFromLink(BPLib_STOR_CACHE_StateSeldblock(state));
}

/**
 * @brief Cast a block to a duct type
 *
 * @param cb
 * @return BPLib_STOR_CACHE_Duct_t*
 */
BPLib_STOR_CACHE_Duct_t *BPLib_STOR_CACHE_DuctCast(BPLib_STOR_CACHE_Block_t *cb);

/* Allows reconstitution of the duct object from a cache state pointer */
static inline BPLib_STOR_CACHE_Duct_t *BPLib_STOR_CACHE_GetDuct(BPLib_STOR_CACHE_State_t *state)
{
    return BPLib_STOR_CACHE_DuctCast(BPLib_STOR_CACHE_StateSeldblock(state));
}

/* Allows reconstitution of the queue struct from an RBT link pointer */
#define BPLib_STOR_CACHE_EntryFromLink(ptr, member) \
    BPLib_STOR_CACHE_EntryGetContainer(ptr, offsetof(BPLib_STOR_CACHE_Entry_t, member))
static inline BPLib_STOR_CACHE_Entry_t *BPLib_STOR_CACHE_EntryGetContainer(const BPLib_MEM_RBT_Link_t *link, size_t offset)
{
    return (BPLib_STOR_CACHE_Entry_t *)(void *)((uint8_t *)link - offset);
}

/**
 * @brief Gets the administrative block for the given pool
 *
 * This is always the first block in the pool.
 *
 * @param pool
 * @return BPLib_STOR_CACHE_lockAdminContent_t*
 */
static inline BPLib_STOR_CACHE_BlockAdminContent_t *BPLib_STOR_CACHE_GetAdmin(BPLib_STOR_CACHE_Pool_t *pool)
{
    /* this just confirms that the passed-in pointer looks OK */
    return &pool->admin_block.u.admin;
}

void BPLib_STOR_CACHE_CustodyFinalizeDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Entry_t *store_entry);
void BPLib_STOR_CACHE_CustodyStoreBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Block_t *qblk);
bool BPLib_STOR_CACHE_CustodyCheckDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Block_t *qblk);

void BPLib_STOR_CACHE_FsmExecute(BPLib_STOR_CACHE_Block_t *sblk);

int BPLib_STOR_CACHE_EntryTreeInsertUnsorted(const BPLib_MEM_RBT_Link_t *node, void *arg);

void BPLib_STOR_CACHE_EntryMakePending(BPLib_STOR_CACHE_Entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags);

int  BPLib_STOR_CACHE_EgressImpl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);
void BPLib_STOR_CACHE_FlushPending(BPLib_STOR_CACHE_State_t *state);
int  BPLib_STOR_CACHE_DoPoll(BPLib_STOR_CACHE_State_t *state);
int  BPLib_STOR_CACHE_DoRouteUp(BPLib_STOR_CACHE_State_t *state, bp_ipn_t dest, bp_ipn_t mask);
int  BPLib_STOR_CACHE_DoIntfStatechange(BPLib_STOR_CACHE_State_t *state, bool is_up);
int  BPLib_STOR_CACHE_EventImpl(void *event_arg, BPLib_STOR_CACHE_Block_t *intf_block);
int  BPLib_STOR_CACHE_ProcessPending(void *arg, BPLib_STOR_CACHE_Block_t *job);
int  BPLib_STOR_CACHE_DestructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_ConstructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_DestructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_ConstructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *sblk);
int  BPLib_STOR_CACHE_DestructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *rblk);
int  BPLib_STOR_CACHE_ConstructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk);

void BPLib_STOR_CACHE_CustodyInsertTrackingBlock(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                               BPLib_STOR_CACHE_CustodianInfo_t *custody_info);
int  BPLib_STOR_CACHE_CustodyFindDacsMatch(const BPLib_MEM_RBT_Link_t *node, void *arg);
bool BPLib_STOR_CACHE_CustodyFindPendingDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *dacs_info);
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_CustodyCreateDacs(BPLib_STOR_CACHE_State_t                *state,
                                                  BPLib_STOR_CACHE_BblockPrimary_t      **pri_block_out,
                                                  BPLIB_CT_AcceptPayloadBlock_t **pay_out);
void              BPLib_STOR_CACHE_CustodyOpenDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *custody_info);
void BPLib_STOR_CACHE_CustodyAppendDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *custody_info);
void BPLib_STOR_CACHE_CustodyUpdateTrackingBlock(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *custody_info);
int  BPLib_STOR_CACHE_CustodyFindBundleMatch(const BPLib_MEM_RBT_Link_t *node, void *arg);
void BPLib_STOR_CACHE_CustodyProcessBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                        BPLib_STOR_CACHE_CustodianInfo_t *custody_info);
void BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
                                                    const BPLIB_CT_AcceptPayloadBlock_t *ack_payload);
void BPLib_STOR_CACHE_CustodyInitInfoFromPblock(BPLib_STOR_CACHE_CustodianInfo_t *custody_info,
                                               BPLib_STOR_CACHE_BblockPrimary_t *pri_block);
void BPLib_STOR_CACHE_CustodyAckTrackingBlock(BPLib_STOR_CACHE_State_t                *state,
                                            const BPLib_STOR_CACHE_CustodianInfo_t *custody_info);

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateIdleEval(BPLib_STOR_CACHE_Entry_t *store_entry);
BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateQueueEval(BPLib_STOR_CACHE_Entry_t *store_entry);
void                      BPLib_STOR_CACHE_FsmStateQueueEnter(BPLib_STOR_CACHE_Entry_t *store_entry);
void                      BPLib_STOR_CACHE_FsmStateQueueExit(BPLib_STOR_CACHE_Entry_t *store_entry);
BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateDeleteEval(BPLib_STOR_CACHE_Entry_t *store_entry);
void                      BPLib_STOR_CACHE_FsmStateDeleteEnter(BPLib_STOR_CACHE_Entry_t *store_entry);
void                      BPLib_STOR_CACHE_FsmReschedule(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Entry_t *store_entry);
BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateGenerateDacsEval(BPLib_STOR_CACHE_Entry_t *store_entry);
void                      BPLib_STOR_CACHE_FsmStateGenerateDacsExit(BPLib_STOR_CACHE_Entry_t *store_entry);
void BPLib_STOR_CACHE_FsmTransitionState(BPLib_STOR_CACHE_Entry_t *entry, BPLib_STOR_CACHE_EntryState_t next_state);
BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmGetNextState(BPLib_STOR_CACHE_Entry_t *entry);

void BPLib_STOR_CACHE_InitBaseObject(BPLib_STOR_CACHE_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);



/**
 * @brief Append a single bundle to the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @param cpb
 */
void BPLib_STOR_CACHE_SubqPushSingle(BPLib_STOR_CACHE_SubqBase_t *subq, BPLib_STOR_CACHE_Block_t *cpb);

/**
 * @brief Get the next bundle from the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @return BPLib_STOR_CACHE_Block_t*
 */
BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_SubqPullSingle(BPLib_STOR_CACHE_SubqBase_t *subq);

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
uint32_t BPLib_STOR_CACHE_ListCountBlocks(BPLib_STOR_CACHE_Block_t *list);

/**
 * @brief Pushes an entire list of blocks into a subq
 *
 * Similar to BPLib_STOR_CACHE_SubqMoveAll() but the source is just a simple list rather
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
uint32_t BPLib_STOR_CACHE_SubqMergeList(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_Block_t *list);

/**
 * @brief Merges the entire contents of the source subq into the destination
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst
 * @param subq_src
 * @return The number of queue entries moved
 */
uint32_t BPLib_STOR_CACHE_SubqMoveAll(BPLib_STOR_CACHE_SubqBase_t *subq_dst, BPLib_STOR_CACHE_SubqBase_t *subq_src);

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
uint32_t BPLib_STOR_CACHE_SubqDropAll(BPLib_STOR_CACHE_Pool_t *pool, BPLib_STOR_CACHE_SubqBase_t *subq);

/* gets to the underlying block content (which may be a ref block) */
BPLib_STOR_CACHE_BlockContent_t       *BPLib_STOR_CACHE_GetBlockContent(BPLib_STOR_CACHE_Block_t *cb);
const BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_GetBlockContentConst(const BPLib_STOR_CACHE_Block_t *cb);

void BPLib_STOR_CACHE_JobCancelInternal(BPLib_STOR_CACHE_Job_t *job);
void BPLib_STOR_CACHE_JobMarkActiveInternal(BPLib_STOR_CACHE_Block_t *active_list, BPLib_STOR_CACHE_Job_t *job);
void BPLib_STOR_CACHE_JobMarkActive(BPLib_STOR_CACHE_Job_t *job);

BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_BlockDereferenceContent(BPLib_STOR_CACHE_Block_t *cb);
BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_AllocBlockInternal(BPLib_STOR_CACHE_Pool_t *pool,
    BPLib_STOR_CACHE_Blocktype_t blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority);

#endif /* BPLIB_STOR_CACHE_INTERNAL_H */

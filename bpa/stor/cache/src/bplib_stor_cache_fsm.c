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

#include <assert.h>

#include <stdio.h>

#include "../../qm/inc/bplib_stor_qm_ducts.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_ref.h"
#include "bplib_stor_cache_module_api.h"

typedef BPLib_STOR_CACHE_EntryState_t (*BPLib_STOR_CACHE_FsmStateEvalFunc_t)(BPLib_STOR_CACHE_Entry_t *);
typedef void (*BPLib_STOR_CACHE_FsmStateChangeFunc_t)(BPLib_STOR_CACHE_Entry_t *);

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateIdleEval(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    BPLib_STOR_CACHE_Block_t *pblk;

    if (BPLib_STOR_CACHE_TimeCompare(store_entry->parent->action_time, store_entry->expire_time) >= 0)
    {
        /* bundle has reached the end of its useful life, so it can be discarded */
        return BPLib_STOR_CACHE_EntryStateUndefined;
    }

    // TODO Fix incomplete type.
    #define VALID_STORE_ENTRY
    #ifdef VALID_STORE_ENTRY

    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCAL_CUSTODY) == 0)
    {
        /* no longer have local custody of the bundle, so the content can be deleted,
         * but the metadata can be retained for a holdover period in case the previous custodian
         * sends it again */
        return BPLib_STOR_CACHE_EntryStateDelete;
    }

    if ((store_entry->flags & BPLIB_STORE_FLAGS_ACTION_WAIT_STATE) == 0)
    {
        /* bundle is due for [re]transmit */
        if (store_entry->refptr == NULL && store_entry->offload_sid != 0)
        {
            store_entry->parent->offload_api->restore(store_entry->parent->offload_blk, store_entry->offload_sid,
                                                      &pblk);
            store_entry->refptr = BPLib_STOR_CACHE_RefCreate(pblk);
        }

        if (store_entry->refptr != NULL)
        {
            return BPLib_STOR_CACHE_EntryStateQueue;
        }
    }

    #endif // VALID_STORE_ENTRY

    /* no change */
    return BPLib_STOR_CACHE_EntryStateIdle;
}

static void BPLib_STOR_CACHE_FsmStateIdleEnter(BPLib_STOR_CACHE_Entry_t *store_entry) {}

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateQueueEval(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCALLY_QUEUED) == 0)
    {
        /* go back to idle state (will be reevaluated from there) */
        return BPLib_STOR_CACHE_EntryStateIdle;
    }

    return BPLib_STOR_CACHE_EntryStateQueue; /* no change */
}

void BPLib_STOR_CACHE_FsmStateQueueEnter(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    BPLib_STOR_CACHE_Block_t *rblk;
    BPLib_STOR_QM_Duct_t  *self_duct;

    store_entry->flags |= BPLIB_STORE_FLAG_PENDING_FORWARD;

    rblk = BPLib_STOR_CACHE_RefMakeBlock(store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF, store_entry);
    if (rblk != NULL)
    {
        self_duct = BPLib_STOR_CACHE_GetDuct(store_entry->parent);

        /*
         * note - the flag is always set here, even if it does not actually make it into the queue.
         *
         * If it fails to push to the queue, it is immediately recycled, and when the destructor runs
         * the flag will be cleared as normal.  This keeps things synchronized in that it won't transition
         * back to idle until the referring block is actually removed, even though it was never really
         * queued.
         */
        store_entry->flags |= BPLIB_STORE_FLAG_LOCALLY_QUEUED;
        if (!BPLib_STOR_QM_DuctTryPush(&self_duct->ingress, rblk, BPLIB_MONOTIME_ZERO))
        {
            BPLib_STOR_CACHE_RecycleBlock(rblk);
        }
    }
}

void BPLib_STOR_CACHE_FsmStateQueueExit(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;

    pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Dereference(store_entry->refptr));

    if (bp_handle_is_valid(pri_block->data.delivery.egress_intf_id))
    {
        store_entry->flags &= ~BPLIB_STORE_FLAG_PENDING_FORWARD;

        if (pri_block->data.delivery.delivery_policy !=BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking)
        {
            /* If not doing full custody tracking, then the egress CLA is considered the implicit custodian */
            store_entry->flags &= ~BPLIB_STORE_FLAG_LOCAL_CUSTODY;
        }
        else
        {
            /* reschedule the next retransmit time based on the anticipted round-trip time of the egress intf */
            /* JPHFIX: this is fixed for now, but should be configurable/specific to the egress intf */
            store_entry->action_time =
                BPLib_STOR_CACHE_TimeAddMs(pri_block->data.delivery.egress_time, pri_block->data.delivery.local_retx_interval);
            store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
        }
    }

    if (store_entry->offload_sid != 0)
    {
        BPLib_STOR_CACHE_RefRelease(store_entry->refptr);
        store_entry->refptr = NULL;
    }
}

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateDeleteEval(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        /* check if the activity flag is set, meaning that something looked at this entry
         * since the last time this was checked. */
        if ((store_entry->flags & BPLIB_STORE_FLAG_ACTIVITY) == 0)
        {
            /* nothing used this, so should be safe to finally discard it */
            return BPLib_STOR_CACHE_EntryStateUndefined;
        }

        /* clear it now, it will be re-set if something uses this entry */
        store_entry->flags &= ~BPLIB_STORE_FLAG_ACTIVITY;
        store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
        store_entry->action_time = BPLib_STOR_CACHE_TimeAddMs(store_entry->parent->action_time, BP_CACHE_AGE_OUT_TIME);
    }

    return BPLib_STOR_CACHE_EntryStateDelete;
}

void BPLib_STOR_CACHE_FsmStateDeleteEnter(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    // TODO Fix incomplete type.
    #ifdef VALID_STORE_ENTRY

    if (store_entry->refptr != NULL)
    {
        BPLib_STOR_CACHE_RefRelease(store_entry->refptr);
        store_entry->refptr = NULL;
    }

    if (store_entry->offload_sid != 0)
    {
        store_entry->parent->offload_api->release(store_entry->parent->offload_blk, store_entry->offload_sid);
    }

    store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
    store_entry->action_time =BPLib_STOR_CACHE_TimeAddMs(store_entry->parent->action_time, BP_CACHE_AGE_OUT_TIME);
    #endif // VALID_STORE_ENTRY
}

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateGenerateDacsEval(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    /* check if dacs is due for transmit */
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        return BPLib_STOR_CACHE_EntryStateIdle;
    }

    return BPLib_STOR_CACHE_EntryStateGenerateDacs;
}

void BPLib_STOR_CACHE_FsmStateGenerateDacsExit(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    #ifdef CUSTODY
    BPLib_STOR_CACHE_CustodyFinalizeDacs(store_entry->parent, store_entry);
    #endif // CUSTODY
}

void BPLib_STOR_CACHE_FsmReschedule(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Entry_t *store_entry)
{
    BPLib_TIME_MonotonicTime_t ref_time;
    uint64_t prev_key;

    ref_time = state->action_time;

    if ((store_entry->flags & BPLIB_STORE_FLAGS_ACTION_WAIT_STATE) == 0)
    {
        /* item is pending action but blocked for some (hopefully) temporary external reason, so retry more aggressively
         */
        ref_time =BPLib_STOR_CACHE_TimeAddMs(ref_time, BP_CACHE_FAST_RETRY_TIME);
    }
    else
    {
        /* item is blocked for now, but don't want to leave it in a state where it is never checked again at all */
        ref_time =BPLib_STOR_CACHE_TimeAddMs(ref_time, BP_CACHE_IDLE_RETRY_TIME);
    }

    /* calculate the time for the next action event */
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) != 0 &&
       BPLib_STOR_CACHE_TimeCompare(ref_time, store_entry->action_time) > 0)
    {
        ref_time = store_entry->action_time;
    }

    /*
     * determine the batch/bucket that this should go into
     */
    prev_key = BPLib_MEM_RBT_GetKeyValue(&store_entry->time_rbt_link);
    if (BPLIB_TIME_TO_INT(ref_time) != prev_key)
    {
        /*
         * If this was in the time index, the old entry needs to be removed first,
         * then it needs to be added in the new spot
         */
        if (prev_key != 0)
        {
            BPLib_MEM_RBT_ExtractNode(&state->time_index, &store_entry->time_rbt_link);
        }
        BPLib_MEM_RBT_InsertValueGeneric(BPLIB_TIME_TO_INT(ref_time), &state->time_index, &store_entry->time_rbt_link,
                                       BPLib_STOR_CACHE_EntryTreeInsertUnsorted, NULL);
    }
}

static BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateNoopEval(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    /* entries which reach this state will be discarded immediately */
    return BPLib_STOR_CACHE_EntryStateUndefined;
}

BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmGetNextState(BPLib_STOR_CACHE_Entry_t *entry)
{
    static const BPLib_STOR_CACHE_FsmStateEvalFunc_t STATE_EVAL_TABLE[BPLib_STOR_CACHE_EntryStateMax] = {
        [BPLib_STOR_CACHE_EntryStateUndefined]     = BPLib_STOR_CACHE_FsmStateNoopEval,
        [BPLib_STOR_CACHE_EntryStateIdle]          = BPLib_STOR_CACHE_FsmStateIdleEval,
        [BPLib_STOR_CACHE_EntryStateQueue]         = BPLib_STOR_CACHE_FsmStateQueueEval,
        [BPLib_STOR_CACHE_EntryStateDelete]        = BPLib_STOR_CACHE_FsmStateDeleteEval,
        [BPLib_STOR_CACHE_EntryStateGenerateDacs] = BPLib_STOR_CACHE_FsmStateGenerateDacsEval};

    BPLib_STOR_CACHE_FsmStateEvalFunc_t state_eval_func;
    BPLib_STOR_CACHE_EntryState_t         state;

    state = entry->state;
    if (state < BPLib_STOR_CACHE_EntryStateMax)
    {
        state_eval_func = STATE_EVAL_TABLE[state];
    }
    else
    {
        state_eval_func = NULL;
    }

    /* this should have always produced a non-NULL evaluation function, anything else is a bug that
     * will cause it to permanently remain in this undefined state */
    assert(state_eval_func != NULL);
    if (state_eval_func != NULL)
    {
        state = state_eval_func(entry);
    }

    return state;
}

void BPLib_STOR_CACHE_FsmTransitionState(BPLib_STOR_CACHE_Entry_t *entry, BPLib_STOR_CACHE_EntryState_t next_state)
{
    static const BPLib_STOR_CACHE_FsmStateChangeFunc_t STATE_ENTER_TABLE[BPLib_STOR_CACHE_EntryStateMax] = {
        [BPLib_STOR_CACHE_EntryStateIdle]   = BPLib_STOR_CACHE_FsmStateIdleEnter,
        [BPLib_STOR_CACHE_EntryStateQueue]  = BPLib_STOR_CACHE_FsmStateQueueEnter,
        [BPLib_STOR_CACHE_EntryStateDelete] = BPLib_STOR_CACHE_FsmStateDeleteEnter};

    static const BPLib_STOR_CACHE_FsmStateChangeFunc_t STATE_EXIT_TABLE[BPLib_STOR_CACHE_EntryStateMax] = {
        [BPLib_STOR_CACHE_EntryStateQueue]         = BPLib_STOR_CACHE_FsmStateQueueExit,
        [BPLib_STOR_CACHE_EntryStateGenerateDacs] = BPLib_STOR_CACHE_FsmStateGenerateDacsExit};

    BPLib_STOR_CACHE_FsmStateChangeFunc_t state_enter_func;
    BPLib_STOR_CACHE_FsmStateChangeFunc_t state_exit_func;

    /* entry->state is the state we are leaving */
    if (entry->state < BPLib_STOR_CACHE_EntryStateMax)
    {
        state_exit_func = STATE_EXIT_TABLE[entry->state];
    }
    else
    {
        state_exit_func = NULL;
    }

    /* next_state is the state we are entering */
    if (next_state < BPLib_STOR_CACHE_EntryStateMax)
    {
        state_enter_func = STATE_ENTER_TABLE[next_state];
    }
    else
    {
        state_enter_func = NULL;
    }

    /* both the entry and exit functions are allowed to be NULL (no-op) */
    if (state_exit_func != NULL)
    {
        state_exit_func(entry);
    }
    if (state_enter_func != NULL)
    {
        state_enter_func(entry);
    }

    /* save the next state as the current */
    entry->state = next_state;
}

static void BPLib_STOR_CACHE_FsmDebugReportDiscard(BPLib_STOR_CACHE_Entry_t *store_entry)
{
    fprintf(stderr, "discarding bundle ID %lu.%lu @%lu\n", (unsigned long)store_entry->duct_id_copy.node_number,
            (unsigned long)store_entry->duct_id_copy.service_number, (unsigned long)store_entry->duct_seq_copy);
}

void BPLib_STOR_CACHE_FsmExecute(BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_State_t      *state;
    BPLib_STOR_CACHE_Entry_t      *store_entry;
    BPLib_STOR_CACHE_EntryState_t next_state;

    /* This cast should always work, unless there is a bug */
    store_entry = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        state = store_entry->parent;

        /*
         * Check timer conditions that may indicate action is needed on this bundle
         */
        if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) != 0 &&
           BPLib_STOR_CACHE_TimeCompare(state->action_time, store_entry->action_time) >= 0)
        {
            /*
             * Note, there is also a possibility that this bundle is gets sent to a CLA egress queue awaiting
             * the external software entity to call cla_egress() to get it, but it never does.  This should not
             * generate duplicate/multiple retransmit copies if that is the case.
             *
             * To avoid this, the "transmit_time" is set to infinite for now, and shouldn't be reset to something
             * real until it is confirmed that there is no bundle copy in pending output anymore (that is,
             * BPLib_STOR_CACHE_HandleRefRecycle() was called and it was cleaned up).
             *
             * This is mainly to avoid any potential snowball effects from a misbehaving CLA.  If the CLA was
             * not able to fetch data, it should declare itself DOWN and then all is OK (because existing entries
             * in the egress queue get purged when an intf goes down, and we don't deliver anything new to it).
             */
            store_entry->flags &= ~BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
            store_entry->action_time = (BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(BP_CACHE_TIME_INFINITE);
        }

        next_state = BPLib_STOR_CACHE_FsmGetNextState(store_entry);
        if (next_state != store_entry->state)
        {
            ++state->fsm_state_exit_count[store_entry->state];
            BPLib_STOR_CACHE_FsmTransitionState(store_entry, next_state);
            ++state->fsm_state_enter_count[next_state];
        }

        /* entries get set into the "undefined" state once the FSM determines it is no longer useful at all */
        if (next_state == BPLib_STOR_CACHE_EntryStateUndefined)
        {
            ++state->discard_count;
            BPLib_STOR_CACHE_FsmDebugReportDiscard(store_entry);
            BPLib_STOR_CACHE_RecycleBlock(sblk);
        }
        else
        {
            /*
             * The main objective here is to always periodically revisit items in the
             * idle list at some point, don't let them linger indefinitely without
             * at least looking at them.
             */
            BPLib_STOR_CACHE_FsmReschedule(state, store_entry);
            BPLib_STOR_CACHE_InsertBefore(&state->idle_list, sblk);
        }
    }
}

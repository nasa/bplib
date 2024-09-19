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

#include "BPLib_STOR_CACHE_Module_cache_internal.h"

typedef BPLib_STOR_CACHE_Module_entry_state_t (*BPLib_STOR_CACHE_Module_fsm_state_eval_func_t)(BPLib_STOR_CACHE_Module_entry_t *);
typedef void (*BPLib_STOR_CACHE_Module_fsm_state_change_func_t)(BPLib_STOR_CACHE_Module_entry_t *);

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_idle_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    BPLib_STOR_CACHE_Block_t *pblk;

    if (store_entry->parent->action_time >= store_entry->expire_time)
    {
        /* bundle has reached the end of its useful life, so it can be discarded */
        return BPLib_STOR_CACHE_Module_entry_state_undefined;
    }

    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCAL_CUSTODY) == 0)
    {
        /* no longer have local custody of the bundle, so the content can be deleted,
         * but the metadata can be retained for a holdover period in case the previous custodian
         * sends it again */
        return BPLib_STOR_CACHE_Module_entry_state_delete;
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
            return BPLib_STOR_CACHE_Module_entry_state_queue;
        }
    }

    /* no change */
    return BPLib_STOR_CACHE_Module_entry_state_idle;
}

static void BPLib_STOR_CACHE_Module_fsm_state_idle_enter(BPLib_STOR_CACHE_Module_entry_t *store_entry) {}

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_queue_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCALLY_QUEUED) == 0)
    {
        /* go back to idle state (will be reevaluated from there) */
        return BPLib_STOR_CACHE_Module_entry_state_idle;
    }

    return BPLib_STOR_CACHE_Module_entry_state_queue; /* no change */
}

void BPLib_STOR_CACHE_Module_fsm_state_queue_enter(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    #ifdef STOR
    BPLib_STOR_CACHE_Block_t *rblk;
    BPLib_STOR_CACHE_Flow_t  *self_flow;

    store_entry->flags |= BPLIB_STORE_FLAG_PENDING_FORWARD;

    rblk = BPLib_STOR_CACHE_RefMakeBlock(store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF, store_entry);
    if (rblk != NULL)
    {
        self_flow = BPLib_STOR_CACHE_Module_get_flow(store_entry->parent);

        /*
         * note - the flag is always set here, even if it does not actually make it into the queue.
         *
         * If it fails to push to the queue, it is immediately recycled, and when the destructor runs
         * the flag will be cleared as normal.  This keeps things synchronized in that it won't transition
         * back to idle until the referring block is actually removed, even though it was never really
         * queued.
         */
        store_entry->flags |= BPLIB_STORE_FLAG_LOCALLY_QUEUED;
        if (!BPLib_STOR_CACHE_FlowTryPush(&self_flow->ingress, rblk, 0))
        {
            BPLib_STOR_CACHE_RecycleBlock(rblk);
        }
    }

    #endif // STOR
}

void BPLib_STOR_CACHE_Module_fsm_state_queue_exit(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    BPLib_STOR_CACHE_BblockPrimary_t *pri_block;

    pri_block = BPLib_STOR_CACHE_BblockPrimaryCast(BPLib_STOR_CACHE_Dereference(store_entry->refptr));

    if (bp_handle_is_valid(pri_block->data.delivery.egress_intf_id))
    {
        store_entry->flags &= ~BPLIB_STORE_FLAG_PENDING_FORWARD;

        if (pri_block->data.delivery.delivery_policy != bplib_policy_delivery_custody_tracking)
        {
            /* If not doing full custody tracking, then the egress CLA is considered the implicit custodian */
            store_entry->flags &= ~BPLIB_STORE_FLAG_LOCAL_CUSTODY;
        }
        else
        {
            /* reschedule the next retransmit time based on the anticipted round-trip time of the egress intf */
            /* JPHFIX: this is fixed for now, but should be configurable/specific to the egress intf */
            store_entry->action_time =
                pri_block->data.delivery.egress_time + pri_block->data.delivery.local_retx_interval;
            store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
        }
    }

    if (store_entry->offload_sid != 0)
    {
        BPLib_STOR_CACHE_RefRelease(store_entry->refptr);
        store_entry->refptr = NULL;
    }
}

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_delete_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        /* check if the activity flag is set, meaning that something looked at this entry
         * since the last time this was checked. */
        if ((store_entry->flags & BPLIB_STORE_FLAG_ACTIVITY) == 0)
        {
            /* nothing used this, so should be safe to finally discard it */
            return BPLib_STOR_CACHE_Module_entry_state_undefined;
        }

        /* clear it now, it will be re-set if something uses this entry */
        store_entry->flags &= ~BPLIB_STORE_FLAG_ACTIVITY;
        store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
        store_entry->action_time = store_entry->parent->action_time + BP_CACHE_AGE_OUT_TIME;
    }

    return BPLib_STOR_CACHE_Module_entry_state_delete;
}

void BPLib_STOR_CACHE_Module_fsm_state_delete_enter(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
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
    store_entry->action_time = store_entry->parent->action_time + BP_CACHE_AGE_OUT_TIME;
}

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    /* check if dacs is due for transmit */
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        return BPLib_STOR_CACHE_Module_entry_state_idle;
    }

    return BPLib_STOR_CACHE_Module_entry_state_generate_dacs;
}

void BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_exit(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    BPLib_STOR_CACHE_Module_custody_finalize_dacs(store_entry->parent, store_entry);
}

void BPLib_STOR_CACHE_Module_fsm_reschedule(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    uint64_t ref_time;
    uint64_t prev_key;

    ref_time = state->action_time;

    if ((store_entry->flags & BPLIB_STORE_FLAGS_ACTION_WAIT_STATE) == 0)
    {
        /* item is pending action but blocked for some (hopefully) temporary external reason, so retry more aggressively
         */
        ref_time += BP_CACHE_FAST_RETRY_TIME;
    }
    else
    {
        /* item is blocked for now, but don't want to leave it in a state where it is never checked again at all */
        ref_time += BP_CACHE_IDLE_RETRY_TIME;
    }

    /* calculate the time for the next action event */
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) != 0 && ref_time > store_entry->action_time)
    {
        ref_time = store_entry->action_time;
    }

    /*
     * determine the batch/bucket that this should go into
     */
    prev_key = BPLib_STOR_CACHE_RBT_GetKeyValue(&store_entry->time_rbt_link);
    if (ref_time != prev_key)
    {
        /*
         * If this was in the time index, the old entry needs to be removed first,
         * then it needs to be added in the new spot
         */
        if (prev_key != 0)
        {
            BPLib_STOR_CACHE_RBT_ExtractNode(&state->time_jphfix_index, &store_entry->time_rbt_link);
        }
        BPLib_STOR_CACHE_RBT_InsertValueGeneric(ref_time, &state->time_jphfix_index, &store_entry->time_rbt_link,
                                       BPLib_STOR_CACHE_Module_entry_tree_insert_unsorted, NULL);
    }
}

static BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_state_noop_eval(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    /* entries which reach this state will be discarded immediately */
    return BPLib_STOR_CACHE_Module_entry_state_undefined;
}

BPLib_STOR_CACHE_Module_entry_state_t BPLib_STOR_CACHE_Module_fsm_get_next_state(BPLib_STOR_CACHE_Module_entry_t *entry)
{
    static const BPLib_STOR_CACHE_Module_fsm_state_eval_func_t STATE_EVAL_TABLE[BPLib_STOR_CACHE_Module_entry_state_max] = {
        [BPLib_STOR_CACHE_Module_entry_state_undefined]     = BPLib_STOR_CACHE_Module_fsm_state_noop_eval,
        [BPLib_STOR_CACHE_Module_entry_state_idle]          = BPLib_STOR_CACHE_Module_fsm_state_idle_eval,
        [BPLib_STOR_CACHE_Module_entry_state_queue]         = BPLib_STOR_CACHE_Module_fsm_state_queue_eval,
        [BPLib_STOR_CACHE_Module_entry_state_delete]        = BPLib_STOR_CACHE_Module_fsm_state_delete_eval,
        [BPLib_STOR_CACHE_Module_entry_state_generate_dacs] = BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_eval};

    BPLib_STOR_CACHE_Module_fsm_state_eval_func_t state_eval_func;
    BPLib_STOR_CACHE_Module_entry_state_t         state;

    state = entry->state;
    if (state < BPLib_STOR_CACHE_Module_entry_state_max)
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

void BPLib_STOR_CACHE_Module_fsm_transition_state(BPLib_STOR_CACHE_Module_entry_t *entry, BPLib_STOR_CACHE_Module_entry_state_t next_state)
{
    static const BPLib_STOR_CACHE_Module_fsm_state_change_func_t STATE_ENTER_TABLE[BPLib_STOR_CACHE_Module_entry_state_max] = {
        [BPLib_STOR_CACHE_Module_entry_state_idle]   = BPLib_STOR_CACHE_Module_fsm_state_idle_enter,
        [BPLib_STOR_CACHE_Module_entry_state_queue]  = BPLib_STOR_CACHE_Module_fsm_state_queue_enter,
        [BPLib_STOR_CACHE_Module_entry_state_delete] = BPLib_STOR_CACHE_Module_fsm_state_delete_enter};

    static const BPLib_STOR_CACHE_Module_fsm_state_change_func_t STATE_EXIT_TABLE[BPLib_STOR_CACHE_Module_entry_state_max] = {
        [BPLib_STOR_CACHE_Module_entry_state_queue]         = BPLib_STOR_CACHE_Module_fsm_state_queue_exit,
        [BPLib_STOR_CACHE_Module_entry_state_generate_dacs] = BPLib_STOR_CACHE_Module_fsm_state_generate_dacs_exit};

    BPLib_STOR_CACHE_Module_fsm_state_change_func_t state_enter_func;
    BPLib_STOR_CACHE_Module_fsm_state_change_func_t state_exit_func;

    /* entry->state is the state we are leaving */
    if (entry->state < BPLib_STOR_CACHE_Module_entry_state_max)
    {
        state_exit_func = STATE_EXIT_TABLE[entry->state];
    }
    else
    {
        state_exit_func = NULL;
    }

    /* next_state is the state we are entering */
    if (next_state < BPLib_STOR_CACHE_Module_entry_state_max)
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

static void BPLib_STOR_CACHE_Module_fsm_debug_report_discard(BPLib_STOR_CACHE_Module_entry_t *store_entry)
{
    fprintf(stderr, "discarding bundle ID %lu.%lu @%lu\n", (unsigned long)store_entry->flow_id_copy.node_number,
            (unsigned long)store_entry->flow_id_copy.service_number, (unsigned long)store_entry->flow_seq_copy);
}

void BPLib_STOR_CACHE_Module_fsm_execute(BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_Module_state_t      *state;
    BPLib_STOR_CACHE_Module_entry_t      *store_entry;
    BPLib_STOR_CACHE_Module_entry_state_t next_state;

    /* This cast should always work, unless there is a bug */
    store_entry = BPLib_STOR_CACHE_GenericDataCast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry != NULL)
    {
        state = store_entry->parent;

        /*
         * Check timer conditions that may indicate action is needed on this bundle
         */
        if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) != 0 &&
            state->action_time >= store_entry->action_time)
        {
            /*
             * Note, there is also a possibility that this bundle is gets sent to a CLA egress queue awaiting
             * the external software entity to call cla_egress() to get it, but it never does.  This should not
             * generate duplicate/multiple retransmit copies if that is the case.
             *
             * To avoid this, the "transmit_time" is set to infinite for now, and shouldn't be reset to something
             * real until it is confirmed that there is no bundle copy in pending output anymore (that is,
             * BPLib_STOR_CACHE_Module_handle_ref_recycle() was called and it was cleaned up).
             *
             * This is mainly to avoid any potential snowball effects from a misbehaving CLA.  If the CLA was
             * not able to fetch data, it should declare itself DOWN and then all is OK (because existing entries
             * in the egress queue get purged when an intf goes down, and we don't deliver anything new to it).
             */
            store_entry->flags &= ~BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
            store_entry->action_time = BP_CACHE_TIME_INFINITE;
        }

        next_state = BPLib_STOR_CACHE_Module_fsm_get_next_state(store_entry);
        if (next_state != store_entry->state)
        {
            ++state->fsm_state_exit_count[store_entry->state];
            BPLib_STOR_CACHE_Module_fsm_transition_state(store_entry, next_state);
            ++state->fsm_state_enter_count[next_state];
        }

        /* entries get set into the "undefined" state once the FSM determines it is no longer useful at all */
        if (next_state == BPLib_STOR_CACHE_Module_entry_state_undefined)
        {
            ++state->discard_count;
            BPLib_STOR_CACHE_Module_fsm_debug_report_discard(store_entry);
            BPLib_STOR_CACHE_RecycleBlock(sblk);
        }
        else
        {
            /*
             * The main objective here is to always periodically revisit items in the
             * idle list at some point, don't let them linger indefinitely without
             * at least looking at them.
             */
            BPLib_STOR_CACHE_Module_fsm_reschedule(state, store_entry);
            BPLib_STOR_CACHE_InsertBefore(&state->idle_list, sblk);
        }
    }
}

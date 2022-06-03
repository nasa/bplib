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

#include "v7_cache_internal.h"

typedef bplib_cache_entry_state_t (*bplib_cache_fsm_state_eval_func_t)(bplib_cache_entry_t *);
typedef void (*bplib_cache_fsm_state_change_func_t)(bplib_cache_entry_t *);

static bplib_cache_entry_state_t bplib_cache_fsm_state_idle_eval(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_bblock_primary_t *pri_block;

    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

    if (store_entry->parent->action_time >=
        (pri_block->pri_logical_data.creationTimeStamp.time + pri_block->pri_logical_data.lifetime))
    {
        /* bundle has reached the end of its useful life, so it can be discarded */
        return bplib_cache_entry_state_undefined;
    }

    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCAL_CUSTODY) == 0)
    {
        /* no longer have local custody of the bundle, so the content can be deleted,
         * but the metadata can be retained for a holdover period in case the previous custodian
         * sends it again */
        return bplib_cache_entry_state_delete;
    }

    if ((store_entry->flags & BPLIB_STORE_FLAGS_ACTION_WAIT_STATE) == 0)
    {
        /* bundle is due for [re]transmit */
        return bplib_cache_entry_state_queue;
    }

    /* no change */
    return bplib_cache_entry_state_idle;
}

static void bplib_cache_fsm_state_idle_enter(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_bblock_primary_t *pri_block;

    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

    if (bp_handle_is_valid(pri_block->delivery_data.egress_intf_id))
    {
        /* reschedule the next retransmit time based on the anticipted round-trip time of the egress intf */
        /* JPHFIX: this is fixed for now, but should be configurable/specific to the egress intf */
        store_entry->action_time = pri_block->delivery_data.egress_time + pri_block->delivery_data.local_retx_interval;
        store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
    }
}

static bplib_cache_entry_state_t bplib_cache_fsm_state_queue_eval(bplib_cache_entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_LOCALLY_QUEUED) == 0)
    {
        /* go back to idle state (will be reevaluated from there) */
        return bplib_cache_entry_state_idle;
    }

    return bplib_cache_entry_state_queue; /* no change */
}

static void bplib_cache_fsm_state_queue_enter(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_block_t          *rblk;
    bplib_mpool_bblock_primary_t *pri_block;
    bplib_mpool_flow_t           *self_flow;

    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

    if (pri_block != NULL)
    {
        /* Set egress intf to invalid - which can be used as a marker for items which are
         * blocked from egress (it will be set valid again when it actually goes out, so if
         * it stays invalid, that means there is no egress intf available) */
        pri_block->delivery_data.egress_intf_id = BP_INVALID_HANDLE;
        pri_block->delivery_data.egress_time    = 0;
    }

    rblk = bplib_mpool_ref_make_block(store_entry->refptr, BPLIB_STORE_SIGNATURE_BLOCKREF, store_entry);
    if (rblk != NULL)
    {
        self_flow = bplib_cache_get_flow(store_entry->parent);

        /*
         * note - the flag is always set here, even if it does not actually make it into the queue.
         *
         * If it fails to push to the queue, it is immediately recycled, and when the destructor runs
         * the flag will be cleared as normal.  This keeps things synchronized in that it won't transition
         * back to idle until the referring block is actually removed, even though it was never really
         * queued.
         */
        store_entry->flags |= BPLIB_STORE_FLAG_LOCALLY_QUEUED;
        if (!bplib_mpool_flow_try_push(&self_flow->ingress, rblk, 0))
        {
            bplib_mpool_recycle_block(rblk);
        }
    }
}

static void bplib_cache_fsm_state_queue_exit(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_bblock_primary_t *pri_block;

    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));

    if (bp_handle_is_valid(pri_block->delivery_data.egress_intf_id) &&
        pri_block->delivery_data.delivery_policy != bplib_policy_delivery_custody_tracking)
    {
        /* If not doing full custody tracking, then the egress CLA is considered the implicit custodian */
        store_entry->flags &= ~BPLIB_STORE_FLAG_LOCAL_CUSTODY;
    }
}

static bplib_cache_entry_state_t bplib_cache_fsm_state_delete_eval(bplib_cache_entry_t *store_entry)
{
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        /* check if the activity flag is set, meaning that something looked at this entry
         * since the last time this was checked. */
        if ((store_entry->flags & BPLIB_STORE_FLAG_ACTIVITY) == 0)
        {
            /* nothing used this, so should be safe to finally discard it */
            return bplib_cache_entry_state_undefined;
        }

        /* clear it now, it will be re-set if something uses this entry */
        store_entry->flags &= ~BPLIB_STORE_FLAG_ACTIVITY;
        store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
        store_entry->action_time = store_entry->parent->action_time + BP_CACHE_AGE_OUT_TIME;
    }

    return bplib_cache_entry_state_delete;
}

static void bplib_cache_fsm_state_delete_enter(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_bblock_primary_t *pri_block;

    /* the bundle will not be forwarded again, so content is no longer useful, but hang onto the metadata for now */
    /* this recovers the bulk of the memory associated with this bundle */
    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));
    if (pri_block != NULL)
    {
        bplib_mpool_bblock_primary_drop_encode(pri_block);
        bplib_mpool_recycle_all_blocks_in_list(NULL, &pri_block->cblock_list);
    }

    store_entry->flags |= BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
    store_entry->action_time = store_entry->parent->action_time + BP_CACHE_AGE_OUT_TIME;
}

static bplib_cache_entry_state_t bplib_cache_fsm_state_generate_dacs_eval(bplib_cache_entry_t *store_entry)
{
    /* check if dacs is due for transmit */
    if ((store_entry->flags & BPLIB_STORE_FLAG_ACTION_TIME_WAIT) == 0)
    {
        return bplib_cache_entry_state_idle;
    }

    return bplib_cache_entry_state_generate_dacs;
}

static void bplib_cache_fsm_state_generate_dacs_exit(bplib_cache_entry_t *store_entry)
{
    bplib_cache_custody_finalize_dacs(store_entry->parent, store_entry);
}

static void bplib_cache_fsm_reschedule(bplib_cache_state_t *state, bplib_cache_entry_t *store_entry)
{
    uint64_t ref_time;

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
    ref_time |= (1 << BP_CACHE_TIME_BUCKET_SHIFT) - 1;

    if (ref_time != store_entry->next_eval_time)
    {
        /*
         * If this was in the time index, the old entry needs to be removed first,
         * then it needs to be added in the new spot
         */
        bplib_cache_remove_from_subindex(&state->time_index, &store_entry->time_link);
        bplib_cache_add_to_subindex(&state->time_index, &store_entry->time_link, ref_time);
        store_entry->next_eval_time = ref_time;
    }
}

static bplib_cache_entry_state_t bplib_cache_fsm_get_next_state(bplib_cache_entry_t *entry)
{
    static const bplib_cache_fsm_state_eval_func_t STATE_EVAL_TABLE[bplib_cache_entry_state_max] = {
        [bplib_cache_entry_state_idle]          = bplib_cache_fsm_state_idle_eval,
        [bplib_cache_entry_state_queue]         = bplib_cache_fsm_state_queue_eval,
        [bplib_cache_entry_state_delete]        = bplib_cache_fsm_state_delete_eval,
        [bplib_cache_entry_state_generate_dacs] = bplib_cache_fsm_state_generate_dacs_eval};

    bplib_cache_fsm_state_eval_func_t state_eval_func;
    bplib_cache_entry_state_t         state;

    state = entry->state;
    if (state < bplib_cache_entry_state_max)
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

static void bplib_cache_fsm_transition_state(bplib_cache_entry_t *entry, bplib_cache_entry_state_t next_state)
{
    static const bplib_cache_fsm_state_change_func_t STATE_ENTER_TABLE[bplib_cache_entry_state_max] = {
        [bplib_cache_entry_state_idle]   = bplib_cache_fsm_state_idle_enter,
        [bplib_cache_entry_state_queue]  = bplib_cache_fsm_state_queue_enter,
        [bplib_cache_entry_state_delete] = bplib_cache_fsm_state_delete_enter};

    static const bplib_cache_fsm_state_change_func_t STATE_EXIT_TABLE[bplib_cache_entry_state_max] = {
        [bplib_cache_entry_state_queue]         = bplib_cache_fsm_state_queue_exit,
        [bplib_cache_entry_state_generate_dacs] = bplib_cache_fsm_state_generate_dacs_exit};

    bplib_cache_fsm_state_change_func_t state_enter_func;
    bplib_cache_fsm_state_change_func_t state_exit_func;

    /* entry->state is the state we are leaving */
    if (entry->state < bplib_cache_entry_state_max)
    {
        state_exit_func = STATE_EXIT_TABLE[entry->state];
    }
    else
    {
        state_exit_func = NULL;
    }

    /* next_state is the state we are entering */
    if (next_state < bplib_cache_entry_state_max)
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

static void bplib_cache_fsm_debug_report_discard(bplib_cache_entry_t *store_entry)
{
    bplib_mpool_bblock_primary_t *pri_block;

    /* the bundle will not be forwarded again, so content is no longer useful, but hang onto the metadata for now */
    /* this recovers the bulk of the memory associated with this bundle */
    pri_block = bplib_mpool_bblock_primary_cast(bplib_mpool_dereference(store_entry->refptr));
    assert(pri_block != NULL);

    fprintf(stderr, "discarding bundle ID %lu.%lu @%lu\n",
            (unsigned long)pri_block->pri_logical_data.sourceEID.ssp.ipn.node_number,
            (unsigned long)pri_block->pri_logical_data.sourceEID.ssp.ipn.service_number,
            (unsigned long)pri_block->pri_logical_data.creationTimeStamp.sequence_num);
}

void bplib_cache_fsm_execute(bplib_mpool_block_t *sblk)
{
    bplib_cache_state_t      *state;
    bplib_cache_entry_t      *store_entry;
    bplib_cache_entry_state_t next_state;

    /* This cast should always work, unless there is a bug */
    store_entry = bplib_mpool_generic_data_cast(sblk, BPLIB_STORE_SIGNATURE_ENTRY);
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
             * bplib_cache_handle_ref_recycle() was called and it was cleaned up).
             *
             * This is mainly to avoid any potential snowball effects from a misbehaving CLA.  If the CLA was
             * not able to fetch data, it should declare itself DOWN and then all is OK (because existing entries
             * in the egress queue get purged when an intf goes down, and we don't deliver anything new to it).
             */
            store_entry->flags &= ~BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
            store_entry->action_time = BP_CACHE_TIME_INFINITE;
        }

        next_state = bplib_cache_fsm_get_next_state(store_entry);
        if (next_state != store_entry->state)
        {
            bplib_cache_fsm_transition_state(store_entry, next_state);
        }

        if (next_state == bplib_cache_entry_state_undefined)
        {
            bplib_cache_fsm_debug_report_discard(store_entry);
            bplib_mpool_recycle_block(sblk);
        }
        else
        {
            /*
             * The main objective here is to always periodically revisit items in the
             * idle list at some point, don't let them linger indefinitely without
             * at least looking at them.
             */
            bplib_cache_fsm_reschedule(state, store_entry);
            bplib_mpool_insert_before(&state->idle_list, sblk);
        }
    }
}

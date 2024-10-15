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

#include "bplib_api_types.h"

#include "../qm/inc/bplib_stor_qm.h"
#include "../qm/inc/bplib_stor_qm_ducts.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_internal.h"
#include "bplib_stor_cache_ref.h"
#include "bplib_stor_cache_module_api.h"
#include "bplib_stor_cache_block.h"

// TODO Remove OSAL #define bplog(flags, evt, ...) BPLIB_MEM_OS_Log(__FILE__, __LINE__, flags, evt, __VA_ARGS__)
#ifdef KEEP_BPLOG
int bplog(uint32_t *flags, uint32_t event, ...)
{
    if (event > 0)
    {
        return BPLIB_ERROR;
    }
    else
    {
        return BPLIB_SUCCESS;
    }
}
#endif // KEEP_BPLOG

BPLib_STOR_CACHE_State_t *BPLib_STOR_CACHE_GetState(BPLib_STOR_CACHE_Block_t *intf_block)
{
    BPLib_STOR_CACHE_State_t *state;

    state = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)intf_block, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): storage_block incorrect for BPLib_STOR_CACHE_State_t\n", __func__);
        return NULL;
    }

    return state;
}

int BPLib_STOR_CACHE_EntryTreeInsertUnsorted(const BPLib_MEM_RBT_Link_t *node, void *arg)
{
    /* For the time/dest indices, it only searches by key (dtn time) and it does not matter
     * what order nodes with the same key appear in.  So this always returns 1.  Note
     * this can only be used for insertion, it cannot be used for finding nodes, because
     * no actual comparison is done. */
    return 1;
}

void BPLib_STOR_CACHE_EntryMakePending(BPLib_STOR_CACHE_Entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags)
{
    BPLib_STOR_CACHE_Block_t *sblk;

    /* update the flags based on the event */
    store_entry->flags |= set_flags;
    store_entry->flags &= ~clear_flags;

    sblk = (BPLib_STOR_CACHE_Block_t *)BPLib_MEM_GenericDataUncast(store_entry, BPLib_STOR_CACHE_BlocktypeGeneric, BPLIB_STORE_SIGNATURE_ENTRY);
    printf("%s:%d sblk is 0x%016lx\n", __FILE__, __LINE__, (uint64_t)sblk);
    assert(sblk != NULL);

    BPLib_STOR_CACHE_ExtractNode(sblk);
    BPLib_STOR_CACHE_InsertBefore(&store_entry->parent->pending_list, sblk);
    #ifdef QM
    BPLib_STOR_CACHE_JobMarkActive(&store_entry->parent->pending_job);
    #endif // QM
}

int BPLib_STOR_CACHE_EgressImpl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
{
    BPLib_STOR_QM_Duct_t  *duct;
    BPLib_STOR_CACHE_Block_t *qblk;
    BPLib_STOR_CACHE_Block_t *intf_block;
    BPLib_STOR_CACHE_State_t *state;
    int                  forward_count;

    intf_block = BPLib_STOR_CACHE_GetBlockFromLink(subq_src);
    state      = BPLib_STOR_CACHE_GetState(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    duct = BPLib_STOR_QM_DuctCast(intf_block);
    if (duct == NULL)
    {
        return -1;
    }

    state->action_time = BPLib_STOR_CACHE_GetMonotonicTime();
    forward_count      = 0;
    while (true)
    {
        qblk = BPLib_STOR_QM_DuctTryPull(&duct->egress, BPLIB_MONOTIME_ZERO);
        if (qblk == NULL)
        {
            /* no more bundles */
            break;
        }

        ++forward_count;

        #ifdef CUSTODY
        /* Is this a data bundle that needs to be stored, or is this a custody ack? */
        if (!BPLib_STOR_CACHE_CustodyCheckDacs(state, qblk))
        {
            BPLib_STOR_CACHE_CustodyStoreBundle(state, qblk);
        }
        #endif // CUSTODY

        /*
         * The original/input ref to the bundle can be removed without issue, a copy
         * was stored, so this should not trigger a zero refcount unless the storage
         * failed.
         */
        if (BPLib_STOR_CACHE_IsIndirectBlock(qblk))
        {
            BPLib_STOR_CACHE_RecycleBlock(qblk);
        }
    }

    return forward_count;
}

void BPLib_STOR_CACHE_FlushPending(BPLib_STOR_CACHE_State_t *state)
{
    BPLib_STOR_CACHE_ListIter_t list_it;
    int                         status;
    BPLib_STOR_QM_Duct_t       *self_duct;

    self_duct = BPLib_STOR_CACHE_GetDuct(state);

    /* Attempt to re-queue all bundles in the pending list */
    /* In some cases the bundle can get re-added to the pending list, so this is done in a loop */
    status = BPLib_STOR_CACHE_ListIterGotoFirst(&state->pending_list, &list_it);
    while (status == BPLIB_SUCCESS && BPLib_STOR_CACHE_SubqWorkitemMayPush(&self_duct->ingress))
    {
        /* removal of an iterator node is allowed */
        BPLib_STOR_CACHE_ExtractNode(list_it.position);
        // TODO 1 Define BPLib_STOR_CACHE_FsmExecute
        #ifdef FSM_DEFINED
        BPLib_STOR_CACHE_FsmExecute(list_it.position);
        #endif // FSM_DEFINED
        status = BPLib_STOR_CACHE_ListIterForward(&list_it);
    }
}

int BPLib_STOR_CACHE_DoPoll(BPLib_STOR_CACHE_State_t *state)
{
    BPLib_MEM_RBT_Iter_t     rbt_it;
    BPLib_STOR_CACHE_Entry_t *store_entry;
    bp_val_t             ref_val;
    int                  rbt_status;

    ref_val = BPLIB_TIME_TO_INT(BPLib_STOR_CACHE_GetMonotonicTime());
    rbt_status = BPLib_MEM_RBT_IterGotoMax(ref_val, &state->time_index, &rbt_it);
    while (rbt_status == BPLIB_SUCCESS)
    {
        store_entry = BPLib_STOR_CACHE_EntryFromLink(rbt_it.position, time_rbt_link);

        /* preemptively move the iterator - the current entry will be removed,
         * and if that was done first, it would invalidate the iterator */
        rbt_status = BPLib_MEM_RBT_IterPrev(&rbt_it);

        BPLib_STOR_CACHE_EntryMakePending(store_entry, 0, 0);

        /* done with this entry in the time index (will be re-added when pending_list is processed) */
        BPLib_MEM_RBT_ExtractNode(&state->time_index, &store_entry->time_rbt_link);
    }

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DoRouteUp(BPLib_STOR_CACHE_State_t *state, bp_ipn_t dest, bp_ipn_t mask)
{
    BPLib_MEM_RBT_Iter_t     rbt_it;
    BPLib_STOR_CACHE_Entry_t *store_entry;
    int                  rbt_status;
    bp_ipn_t             curr_ipn;

    rbt_status = BPLib_MEM_RBT_IterGotoMin(dest, &state->dest_eid_index, &rbt_it);
    while (rbt_status == BPLIB_SUCCESS)
    {
        curr_ipn = BPLib_MEM_RBT_GetKeyValue(rbt_it.position);
        if ((curr_ipn & mask) != dest)
        {
            /* no longer a queue match, all done */
            break;
        }

        rbt_status  = BPLib_MEM_RBT_IterNext(&rbt_it);
        store_entry = BPLib_STOR_CACHE_EntryFromLink(rbt_it.position, time_rbt_link);
        BPLib_STOR_CACHE_EntryMakePending(store_entry, 0, 0);
    }

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DoIntfStatechange(BPLib_STOR_CACHE_State_t *state, bool is_up)
{
    BPLib_STOR_QM_Duct_t *self_duct;

    self_duct = BPLib_STOR_CACHE_GetDuct(state);
    if (!is_up)
    {
        self_duct->ingress.current_depth_limit = 0;
        self_duct->egress.current_depth_limit  = 0;
    }
    else if ((self_duct->current_state_flags & BPLIB_CACHE_STATE_FLAG_ENDPOINT) != 0)
    {
        self_duct->ingress.current_depth_limit = BP_MPOOL_SHORT_SUBQ_DEPTH;
        self_duct->egress.current_depth_limit  = BP_MPOOL_SHORT_SUBQ_DEPTH;
    }
    else
    {
        self_duct->ingress.current_depth_limit = BPLIB_MAX_SUBQ_DEPTH;
        self_duct->egress.current_depth_limit  = BPLIB_MAX_SUBQ_DEPTH;
    }
    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_EventImpl(void *event_arg, BPLib_STOR_CACHE_Block_t *intf_block)
{
    BPLib_STOR_CACHE_State_t              *state;
    BPLib_STOR_QM_DuctGenericEvent_t *event;
    bp_handle_t                       self_intf_id;

    event        = event_arg;
    self_intf_id = BPLib_STOR_CACHE_GetExternalId(intf_block);
    state        = BPLib_STOR_CACHE_GetState(intf_block);
    if (state == NULL)
    {
        return -1;
    }

    state->action_time = BPLib_STOR_CACHE_GetMonotonicTime();
    if (event->event_type == BPLib_STOR_QM_DuctEventPoll)
    {
        BPLib_STOR_CACHE_DoPoll(state);
    }
    else if ((event->event_type == BPLib_STOR_QM_DuctEventUp || event->event_type == BPLib_STOR_QM_DuctEventDown) &&
             bp_handle_equal(self_intf_id, event->intf_state.intf_id))
    {
        BPLib_STOR_CACHE_DoIntfStatechange(state, event->event_type == BPLib_STOR_QM_DuctEventUp);
    }

    /* any sort of action may have put bundles in the pending queue, so flush it now */
    BPLib_STOR_CACHE_FlushPending(state);

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_ProcessPending(void *arg, BPLib_STOR_CACHE_Block_t *job)
{
    BPLib_STOR_CACHE_FlushPending(BPLib_STOR_CACHE_GetState(BPLib_STOR_CACHE_GetBlockFromLink(job)));
    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_ConstructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
{
    #ifdef QM
    BPLib_STOR_CACHE_State_t *state;

    state = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        return BPLIB_ERROR;
    }

    BPLib_STOR_CACHE_JobInit(sblk, &state->pending_job);

    state->pending_job.handler = BPLib_STOR_CACHE_ProcessPending;

    BPLib_STOR_CACHE_InitListHead(sblk, &state->pending_list);
    BPLib_STOR_CACHE_InitListHead(sblk, &state->idle_list);

    BPLib_MEM_RBT_InitRoot(&state->bundle_index);
    BPLib_MEM_RBT_InitRoot(&state->dacs_index);
    BPLib_MEM_RBT_InitRoot(&state->dest_eid_index);
    BPLib_MEM_RBT_InitRoot(&state->time_index);

    #endif // QM

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DestructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_State_t *state;

    state = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state == NULL)
    {
        return BPLIB_ERROR;
    }

    /* At this point, all the sub-indices and lists should be empty.  The application
     * should have made this so before attempting to delete the intf.
     * If not so, they cannot be cleaned up now, because the state object is no longer valid,
     * the desctructors for these objects will not work correctly */
    assert(BPLib_MEM_RBT_TreeIsEmpty(&state->time_index));
    assert(BPLib_MEM_RBT_TreeIsEmpty(&state->dest_eid_index));
    assert(BPLib_MEM_RBT_TreeIsEmpty(&state->bundle_index));
    assert(BPLib_MEM_RBT_TreeIsEmpty(&state->dacs_index));
    assert(BPLib_STOR_CACHE_IsLinkUnattached(&state->idle_list));
    assert(BPLib_STOR_CACHE_IsLinkUnattached(&state->pending_list));

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_ConstructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_Entry_t *store_entry;

    store_entry = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry == NULL)
    {
        return BPLIB_ERROR;
    }

    store_entry->parent = arg;

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DestructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_Entry_t *store_entry;
    BPLib_STOR_CACHE_State_t *state;

    store_entry = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_ENTRY);
    if (store_entry == NULL)
    {
        return BPLIB_ERROR;
    }

    state = store_entry->parent;

    /* need to make sure this is removed from all index trees */
    if (BPLib_MEM_RBT_NodeIsMember(&state->dacs_index, &store_entry->hash_rbt_link))
    {
        BPLib_MEM_RBT_ExtractNode(&state->dacs_index, &store_entry->hash_rbt_link);
    }
    if (BPLib_MEM_RBT_NodeIsMember(&state->bundle_index, &store_entry->hash_rbt_link))
    {
        BPLib_MEM_RBT_ExtractNode(&state->bundle_index, &store_entry->hash_rbt_link);
    }

    /* for the time index, 0 is an invalid key value and means it was never added.
     * This is a faster/easier way to check than BPLib_MEM_RBT_NodeIsMember(), but can
     * only be used if it the link is only associated with a single tree */
    if (BPLib_MEM_RBT_GetKeyValue(&store_entry->time_rbt_link) != 0)
    {
        BPLib_MEM_RBT_ExtractNode(&state->time_index, &store_entry->time_rbt_link);
    }
    if (BPLib_MEM_RBT_GetKeyValue(&store_entry->dest_eid_rbt_link) != 0)
    {
        BPLib_MEM_RBT_ExtractNode(&state->dest_eid_index, &store_entry->dest_eid_rbt_link);
    }

    /* release the refptr */
    BPLib_STOR_CACHE_RefRelease(store_entry->refptr);
    store_entry->refptr = NULL;

    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_ConstructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
{
    BPLib_STOR_CACHE_Blockref_t *blockref;
    BPLib_STOR_CACHE_Entry_t    *store_entry;

    store_entry = arg;
    blockref    = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (blockref == NULL)
    {
        return BPLIB_ERROR;
    }

    /*
     * note, this needs a ref back to the block itself, not the store_entry object.
     */
    blockref->storage_entry = store_entry;
    return BPLIB_SUCCESS;
}

int BPLib_STOR_CACHE_DestructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *rblk)
{
    BPLib_STOR_CACHE_Blockref_t *block_ref;

    block_ref = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)rblk, BPLIB_STORE_SIGNATURE_BLOCKREF);
    if (block_ref == NULL)
    {
        return BPLIB_ERROR;
    }

    assert(block_ref->storage_entry != NULL);

    /*
     * always put back into pending_list, this will re-evalute its current
     * state and reclassify it as appropriate.  This also clears the BPLIB_STORE_FLAG_LOCALLY_QUEUED
     * flag.
     */
    BPLib_STOR_CACHE_EntryMakePending(block_ref->storage_entry, 0, BPLIB_STORE_FLAG_LOCALLY_QUEUED);

    return BPLIB_SUCCESS;
}

void BPLib_STOR_CACHE_Init(BPLib_STOR_CACHE_Pool_t *pool)
{
    const BPLib_STOR_CACHE_BlocktypeApi_t state_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = BPLib_STOR_CACHE_ConstructState,
        .destruct  = BPLib_STOR_CACHE_DestructState,
    };

    const BPLib_STOR_CACHE_BlocktypeApi_t entry_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = BPLib_STOR_CACHE_ConstructEntry,
        .destruct  = BPLib_STOR_CACHE_DestructEntry,
    };

    const BPLib_STOR_CACHE_BlocktypeApi_t blockref_api = (BPLib_STOR_CACHE_BlocktypeApi_t) {
        .construct = BPLib_STOR_CACHE_ConstructBlockref,
        .destruct  = BPLib_STOR_CACHE_DestructBlockref,
    };

    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_STORE_SIGNATURE_STATE, &state_api, sizeof(BPLib_STOR_CACHE_State_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_STORE_SIGNATURE_ENTRY, &entry_api, sizeof(BPLib_STOR_CACHE_Entry_t));
    BPLib_STOR_CACHE_RegisterBlocktype(pool, BPLIB_STORE_SIGNATURE_BLOCKREF, &blockref_api, sizeof(BPLib_STOR_CACHE_Blockref_t));
}

bp_handle_t BPLib_STOR_CACHE_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    BPLib_STOR_CACHE_State_t *state;
    BPLib_STOR_CACHE_Block_t *sblk;
    #ifdef QM
    BPLib_STOR_CACHE_Pool_t       *pool;
    #endif
    BPLib_STOR_CACHE_Ref_t    duct_block_ref;
    bp_handle_t          storage_intf_id;

    #ifdef QM  // TODO Enable QM Cache Init.
    pool = BPLib_STOR_QM_GetQtblPool(tbl);

    /* register Mem Cache storage module */
    BPLib_STOR_CACHE_Init(pool);

    sblk = BPLib_STOR_QM_DuctAlloc(pool, BPLIB_STORE_SIGNATURE_STATE, pool);
    #else // QM
    sblk = NULL;
    #endif // QM
    if (sblk == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "%s(): Insufficient memory to create file storage\n", __func__);
        return BP_INVALID_HANDLE;
    }

    /* this must always work, it was just created above */
    duct_block_ref = BPLib_STOR_CACHE_RefCreate(sblk);
    state          = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)sblk, BPLIB_STORE_SIGNATURE_STATE);

    #ifdef QM
    storage_intf_id = BPLib_STOR_CACHE_DataserviceAttach(tbl, service_addr, BPLib_STOR_CACHE_DataserviceTypeStorage, duct_block_ref);
    #else // QM
    storage_intf_id = BP_INVALID_HANDLE;
    #endif // QM

    if (!bp_handle_is_valid(storage_intf_id))
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): cannot attach - service addr invalid?\n", __func__);
        BPLib_STOR_CACHE_RefRelease(duct_block_ref);
    }
    else
    {
        /* there should be no reason for any of these reg calls to fail */
        BPLib_STOR_QM_RegisterForwardEgressHandler(tbl, storage_intf_id, BPLib_STOR_CACHE_EgressImpl);
        BPLib_STOR_QM_RegisterForwardIngressHandler(tbl, storage_intf_id, BPLib_STOR_QM_IngressToParent);
        BPLib_STOR_QM_RegisterEventHandler(tbl, storage_intf_id, BPLib_STOR_CACHE_EventImpl);

        /* This will keep the ref to itself inside of the state struct, this
         * creates a circular reference and prevents the refcount from ever becoming 0
         */
        state->self_addr = *service_addr;
    }

    return storage_intf_id;
}

int BPLib_STOR_CACHE_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
{
    BPLib_STOR_CACHE_State_t *state;
    BPLib_STOR_CACHE_Ref_t    duct_block_ref;
    int                  status;

    #ifdef QM  // TODO Cache Detach
    duct_block_ref =BPLib_STOR_CACHE_DataserviceDetach(tbl, service_addr);
    #else // QM
    duct_block_ref = NULL;
    #endif // QM
    if (duct_block_ref != NULL)
    {
        state = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)BPLib_STOR_CACHE_Dereference(duct_block_ref), BPLIB_STORE_SIGNATURE_STATE);
    }
    else
    {
        state = NULL;
    }

    if (state == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): cannot detach - service addr invalid?\n", __func__);
        status = BPLIB_ERROR;
    }
    else
    {
        /* Release the local ref - this should cause the refcount to become 0 */
        BPLib_STOR_CACHE_RefRelease(duct_block_ref);
        status = BPLIB_SUCCESS;
    }

    return status;
}

bp_handle_t BPLib_STOR_CACHE_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t cache_intf_id,
                                                const BPLib_STOR_CACHE_ModuleApi_t *api, void *init_arg)
{
    BPLib_STOR_CACHE_State_t *state;
    BPLib_STOR_CACHE_Block_t *cblk;
    BPLib_STOR_CACHE_Block_t *svc;
    BPLib_STOR_CACHE_Ref_t    parent_ref;
    int                  status;
    bp_handle_t          handle;

    svc    = NULL;
    handle = BP_INVALID_HANDLE;
    cblk   = BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_QM_GetQtblPool(tbl), cache_intf_id);
    state  = BPLib_MEM_GenericDataCast((BPLib_MEM_Block_t *)cblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        parent_ref = BPLib_STOR_CACHE_RefCreate(cblk);
        if (parent_ref != NULL)
        {
            // TODO fix api error incomplete type.
            #ifdef VALID_API_TYPEDEF
            svc = api->instantiate(parent_ref, init_arg);
            #endif // VALID_API_TYPEDEF
            BPLib_STOR_CACHE_RefRelease(parent_ref);
        }
    }
    else
    {
        svc = NULL;
    }

    if (svc != NULL)
    {
        switch (api->module_type)
        {
            case BPLib_STOR_CACHE_ModuleTypeOffload:
                state->offload_api = (const BPLib_STOR_PS_OffloadApi_t *)api;
                state->offload_blk = svc;
                status             = BPLIB_SUCCESS;
                break;
            default:
                status = BPLIB_ERROR;
                break;
        }

        if (status == BPLIB_SUCCESS)
        {
            handle = BPLib_STOR_CACHE_GetExternalId(cblk);
        }
    }

    return handle;
}

#define BPLib_STOR_CACHE_DebugFsmStatePrint(s, n) BPLib_STOR_CACHE_DebugFsmStatePrintImpl(s, n, "[" #n "]")
void BPLib_STOR_CACHE_DebugFsmStatePrintImpl(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_EntryState_t n,
                                            const char *fsmname)
{
    uint32_t entercount;
    uint32_t exitcount;

    entercount = state->fsm_state_enter_count[n];
    exitcount  = state->fsm_state_exit_count[n];

    fprintf(stderr, " STATE STATS: %45s enter=%lu exit=%lu current=%lu\n", fsmname, (unsigned long)entercount,
            (unsigned long)exitcount, (unsigned long)(entercount - exitcount));
}

void BPLib_STOR_CACHE_DebugScanQueue(void *tbl, bp_handle_t intf_id)
{
    #ifdef QM
    BPLib_STOR_CACHE_Ref_t    intf_block_ref;
    BPLib_STOR_CACHE_State_t *state;

    intf_block_ref = BPLib_STOR_QM_GetIntfControlblock(tbl, intf_id);
    if (intf_block_ref == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): Parent intf invalid\n", __func__);
        return;
    }

    state = BPLib_STOR_CACHE_GetState(BPLib_STOR_CACHE_Dereference(intf_block_ref));
    if (state == NULL)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "%s(): Parent intf is not a storage cache\n", __func__);
        return;
    }

    fprintf(stderr, "DEBUG: %s() intf_id=%d\n", __func__, bp_handle_printable(intf_id));

    BPLib_STOR_CACHE_DebugFsmStatePrint(state, BPLib_STOR_CACHE_EntryStateUndefined);
    BPLib_STOR_CACHE_DebugFsmStatePrint(state, BPLib_STOR_CACHE_EntryStateIdle);
    BPLib_STOR_CACHE_DebugFsmStatePrint(state, BPLib_STOR_CACHE_EntryStateQueue);
    BPLib_STOR_CACHE_DebugFsmStatePrint(state, BPLib_STOR_CACHE_EntryStateDelete);
    BPLib_STOR_CACHE_DebugFsmStatePrint(state, BPLib_STOR_CACHE_EntryStateGenerateDacs);
    fprintf(stderr, " DISCARDED BUNDLES: %lu\n\n", (unsigned long)state->discard_count);

    BPLib_STOR_QM_ReleaseIntfControlblock(tbl, intf_block_ref);
    #endif // QM
}

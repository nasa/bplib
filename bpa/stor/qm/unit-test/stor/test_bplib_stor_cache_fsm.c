/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 * Includes
 */
#include "test_bplib_cache.h"
#include "bplib_stor_cache_block.h"

void test_BPLib_STOR_CACHE_FsmStateIdleEval(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateIdleEval(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t       store_entry;
    BPLib_STOR_CACHE_State_t       parent;
    BPLib_STOR_CACHE_OffloadApi_t offload_api;
    BPLib_STOR_CACHE_Block_t       refptr;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&parent, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&offload_api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    store_entry.parent  = &parent;
    offload_api.restore = test_BPLib_STOR_CACHE_RestoreStub;
    parent.offload_api  = &offload_api;

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_FsmStateIdleEval(&store_entry), 0);

    store_entry.expire_time = 10;
    store_entry.flags       = 0;
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_FsmStateIdleEval(&store_entry), 0);

    store_entry.flags       = BPLIB_STORE_FLAG_LOCAL_CUSTODY;
    store_entry.offload_sid = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_FsmStateIdleEval(&store_entry), 0);
}

void test_BPLib_STOR_CACHE_FsmStateQueueEval(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateQueueEval(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UtAssert_UINT32_GT(BPLib_STOR_CACHE_FsmStateQueueEval(&store_entry), 0);

    store_entry.flags = 8;
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_FsmStateQueueEval(&store_entry), 0);
}

void test_BPLib_STOR_CACHE_FsmStateQueueEnter(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmStateQueueEnter(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;
    BPLib_STOR_CACHE_Block_t blk;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPush), UT_cache_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmStateQueueEnter(&store_entry));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_FsmExecute(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmExecute(BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Block_t sblk1;
    BPLib_STOR_CACHE_Entry_t store_entry;
    BPLib_STOR_CACHE_State_t state;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    sblk.type          = BPLib_STOR_CACHE_BlocktypeGeneric;
    sblk.parent_offset = sizeof(BPLib_STOR_CACHE_Block_t);
    sblk.next          = &sblk1;
    store_entry.parent = &state;
    store_entry.flags  = BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
    store_entry.state  = BPLib_STOR_CACHE_EntryStateIdle;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmExecute(&sblk));

    store_entry.expire_time = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_GetKeyValue), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmExecute(&sblk));

    store_entry.state = BPLib_STOR_CACHE_EntryStateIdle;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmExecute(&sblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_FsmStateQueueExit(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmStateQueueExit(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t          store_entry;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    pri_block.data.delivery.egress_intf_id = BPLIB_HANDLE_RAM_STORE_BASE;
    store_entry.offload_sid                = 1;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmStateQueueExit(&store_entry));

    pri_block.data.delivery.delivery_policy =BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmStateQueueExit(&store_entry));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_FsmStateDeleteEval(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateDeleteEval(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;
    BPLib_STOR_CACHE_State_t state;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UtAssert_NULL(BPLib_STOR_CACHE_FsmStateDeleteEval(&store_entry));

    store_entry.flags  = 1;
    store_entry.parent = &state;
    UtAssert_NOT_NULL(BPLib_STOR_CACHE_FsmStateDeleteEval(&store_entry));
}

void test_BPLib_STOR_CACHE_FsmStateDeleteEnter(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmStateDeleteEnter(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t       store_entry;
    BPLib_STOR_CACHE_State_t       state;
    BPLib_STOR_CACHE_Block_t       refptr;
    BPLib_STOR_CACHE_OffloadApi_t offload_api;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&offload_api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));
    offload_api.release             = test_BPLib_STOR_CACHE_ReleaseStub;
    store_entry.parent              = &state;
    store_entry.refptr              = (BPLib_STOR_CACHE_Ref_t)&refptr;
    store_entry.offload_sid         = 1;
    store_entry.parent->offload_api = &offload_api;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmStateDeleteEnter(&store_entry));
}

void test_BPLib_STOR_CACHE_FsmReschedule(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmReschedule(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_GetKeyValue), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_GetKeyValue), UT_cache_uint64_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmReschedule(&state, &store_entry));

    store_entry.flags = 4;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmReschedule(&state, &store_entry));
}

void test_BPLib_STOR_CACHE_FsmStateGenerateDacsEval(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmStateGenerateDacsEval(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UtAssert_NOT_NULL(BPLib_STOR_CACHE_FsmStateGenerateDacsEval(&store_entry));

    store_entry.flags = 4;
    UtAssert_NOT_NULL(BPLib_STOR_CACHE_FsmStateGenerateDacsEval(&store_entry));
}

void test_BPLib_STOR_CACHE_FsmStateGenerateDacsExit(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmStateGenerateDacsExit(BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_NodeIsMember), UT_cache_bool_Handler, NULL);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmStateGenerateDacsExit(&store_entry));
}

void test_BPLib_STOR_CACHE_FsmTransitionState(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FsmTransitionState(BPLib_STOR_CACHE_Entry_t *entry, BPLib_STOR_CACHE_EntryState_t next_state)
     */
    BPLib_STOR_CACHE_Entry_t entry;

    memset(&entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    entry.state                          = 10;
    BPLib_STOR_CACHE_EntryState_t next_state = 10;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmTransitionState(&entry, next_state));

    entry.state = BPLib_STOR_CACHE_EntryStateGenerateDacs;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FsmTransitionState(&entry, next_state));
}

void test_BPLib_STOR_CACHE_FsmGetNextState(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_EntryState_t BPLib_STOR_CACHE_FsmGetNextState(BPLib_STOR_CACHE_Entry_t *entry)
     */
    BPLib_STOR_CACHE_Entry_t entry;

    memset(&entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_FsmGetNextState(&entry), 0);
}

void Test_BplibCacheFsm_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateIdleEval, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateIdleEval");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateQueueEval, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateQueueEval");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateQueueEnter, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateQueueEnter");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmExecute, NULL, NULL, "Test BPLib_STOR_CACHE_FsmExecute");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateQueueExit, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateQueueExit");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateDeleteEval, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateDeleteEval");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateDeleteEnter, NULL, NULL, "Test BPLib_STOR_CACHE_FsmStateDeleteEnter");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmReschedule, NULL, NULL, "Test BPLib_STOR_CACHE_FsmReschedule");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateGenerateDacsEval, NULL, NULL,
               "Test BPLib_STOR_CACHE_FsmStateGenerateDacsEval");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmStateGenerateDacsExit, NULL, NULL,
               "Test BPLib_STOR_CACHE_FsmStateGenerateDacsExit");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmTransitionState, NULL, NULL, "Test BPLib_STOR_CACHE_FsmTransitionState");
    UtTest_Add(test_BPLib_STOR_CACHE_FsmGetNextState, NULL, NULL, "Test BPLib_STOR_CACHE_FsmGetNextState");
}

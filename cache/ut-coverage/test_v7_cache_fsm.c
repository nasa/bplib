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

void test_bplib_cache_fsm_state_idle_eval(void)
{
    /* Test function for:
     * bplib_cache_entry_state_t bplib_cache_fsm_state_idle_eval(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t       store_entry;
    bplib_cache_state_t       parent;
    bplib_cache_offload_api_t offload_api;
    bplib_mpool_block_t       refptr;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&parent, 0, sizeof(bplib_cache_state_t));
    memset(&offload_api, 0, sizeof(bplib_cache_offload_api_t));
    memset(&refptr, 0, sizeof(bplib_mpool_block_t));
    store_entry.parent  = &parent;
    offload_api.restore = test_bplib_cache_restore_stub;
    parent.offload_api  = &offload_api;

    UtAssert_UINT32_EQ(bplib_cache_fsm_state_idle_eval(&store_entry), 0);

    store_entry.expire_time = 10;
    store_entry.flags       = 0;
    UtAssert_UINT32_GT(bplib_cache_fsm_state_idle_eval(&store_entry), 0);

    store_entry.flags       = BPLIB_STORE_FLAG_LOCAL_CUSTODY;
    store_entry.offload_sid = 1;
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_cache_AltHandler_PointerReturn, &refptr);
    UtAssert_UINT32_GT(bplib_cache_fsm_state_idle_eval(&store_entry), 0);
}

void test_bplib_cache_fsm_state_queue_eval(void)
{
    /* Test function for:
     * bplib_cache_entry_state_t bplib_cache_fsm_state_queue_eval(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t store_entry;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UtAssert_UINT32_GT(bplib_cache_fsm_state_queue_eval(&store_entry), 0);

    store_entry.flags = 8;
    UtAssert_UINT32_GT(bplib_cache_fsm_state_queue_eval(&store_entry), 0);
}

void test_bplib_cache_fsm_state_queue_enter(void)
{
    /* Test function for:
     * void bplib_cache_fsm_state_queue_enter(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t store_entry;
    bplib_mpool_block_t blk;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&blk, 0, sizeof(bplib_mpool_block_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_try_push), UT_cache_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_make_block), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(bplib_cache_fsm_state_queue_enter(&store_entry));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_make_block), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_fsm_execute(void)
{
    /* Test function for:
     * void bplib_cache_fsm_execute(bplib_mpool_block_t *sblk)
     */
    bplib_mpool_block_t sblk;
    bplib_mpool_block_t sblk1;
    bplib_cache_entry_t store_entry;
    bplib_cache_state_t state;

    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&sblk1, 0, sizeof(bplib_mpool_block_t));
    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    sblk.type          = bplib_mpool_blocktype_generic;
    sblk.parent_offset = sizeof(bplib_mpool_block_t);
    sblk.next          = &sblk1;
    store_entry.parent = &state;
    store_entry.flags  = BPLIB_STORE_FLAG_ACTION_TIME_WAIT;
    store_entry.state  = bplib_cache_entry_state_idle;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UtAssert_VOIDCALL(bplib_cache_fsm_execute(&sblk));

    store_entry.expire_time = 1;
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_get_key_value), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(bplib_cache_fsm_execute(&sblk));

    store_entry.state = bplib_cache_entry_state_idle;
    UtAssert_VOIDCALL(bplib_cache_fsm_execute(&sblk));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_fsm_state_queue_exit(void)
{
    /* Test function for:
     * void bplib_cache_fsm_state_queue_exit(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t          store_entry;
    bplib_mpool_bblock_primary_t pri_block;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&pri_block, 0, sizeof(bplib_mpool_bblock_primary_t));
    pri_block.data.delivery.egress_intf_id = BPLIB_HANDLE_RAM_STORE_BASE;
    store_entry.offload_sid                = 1;

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UtAssert_VOIDCALL(bplib_cache_fsm_state_queue_exit(&store_entry));

    pri_block.data.delivery.delivery_policy = bplib_policy_delivery_custody_tracking;
    UtAssert_VOIDCALL(bplib_cache_fsm_state_queue_exit(&store_entry));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_bblock_primary_cast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_bplib_cache_fsm_state_delete_eval(void)
{
    /* Test function for:
     * bplib_cache_entry_state_t bplib_cache_fsm_state_delete_eval(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t store_entry;
    bplib_cache_state_t state;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));

    UtAssert_NULL(bplib_cache_fsm_state_delete_eval(&store_entry));

    store_entry.flags  = 1;
    store_entry.parent = &state;
    UtAssert_NOT_NULL(bplib_cache_fsm_state_delete_eval(&store_entry));
}

void test_bplib_cache_fsm_state_delete_enter(void)
{
    /* Test function for:
     * void bplib_cache_fsm_state_delete_enter(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t       store_entry;
    bplib_cache_state_t       state;
    bplib_mpool_block_t       refptr;
    bplib_cache_offload_api_t offload_api;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));
    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&refptr, 0, sizeof(bplib_mpool_block_t));
    memset(&offload_api, 0, sizeof(bplib_cache_offload_api_t));
    offload_api.release             = test_bplib_cache_release_stub;
    store_entry.parent              = &state;
    store_entry.refptr              = (bplib_mpool_ref_t)&refptr;
    store_entry.offload_sid         = 1;
    store_entry.parent->offload_api = &offload_api;

    UtAssert_VOIDCALL(bplib_cache_fsm_state_delete_enter(&store_entry));
}

void test_bplib_cache_fsm_reschedule(void)
{
    /* Test function for:
     * void bplib_cache_fsm_reschedule(bplib_cache_state_t *state, bplib_cache_entry_t *store_entry)
     */
    bplib_cache_state_t state;
    bplib_cache_entry_t store_entry;

    memset(&state, 0, sizeof(bplib_cache_state_t));
    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UT_SetDefaultReturnValue(UT_KEY(bplib_rbt_get_key_value), 1);
    UT_SetHandlerFunction(UT_KEY(bplib_rbt_get_key_value), UT_cache_uint64_Handler, NULL);
    UtAssert_VOIDCALL(bplib_cache_fsm_reschedule(&state, &store_entry));

    store_entry.flags = 4;
    UtAssert_VOIDCALL(bplib_cache_fsm_reschedule(&state, &store_entry));
}

void test_bplib_cache_fsm_state_generate_dacs_eval(void)
{
    /* Test function for:
     * bplib_cache_entry_state_t bplib_cache_fsm_state_generate_dacs_eval(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t store_entry;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UtAssert_NOT_NULL(bplib_cache_fsm_state_generate_dacs_eval(&store_entry));

    store_entry.flags = 4;
    UtAssert_NOT_NULL(bplib_cache_fsm_state_generate_dacs_eval(&store_entry));
}

void test_bplib_cache_fsm_state_generate_dacs_exit(void)
{
    /* Test function for:
     * void bplib_cache_fsm_state_generate_dacs_exit(bplib_cache_entry_t *store_entry)
     */
    bplib_cache_entry_t store_entry;

    memset(&store_entry, 0, sizeof(bplib_cache_entry_t));

    UT_SetHandlerFunction(UT_KEY(bplib_rbt_node_is_member), UT_cache_bool_Handler, NULL);

    UtAssert_VOIDCALL(bplib_cache_fsm_state_generate_dacs_exit(&store_entry));
}

void test_bplib_cache_fsm_transition_state(void)
{
    /* Test function for:
     * void bplib_cache_fsm_transition_state(bplib_cache_entry_t *entry, bplib_cache_entry_state_t next_state)
     */
    bplib_cache_entry_t entry;

    memset(&entry, 0, sizeof(bplib_cache_entry_t));
    entry.state                          = 10;
    bplib_cache_entry_state_t next_state = 10;

    UtAssert_VOIDCALL(bplib_cache_fsm_transition_state(&entry, next_state));

    entry.state = bplib_cache_entry_state_generate_dacs;
    UtAssert_VOIDCALL(bplib_cache_fsm_transition_state(&entry, next_state));
}

void test_bplib_cache_fsm_get_next_state(void)
{
    /* Test function for:
     * bplib_cache_entry_state_t bplib_cache_fsm_get_next_state(bplib_cache_entry_t *entry)
     */
    bplib_cache_entry_t entry;

    memset(&entry, 0, sizeof(bplib_cache_entry_t));

    UtAssert_UINT32_EQ(bplib_cache_fsm_get_next_state(&entry), 0);
}

void TestBplibCacheFsm_Register(void)
{
    UtTest_Add(test_bplib_cache_fsm_state_idle_eval, NULL, NULL, "Test bplib_cache_fsm_state_idle_eval");
    UtTest_Add(test_bplib_cache_fsm_state_queue_eval, NULL, NULL, "Test bplib_cache_fsm_state_queue_eval");
    UtTest_Add(test_bplib_cache_fsm_state_queue_enter, NULL, NULL, "Test bplib_cache_fsm_state_queue_enter");
    UtTest_Add(test_bplib_cache_fsm_execute, NULL, NULL, "Test bplib_cache_fsm_execute");
    UtTest_Add(test_bplib_cache_fsm_state_queue_exit, NULL, NULL, "Test bplib_cache_fsm_state_queue_exit");
    UtTest_Add(test_bplib_cache_fsm_state_delete_eval, NULL, NULL, "Test bplib_cache_fsm_state_delete_eval");
    UtTest_Add(test_bplib_cache_fsm_state_delete_enter, NULL, NULL, "Test bplib_cache_fsm_state_delete_enter");
    UtTest_Add(test_bplib_cache_fsm_reschedule, NULL, NULL, "Test bplib_cache_fsm_reschedule");
    UtTest_Add(test_bplib_cache_fsm_state_generate_dacs_eval, NULL, NULL,
               "Test bplib_cache_fsm_state_generate_dacs_eval");
    UtTest_Add(test_bplib_cache_fsm_state_generate_dacs_exit, NULL, NULL,
               "Test bplib_cache_fsm_state_generate_dacs_exit");
    UtTest_Add(test_bplib_cache_fsm_transition_state, NULL, NULL, "Test bplib_cache_fsm_transition_state");
    UtTest_Add(test_bplib_cache_fsm_get_next_state, NULL, NULL, "Test bplib_cache_fsm_get_next_state");
}

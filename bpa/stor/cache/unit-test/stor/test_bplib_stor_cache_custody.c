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

void test_BPLib_STOR_CACHE_Module_custody_finalize_dacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_finalize_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_entry_t *store_entry)
     */
    BPLib_STOR_CACHE_Module_state_t state;
    BPLib_STOR_CACHE_Module_entry_t store_entry;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_NodeIsMember), UT_cache_bool_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_finalize_dacs(&state, &store_entry));
}

void test_BPLib_STOR_CACHE_Module_custody_check_dacs(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_Module_custody_check_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Block_t *qblk)
     */
    BPLib_STOR_CACHE_Module_state_t            state;
    BPLib_STOR_CACHE_Block_t            qblk;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_BblockCanonical_t c_block;
    BPLib_STOR_CACHE_RBT_Link_t               custody_rbt_link;
    BPLib_STOR_CACHE_Block_t            blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&qblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_sizet_Handler, NULL);
    pri_block.data.logical.controlFlags.isAdminRecord = true;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    c_block.canonical_logical_data.data.custody_accept_payload_block.num_entries = 1;
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_Module_custody_check_dacs(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_store_bundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_store_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Block_t *qblk)
     */
    BPLib_STOR_CACHE_Module_state_t            state;
    BPLib_STOR_CACHE_Block_t            qblk;
    BPLib_STOR_CACHE_Block_t            qblk1;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_RBT_Link_t               custody_rbt_link;
    BPLib_STOR_CACHE_Module_entry_t            store_entry;
    BPLib_STOR_CACHE_BblockCanonical_t custody_block;
    BPLib_STOR_CACHE_Module_offload_api_t      offload_api;
    BPLib_STOR_CACHE_Block_t            sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&qblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&qblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));
    memset(&custody_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&offload_api, 0, sizeof(BPLib_STOR_CACHE_Module_offload_api_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetParentPoolFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_store_bundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_store_bundle(&state, &qblk));

    pri_block.data.delivery.committed_storage_id = 1;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, &qblk1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_store_bundle(&state, &qblk));

    offload_api.offload                     = test_BPLib_STOR_CACHE_Module_offload_stub;
    state.offload_api                       = &offload_api;
    pri_block.data.delivery.delivery_policy = bplib_policy_delivery_custody_tracking;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_store_bundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_store_bundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_insert_tracking_block(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_insert_tracking_block(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * *pri_block, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t            state;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_Module_custodian_info_t   custody_info;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t custody_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&custody_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &custody_block);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_insert_tracking_block(&state, &pri_block, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_find_dacs_match(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_custody_find_dacs_match(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg)
     */
    BPLib_STOR_CACHE_Module_custodian_info_t custody_info;
    BPLib_STOR_CACHE_Module_entry_t          store_entry;

    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));

    store_entry.state = BPLib_STOR_CACHE_Module_entry_state_generate_dacs;

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Module_custody_find_dacs_match(&store_entry.hash_rbt_link, &custody_info), 0);
}

void test_BPLib_STOR_CACHE_Module_custody_find_pending_dacs(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_Module_custody_find_pending_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *dacs_info)
     */
    BPLib_STOR_CACHE_Module_state_t          state;
    BPLib_STOR_CACHE_Module_custodian_info_t dacs_info;
    BPLib_STOR_CACHE_RBT_Link_t             custody_rbt_link;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&dacs_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_STOR_CACHE_RBT_Link_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_find_pending_dacs(&state, &dacs_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_create_dacs(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_Module_custody_create_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * **pri_block_out, bp_custody_accept_payload_block_t **pay_out)
     */
    BPLib_STOR_CACHE_Module_state_t                state;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block;
    BPLib_STOR_CACHE_BblockPrimary_t      *pri_block_out = &pri_block;
    bp_custody_accept_payload_block_t  pay;
    bp_custody_accept_payload_block_t *pay_out = &pay;
    BPLib_STOR_CACHE_Block_t                blk;
    BPLib_STOR_CACHE_BblockCanonical_t     c_block;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block1;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block2;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&pay, 0, sizeof(bp_custody_accept_payload_block_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&pri_block1, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&pri_block2, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_cache_AltHandler_PointerReturn, &pri_block1);
    UtAssert_NULL(BPLib_STOR_CACHE_Module_custody_create_dacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block2);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetCurrentTime), UT_cache_GetTime_Handler, NULL);
    UtAssert_NULL(BPLib_STOR_CACHE_Module_custody_create_dacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UtAssert_NULL(BPLib_STOR_CACHE_Module_custody_create_dacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_open_dacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_open_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t               state;
    BPLib_STOR_CACHE_Module_custodian_info_t      custody_info;
    BPLib_STOR_CACHE_Block_t               blk;
    BPLib_STOR_CACHE_Module_entry_t               store_entry;
    BPLib_STOR_CACHE_Ref_t                 ref;
    BPLib_STOR_CACHE_BblockPrimary_t      pri_block;
    BPLib_STOR_CACHE_BblockPrimaryData_t data;
    BPLib_STOR_CACHE_BblockCanonical_t    c_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&data, 0, sizeof(BPLib_STOR_CACHE_BblockPrimaryData_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    pri_block.data = data;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_open_dacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefDuplicate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_open_dacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_append_dacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_append_dacs(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t               state;
    BPLib_STOR_CACHE_Module_custodian_info_t      custody_info;
    BPLib_STOR_CACHE_Module_entry_t               store_entry;
    bp_custody_accept_payload_block_t payload_ref;
    BPLib_STOR_CACHE_Block_t               blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Module_entry_t));
    memset(&payload_ref, 0, sizeof(bp_custody_accept_payload_block_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    payload_ref.num_entries           = BP_DACS_MAX_SEQ_PER_PAYLOAD;
    store_entry.data.dacs.payload_ref = &payload_ref;
    custody_info.store_entry          = &store_entry;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_append_dacs(&state, &custody_info));

    custody_info.sequence_num = 1;
    payload_ref.num_entries   = 2;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_append_dacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_update_tracking_block(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_update_tracking_block(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_Module_custodian_info_t
     * *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t            state;
    BPLib_STOR_CACHE_Module_custodian_info_t   custody_info;
    BPLib_STOR_CACHE_BblockCanonical_t c_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_update_tracking_block(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_find_bundle_match(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Module_custody_find_bundle_match(const BPLib_STOR_CACHE_RBT_Link_t *node, void *arg)
     */
    BPLib_STOR_CACHE_Module_custodian_info_t custody_info;
    BPLib_STOR_CACHE_Module_entry_t          store_entry;

    memset(&store_entry, 0, sizeof(store_entry));
    custody_info.sequence_num  = 1;
    store_entry.flow_seq_copy = 1;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_find_bundle_match(&store_entry.hash_rbt_link, &custody_info));
}

void test_BPLib_STOR_CACHE_Module_custody_process_bundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_process_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
     * BPLib_STOR_CACHE_Module_custodian_info_t *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t          state;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;
    BPLib_STOR_CACHE_Module_custodian_info_t custody_info;
    BPLib_STOR_CACHE_Block_t          blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    custody_info.this_cblk = &blk;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_process_bundle(&state, &pri_block, &custody_info));

    custody_info.final_dest_node = 1;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_process_bundle(&state, &pri_block, &custody_info));
}

void test_BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle(BPLib_STOR_CACHE_Module_state_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * *pri_block, const bp_custody_accept_payload_block_t *ack_payload)
     */
    BPLib_STOR_CACHE_Module_state_t               state;
    BPLib_STOR_CACHE_BblockPrimary_t      pri_block;
    bp_custody_accept_payload_block_t ack_payload;
    BPLib_STOR_CACHE_Block_t               blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ack_payload, 0, sizeof(bp_custody_accept_payload_block_t));
    ack_payload.num_entries = 1;
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle(&state, &pri_block, &ack_payload));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_init_info_from_pblock(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_init_info_from_pblock(BPLib_STOR_CACHE_Module_custodian_info_t *custody_info,
     * BPLib_STOR_CACHE_BblockPrimary_t *pri_block)
     */
    BPLib_STOR_CACHE_Module_custodian_info_t   custody_info;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t cblk;

    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &cblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_init_info_from_pblock(&custody_info, &pri_block));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Module_custody_ack_tracking_block(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_Module_custody_ack_tracking_block(BPLib_STOR_CACHE_Module_state_t *state,const BPLib_STOR_CACHE_Module_custodian_info_t
     * *custody_info)
     */
    BPLib_STOR_CACHE_Module_state_t            state;
    BPLib_STOR_CACHE_Module_custodian_info_t   custody_info;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t cblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_Module_state_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_Module_custodian_info_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    custody_info.prev_cblk = &blk;
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_Module_custody_ack_tracking_block(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void TestBplibCacheCustody_Register(void)
{
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_finalize_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_finalize_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_check_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_check_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_store_bundle, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_store_bundle");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_insert_tracking_block, NULL, NULL,
               "Test BPLib_STOR_CACHE_Module_custody_insert_tracking_block");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_find_dacs_match, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_find_dacs_match");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_find_pending_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_find_pending_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_create_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_create_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_open_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_open_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_append_dacs, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_append_dacs");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_update_tracking_block, NULL, NULL,
               "Test BPLib_STOR_CACHE_Module_custody_update_tracking_block");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_find_bundle_match, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_find_bundle_match");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_process_bundle, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_process_bundle");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle, NULL, NULL,
               "Test BPLib_STOR_CACHE_Module_custody_process_remote_dacs_bundle");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_init_info_from_pblock, NULL, NULL,
               "Test BPLib_STOR_CACHE_Module_custody_init_info_from_pblock");
    UtTest_Add(test_BPLib_STOR_CACHE_Module_custody_ack_tracking_block, NULL, NULL, "Test BPLib_STOR_CACHE_Module_custody_ack_tracking_block");
}

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
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"

#include "test_BPLib_STOR_CACHE_StorQm.h"

void Test_BPLib_STOR_CACHE_CustodyFinalizeDacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyFinalizeDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Entry_t *store_entry)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_NodeIsMember), UT_cache_bool_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyFinalizeDacs(&state, &store_entry));
}

void Test_BPLib_STOR_CACHE_CustodyCheckDacs(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_CustodyCheckDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Block_t *qblk)
     */
    BPLib_STOR_CACHE_State_t            state;
    BPLib_STOR_CACHE_Block_t            qblk;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_BblockCanonical_t c_block;
    BPLib_MEM_RBT_Link_t               custody_rbt_link;
    BPLib_STOR_CACHE_Block_t            blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&qblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_MEM_RBT_Link_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_sizet_Handler, NULL);
    pri_block.data.logical.controlFlags.isAdminRecord = true;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    c_block.canonical_logical_data.data.custody_accept_payload_block.num_entries = 1;
    UtAssert_BOOL_TRUE(BPLib_STOR_CACHE_CustodyCheckDacs(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyStoreBundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyStoreBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_Block_t *qblk)
     */
    BPLib_STOR_CACHE_State_t            state;
    BPLib_STOR_CACHE_Block_t            qblk;
    BPLib_STOR_CACHE_Block_t            qblk1;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_MEM_RBT_Link_t               custody_rbt_link;
    BPLib_STOR_CACHE_Entry_t            store_entry;
    BPLib_STOR_CACHE_BblockCanonical_t custody_block;
    BPLib_STOR_PS_OffloadApi_t      offload_api;
    BPLib_STOR_CACHE_Block_t            sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&qblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&qblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_MEM_RBT_Link_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&custody_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&offload_api, 0, sizeof(BPLib_STOR_PS_OffloadApi_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetParentPoolFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyStoreBundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefFromBlock), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyStoreBundle(&state, &qblk));

    pri_block.data.delivery.committed_storage_id = 1;

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, &qblk1);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyStoreBundle(&state, &qblk));

    offload_api.offload                     = Test_BPLib_STOR_CACHE_OffloadStub;
    state.offload_api                       = &offload_api;
    pri_block.data.delivery.delivery_policy =BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyStoreBundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyStoreBundle(&state, &qblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyInsertTrackingBlock(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyInsertTrackingBlock(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * *pri_block, BPLib_STOR_CACHE_CustodianInfo_t *custody_info)
     */
    BPLib_STOR_CACHE_State_t            state;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_CustodianInfo_t   custody_info;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t custody_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&custody_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &custody_block);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyInsertTrackingBlock(&state, &pri_block, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyFindDacsMatch(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_CustodyFindDacsMatch(const BPLib_MEM_RBT_Link_t *node, void *arg)
     */
    BPLib_STOR_CACHE_CustodianInfo_t custody_info;
    BPLib_STOR_CACHE_Entry_t          store_entry;

    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    store_entry.state = BPLib_STOR_CACHE_EntryStateGenerateDacs;

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_CustodyFindDacsMatch(&store_entry.hash_rbt_link, &custody_info), 0);
}

void Test_BPLib_STOR_CACHE_CustodyFindPendingDacs(void)
{
    /* Test function for:
     * bool BPLib_STOR_CACHE_CustodyFindPendingDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *dacs_info)
     */
    BPLib_STOR_CACHE_State_t          state;
    BPLib_STOR_CACHE_CustodianInfo_t dacs_info;
    BPLib_MEM_RBT_Link_t             custody_rbt_link;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&dacs_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&custody_rbt_link, 0, sizeof(BPLib_MEM_RBT_Link_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, &custody_rbt_link);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyFindPendingDacs(&state, &dacs_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_SearchGeneric), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyCreateDacs(void)
{
    /* Test function for:
     * BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_CustodyCreateDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * **pri_block_out, BPLIB_CT_AcceptPayloadBlock_t **pay_out)
     */
    BPLib_STOR_CACHE_State_t                state;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block;
    BPLib_STOR_CACHE_BblockPrimary_t      *pri_block_out = &pri_block;
    BPLIB_CT_AcceptPayloadBlock_t  pay;
    BPLIB_CT_AcceptPayloadBlock_t *pay_out = &pay;
    BPLib_STOR_CACHE_Block_t                blk;
    BPLib_STOR_CACHE_BblockCanonical_t     c_block;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block1;
    BPLib_STOR_CACHE_BblockPrimary_t       pri_block2;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&pay, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    memset(&pri_block1, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&pri_block2, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_cache_AltHandler_PointerReturn, &pri_block1);
    UtAssert_NULL(BPLib_STOR_CACHE_CustodyCreateDacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block2);
    UtAssert_NULL(BPLib_STOR_CACHE_CustodyCreateDacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UtAssert_NULL(BPLib_STOR_CACHE_CustodyCreateDacs(&state, &pri_block_out, &pay_out));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyOpenDacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyOpenDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *custody_info)
     */
    BPLib_STOR_CACHE_State_t               state;
    BPLib_STOR_CACHE_CustodianInfo_t      custody_info;
    BPLib_STOR_CACHE_Block_t               blk;
    BPLib_STOR_CACHE_Entry_t               store_entry;
    BPLib_STOR_CACHE_Ref_t                 ref;
    BPLib_STOR_CACHE_BblockPrimary_t      pri_block;
    BPLib_STOR_CACHE_BblockPrimaryData_t data;
    BPLib_STOR_CACHE_BblockCanonical_t    c_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&data, 0, sizeof(BPLib_STOR_CACHE_BblockPrimaryData_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));
    pri_block.data = data;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyOpenDacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefDuplicate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyOpenDacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyAppendDacs(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyAppendDacs(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t *custody_info)
     */
    BPLib_STOR_CACHE_State_t               state;
    BPLib_STOR_CACHE_CustodianInfo_t      custody_info;
    BPLib_STOR_CACHE_Entry_t               store_entry;
    BPLIB_CT_AcceptPayloadBlock_t payload_ref;
    BPLib_STOR_CACHE_Block_t               blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&payload_ref, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    payload_ref.num_entries           = BPLIB_DACS_MAX_SEQ_PER_PAYLOAD;
    store_entry.data.dacs.payload_ref = &payload_ref;
    custody_info.store_entry          = &store_entry;

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyAppendDacs(&state, &custody_info));

    custody_info.sequence_num = 1;
    payload_ref.num_entries   = 2;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyAppendDacs(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyUpdateTrackingBlock(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyUpdateTrackingBlock(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_CustodianInfo_t
     * *custody_info)
     */
    BPLib_STOR_CACHE_State_t            state;
    BPLib_STOR_CACHE_CustodianInfo_t   custody_info;
    BPLib_STOR_CACHE_BblockCanonical_t c_block;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&c_block, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &c_block);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyUpdateTrackingBlock(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyFindBundleMatch(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_CustodyFindBundleMatch(const BPLib_MEM_RBT_Link_t *node, void *arg)
     */
    BPLib_STOR_CACHE_CustodianInfo_t custody_info;
    BPLib_STOR_CACHE_Entry_t          store_entry;

    memset(&store_entry, 0, sizeof(store_entry));
    custody_info.sequence_num  = 1;
    store_entry.duct_seq_copy = 1;

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyFindBundleMatch(&store_entry.hash_rbt_link, &custody_info));
}

void Test_BPLib_STOR_CACHE_CustodyProcessBundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyProcessBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t *pri_block,
     * BPLib_STOR_CACHE_CustodianInfo_t *custody_info)
     */
    BPLib_STOR_CACHE_State_t          state;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;
    BPLib_STOR_CACHE_CustodianInfo_t custody_info;
    BPLib_STOR_CACHE_Block_t          blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    custody_info.this_cblk = &blk;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyProcessBundle(&state, &pri_block, &custody_info));

    custody_info.final_dest_node = 1;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyProcessBundle(&state, &pri_block, &custody_info));
}

void Test_BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle(BPLib_STOR_CACHE_State_t *state, BPLib_STOR_CACHE_BblockPrimary_t
     * *pri_block, const BPLIB_CT_AcceptPayloadBlock_t *ack_payload)
     */
    BPLib_STOR_CACHE_State_t               state;
    BPLib_STOR_CACHE_BblockPrimary_t      pri_block;
    BPLIB_CT_AcceptPayloadBlock_t ack_payload;
    BPLib_STOR_CACHE_Block_t               blk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&ack_payload, 0, sizeof(BPLIB_CT_AcceptPayloadBlock_t));
    ack_payload.num_entries = 1;
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &blk);

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle(&state, &pri_block, &ack_payload));

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyInitInfoFromPblock(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyInitInfoFromPblock(BPLib_STOR_CACHE_CustodianInfo_t *custody_info,
     * BPLib_STOR_CACHE_BblockPrimary_t *pri_block)
     */
    BPLib_STOR_CACHE_CustodianInfo_t   custody_info;
    BPLib_STOR_CACHE_BblockPrimary_t   pri_block;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t cblk;

    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_AltHandler_PointerReturn, &blk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &cblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyInitInfoFromPblock(&custody_info, &pri_block));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryLocateCanonical), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_CustodyAckTrackingBlock(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_CustodyAckTrackingBlock(BPLib_STOR_CACHE_State_t *state,const BPLib_STOR_CACHE_CustodianInfo_t
     * *custody_info)
     */
    BPLib_STOR_CACHE_State_t            state;
    BPLib_STOR_CACHE_CustodianInfo_t   custody_info;
    BPLib_STOR_CACHE_Block_t            blk;
    BPLib_STOR_CACHE_BblockCanonical_t cblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&custody_info, 0, sizeof(BPLib_STOR_CACHE_CustodianInfo_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    custody_info.prev_cblk = &blk;
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_BblockCanonical_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, &blk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_CustodyAckTrackingBlock(&state, &custody_info));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockCanonicalCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BplibCacheCustody_Register(void)
{
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyFinalizeDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyFinalizeDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyCheckDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyCheckDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyStoreBundle, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyStoreBundle");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyInsertTrackingBlock, NULL, NULL,
               "Test BPLib_STOR_CACHE_CustodyInsertTrackingBlock");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyFindDacsMatch, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyFindDacsMatch");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyFindPendingDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyFindPendingDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyCreateDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyCreateDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyOpenDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyOpenDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyAppendDacs, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyAppendDacs");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyUpdateTrackingBlock, NULL, NULL,
               "Test BPLib_STOR_CACHE_CustodyUpdateTrackingBlock");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyFindBundleMatch, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyFindBundleMatch");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyProcessBundle, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyProcessBundle");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle, NULL, NULL,
               "Test BPLib_STOR_CACHE_CustodyProcessRemoteDacsBundle");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyInitInfoFromPblock, NULL, NULL,
               "Test BPLib_STOR_CACHE_CustodyInitInfoFromPblock");
    UtTest_Add(Test_BPLib_STOR_CACHE_CustodyAckTrackingBlock, NULL, NULL, "Test BPLib_STOR_CACHE_CustodyAckTrackingBlock");
}

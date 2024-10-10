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

#include "test_bplib_stor_cache.h"
#include "bplib_stor_qm.h"
#include "bplib_stor_cache_block.h"

static void BPLib_STOR_QM_GetTimeHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t retval = 1000;
    UT_Stub_SetReturnValue(FuncKey, retval);
}

void test_BPLib_STOR_QM_QueueIngressToParent(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_QueueIngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    subq_src.parent_offset = 20;
    subq_src.type          = BPLib_STOR_CACHE_BlocktypeDuct;
    subq_src.next          = calloc(1, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetBlockFromLink), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_QueueIngressToParent(NULL, &subq_src), 0);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_QueueIngressToParent(NULL, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_IngressQueueSingleBundle(void)
{
    /* Test function for:
     * void BPLib_STOR_QM_IngressQueueSingleBundle(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_STOR_CACHE_Block_t *pblk)
     */
    BPLib_STOR_QM_QueueTbl_t             tbl;
    BPLib_STOR_QM_QueueEntry_t           queue_entry;
    BPLib_STOR_CACHE_Block_t          pblk;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&queue_entry, 0, sizeof(BPLib_STOR_QM_QueueEntry_t));
    tbl.queue_tbl          = &queue_entry;
    tbl.queue_tbl->intf_id = BPLIB_HANDLE_FLASH_STORE_BASE;
    memset(&pblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    pri_block.data.delivery.delivery_policy =BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking;
    tbl.registered_queues                   = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UtAssert_VOIDCALL(BPLib_STOR_QM_IngressQueueSingleBundle((BPLib_STOR_QM_QueueTbl_t *)&tbl, &pblk));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_IngressBaseintfForwarder(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_IngressBaseintfForwarder(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_IngressBaseintfForwarder(&rtbl, NULL), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_baseintf_AltHandler_PointerReturn, &subq_src);
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_IngressBaseintfForwarder(&rtbl, NULL), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_RegisterGenericIntf(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_QM_RegisterGenericIntf(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t parent_intf_id,
     * BPLib_STOR_CACHE_Block_t *duct_block)
     */
    BPLib_STOR_QM_QueueTbl_t    tbl;
    bp_handle_t         parent_intf_id;
    BPLib_STOR_CACHE_Block_t duct_block;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&parent_intf_id, 0, sizeof(bp_handle_t));
    parent_intf_id.hdl = 101;
    memset(&duct_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    duct_block.parent_offset = 20;
    duct_block.type          = BPLib_STOR_CACHE_BlocktypeDuct;

    UtAssert_UINT32_EQ(BPLib_STOR_QM_RegisterGenericIntf(&tbl, parent_intf_id, &duct_block).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct_block);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_RegisterGenericIntf(&tbl, parent_intf_id, &duct_block).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_RegisterForwardIngressHandler(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_RegisterForwardIngressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id,
     * BPLib_STOR_CACHE_CallbackFunc_t ingress)
     */
    BPLib_STOR_QM_QueueTbl_t tbl;
    bp_handle_t      intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t ingress = NULL;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_RegisterForwardIngressHandler(&tbl, intf_id, ingress), 0);
}

void test_BPLib_STOR_QM_RegisterForwardEgressHandler(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_RegisterForwardEgressHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id,
     * BPLib_STOR_CACHE_CallbackFunc_t ingress)
     */
    BPLib_STOR_QM_QueueTbl_t            tbl;
    bp_handle_t                 intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t egress = NULL;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_RegisterForwardEgressHandler(&tbl, intf_id, egress), 0);
}

void test_BPLib_STOR_QM_PushEgressBundle(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_PushEgressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
     */
    BPLib_STOR_QM_QueueTbl_t     tbl;
    bp_handle_t          intf_id;
    BPLib_STOR_CACHE_Block_t *cb = NULL;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_PushEgressBundle(&tbl, intf_id, cb), 0);
}

void test_BPLib_STOR_QM_MaintenanceRequestWait(void)
{
    /* Test function for:
     * void BPLib_STOR_QM_MaintenanceRequestWait(BPLib_STOR_QM_QueueTbl_t *tbl)
     */
    BPLib_STOR_QM_QueueTbl_t rtbl;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    rtbl.last_intf_poll = 0;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsGetDtntimeMs), BPLib_STOR_QM_GetTimeHandler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_QM_MaintenanceRequestWait(&rtbl));
}

void test_BPLib_STOR_QM_RegisterEventHandler(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_RegisterEventHandler(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_CallbackFunc_t
     * event)
     */
    BPLib_STOR_QM_QueueTbl_t            tbl;
    bp_handle_t                 intf_id;
    BPLib_STOR_CACHE_CallbackFunc_t event = NULL;
    BPLib_STOR_QM_Duct_t          duct;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_RegisterEventHandler(&tbl, intf_id, event), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_RegisterEventHandler(&tbl, intf_id, event), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_DelIntf(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_DelIntf(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t   tbl;
    BPLib_STOR_QM_QueueEntry_t queue_entry;
    bp_handle_t        intf_id;
    BPLib_STOR_CACHE_Ref_t  ref;
    BPLib_STOR_QM_Duct_t ifp;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&queue_entry, 0, sizeof(BPLib_STOR_QM_QueueEntry_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&ifp, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_DelIntf((BPLib_STOR_QM_QueueTbl_t *)&tbl, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &ifp);
    tbl.registered_queues = 1;
    tbl.queue_tbl         = &queue_entry;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_DelIntf((BPLib_STOR_QM_QueueTbl_t *)&tbl, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_GetNextAvailIntf(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_QM_GetNextAvailIntf(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest)
     */
    BPLib_STOR_QM_QueueTbl_t tbl;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    bp_ipn_t dest = 101;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_GetNextAvailIntf(&tbl, dest).hdl, 0);
}

void test_BPLib_STOR_QM_PushIngressBundle(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_PushIngressBundle(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, BPLib_STOR_CACHE_Block_t *cb)
     */
    BPLib_STOR_QM_QueueTbl_t     tbl;
    bp_handle_t          intf_id;
    BPLib_STOR_CACHE_Block_t *cb = NULL;
    BPLib_STOR_QM_Duct_t   duct_block;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&duct_block, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct_block);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_PushIngressBundle(&tbl, intf_id, cb), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_Add(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_Add(BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t   rtbl;
    BPLib_STOR_QM_QueueEntry_t queue_entry;
    BPLib_STOR_CACHE_Pool_t      pool;
    bp_ipn_t           dest = 101;
    bp_ipn_t           mask = 100;
    bp_handle_t        intf_id;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&queue_entry, 0, sizeof(BPLib_STOR_QM_QueueEntry_t));
    rtbl.queue_tbl = &queue_entry;
    memset(&pool, 0, sizeof(BPLib_STOR_CACHE_Pool_t));
    rtbl.pool = &pool;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsCalloc), UT_lib_AltHandler_PointerReturn, &rtbl);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, &pool);
    memset(&intf_id, 0, sizeof(bp_handle_t));

    rtbl.registered_queues = 20;
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_Add((BPLib_STOR_QM_QueueTbl_t *)&rtbl, dest, mask, intf_id), 0);

    rtbl.registered_queues = 1;
    rtbl.max_queues        = 100;
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_Add((BPLib_STOR_QM_QueueTbl_t *)&rtbl, dest, mask, intf_id), 0);

    mask             = 0;
    queue_entry.mask = 0;
    queue_entry.dest = 101;
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_Add((BPLib_STOR_QM_QueueTbl_t *)&rtbl, dest, mask, intf_id), 0);

    queue_entry.mask       = 1;
    rtbl.registered_queues = 2;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_Add((BPLib_STOR_QM_QueueTbl_t *)&rtbl, dest, mask, intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsCalloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_Del(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_Del(BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, bp_ipn_t mask, bp_handle_t intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t   tbl;
    BPLib_STOR_QM_QueueEntry_t queue_entry;
    bp_ipn_t           dest = 101;
    bp_ipn_t           mask = 100;
    bp_handle_t        intf_id;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&queue_entry, 0, sizeof(BPLib_STOR_QM_QueueEntry_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_QM_Del((BPLib_STOR_QM_QueueTbl_t *)&tbl, dest, mask, intf_id), 0);
    tbl.registered_queues = 1;
    tbl.queue_tbl         = &queue_entry;
    tbl.queue_tbl->dest   = 101;
    tbl.queue_tbl->mask   = 100;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_Del((BPLib_STOR_QM_QueueTbl_t *)&tbl, dest, mask, intf_id), 0);
}

void test_BPLib_STOR_QM_GetNextIntfWithFlags(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_QM_GetNextIntfWithFlags(const BPLib_STOR_QM_QueueTbl_t *tbl, bp_ipn_t dest, uint32_t req_flags,
     * uint32_t flag_mask)
     */
    BPLib_STOR_QM_QueueTbl_t   tbl;
    BPLib_STOR_QM_QueueEntry_t queue_entry;
    bp_ipn_t           dest      = 101;
    uint32_t           req_flags = 0;
    uint32_t           flag_mask = 0;
    BPLib_STOR_QM_Duct_t duct_block;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&queue_entry, 0, sizeof(BPLib_STOR_QM_QueueEntry_t));
    tbl.queue_tbl = &queue_entry;
    memset(&duct_block, 0, sizeof(BPLib_STOR_QM_Duct_t));

    tbl.registered_queues = 1;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_GetNextIntfWithFlags((BPLib_STOR_QM_QueueTbl_t *)&tbl, dest, req_flags, flag_mask).hdl,
                       0);

    flag_mask = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct_block);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_GetNextIntfWithFlags((BPLib_STOR_QM_QueueTbl_t *)&tbl, dest, req_flags, flag_mask).hdl,
                       0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_IntfSetFlags(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_IntfSetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
     */
    BPLib_STOR_QM_QueueTbl_t  tbl;
    bp_handle_t       intf_id = BP_INVALID_HANDLE;
    uint32_t          flags   = 0;
    BPLib_STOR_CACHE_Ref_t ref;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctModifyFlags), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_IntfSetFlags(&tbl, intf_id, flags), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_IntfUnsetFlags(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_IntfUnsetFlags(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id, uint32_t flags)
     */
    BPLib_STOR_QM_QueueTbl_t  tbl;
    bp_handle_t       intf_id = BP_INVALID_HANDLE;
    uint32_t          flags   = 0;
    BPLib_STOR_CACHE_Ref_t ref;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctModifyFlags), UT_lib_bool_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_IntfUnsetFlags(&tbl, intf_id, flags), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_QM_MaintenanceCompleteWait(void)
{
    /* Test function for:
     * void BPLib_STOR_QM_MaintenanceCompleteWait(BPLib_STOR_QM_QueueTbl_t *tbl)
     */
    BPLib_STOR_QM_QueueTbl_t rtbl;
    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    UtAssert_VOIDCALL(BPLib_STOR_QM_MaintenanceCompleteWait(&rtbl));
}

void test_BPLib_STOR_QM_PeriodicMaintenance(void)
{
    /* Test function for:
     * void BPLib_STOR_QM_PeriodicMaintenance(BPLib_STOR_QM_QueueTbl_t *tbl)
     */
    BPLib_STOR_QM_QueueTbl_t rtbl;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsGetDtntimeMs), BPLib_STOR_QM_GetTimeHandler, NULL);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BP_ERROR);
    UtAssert_VOIDCALL(BPLib_STOR_QM_PeriodicMaintenance((BPLib_STOR_QM_QueueTbl_t *)&rtbl));

    rtbl.poll_count = 1;
    UtAssert_VOIDCALL(BPLib_STOR_QM_PeriodicMaintenance((BPLib_STOR_QM_QueueTbl_t *)&rtbl));
}

void test_BPLib_STOR_QM_AllocTable(void)
{
    /* Test function for:
     * BPLib_STOR_QM_QueueTbl_t *BPLib_STOR_QM_AllocTable(uint32_t max_queues, size_t cache_mem_size)
     */
    uint32_t         max_queues     = 0;
    size_t           cache_mem_size = 1000;
    BPLib_STOR_QM_QueueTbl_t tbl;
    BPLib_STOR_CACHE_Pool_t    pool;

    memset(&tbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&pool, 0, sizeof(BPLib_STOR_CACHE_Pool_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsCalloc), UT_lib_sizet_Handler, NULL);
    UtAssert_NULL(BPLib_STOR_QM_AllocTable(max_queues, cache_mem_size));

    max_queues = 1;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsCalloc), UT_lib_AltHandler_PointerReturn, &tbl);
    UtAssert_NULL(BPLib_STOR_QM_AllocTable(max_queues, cache_mem_size));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, &pool);
    UtAssert_NOT_NULL(BPLib_STOR_QM_AllocTable(max_queues, cache_mem_size));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_OsCalloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_Create), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BplibCache_Routing_Register(void)
{
    UtTest_Add(test_BPLib_STOR_QM_QueueIngressToParent, NULL, NULL, "Test BPLib_STOR_QM_QueueIngressToParent");
    UtTest_Add(test_BPLib_STOR_QM_IngressQueueSingleBundle, NULL, NULL,
               "Test BPLib_STOR_QM_IngressQueueSingleBundle");
    UtTest_Add(test_BPLib_STOR_QM_IngressBaseintfForwarder, NULL, NULL, "Test BPLib_STOR_QM_IngressBaseintfForwarder");
    UtTest_Add(test_BPLib_STOR_QM_RegisterForwardIngressHandler, NULL, NULL,
               "Test BPLib_STOR_QM_RegisterForwardIngressHandler");
    UtTest_Add(test_BPLib_STOR_QM_RegisterForwardEgressHandler, NULL, NULL,
               "Test BPLib_STOR_QM_RegisterForwardEgressHandler");
    UtTest_Add(test_BPLib_STOR_QM_RegisterGenericIntf, NULL, NULL, "Test BPLib_STOR_QM_RegisterGenericIntf");
    UtTest_Add(test_BPLib_STOR_QM_RegisterEventHandler, NULL, NULL, "Test BPLib_STOR_QM_RegisterEventHandler");
    UtTest_Add(test_BPLib_STOR_QM_DelIntf, NULL, NULL, "Test BPLib_STOR_QM_DelIntf");
    UtTest_Add(test_BPLib_STOR_QM_GetNextAvailIntf, NULL, NULL, "Test BPLib_STOR_QM_GetNextAvailIntf");
    UtTest_Add(test_BPLib_STOR_QM_PushIngressBundle, NULL, NULL, "Test BPLib_STOR_QM_PushIngressBundle");
    UtTest_Add(test_BPLib_STOR_QM_IntfSetFlags, NULL, NULL, "Test BPLib_STOR_QM_IntfSetFlags");
    UtTest_Add(test_BPLib_STOR_QM_IntfUnsetFlags, NULL, NULL, "Test BPLib_STOR_QM_IntfUnsetFlags");
    UtTest_Add(test_BPLib_STOR_QM_Add, NULL, NULL, "Test BPLib_STOR_QM_Add");
    UtTest_Add(test_BPLib_STOR_QM_Del, NULL, NULL, "Test BPLib_STOR_QM_Del");
    UtTest_Add(test_BPLib_STOR_QM_GetNextIntfWithFlags, NULL, NULL, "Test BPLib_STOR_QM_GetNextIntfWithFlags");
    UtTest_Add(test_BPLib_STOR_QM_PushEgressBundle, NULL, NULL, "Test BPLib_STOR_QM_PushEgressBundle");
    UtTest_Add(test_BPLib_STOR_QM_MaintenanceCompleteWait, NULL, NULL, "Test BPLib_STOR_QM_MaintenanceCompleteWait");
    UtTest_Add(test_BPLib_STOR_QM_PeriodicMaintenance, NULL, NULL, "Test BPLib_STOR_QM_PeriodicMaintenance");
    UtTest_Add(test_BPLib_STOR_QM_MaintenanceRequestWait, NULL, NULL, "Test BPLib_STOR_QM_MaintenanceRequestWait");
    UtTest_Add(test_BPLib_STOR_QM_AllocTable, NULL, NULL, "Test BPLib_STOR_QM_AllocTable");
}

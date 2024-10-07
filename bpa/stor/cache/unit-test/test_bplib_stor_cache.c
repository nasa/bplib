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

#include "test_bplib_stor_cache.h"
#include "utilities/bplib_stor_cache_utils.h"

#ifdef QM
#include "bplib_stor_cache_ducts.h"
#endif // QM

void test_BPLib_STOR_CACHE_EntryMakePending(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_EntryMakePending(BPLib_STOR_CACHE_Entry_t *store_entry, uint32_t set_flags, uint32_t clear_flags)
     */
    BPLib_STOR_CACHE_Entry_t store_entry;
    uint32_t            set_flags   = 1;
    uint32_t            clear_flags = 0;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_EntryMakePending(&store_entry, set_flags, clear_flags));
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

#ifdef QM
void test_BPLib_STOR_CACHE_Attach(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    BPLib_STOR_QM_QueueTbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_State_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetMpool), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctAlloc), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctAlloc), UT_cache_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceAttach), UT_cache_valid_bphandle_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_Attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Detach(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    BPLib_STOR_QM_QueueTbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    BPLib_STOR_CACHE_Ref_t   duct_block_ref;
    BPLib_STOR_CACHE_State_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    service_addr.node_number    = 100;
    service_addr.service_number = 101;
    memset(&duct_block_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceDetach), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_Detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceDetach), UT_cache_AltHandler_PointerReturn, &duct_block_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceDetach), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_RegisterModuleService(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t cache_intf_id, const
     * BPLib_STOR_CACHE_ModuleApi_t *api, void *init_arg)
     */
    BPLib_STOR_QM_QueueTbl_t        *tbl           = NULL;
    bp_handle_t              cache_intf_id = BP_INVALID_HANDLE;
    BPLib_STOR_CACHE_ModuleApi_t api;
    void                    *init_arg = NULL;
    BPLib_STOR_CACHE_State_t      state;
    BPLib_STOR_CACHE_Ref_t        parent_ref;
    BPLib_STOR_CACHE_Block_t      cblk;

    memset(&api, 0, sizeof(BPLib_STOR_CACHE_ModuleApi_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&parent_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_RegisterModuleService(tbl, cache_intf_id, NULL, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &parent_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, &cblk);
    api.instantiate = test_BPLib_STOR_CACHE_InstantiateStub;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_RegisterModuleService(tbl, cache_intf_id, &api, init_arg).hdl, 0);
    api.module_type = BPLib_STOR_CACHE_TypeOffload;
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_RegisterModuleService(tbl, cache_intf_id, &api, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Configure(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Configure(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key,
     * BPLib_STOR_CACHE_Valtype_t vt, const void *val)
     */
    BPLib_STOR_QM_QueueTbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    BPLib_STOR_CACHE_Valtype_t vt             = BPLib_STOR_CACHE_ValtypeInteger;
    void                        *val            = NULL;
    BPLib_STOR_CACHE_State_t          state;
    BPLib_STOR_CACHE_Block_t          blk;
    BPLib_STOR_CACHE_OffloadApi_t    api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.configure = test_BPLib_STOR_CACHE_ConfigureStub;

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Configure(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Query(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Query(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_Valtype_t
     * vt, const void **val)
     */
    BPLib_STOR_QM_QueueTbl_t            *tbl            = NULL;
    bp_handle_t                  module_intf_id = BP_INVALID_HANDLE;
    int                          key            = 0;
    BPLib_STOR_CACHE_Valtype_t vt             = BPLib_STOR_CACHE_ValtypeInteger;
    void                        *val            = NULL;
    BPLib_STOR_CACHE_State_t          state;
    BPLib_STOR_CACHE_Block_t          blk;
    BPLib_STOR_CACHE_OffloadApi_t    api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));

    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.query     = test_BPLib_STOR_CACHE_QueryStub;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Query(tbl, module_intf_id, key, vt, val), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Start(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Start(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    BPLib_STOR_CACHE_State_t       state;
    BPLib_STOR_CACHE_Block_t       blk;
    BPLib_STOR_CACHE_OffloadApi_t api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.start     = test_BPLib_STOR_CACHE_StartstopStub;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Start(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_Stop(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_Stop(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t         *tbl = NULL;
    bp_handle_t               module_intf_id;
    BPLib_STOR_CACHE_State_t       state;
    BPLib_STOR_CACHE_Block_t       blk;
    BPLib_STOR_CACHE_OffloadApi_t api;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&blk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&api, 0, sizeof(BPLib_STOR_CACHE_OffloadApi_t));
    state.offload_blk = &blk;
    state.offload_api = &api;
    api.std.stop      = test_BPLib_STOR_CACHE_StartstopStub;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_Stop(tbl, module_intf_id), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_DebugScanQueue(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_DebugScanQueue(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id)
     */
    BPLib_STOR_QM_QueueTbl_t   *tbl = NULL;
    bp_handle_t         intf_id;
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Ref_t   duct_block_ref;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct_block_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetIntfControlblock), UT_cache_sizet_Handler, NULL);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_DebugScanQueue(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetIntfControlblock), UT_cache_AltHandler_PointerReturn,
                          &duct_block_ref);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_DebugScanQueue(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_DebugScanQueue(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetIntfControlblock), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_EgressImpl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_EgressImpl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    void               *arg = NULL;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Duct_t  duct;

    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_CACHE_Duct_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctTryPull), UT_cache_egress_AltHandler_PointerReturn, &subq_src);
    subq_src.type = BPLib_STOR_CACHE_BlocktypeRef;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctTryPull), UT_cache_sizet_Handler, NULL);
}

void test_BPLib_STOR_CACHE_FlushPending(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FlushPending(BPLib_STOR_CACHE_State_t *state)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Duct_t  duct;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_CACHE_Duct_t));
    duct.ingress.current_depth_limit = 2;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BPLIB_ERROR);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlushPending(&state));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
}
#endif // QM

void test_BPLib_STOR_CACHE_DoPoll(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DoPoll(BPLib_STOR_CACHE_State_t *state)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_IterGotoMax), BPLIB_ERROR);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DoPoll(&state), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

#ifdef QM
void test_BPLib_STOR_CACHE_DoRouteUp(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DoRouteUp(BPLib_STOR_CACHE_State_t *state, bp_ipn_t dest, bp_ipn_t mask)
     */
    BPLib_STOR_CACHE_State_t state;
    bp_ipn_t            dest;
    bp_ipn_t            mask = 0;
    BPLib_STOR_CACHE_Block_t sblk;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    dest = 1;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_IterGotoMin), BPLIB_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_GetKeyValue), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_GetKeyValue), UT_cache_uint64_Handler, NULL);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DoRouteUp(&state, dest, mask), 0);
}

void test_BPLib_STOR_CACHE_DoIntfStatechange(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DoIntfStatechange(BPLib_STOR_CACHE_State_t *state, bool is_up)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_CACHE_Duct_t  duct;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_CACHE_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DoIntfStatechange(&state, false), 0);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DoIntfStatechange(&state, true), 0);

    duct.current_state_flags = 8;
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DoIntfStatechange(&state, true), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_EventImpl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_EventImpl(void *event_arg, BPLib_STOR_CACHE_Block_t *intf_block)
     */
    BPLib_STOR_CACHE_DuctGenericEvent_t event_arg;
    BPLib_STOR_CACHE_Block_t              intf_block;
    BPLib_STOR_CACHE_State_t              state;
    BPLib_STOR_CACHE_Duct_t               duct;

    memset(&event_arg, 0, sizeof(BPLib_STOR_CACHE_DuctGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_CACHE_Duct_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EventImpl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    event_arg.event_type = BPLib_STOR_CACHE_DuctEventPoll;
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_IterGotoMax), BPLIB_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BPLIB_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterGotoFirst), BPLIB_ERROR);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_EventImpl(&event_arg, &intf_block), 0);

    event_arg.event_type         = BPLib_STOR_CACHE_DuctEventUp;
    event_arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_EventImpl(&event_arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_ProcessPending(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ProcessPending(void *arg, BPLib_STOR_CACHE_Block_t *job)
     */
    BPLib_STOR_CACHE_Block_t job;
    BPLib_STOR_CACHE_Duct_t  duct;

    memset(&job, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct, 0, sizeof(BPLib_STOR_CACHE_Duct_t));

    duct.ingress.current_depth_limit = 2;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BPLIB_ERROR);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ProcessPending(NULL, &job), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
}
#endif // QM

void test_BPLib_STOR_CACHE_DestructState(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DestructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Block_t sblk1;
    BPLib_STOR_CACHE_State_t state;
    BPLib_MEM_RBT_Root_t    index;
    BPLib_MEM_RBT_Link_t    rlink;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&index, 0, sizeof(BPLib_MEM_RBT_Root_t));
    memset(&rlink, 0, sizeof(BPLib_MEM_RBT_Link_t));
    index.root              = &rlink;
    state.idle_list         = sblk1;
    state.pending_list      = sblk1;
    state.idle_list.next    = &state.idle_list;
    state.pending_list.next = &state.pending_list;

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_DestructState(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_TreeIsEmpty), UT_cache_bool_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DestructState(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_ConstructEntry(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ConstructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Entry_t store_entry;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConstructEntry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ConstructEntry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_DestructEntry(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DestructEntry(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Entry_t store_entry;
    BPLib_STOR_CACHE_State_t state;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_DestructEntry(NULL, &sblk), 0);

    store_entry.parent = &state;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &store_entry);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_MEM_RBT_GetKeyValue), 1);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_RBT_GetKeyValue), UT_cache_uint64_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DestructEntry(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_ConstructBlockref(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ConstructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t    sblk;
    BPLib_STOR_CACHE_Blockref_t blockref;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blockref, 0, sizeof(BPLib_STOR_CACHE_Blockref_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConstructBlockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &blockref);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ConstructBlockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_DestructBlockref(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_DestructBlockref(void *arg, BPLib_STOR_CACHE_Block_t *rblk)
     */
    BPLib_STOR_CACHE_Block_t    sblk;
    BPLib_STOR_CACHE_Block_t    sblk1;
    BPLib_STOR_CACHE_Blockref_t blockref;
    BPLib_STOR_CACHE_Entry_t    store_entry;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&sblk1, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&blockref, 0, sizeof(BPLib_STOR_CACHE_Blockref_t));
    memset(&store_entry, 0, sizeof(BPLib_STOR_CACHE_Entry_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_DestructBlockref(NULL, &sblk), 0);

    blockref.storage_entry = &store_entry;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &blockref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, &sblk1);

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_DestructBlockref(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataUncast), UT_cache_AltHandler_PointerReturn, NULL);
}

void test_BPLib_STOR_CACHE_ConstructState(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ConstructState(void *arg, BPLib_STOR_CACHE_Block_t *sblk)
     */
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_State_t state;

    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_ConstructState(NULL, &sblk), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_ConstructState(NULL, &sblk), 0);
}

void test_BPLib_STOR_CACHE_EntryTreeInsertUnsorted(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_EntryTreeInsertUnsorted(const BPLib_MEM_RBT_Link_t *node, void *arg)
     */
    BPLib_MEM_RBT_Link_t node;

    memset(&node, 0, sizeof(BPLib_MEM_RBT_Link_t));

    UtAssert_VOIDCALL(BPLib_STOR_CACHE_EntryTreeInsertUnsorted(&node, NULL));
}

void Test_BplibStorCache_Register(void)
{
    ADD_TEST(Test_BPLib_STOR_CACHE_BblockPrimaryAlloc);
    // UtTest_Add(test_BPLib_STOR_CACHE_EntryMakePending, NULL, NULL, "Test BPLib_STOR_CACHE_EntryMakePending");
    // UtTest_Add(test_BPLib_STOR_CACHE_Attach, NULL, NULL, "Test BPLib_STOR_CACHE_Attach");
    // UtTest_Add(test_BPLib_STOR_CACHE_Detach, NULL, NULL, "Test BPLib_STOR_CACHE_Detach");
    // UtTest_Add(test_BPLib_STOR_CACHE_RegisterModuleService, NULL, NULL, "Test BPLib_STOR_CACHE_RegisterModuleService");
    // UtTest_Add(test_BPLib_STOR_CACHE_Configure, NULL, NULL, "Test BPLib_STOR_CACHE_Configure");
    // UtTest_Add(test_BPLib_STOR_CACHE_Query, NULL, NULL, "Test BPLib_STOR_CACHE_Query");
    // UtTest_Add(test_BPLib_STOR_CACHE_Start, NULL, NULL, "Test BPLib_STOR_CACHE_Start");
    // UtTest_Add(test_BPLib_STOR_CACHE_Stop, NULL, NULL, "Test BPLib_STOR_CACHE_Stop");
    // UtTest_Add(test_BPLib_STOR_CACHE_DebugScanQueue, NULL, NULL, "Test BPLib_STOR_CACHE_DebugScanQueue");
    // UtTest_Add(test_BPLib_STOR_CACHE_EgressImpl, NULL, NULL, "Test BPLib_STOR_CACHE_EgressImpl");
    // UtTest_Add(test_BPLib_STOR_CACHE_FlushPending, NULL, NULL, "Test BPLib_STOR_CACHE_FlushPending");
    // UtTest_Add(test_BPLib_STOR_CACHE_DoPoll, NULL, NULL, "Test BPLib_STOR_CACHE_DoPoll");
    // UtTest_Add(test_BPLib_STOR_CACHE_DoRouteUp, NULL, NULL, "Test BPLib_STOR_CACHE_DoRouteUp");
    // UtTest_Add(test_BPLib_STOR_CACHE_DoIntfStatechange, NULL, NULL, "Test BPLib_STOR_CACHE_DoIntfStatechange");
    // UtTest_Add(test_BPLib_STOR_CACHE_EventImpl, NULL, NULL, "Test BPLib_STOR_CACHE_EventImpl");
    // UtTest_Add(test_BPLib_STOR_CACHE_ProcessPending, NULL, NULL, "Test BPLib_STOR_CACHE_ProcessPending");
    // UtTest_Add(test_BPLib_STOR_CACHE_DestructState, NULL, NULL, "Test BPLib_STOR_CACHE_DestructState");
    // UtTest_Add(test_BPLib_STOR_CACHE_ConstructEntry, NULL, NULL, "Test BPLib_STOR_CACHE_ConstructEntry");
    // UtTest_Add(test_BPLib_STOR_CACHE_DestructEntry, NULL, NULL, "Test BPLib_STOR_CACHE_DestructEntry");
    // UtTest_Add(test_BPLib_STOR_CACHE_ConstructBlockref, NULL, NULL, "Test BPLib_STOR_CACHE_ConstructBlockref");
    // UtTest_Add(test_BPLib_STOR_CACHE_DestructBlockref, NULL, NULL, "Test BPLib_STOR_CACHE_DestructBlockref");
    // UtTest_Add(test_BPLib_STOR_CACHE_ConstructState, NULL, NULL, "Test BPLib_STOR_CACHE_ConstructState");
    // UtTest_Add(test_BPLib_STOR_CACHE_EntryTreeInsertUnsorted, NULL, NULL, "Test BPLib_STOR_CACHE_EntryTreeInsertUnsorted");
}

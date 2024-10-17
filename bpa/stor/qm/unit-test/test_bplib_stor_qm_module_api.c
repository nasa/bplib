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

#include "bplib_stor_cache_types.h"
#include "../../cache/src/bplib_stor_cache_base_internal.h"

#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_ref.h"

#include "bplib_stor_qm.h"

#include "bplib_stor_cache_internal.h"
#include "test_bplib_stor_qm.h"

void Test_BPLib_STOR_QM_Attach(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_QM_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
     */
    BPLib_STOR_QM_QueueTbl_t   *tbl = NULL;
    bp_ipn_addr_t       service_addr;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_State_t state;

    memset(&service_addr, 0, sizeof(bp_ipn_addr_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetQtblPool), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_cache_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_Attach(tbl, &service_addr).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_sizet_Handler, NULL);

    // TODO Fix the usage of UT_SetHandlerFunction with non-NULL UserObj.
    UtAssert_UINT32_EQ(BPLib_STOR_QM_Attach(tbl, &service_addr).hdl, 0);

    // UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceAttach), UT_cache_valid_bphandle_Handler, NULL);
    // UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_QM_Attach(tbl, &service_addr).hdl, 0);


    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_QM_Detach(void)
{
    /* Test function for:
     * int BPLib_STOR_QM_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr)
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
    UtAssert_UINT32_GT(BPLib_STOR_QM_Detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceDetach), UT_cache_AltHandler_PointerReturn, &duct_block_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_EQ(BPLib_STOR_QM_Detach(tbl, &service_addr), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_DataserviceDetach), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_QM_RegisterModuleService(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_QM_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t cache_intf_id, const
     * BPLib_STOR_QM_ModuleApi_t *api, void *init_arg)
     */
    BPLib_STOR_QM_QueueTbl_t        *tbl           = NULL;
    bp_handle_t                      cache_intf_id = BP_INVALID_HANDLE;
    BPLib_STOR_QM_ModuleApi_t api;
    void                            *init_arg = NULL;
    BPLib_STOR_CACHE_State_t         state;
    BPLib_STOR_CACHE_Ref_t           parent_ref;
    BPLib_STOR_CACHE_Block_t         cblk;

    memset(&api, 0, sizeof(BPLib_STOR_QM_ModuleApi_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&parent_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&cblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));

    UtAssert_UINT32_EQ(BPLib_STOR_QM_RegisterModuleService(tbl, cache_intf_id, NULL, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, &parent_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, &cblk);
    api.instantiate = Test_BPLib_STOR_QM_InstantiateStub;
    UtAssert_UINT32_EQ(BPLib_STOR_QM_RegisterModuleService(tbl, cache_intf_id, &api, init_arg).hdl, 0);
    api.module_type = BPLib_STOR_QM_ModuleTypeOffload;
    UtAssert_UINT32_GT(BPLib_STOR_QM_RegisterModuleService(tbl, cache_intf_id, &api, init_arg).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_DebugScanQueue(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_DebugScanQueue(void *tbl, bp_handle_t intf_id)
     */
    void   *tbl = NULL;
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

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_DebugScanQueue(tbl, intf_id));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_GetIntfControlblock), UT_cache_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
}

void Test_BPLib_STOR_CACHE_EgressImpl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_EgressImpl(void *arg, BPLib_STOR_CACHE_Block_t *subq_src)
     */
    void               *arg = NULL;
    BPLib_STOR_CACHE_Block_t subq_src;
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&subq_src, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, &state);
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    #ifdef QM_DUCT
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    #endif // QM_DUCT
    // TODO Avoid reaching into QM?
    // TODO     UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_cache_egress_AltHandler_PointerReturn, &subq_src);
    subq_src.type = BPLib_STOR_CACHE_BlocktypeRef;
    UtAssert_UINT32_NEQ(BPLib_STOR_CACHE_EgressImpl(arg, &subq_src), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_MEM_GenericDataCast), UT_cache_AltHandler_PointerReturn, NULL);
    #ifdef QM_DUCT
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
    #endif // QM_DUCT
    // TODO Avoid reaching into QM.
    // TODO     UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_cache_sizet_Handler, NULL);
}

void Test_BPLib_STOR_CACHE_FlushPending(void)
{
    /* Test function for:
     * void BPLib_STOR_CACHE_FlushPending(BPLib_STOR_CACHE_State_t *state)
     */
    BPLib_STOR_CACHE_State_t state;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&state, 0, sizeof(BPLib_STOR_CACHE_State_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));
    duct.ingress.current_depth_limit = 2;

    #ifdef QM_DUCT
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_cache_AltHandler_PointerReturn, &duct);
    #endif // QM_DUCT

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_ListIterForward), BPLIB_ERROR);
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_FlushPending(&state));

    #ifdef QM_DUCT
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_cache_AltHandler_PointerReturn, NULL);
    #endif // QM_DUCT
}



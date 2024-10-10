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

#include "test_bplib_stor_qm.h"
#include "bplib_stor_qm.h"

#include "bplib_stor_cache_block.h"

void test BPLib_STOR_CACHE_CreateClaIntf(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_CreateClaIntf(BPLib_STOR_QM_QueueTbl_t *rtbl)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    BPLib_STOR_CACHE_Block_t sblk;
    bp_handle_t         self_intf_id;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&self_intf_id, 0, sizeof(bp_handle_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_CreateClaIntf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_CreateClaIntf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_CreateClaIntf(&rtbl).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_ClaIngress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ClaIngress(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, const void *bundle, size_t size, uint32_t
     * timeout)
     */
    BPLib_STOR_QM_QueueTbl_t  rtbl;
    bp_handle_t       intf_id;
    void             *bundle  = NULL;
    size_t            size    = 100;
    uint64_t          timeout = 3000;
    BPLib_STOR_CACHE_Ref_t duct_ref;
   BPLib_STOR_CACHE_ClaStats_t stats;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&stats, 0, sizeof BPLib_STOR_CACHE_ClaStats_t));

    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_ClaIngress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GetParentPoolFromLink), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_ClaIngress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaIngress(&rtbl, intf_id, bundle, size, timeout), 0);

    timeout = 0;
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaIngress(&rtbl, intf_id, bundle, size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_ClaEgress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ClaEgress(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, void *bundle, size_t *size, uint32_t timeout)
     */
    BPLib_STOR_QM_QueueTbl_t  rtbl;
    bp_handle_t       intf_id;
    void             *bundle  = NULL;
    size_t            size    = 100;
    uint64_t          timeout = 0;
    BPLib_STOR_CACHE_Ref_t duct_ref;
   BPLib_STOR_CACHE_ClaStats_t stats;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&intf_id, 0, sizeof(bp_handle_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&stats, 0, sizeof BPLib_STOR_CACHE_ClaStats_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_ClaEgress(&rtbl, intf_id, bundle, &size, timeout), 0);

    timeout = 3000;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_ClaEgress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &stats);
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaEgress(&rtbl, intf_id, bundle, &size, timeout), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_ClaEventImpl(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ClaEventImpl(void *arg, BPLib_STOR_CACHE_Block_t *intf_block)
     */
    BPLib_STOR_QM_DuctGenericEvent_t arg;
    BPLib_STOR_CACHE_Block_t              intf_block;
    BPLib_STOR_QM_Duct_t               duct;

    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaEventImpl(&arg, &intf_block), 0);

    memset(&arg, 0, sizeof(BPLib_STOR_QM_DuctGenericEvent_t));
    memset(&intf_block, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    arg.event_type = BPLib_STOR_QM_DuctEventUp;
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaEventImpl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    arg.intf_state.intf_id = BPLIB_HANDLE_MPOOL_BASE;
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaEventImpl(&arg, &intf_block), 0);

    arg.event_type = BPLib_STOR_QM_DuctEventDown;
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_ClaEventImpl(&arg, &intf_block), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_GenericBundleIngress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_GenericBundleIngress(BPLib_STOR_CACHE_Ref_t duct_ref, const void *content, size_t size, uint64_t
     * time_limit)
     */
    BPLib_STOR_CACHE_BlockContent_t  duct_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    BPLib_STOR_QM_Duct_t           duct;
    BPLib_STOR_CACHE_Block_t          pblk;
    BPLib_STOR_CACHE_Ref_t            refptr;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));
    memset(&pblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UtAssert_INT32_EQ(BPLib_STOR_CACHE_GenericBundleIngress(&duct_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_GenericBundleIngress(&duct_ref, content, size, time_limit), BP_ERROR);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPush), UT_lib_int8_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, &pblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, &refptr);
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_GenericBundleIngress(&duct_ref, content, size, time_limit), BP_ERROR);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_GenericBundleIngress(&duct_ref, content, size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefMakeBlock), UT_lib_AltHandler_PointerReturn, NULL);
}

void test BPLib_STOR_CACHE_GenericBundleEgress(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_GenericBundleEgress(BPLib_STOR_CACHE_Ref_t duct_ref, void *content, size_t *size, uint64_t time_limit)
     */
    BPLib_STOR_CACHE_BlockContent_t  duct_ref;
    void                        *content    = NULL;
    size_t                       size       = 1;
    uint64_t                     time_limit = 0;
    BPLib_STOR_QM_Duct_t           duct;
    BPLib_STOR_CACHE_Block_t          pblk;
    BPLib_STOR_CACHE_Ref_t            refptr;
    BPLib_STOR_CACHE_BblockPrimary_t pri_block;

    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_BlockContent_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));
    memset(&pblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&refptr, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&pri_block, 0, sizeof(BPLib_STOR_CACHE_BblockPrimary_t));

    UtAssert_INT32_EQ(BPLib_STOR_CACHE_GenericBundleEgress(&duct_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_compute_full_bundle_size), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_out), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_GenericBundleEgress(&duct_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(v7_copy_full_bundle_in), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, &pblk);
    UtAssert_INT32_NEQ(BPLib_STOR_CACHE_GenericBundleEgress(&duct_ref, content, &size, time_limit), 0);

    size = 0;
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, &pri_block);
    UtAssert_INT32_EQ(BPLib_STOR_CACHE_GenericBundleEgress(&duct_ref, content, &size, time_limit), 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctTryPull), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BblockPrimaryCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void Test_BplibCache_ClaApi_Register(void)
{
    UtTest_Add(test BPLib_STOR_CACHE_CreateClaIntf, NULL, NULL, "Test BPLib_STOR_CACHE_CreateClaIntf");
    UtTest_Add(test BPLib_STOR_CACHE_ClaIngress, NULL, NULL, "Test BPLib_STOR_CACHE_ClaIngress");
    UtTest_Add(test BPLib_STOR_CACHE_ClaEgress, NULL, NULL, "Test BPLib_STOR_CACHE_ClaEgress");
    UtTest_Add(test BPLib_STOR_CACHE_ClaEventImpl, NULL, NULL, "Test BPLib_STOR_CACHE_ClaEventImpl");
    UtTest_Add(test BPLib_STOR_CACHE_GenericBundleIngress, NULL, NULL, "Test BPLib_STOR_CACHE_GenericBundleIngress");
    UtTest_Add(test BPLib_STOR_CACHE_GenericBundleEgress, NULL, NULL, "Test BPLib_STOR_CACHE_GenericBundleEgress");
}

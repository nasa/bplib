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

/*
 * Includes
 */
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "bplib.h"
#include "bplib_file_offload.h"
#include "test_bplib_base.h"

#include "test_bplib_stor_cache.h"

#include "bplib_stor_qm.h"

const BPLib_STOR_CACHE_ModuleApi_t *BPLIB_FILE_OFFLOAD_API = NULL;

void test_BPLib_STOR_CACHE_CreateRamStorage(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_CreateRamStorage(BPLib_STOR_QM_QueueTbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
     */
    BPLib_STOR_QM_QueueTbl_t *rtbl = NULL;
    bp_ipn_addr_t     storage_addr;

    memset(&storage_addr, 0, sizeof(bp_ipn_addr_t));

    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_CreateRamStorage(rtbl, &storage_addr).hdl, 0);
}

void test_BPLib_STOR_CACHE_CreateFileStorage(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_CreateFileStorage(BPLib_STOR_QM_QueueTbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
     */
    BPLib_STOR_QM_QueueTbl_t *rtbl = NULL;
    bp_ipn_addr_t     storage_addr;

    memset(&storage_addr, 0, sizeof(bp_ipn_addr_t));

    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RegisterModuleService), 0x1);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_Attach), 0x1);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_CreateFileStorage(rtbl, &storage_addr).hdl, 0);
    UT_SetDefaultReturnValue(UT_KEY(BPLib_STOR_CACHE_RegisterModuleService), 0);
}

void test_BPLib_STOR_CACHE_CreateNodeIntf(void)
{
    /* Test function for:
     * bp_handle_t BPLib_STOR_CACHE_CreateNodeIntf(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_ipn_t node_num)
     */
    BPLib_STOR_QM_QueueTbl_t    rtbl;
    bp_ipn_t            node_num = 101;
    BPLib_STOR_CACHE_Block_t sblk;
    BPLib_STOR_CACHE_Ref_t   duct_ref;
    BPLib_STOR_QM_Duct_t  duct;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));
    memset(&sblk, 0, sizeof(BPLib_STOR_CACHE_Block_t));
    memset(&duct_ref, 0, sizeof(BPLib_STOR_CACHE_Ref_t));
    memset(&duct, 0, sizeof(BPLib_STOR_QM_Duct_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_CreateNodeIntf(&rtbl, node_num).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, &duct_ref);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, &duct);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_RefCreate), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_BlockFromExternalId), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_CreateNodeIntf(&rtbl, node_num).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctAlloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_GenericDataCast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_QM_DuctCast), UT_lib_AltHandler_PointerReturn, NULL);
}

void test_bplib_display(void)
{
    /* Test function for:
     * int bplib_display(const void *bundle, size_t size, uint32_t *flags)
     */
    int    bundle[] = {1, 2, 3, 4, 5, 6};
    size_t size     = 6;

    UtAssert_UINT32_EQ(bplib_display(bundle, size, 0), 0);
}

void test_bplib_eid2ipn(void)
{
    /* Test function for:
     * int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service)
     */
    const char *eid     = NULL;
    char        a[10]   = "ipn:101.11";
    char        b[10]   = "100.1";
    char        c[10]   = "ipn:100";
    char        d[10]   = "ipn:.11";
    size_t      len     = 15;
    bp_ipn_t    node    = 15;
    bp_ipn_t    service = 10;

    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    eid = b;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    eid = a;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    eid = d;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    len = 8;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    eid = c;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    len = 5;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
    len = 150;
    UtAssert_UINT32_EQ(bplib_eid2ipn(eid, len, &node, &service), 0);
}
void test_bplib_ipn2eid(void)
{
    /* Test function for:
     * int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service)
     */
    char    *eid     = NULL;
    char     a[10]   = "ipn:100.1";
    size_t   len     = sizeof(eid);
    bp_ipn_t node    = 15;
    bp_ipn_t service = 10;

    UtAssert_UINT32_EQ(bplib_ipn2eid(eid, len, node, service), 0);
    eid = a;
    UtAssert_UINT32_EQ(bplib_ipn2eid(eid, len, node, service), 0);
    len = 5;
    UtAssert_UINT32_EQ(bplib_ipn2eid(eid, len, node, service), 0);
    len = 150;
    UtAssert_UINT32_EQ(bplib_ipn2eid(eid, len, node, service), 0);
}

void test_BPLib_STOR_CACHE_QueryInteger(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_QueryInteger(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value)
     */
    BPLib_STOR_QM_QueueTbl_t rtbl;
    bp_handle_t      intf_id;
    intf_id.hdl     = 5;
    bp_sval_t value = 10;

    memset(&rtbl, 0, sizeof(BPLib_STOR_QM_QueueTbl_t));

    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_QueryMemCurrentUse), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(BPLib_STOR_CACHE_QueryMemMaxUse), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_QueryInteger(&rtbl, intf_id,BPLib_STOR_CACHE_VariableMemCurrentUse, &value), 0);
    UtAssert_UINT32_EQ(BPLib_STOR_CACHE_QueryInteger(&rtbl, intf_id,BPLib_STOR_CACHE_VariableMemHighUse, &value), 0);
    UtAssert_UINT32_GT(BPLib_STOR_CACHE_QueryInteger(&rtbl, intf_id, 5, &value), 0);
}

void test_BPLib_STOR_CACHE_ConfigInteger(void)
{
    /* Test function for:
     * int BPLib_STOR_CACHE_ConfigInteger(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value)
     */
    BPLib_STOR_QM_QueueTbl_t *rtbl = NULL;
    bp_handle_t       intf_id;

    memset(&intf_id, 0, sizeof(bp_handle_t));

    bplib_variable_t var_id =BPLib_STOR_CACHE_VariableMemCurrentUse;
    bp_sval_t        value  = 10;
    UtAssert_VOIDCALL(BPLib_STOR_CACHE_ConfigInteger(rtbl, intf_id, var_id, value));
}

void Test_BplibStorCache_Register(void)
{
    UtTest_Add(test_bplib_init, NULL, NULL, "Test bplib_init");
    UtTest_Add(test_bplib_deinit, NULL, NULL, "Test bplib_deinit");
    UtTest_Add(testBPLib_STOR_CACHE_CreateRamStorage, NULL, NULL, "Test_BPLib_STOR_CACHE_CreateRamStorage");
    UtTest_Add(testBPLib_STOR_CACHE_CreateFileStorage, NULL, NULL, "Test_BPLib_STOR_CACHE_CreateFileStorage");
    UtTest_Add(testBPLib_STOR_CACHE_CreateNodeIntf, NULL, NULL, "Test_BPLib_STOR_CACHE_CreateNodeIntf");
    UtTest_Add(test_bplib_display, NULL, NULL, "Test bplib_display");
    UtTest_Add(test_bplib_eid2ipn, NULL, NULL, "Test bplib_eid2ipn");
    UtTest_Add(test_bplib_ipn2eid, NULL, NULL, "Test bplib_ipn2eid");
    UtTest_Add(testBPLib_STOR_CACHE_QueryInteger, NULL, NULL, "Test_BPLib_STOR_CACHE_QueryInteger");
    UtTest_Add(testBPLib_STOR_CACHE_ConfigInteger, NULL, NULL, "Test_BPLib_STOR_CACHE_ConfigInteger");
}

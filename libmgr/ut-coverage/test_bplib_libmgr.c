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
#include "test_bplib_base.h"

const bplib_cache_module_api_t *BPLIB_FILE_OFFLOAD_API = NULL;

void test_bplib_init(void)
{
    /* Test function for:
     * int bplib_init(void)
     */
    UtAssert_INT32_EQ(bplib_init(), BP_SUCCESS);
}

void test_bplib_deinit(void)
{
    /* Test function for:
     * void bplib_deinit(void)
     */
    UtAssert_VOIDCALL(bplib_deinit());
}

#ifdef STOR
void test_bplib_create_ram_storage(void)
{
    /* Test function for:
     * bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
     */
    bplib_routetbl_t *rtbl = NULL;
    bp_ipn_addr_t     storage_addr;

    memset(&storage_addr, 0, sizeof(bp_ipn_addr_t));

    UtAssert_UINT32_EQ(bplib_create_ram_storage(rtbl, &storage_addr).hdl, 0);
}

void test_bplib_create_file_storage(void)
{
    /* Test function for:
     * bp_handle_t bplib_create_file_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
     */
    bplib_routetbl_t *rtbl = NULL;
    bp_ipn_addr_t     storage_addr;

    memset(&storage_addr, 0, sizeof(bp_ipn_addr_t));

    UT_SetDefaultReturnValue(UT_KEY(bplib_cache_register_module_service), 0x1);
    UT_SetDefaultReturnValue(UT_KEY(bplib_cache_attach), 0x1);
    UtAssert_UINT32_GT(bplib_create_file_storage(rtbl, &storage_addr).hdl, 0);
    UT_SetDefaultReturnValue(UT_KEY(bplib_cache_register_module_service), 0);
}

void test_bplib_create_node_intf(void)
{
    /* Test function for:
     * bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num)
     */
    bplib_routetbl_t    rtbl;
    bp_ipn_t            node_num = 101;
    bplib_mpool_block_t sblk;
    bplib_mpool_ref_t   flow_ref;
    bplib_mpool_flow_t  flow;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));
    memset(&sblk, 0, sizeof(bplib_mpool_block_t));
    memset(&flow_ref, 0, sizeof(bplib_mpool_ref_t));
    memset(&flow, 0, sizeof(bplib_mpool_flow_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_create_node_intf(&rtbl, node_num).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_AltHandler_PointerReturn, &sblk);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, &flow_ref);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, &flow);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_ref_create), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_block_from_external_id), UT_lib_AltHandler_PointerReturn, NULL);
    UtAssert_UINT32_GT(bplib_create_node_intf(&rtbl, node_num).hdl, 0);

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_alloc), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_generic_data_cast), UT_lib_AltHandler_PointerReturn, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_flow_cast), UT_lib_AltHandler_PointerReturn, NULL);
}
#endif // STOR

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

void test_bplib_query_integer(void)
{
    /* Test function for:
     * int bplib_query_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value)
     */
    bplib_routetbl_t rtbl;
    bp_handle_t      intf_id;
    intf_id.hdl     = 5;
    bp_sval_t value = 10;

    memset(&rtbl, 0, sizeof(bplib_routetbl_t));

    UT_SetHandlerFunction(UT_KEY(bplib_mpool_query_mem_current_use), UT_lib_sizet_Handler, NULL);
    UT_SetHandlerFunction(UT_KEY(bplib_mpool_query_mem_max_use), UT_lib_sizet_Handler, NULL);
    UtAssert_UINT32_EQ(bplib_query_integer(&rtbl, intf_id, bplib_variable_mem_current_use, &value), 0);
    UtAssert_UINT32_EQ(bplib_query_integer(&rtbl, intf_id, bplib_variable_mem_high_use, &value), 0);
    UtAssert_UINT32_GT(bplib_query_integer(&rtbl, intf_id, 5, &value), 0);
}

void test_bplib_config_integer(void)
{
    /* Test function for:
     * int bplib_config_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value)
     */
    bplib_routetbl_t *rtbl = NULL;
    bp_handle_t       intf_id;

    memset(&intf_id, 0, sizeof(bp_handle_t));

    bplib_variable_t var_id = bplib_variable_mem_current_use;
    bp_sval_t        value  = 10;
    UtAssert_VOIDCALL(bplib_config_integer(rtbl, intf_id, var_id, value));
}

void TestBplibBase_Register(void)
{
    UtTest_Add(test_bplib_init, NULL, NULL, "Test bplib_init");
    UtTest_Add(test_bplib_deinit, NULL, NULL, "Test bplib_deinit");
    #ifdef STOR
    UtTest_Add(test_bplib_create_ram_storage, NULL, NULL, "Test bplib_create_ram_storage");
    UtTest_Add(test_bplib_create_file_storage, NULL, NULL, "Test bplib_create_file_storage");
    UtTest_Add(test_bplib_create_node_intf, NULL, NULL, "Test bplib_create_node_intf");
    #endif // STOR
    UtTest_Add(test_bplib_display, NULL, NULL, "Test bplib_display");
    UtTest_Add(test_bplib_eid2ipn, NULL, NULL, "Test bplib_eid2ipn");
    UtTest_Add(test_bplib_ipn2eid, NULL, NULL, "Test bplib_ipn2eid");
    UtTest_Add(test_bplib_query_integer, NULL, NULL, "Test bplib_query_integer");
    UtTest_Add(test_bplib_config_integer, NULL, NULL, "Test bplib_config_integer");
}

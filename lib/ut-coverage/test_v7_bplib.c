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

const bplib_cache_module_api_t *BPLIB_FILE_OFFLOAD_API = NULL;

void test_bplib_init(void)
{
    /* int bplib_init(void) */
    UtAssert_INT32_EQ(bplib_init(), BP_SUCCESS);
}
void test_bplib_deinit(void)
{
    /* void bplib_deinit(void) */
    UtAssert_VOIDCALL(bplib_deinit());
}
void test_bplib_create_ram_storage(void)
{
    /* bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr) */
}
void test_bplib_create_file_storage(void)
{
    /* bp_handle_t bplib_create_file_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr) */
}
void test_bplib_create_node_intf(void)
{
    /* bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num) */
}
void test_bplib_display(void)
{
    /* int bplib_display(const void *bundle, size_t size, uint32_t *flags) */
}
void test_bplib_eid2ipn(void)
{
    /* int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service) */
}
void test_bplib_ipn2eid(void)
{
    /* int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service) */
}
void test_bplib_query_integer(void)
{
    /* int bplib_query_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value)
     */
}
void test_bplib_config_integer(void)
{
    /* int bplib_config_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value)
     */
}

void TestBplibBase_Register(void)
{
    UtTest_Add(test_bplib_init, NULL, NULL, "bplib_init");
    UtTest_Add(test_bplib_deinit, NULL, NULL, "bplib_deinit");
    UtTest_Add(test_bplib_create_ram_storage, NULL, NULL, "bplib_create_ram_storage");
    UtTest_Add(test_bplib_create_file_storage, NULL, NULL, "bplib_create_file_storage");
    UtTest_Add(test_bplib_create_node_intf, NULL, NULL, "bplib_create_node_intf");
    UtTest_Add(test_bplib_display, NULL, NULL, "bplib_display");
    UtTest_Add(test_bplib_eid2ipn, NULL, NULL, "bplib_eid2ipn");
    UtTest_Add(test_bplib_ipn2eid, NULL, NULL, "bplib_ipn2eid");
    UtTest_Add(test_bplib_query_integer, NULL, NULL, "bplib_query_integer");
    UtTest_Add(test_bplib_config_integer, NULL, NULL, "bplib_config_integer");
}

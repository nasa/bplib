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

#include "v7_mpool_flows.h"

void test_bplib_mpool_flow_cast(void)
{
    /* Test function for:
     * bplib_mpool_flow_t *bplib_mpool_flow_cast(bplib_mpool_block_t *cb)
     */
}

void test_bplib_mpool_flow_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_flow_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
     */
}

void test_bplib_mpool_flow_disable(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_flow_disable(bplib_mpool_subq_workitem_t *subq)
     */
}

void test_bplib_mpool_flow_enable(void)
{
    /* Test function for:
     * void     bplib_mpool_flow_enable(bplib_mpool_subq_workitem_t *subq, uint32_t depth_limit)
     */
}

void test_bplib_mpool_flow_try_push(void)
{
    /* Test function for:
     * bool bplib_mpool_flow_try_push(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_block_t *qblk, uint64_t
     * abs_timeout)
     */
}

void test_bplib_mpool_flow_try_move_all(void)
{
    /* Test function for:
     * uint32_t bplib_mpool_flow_try_move_all(bplib_mpool_subq_workitem_t *subq_dst, bplib_mpool_subq_workitem_t
     * *subq_src, uint64_t abs_timeout)
     */
}

void test_bplib_mpool_flow_try_pull(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_flow_try_pull(bplib_mpool_subq_workitem_t *subq_src, uint64_t abs_timeout)
     */
}

void test_bplib_mpool_flow_modify_flags(void)
{
    /* Test function for:
     * bool bplib_mpool_flow_modify_flags(bplib_mpool_block_t *cb, uint32_t set_bits, uint32_t clear_bits)
     */
}

void TestBplibMpoolFlows_Register(void)
{
    UtTest_Add(test_bplib_mpool_flow_cast, NULL, NULL, "bplib_mpool_flow_cast");
    UtTest_Add(test_bplib_mpool_flow_alloc, NULL, NULL, "bplib_mpool_flow_alloc");
    UtTest_Add(test_bplib_mpool_flow_disable, NULL, NULL, "bplib_mpool_flow_disable");
    UtTest_Add(test_bplib_mpool_flow_enable, NULL, NULL, "bplib_mpool_flow_enable");
    UtTest_Add(test_bplib_mpool_flow_try_push, NULL, NULL, "bplib_mpool_flow_try_push");
    UtTest_Add(test_bplib_mpool_flow_try_move_all, NULL, NULL, "bplib_mpool_flow_try_move_all");
    UtTest_Add(test_bplib_mpool_flow_try_pull, NULL, NULL, "bplib_mpool_flow_try_pull");
    UtTest_Add(test_bplib_mpool_flow_modify_flags, NULL, NULL, "bplib_mpool_flow_modify_flags");
}

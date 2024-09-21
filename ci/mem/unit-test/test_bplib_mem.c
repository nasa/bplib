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
 * Include
 */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "bplib_mem_test_utils.h"

#include "test_bplib_mem.h"

#include "bplib_mem.h"

const uint32 UT_TESTBLOCKTYPE_SIG = 0x5f33c01a;

/*
** Test function for
** int BPLib_MEM_Init()
*/
void Test_BPLib_MEM_Init(void)
{
    UtAssert_INT32_EQ(BPLib_MEM_Init(), BPLIB_SUCCESS);
}


void TestBplibMem_Register(void)
{
    UtTest_Add(Test_BPLib_MEM_Init, BPLib_MEM_Test_Setup, BPLib_MEM_Test_Teardown, "Test_BPLib_MEM_Init");
}

void UT_AltHandler_PointerReturnForSignature(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bp_val_t RefSig = UT_Hook_GetArgValueByName(Context, "search_key_value", bp_val_t);

    if (RefSig == UT_TESTBLOCKTYPE_SIG)
    {
        UserObj = NULL;
    }

    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void test_BPLib_MEM_BlockFromExternalId(void)
{
    /* Test function for:
     * bplib_mpool_block_t *test_BPLib_MEM_BlockFromExternalId(bplib_mpool_t *pool, bp_handle_t handle)
     */
    UT_BPLib_MEM_Buf_t   buf;
    bp_handle_t          id1;
    bp_handle_t          id2;

    memset(&buf, 0, sizeof(buf));

    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, BP_INVALID_HANDLE));

    #ifdef STOR // blocktype
    test_setup_mpblock(&buf.pool, &buf.pool.admin_block, bplib_mpool_blocktype_admin, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[0], bplib_mpool_blocktype_primary, 0);
    test_setup_mpblock(&buf.pool, &buf.blk[1], bplib_mpool_blocktype_primary, 0);
    #endif // STOR blocktype
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id1 = BPLib_MEM_GetExternalId(&buf.blk[0].header.base_link)));
    UtAssert_BOOL_TRUE(bp_handle_is_valid(id2 = BPLib_MEM_GetExternalId(&buf.blk[1].header.base_link)));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id1));
    UtAssert_NULL(BPLib_MEM_BlockFromExternalId(&buf.pool, id2));

    buf.pool.admin_block.u.admin.num_bufs_total = 3;
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id1), &buf.blk[0].header.base_link);
    UtAssert_ADDRESS_EQ(BPLib_MEM_BlockFromExternalId(&buf.pool, id2), &buf.blk[1].header.base_link);
}

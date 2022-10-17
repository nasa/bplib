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

#include "v7_mpool_bblocks.h"
#include "v7_mpool_internal.h"

void test_bplib_mpool_bblock_primary_cast(void)
{
    /* Test function for:
     * bplib_mpool_bblock_primary_t *bplib_mpool_bblock_primary_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_primary_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_primary_cast(cb));

    cb->type = bplib_mpool_blocktype_primary;
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_primary_cast(cb), &my_block.u);
}

void test_bplib_mpool_bblock_canonical_cast(void)
{
    /* Test function for:
     * bplib_mpool_bblock_canonical_t *bplib_mpool_bblock_canonical_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_canonical_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_canonical_cast(cb));

    cb->type = bplib_mpool_blocktype_canonical;
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_canonical_cast(cb), &my_block.u);
}

void test_bplib_mpool_bblock_cbor_cast(void)
{
    /* Test function for:
     * void *bplib_mpool_bblock_cbor_cast(bplib_mpool_block_t *cb);
     */
    bplib_mpool_block_content_t my_block;
    bplib_mpool_block_t        *cb = &my_block.header.base_link;

    UtAssert_NULL(bplib_mpool_bblock_cbor_cast(NULL));

    memset(&my_block, 0, sizeof(my_block));
    UtAssert_NULL(bplib_mpool_bblock_cbor_cast(cb));

    cb->type                               = bplib_mpool_blocktype_generic;
    my_block.header.content_type_signature = MPOOL_CACHE_CBOR_DATA_SIGNATURE;
    UtAssert_ADDRESS_EQ(bplib_mpool_bblock_cbor_cast(cb), &my_block.u);
}

void test_bplib_mpool_bblock_cbor_set_size(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_cbor_set_size(bplib_mpool_block_t *cb, size_t user_content_size);
     */
}
void test_bplib_mpool_bblock_primary_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_primary_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg,
     * uint8_t priority, uint64_t timeout);
     */
}
void test_bplib_mpool_bblock_canonical_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_canonical_alloc(bplib_mpool_t *pool, uint32_t magic_number, void
     * *init_arg);
     */
}
void test_bplib_mpool_bblock_cbor_alloc(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_cbor_alloc(bplib_mpool_t *pool);
     */
}
void test_bplib_mpool_bblock_cbor_append(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_cbor_append(bplib_mpool_block_t *head, bplib_mpool_block_t *blk);
     */
}
void test_bplib_mpool_bblock_primary_append(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_primary_append(bplib_mpool_bblock_primary_t *cpb, bplib_mpool_block_t *ccb);
     */
}
void test_bplib_mpool_bblock_primary_locate_canonical(void)
{
    /* Test function for:
     * bplib_mpool_block_t *bplib_mpool_bblock_primary_locate_canonical(bplib_mpool_bblock_primary_t *cpb,
     * bp_blocktype_t block_type);
     */
}
void test_bplib_mpool_bblock_primary_drop_encode(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_primary_drop_encode(bplib_mpool_bblock_primary_t *cpb);
     */
}
void test_bplib_mpool_bblock_canonical_drop_encode(void)
{
    /* Test function for:
     * void bplib_mpool_bblock_canonical_drop_encode(bplib_mpool_bblock_canonical_t *ccb);
     */
}
void test_bplib_mpool_bblock_cbor_export(void)
{
    /* Test function for:
     * size_t bplib_mpool_bblock_cbor_export(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t
     * seek_start, size_t max_count);
     */
}

void TestBplibMpoolBBlocks_Register(void)
{
    UtTest_Add(test_bplib_mpool_bblock_primary_cast, NULL, NULL, "bplib_mpool_bblock_primary_cast");
    UtTest_Add(test_bplib_mpool_bblock_canonical_cast, NULL, NULL, "bplib_mpool_bblock_canonical_cast");
    UtTest_Add(test_bplib_mpool_bblock_cbor_cast, NULL, NULL, "bplib_mpool_bblock_cbor_cast");
    UtTest_Add(test_bplib_mpool_bblock_cbor_set_size, NULL, NULL, "bplib_mpool_bblock_cbor_set_size");
    UtTest_Add(test_bplib_mpool_bblock_primary_alloc, NULL, NULL, "bplib_mpool_bblock_primary_alloc");
    UtTest_Add(test_bplib_mpool_bblock_canonical_alloc, NULL, NULL, "bplib_mpool_bblock_canonical_alloc");
    UtTest_Add(test_bplib_mpool_bblock_cbor_alloc, NULL, NULL, "bplib_mpool_bblock_cbor_alloc");
    UtTest_Add(test_bplib_mpool_bblock_cbor_append, NULL, NULL, "bplib_mpool_bblock_cbor_append");
    UtTest_Add(test_bplib_mpool_bblock_primary_append, NULL, NULL, "bplib_mpool_bblock_primary_append");
    UtTest_Add(test_bplib_mpool_bblock_primary_locate_canonical, NULL, NULL,
               "bplib_mpool_bblock_primary_locate_canonical");
    UtTest_Add(test_bplib_mpool_bblock_primary_drop_encode, NULL, NULL, "bplib_mpool_bblock_primary_drop_encode");
    UtTest_Add(test_bplib_mpool_bblock_canonical_drop_encode, NULL, NULL, "bplib_mpool_bblock_canonical_drop_encode");
    UtTest_Add(test_bplib_mpool_bblock_cbor_export, NULL, NULL, "bplib_mpool_bblock_cbor_export");
}

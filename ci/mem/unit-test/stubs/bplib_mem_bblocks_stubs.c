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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in v7_mpool_bblocks header
 */

#include "bplib_mem.h"
#include "bplib_mem_bundle.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_canonical_alloc()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_bblock_canonical_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_canonical_alloc, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_canonical_alloc, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_bblock_canonical_alloc, uint32_t, magic_number);
    UT_GenStub_AddParam(bplib_mpool_bblock_canonical_alloc, void *, init_arg);

    UT_GenStub_Execute(bplib_mpool_bblock_canonical_alloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_canonical_alloc, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_canonical_cast()
 * ----------------------------------------------------
 */
bplib_mpool_bblock_canonical_t *bplib_mpool_bblock_canonical_cast(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_canonical_cast, bplib_mpool_bblock_canonical_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_canonical_cast, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_bblock_canonical_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_canonical_cast, bplib_mpool_bblock_canonical_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_canonical_drop_encode()
 * ----------------------------------------------------
 */
void bplib_mpool_bblock_canonical_drop_encode(bplib_mpool_bblock_canonical_t *ccb)
{
    UT_GenStub_AddParam(bplib_mpool_bblock_canonical_drop_encode, bplib_mpool_bblock_canonical_t *, ccb);

    UT_GenStub_Execute(bplib_mpool_bblock_canonical_drop_encode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_cbor_alloc()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_bblock_cbor_alloc(bplib_mpool_t *pool)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_cbor_alloc, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_alloc, bplib_mpool_t *, pool);

    UT_GenStub_Execute(bplib_mpool_bblock_cbor_alloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_cbor_alloc, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_cbor_append()
 * ----------------------------------------------------
 */
void bplib_mpool_bblock_cbor_append(bplib_mpool_block_t *head, bplib_mpool_block_t *blk)
{
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_append, bplib_mpool_block_t *, head);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_append, bplib_mpool_block_t *, blk);

    UT_GenStub_Execute(bplib_mpool_bblock_cbor_append, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_cbor_cast()
 * ----------------------------------------------------
 */
void *bplib_mpool_bblock_cbor_cast(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_cbor_cast, void *);

    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_cast, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_bblock_cbor_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_cbor_cast, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_cbor_export()
 * ----------------------------------------------------
 */
size_t bplib_mpool_bblock_cbor_export(bplib_mpool_block_t *list, void *out_ptr, size_t max_out_size, size_t seek_start,
                                      size_t max_count)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_cbor_export, size_t);

    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_export, bplib_mpool_block_t *, list);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_export, void *, out_ptr);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_export, size_t, max_out_size);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_export, size_t, seek_start);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_export, size_t, max_count);

    UT_GenStub_Execute(bplib_mpool_bblock_cbor_export, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_cbor_export, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_cbor_set_size()
 * ----------------------------------------------------
 */
void bplib_mpool_bblock_cbor_set_size(bplib_mpool_block_t *cb, size_t user_content_size)
{
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_set_size, bplib_mpool_block_t *, cb);
    UT_GenStub_AddParam(bplib_mpool_bblock_cbor_set_size, size_t, user_content_size);

    UT_GenStub_Execute(bplib_mpool_bblock_cbor_set_size, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_primary_alloc()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_bblock_primary_alloc(bplib_mpool_t *pool, uint32_t magic_number, void *init_arg,
                                                      uint8_t priority, uint64_t timeout)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_primary_alloc, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_primary_alloc, bplib_mpool_t *, pool);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_alloc, uint32_t, magic_number);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_alloc, void *, init_arg);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_alloc, uint8_t, priority);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_alloc, uint64_t, timeout);

    UT_GenStub_Execute(bplib_mpool_bblock_primary_alloc, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_primary_alloc, bplib_mpool_block_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_primary_append()
 * ----------------------------------------------------
 */
void bplib_mpool_bblock_primary_append(bplib_mpool_bblock_primary_t *cpb, bplib_mpool_block_t *ccb)
{
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_append, bplib_mpool_bblock_primary_t *, cpb);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_append, bplib_mpool_block_t *, ccb);

    UT_GenStub_Execute(bplib_mpool_bblock_primary_append, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_primary_cast()
 * ----------------------------------------------------
 */
bplib_mpool_bblock_primary_t *bplib_mpool_bblock_primary_cast(bplib_mpool_block_t *cb)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_primary_cast, bplib_mpool_bblock_primary_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_primary_cast, bplib_mpool_block_t *, cb);

    UT_GenStub_Execute(bplib_mpool_bblock_primary_cast, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_primary_cast, bplib_mpool_bblock_primary_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_primary_drop_encode()
 * ----------------------------------------------------
 */
void bplib_mpool_bblock_primary_drop_encode(bplib_mpool_bblock_primary_t *cpb)
{
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_drop_encode, bplib_mpool_bblock_primary_t *, cpb);

    UT_GenStub_Execute(bplib_mpool_bblock_primary_drop_encode, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_mpool_bblock_primary_locate_canonical()
 * ----------------------------------------------------
 */
bplib_mpool_block_t *bplib_mpool_bblock_primary_locate_canonical(bplib_mpool_bblock_primary_t *cpb,
                                                                 bp_blocktype_t                block_type)
{
    UT_GenStub_SetupReturnBuffer(bplib_mpool_bblock_primary_locate_canonical, bplib_mpool_block_t *);

    UT_GenStub_AddParam(bplib_mpool_bblock_primary_locate_canonical, bplib_mpool_bblock_primary_t *, cpb);
    UT_GenStub_AddParam(bplib_mpool_bblock_primary_locate_canonical, bp_blocktype_t, block_type);

    UT_GenStub_Execute(bplib_mpool_bblock_primary_locate_canonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_mpool_bblock_primary_locate_canonical, bplib_mpool_block_t *);
}

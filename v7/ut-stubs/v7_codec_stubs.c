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
 * Auto-Generated stub implementations for functions defined in v7_codec header
 */

#include "v7_codec.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for v7_block_decode_canonical()
 * ----------------------------------------------------
 */
int v7_block_decode_canonical(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size,
                              bp_blocktype_t payload_block_hint)
{
    UT_GenStub_SetupReturnBuffer(v7_block_decode_canonical, int);

    UT_GenStub_AddParam(v7_block_decode_canonical, bplib_mpool_bblock_canonical_t *, ccb);
    UT_GenStub_AddParam(v7_block_decode_canonical, const void *, data_ptr);
    UT_GenStub_AddParam(v7_block_decode_canonical, size_t, data_size);
    UT_GenStub_AddParam(v7_block_decode_canonical, bp_blocktype_t, payload_block_hint);

    UT_GenStub_Execute(v7_block_decode_canonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_block_decode_canonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_block_decode_pri()
 * ----------------------------------------------------
 */
int v7_block_decode_pri(bplib_mpool_bblock_primary_t *cpb, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(v7_block_decode_pri, int);

    UT_GenStub_AddParam(v7_block_decode_pri, bplib_mpool_bblock_primary_t *, cpb);
    UT_GenStub_AddParam(v7_block_decode_pri, const void *, data_ptr);
    UT_GenStub_AddParam(v7_block_decode_pri, size_t, data_size);

    UT_GenStub_Execute(v7_block_decode_pri, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_block_decode_pri, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_block_encode_canonical()
 * ----------------------------------------------------
 */
int v7_block_encode_canonical(bplib_mpool_bblock_canonical_t *ccb)
{
    UT_GenStub_SetupReturnBuffer(v7_block_encode_canonical, int);

    UT_GenStub_AddParam(v7_block_encode_canonical, bplib_mpool_bblock_canonical_t *, ccb);

    UT_GenStub_Execute(v7_block_encode_canonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_block_encode_canonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_block_encode_pay()
 * ----------------------------------------------------
 */
int v7_block_encode_pay(bplib_mpool_bblock_canonical_t *ccb, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(v7_block_encode_pay, int);

    UT_GenStub_AddParam(v7_block_encode_pay, bplib_mpool_bblock_canonical_t *, ccb);
    UT_GenStub_AddParam(v7_block_encode_pay, const void *, data_ptr);
    UT_GenStub_AddParam(v7_block_encode_pay, size_t, data_size);

    UT_GenStub_Execute(v7_block_encode_pay, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_block_encode_pay, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_block_encode_pri()
 * ----------------------------------------------------
 */
int v7_block_encode_pri(bplib_mpool_bblock_primary_t *cpb)
{
    UT_GenStub_SetupReturnBuffer(v7_block_encode_pri, int);

    UT_GenStub_AddParam(v7_block_encode_pri, bplib_mpool_bblock_primary_t *, cpb);

    UT_GenStub_Execute(v7_block_encode_pri, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_block_encode_pri, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_compute_full_bundle_size()
 * ----------------------------------------------------
 */
size_t v7_compute_full_bundle_size(bplib_mpool_bblock_primary_t *cpb)
{
    UT_GenStub_SetupReturnBuffer(v7_compute_full_bundle_size, size_t);

    UT_GenStub_AddParam(v7_compute_full_bundle_size, bplib_mpool_bblock_primary_t *, cpb);

    UT_GenStub_Execute(v7_compute_full_bundle_size, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_compute_full_bundle_size, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_copy_full_bundle_in()
 * ----------------------------------------------------
 */
size_t v7_copy_full_bundle_in(bplib_mpool_bblock_primary_t *cpb, const void *buffer, size_t buf_sz)
{
    UT_GenStub_SetupReturnBuffer(v7_copy_full_bundle_in, size_t);

    UT_GenStub_AddParam(v7_copy_full_bundle_in, bplib_mpool_bblock_primary_t *, cpb);
    UT_GenStub_AddParam(v7_copy_full_bundle_in, const void *, buffer);
    UT_GenStub_AddParam(v7_copy_full_bundle_in, size_t, buf_sz);

    UT_GenStub_Execute(v7_copy_full_bundle_in, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_copy_full_bundle_in, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_copy_full_bundle_out()
 * ----------------------------------------------------
 */
size_t v7_copy_full_bundle_out(bplib_mpool_bblock_primary_t *cpb, void *buffer, size_t buf_sz)
{
    UT_GenStub_SetupReturnBuffer(v7_copy_full_bundle_out, size_t);

    UT_GenStub_AddParam(v7_copy_full_bundle_out, bplib_mpool_bblock_primary_t *, cpb);
    UT_GenStub_AddParam(v7_copy_full_bundle_out, void *, buffer);
    UT_GenStub_AddParam(v7_copy_full_bundle_out, size_t, buf_sz);

    UT_GenStub_Execute(v7_copy_full_bundle_out, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_copy_full_bundle_out, size_t);
}

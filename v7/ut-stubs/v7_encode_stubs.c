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
 * Auto-Generated stub implementations for functions defined in v7_encode header
 */

#include "v7_encode.h"
#include "utgenstub.h"

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

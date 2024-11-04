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
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm_encode header
 */

#include "bplib_stor_qm_encode.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for block_encode_canonical()
 * ----------------------------------------------------
 */
int block_encode_canonical(BPLib_STOR_CACHE_BblockCanonical_t *ccb)
{
    UT_GenStub_SetupReturnBuffer(block_encode_canonical, int);

    UT_GenStub_AddParam(block_encode_canonical, BPLib_STOR_CACHE_BblockCanonical_t *, ccb);

    UT_GenStub_Execute(block_encode_canonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(block_encode_canonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for block_encode_pay()
 * ----------------------------------------------------
 */
int block_encode_pay(BPLib_STOR_CACHE_BblockCanonical_t *ccb, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(block_encode_pay, int);

    UT_GenStub_AddParam(block_encode_pay, BPLib_STOR_CACHE_BblockCanonical_t *, ccb);
    UT_GenStub_AddParam(block_encode_pay, const void *, data_ptr);
    UT_GenStub_AddParam(block_encode_pay, size_t, data_size);

    UT_GenStub_Execute(block_encode_pay, Basic, NULL);

    return UT_GenStub_GetReturnValue(block_encode_pay, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for block_encode_pri()
 * ----------------------------------------------------
 */
int block_encode_pri(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    UT_GenStub_SetupReturnBuffer(block_encode_pri, int);

    UT_GenStub_AddParam(block_encode_pri, BPLib_STOR_CACHE_BblockPrimary_t *, cpb);

    UT_GenStub_Execute(block_encode_pri, Basic, NULL);

    return UT_GenStub_GetReturnValue(block_encode_pri, int);
}

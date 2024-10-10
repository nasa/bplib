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

#include "bplib_stor_qm_codec.h"
#include "utgenstub.h"
#include "bplib_stor_cache_block.h"


/*
 * ----------------------------------------------------
 * Generated stub function for v7_compute_full_bundle_size()
 * ----------------------------------------------------
 */
size_t v7_compute_full_bundle_size(BPLib_STOR_CACHE_BblockPrimary_t *cpb)
{
    UT_GenStub_SetupReturnBuffer(v7_compute_full_bundle_size, size_t);

    UT_GenStub_AddParam(v7_compute_full_bundle_size, BPLib_STOR_CACHE_BblockPrimary_t *, cpb);

    UT_GenStub_Execute(v7_compute_full_bundle_size, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_compute_full_bundle_size, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_copy_full_bundle_in()
 * ----------------------------------------------------
 */
size_t v7_copy_full_bundle_in(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *buffer, size_t buf_sz)
{
    UT_GenStub_SetupReturnBuffer(v7_copy_full_bundle_in, size_t);

    UT_GenStub_AddParam(v7_copy_full_bundle_in, BPLib_STOR_CACHE_BblockPrimary_t *, cpb);
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
size_t v7_copy_full_bundle_out(BPLib_STOR_CACHE_BblockPrimary_t *cpb, void *buffer, size_t buf_sz)
{
    UT_GenStub_SetupReturnBuffer(v7_copy_full_bundle_out, size_t);

    UT_GenStub_AddParam(v7_copy_full_bundle_out, BPLib_STOR_CACHE_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(v7_copy_full_bundle_out, void *, buffer);
    UT_GenStub_AddParam(v7_copy_full_bundle_out, size_t, buf_sz);

    UT_GenStub_Execute(v7_copy_full_bundle_out, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_copy_full_bundle_out, size_t);
}

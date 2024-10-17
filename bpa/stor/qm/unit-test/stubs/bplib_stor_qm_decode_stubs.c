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
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm_decode header
 */

#include "bplib_stor_qm_decode.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DecodeCanonical()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_DecodeCanonical(BPLib_STOR_CACHE_BblockCanonical_t *ccb, const void *data_ptr, size_t data_size,
                              bp_blocktype_t payload_block_hint)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DecodeCanonical, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_DecodeCanonical, BPLib_STOR_CACHE_BblockCanonical_t *, ccb);
    UT_GenStub_AddParam(BPLib_STOR_QM_DecodeCanonical, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_STOR_QM_DecodeCanonical, size_t, data_size);
    UT_GenStub_AddParam(BPLib_STOR_QM_DecodeCanonical, bp_blocktype_t, payload_block_hint);

    UT_GenStub_Execute(BPLib_STOR_QM_DecodeCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DecodeCanonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_DecodePrimary()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_DecodePrimary(BPLib_STOR_CACHE_BblockPrimary_t *cpb, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_DecodePrimary, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_DecodePrimary, BPLib_STOR_CACHE_BblockPrimary_t *, cpb);
    UT_GenStub_AddParam(BPLib_STOR_QM_DecodePrimary, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_STOR_QM_DecodePrimary, size_t, data_size);

    UT_GenStub_Execute(BPLib_STOR_QM_DecodePrimary, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_DecodePrimary, int);
}

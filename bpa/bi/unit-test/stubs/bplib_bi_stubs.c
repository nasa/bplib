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
 * Auto-Generated stub implementations for functions defined in bplib_bi header
 */

#include "bplib_bi.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_BlobCopyOut()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_BlobCopyOut(BPLib_Bundle_t *StoredBundle, void *OutputBuffer, size_t OutputBufferSize,
                                    size_t *NumBytesCopied)
{
    UT_GenStub_SetupReturnBuffer(BPLib_BI_BlobCopyOut, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_BI_BlobCopyOut, BPLib_Bundle_t *, StoredBundle);
    UT_GenStub_AddParam(BPLib_BI_BlobCopyOut, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_BI_BlobCopyOut, size_t, OutputBufferSize);
    UT_GenStub_AddParam(BPLib_BI_BlobCopyOut, size_t *, NumBytesCopied);

    UT_GenStub_Execute(BPLib_BI_BlobCopyOut, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_BI_BlobCopyOut, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_RecvCtrlMsg()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_RecvCtrlMsg(BPLib_CLA_CtrlMsg_t *CtrlMsg)
{
    UT_GenStub_SetupReturnBuffer(BPLib_BI_RecvCtrlMsg, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_BI_RecvCtrlMsg, BPLib_CLA_CtrlMsg_t *, CtrlMsg);

    UT_GenStub_Execute(BPLib_BI_RecvCtrlMsg, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_BI_RecvCtrlMsg, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_RecvFullBundleIn()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_RecvFullBundleIn(BPLib_Instance_t *Inst, const void *BundleIn, size_t Size, uint32_t ContId)
{
    UT_GenStub_SetupReturnBuffer(BPLib_BI_RecvFullBundleIn, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, BPLib_Instance_t *, Inst);
    UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, const void *, BundleIn);
    UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, size_t, Size);
    UT_GenStub_AddParam(BPLib_BI_RecvFullBundleIn, uint32_t, ContId);

    UT_GenStub_Execute(BPLib_BI_RecvFullBundleIn, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_BI_RecvFullBundleIn, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_BI_ValidateBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_BI_ValidateBundle(BPLib_Bundle_t *CandidateBundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_BI_ValidateBundle, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_BI_ValidateBundle, BPLib_Bundle_t *, CandidateBundle);

    UT_GenStub_Execute(BPLib_BI_ValidateBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_BI_ValidateBundle, BPLib_Status_t);
}

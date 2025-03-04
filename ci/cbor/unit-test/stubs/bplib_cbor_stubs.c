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
 * Auto-Generated stub implementations for functions defined in bplib_cbor header
 */

#include "bplib_cbor.h"
#include "utgenstub.h"


#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"
#include "qcbor/qcbor_spiffy_decode.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodeBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_DecodeBundle(const void *CandBundle, size_t CandBundleLen, BPLib_Bundle_t *bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodeBundle, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_DecodeBundle, const void *, CandBundle);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeBundle, size_t, CandBundleLen);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeBundle, BPLib_Bundle_t *, bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodeBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodeBundle, BPLib_Status_t);
}


/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeBundle()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_EncodeBundle(BPLib_Bundle_t *StoredBundle,
    void *OutputBuffer,
    size_t OutputBufferSize,
    size_t *NumBytesCopied)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodeBundle, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, BPLib_Bundle_t *, StoredBundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, size_t, OutputBufferSize);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, size_t *, NumBytesCopied);

    UT_GenStub_Execute(BPLib_CBOR_EncodeBundle, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodeBundle, BPLib_Status_t);
}

/*
** TODO: I don't think we need the stubs below anymore
*/
#if 0

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodePrimary()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_EncodePrimary(BPLib_Bundle_t *StoredBundle,
    void *OutputBuffer,
    size_t OutputBufferSize,
    size_t *NumBytesCopied)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodePrimary, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, BPLib_Bundle_t *, StoredBundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, size_t, OutputBufferSize);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, size_t *, NumBytesCopied);

    UT_GenStub_Execute(BPLib_CBOR_EncodePrimary, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodePrimary, BPLib_Status_t);
}


/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeExtensionBlock()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_EncodeExtensionBlock(BPLib_Bundle_t *StoredBundle,
    uint32_t ExtensionBlockIndex,
    void *OutputBuffer,
    size_t OutputBufferSize,
    size_t *NumBytesCopied)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodeExtensionBlock, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodeExtensionBlock, BPLib_Bundle_t *, StoredBundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeExtensionBlock, uint32_t, ExtensionBlockIndex);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeExtensionBlock, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeExtensionBlock, size_t, OutputBufferSize);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeExtensionBlock, size_t *, NumBytesCopied);

    UT_GenStub_Execute(BPLib_CBOR_EncodeExtensionBlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodeExtensionBlock, BPLib_Status_t);
}


/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodePayload()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t *StoredBundle,
    void *OutputBuffer,
    size_t OutputBufferSize,
    size_t *NumBytesCopied)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodePayload, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, BPLib_Bundle_t *, StoredBundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, void *, OutputBuffer);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, size_t, OutputBufferSize);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, size_t *, NumBytesCopied);

    UT_GenStub_Execute(BPLib_CBOR_EncodePayload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodePayload, BPLib_Status_t);
}

#endif

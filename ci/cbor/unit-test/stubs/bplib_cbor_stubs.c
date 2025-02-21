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

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodeAgeData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_DecodeAgeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_DecodeAgeData, QCBORDecodeContext *, DecodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeAgeData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodeAgeData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodeBundle()
 * ----------------------------------------------------
 */
void BPLib_CBOR_DecodeBundle(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_DecodeBundle, QCBORDecodeContext *, DecodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeBundle, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodeBundle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodeCanonical()
 * ----------------------------------------------------
 */
int BPLib_CBOR_DecodeCanonical(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodeCanonical, int);

    UT_GenStub_AddParam(BPLib_CBOR_DecodeCanonical, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeCanonical, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeCanonical, size_t, data_size);

    UT_GenStub_Execute(BPLib_CBOR_DecodeCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodeCanonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodeHopCountData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_DecodeHopCountData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_DecodeHopCountData, QCBORDecodeContext *, DecodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeHopCountData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodeHopCountData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodePayload()
 * ----------------------------------------------------
 */
int BPLib_CBOR_DecodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodePayload, int);

    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, size_t, data_size);

    UT_GenStub_Execute(BPLib_CBOR_DecodePayload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodePayload, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodePrevNodeData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_DecodePrevNodeData(QCBORDecodeContext *DecodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_DecodePrevNodeData, QCBORDecodeContext *, DecodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePrevNodeData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodePrevNodeData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_DecodePrimary()
 * ----------------------------------------------------
 */
void BPLib_CBOR_DecodePrimary(QCBORDecodeContext *DecodeCtx, PrimaryBlock_t *v)
{
    UT_GenStub_AddParam(BPLib_CBOR_DecodePrimary, QCBORDecodeContext *, DecodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePrimary, PrimaryBlock_t *, v);

    UT_GenStub_Execute(BPLib_CBOR_DecodePrimary, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeAgeData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodeAgeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodeAgeData, QCBOREncodeContext *, EncodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeAgeData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodeAgeData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeBundle()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodeBundle(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, QCBOREncodeContext *, EncodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeBundle, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodeBundle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeCRC()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodeCRC(QCBOREncodeContext *enc)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodeCRC, QCBOREncodeContext *, enc);

    UT_GenStub_Execute(BPLib_CBOR_EncodeCRC, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeCanonical()
 * ----------------------------------------------------
 */
int BPLib_CBOR_EncodeCanonical(BPLib_Bundle_t *Bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodeCanonical, int);

    UT_GenStub_AddParam(BPLib_CBOR_EncodeCanonical, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodeCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodeCanonical, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodeHopCountData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodeHopCountData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodeHopCountData, QCBOREncodeContext *, EncodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_EncodeHopCountData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodeHopCountData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodePayload()
 * ----------------------------------------------------
 */
int BPLib_CBOR_EncodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodePayload, int);

    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, size_t, data_size);

    UT_GenStub_Execute(BPLib_CBOR_EncodePayload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodePayload, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodePrevNodeData()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodePrevNodeData(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrevNodeData, QCBOREncodeContext *, EncodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrevNodeData, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodePrevNodeData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EncodePrimary()
 * ----------------------------------------------------
 */
void BPLib_CBOR_EncodePrimary(QCBOREncodeContext *EncodeCtx, BPLib_Bundle_t *Bundle)
{
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, QCBOREncodeContext *, EncodeCtx);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePrimary, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodePrimary, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_Init()
 * ----------------------------------------------------
 */
int BPLib_CBOR_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_Init, int);

    UT_GenStub_Execute(BPLib_CBOR_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_Init, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_ValidateCRC()
 * ----------------------------------------------------
 */
int BPLib_CBOR_ValidateCRC(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_ValidateCRC, int);

    UT_GenStub_Execute(BPLib_CBOR_ValidateCRC, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_ValidateCRC, int);
}

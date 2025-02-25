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
 * Generated stub function for BPLib_CBOR_CanonicalBlockParse()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_CanonicalBlockParse(QCBORDecodeContext *ctx)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_CanonicalBlockParse, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_CanonicalBlockParse, QCBORDecodeContext *, ctx);

    UT_GenStub_Execute(BPLib_CBOR_CanonicalBlockParse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_CanonicalBlockParse, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_CreationTimestampParse()
 * ----------------------------------------------------
 */
QCBORError BPLib_CBOR_CreationTimestampParse(QCBORDecodeContext *ctx, BPLib_CreationTimeStamp_t *CreationTimestamp)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_CreationTimestampParse, QCBORError);

    UT_GenStub_AddParam(BPLib_CBOR_CreationTimestampParse, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_CBOR_CreationTimestampParse, BPLib_CreationTimeStamp_t *, CreationTimestamp);

    UT_GenStub_Execute(BPLib_CBOR_CreationTimestampParse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_CreationTimestampParse, QCBORError);
}

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
 * Generated stub function for BPLib_CBOR_DecodeCanonical()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_DecodeCanonical(QCBORDecodeContext *ctx, BPLib_Bundle_t *bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodeCanonical, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_DecodeCanonical, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodeCanonical, BPLib_Bundle_t *, bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodeCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodeCanonical, BPLib_Status_t);
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
BPLib_Status_t BPLib_CBOR_DecodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodePayload, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePayload, size_t, data_size);

    UT_GenStub_Execute(BPLib_CBOR_DecodePayload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodePayload, BPLib_Status_t);
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
BPLib_Status_t BPLib_CBOR_DecodePrimary(QCBORDecodeContext *ctx, BPLib_Bundle_t *bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_DecodePrimary, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_DecodePrimary, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_CBOR_DecodePrimary, BPLib_Bundle_t *, bundle);

    UT_GenStub_Execute(BPLib_CBOR_DecodePrimary, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_DecodePrimary, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_EIDParse()
 * ----------------------------------------------------
 */
QCBORError BPLib_CBOR_EIDParse(QCBORDecodeContext *ctx, BPLib_EID_t *eid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EIDParse, QCBORError);

    UT_GenStub_AddParam(BPLib_CBOR_EIDParse, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_CBOR_EIDParse, BPLib_EID_t *, eid);

    UT_GenStub_Execute(BPLib_CBOR_EIDParse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EIDParse, QCBORError);
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
BPLib_Status_t BPLib_CBOR_EncodeCanonical(BPLib_Bundle_t *Bundle)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodeCanonical, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodeCanonical, BPLib_Bundle_t *, Bundle);

    UT_GenStub_Execute(BPLib_CBOR_EncodeCanonical, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodeCanonical, BPLib_Status_t);
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
BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t *Bundle, const void *data_ptr, size_t data_size)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_EncodePayload, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, BPLib_Bundle_t *, Bundle);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, const void *, data_ptr);
    UT_GenStub_AddParam(BPLib_CBOR_EncodePayload, size_t, data_size);

    UT_GenStub_Execute(BPLib_CBOR_EncodePayload, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_EncodePayload, BPLib_Status_t);
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
BPLib_Status_t BPLib_CBOR_Init(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_Init, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_CBOR_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_Init, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_PrimaryBlockParse()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_PrimaryBlockParse(QCBORDecodeContext *ctx)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_PrimaryBlockParse, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_CBOR_PrimaryBlockParse, QCBORDecodeContext *, ctx);

    UT_GenStub_Execute(BPLib_CBOR_PrimaryBlockParse, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_PrimaryBlockParse, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_CBOR_ValidateCRC()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_CBOR_ValidateCRC(void)
{
    UT_GenStub_SetupReturnBuffer(BPLib_CBOR_ValidateCRC, BPLib_Status_t);

    UT_GenStub_Execute(BPLib_CBOR_ValidateCRC, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_CBOR_ValidateCRC, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_CRCParserImpl()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_CRCParserImpl(QCBORDecodeContext *ctx, uint64_t *parsed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_CRCParserImpl, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_CRCParserImpl, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_QCBOR_CRCParserImpl, uint64_t *, parsed);

    UT_GenStub_Execute(BPLib_QCBOR_CRCParserImpl, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_CRCParserImpl, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_EIDParserImpl()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_EIDParserImpl(QCBORDecodeContext *ctx, BPLib_EID_t *parsed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_EIDParserImpl, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_EIDParserImpl, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_QCBOR_EIDParserImpl, BPLib_EID_t *, parsed);

    UT_GenStub_Execute(BPLib_QCBOR_EIDParserImpl, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_EIDParserImpl, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_EnterDefiniteArray()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_EnterDefiniteArray(QCBORDecodeContext *ctx, size_t *ArrayLen)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_EnterDefiniteArray, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_EnterDefiniteArray, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_QCBOR_EnterDefiniteArray, size_t *, ArrayLen);

    UT_GenStub_Execute(BPLib_QCBOR_EnterDefiniteArray, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_EnterDefiniteArray, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_ExitDefiniteArray()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_ExitDefiniteArray(QCBORDecodeContext *ctx)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_ExitDefiniteArray, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_ExitDefiniteArray, QCBORDecodeContext *, ctx);

    UT_GenStub_Execute(BPLib_QCBOR_ExitDefiniteArray, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_ExitDefiniteArray, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_TimestampParserImpl()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_TimestampParserImpl(QCBORDecodeContext *ctx, BPLib_CreationTimeStamp_t *parsed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_TimestampParserImpl, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_TimestampParserImpl, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_QCBOR_TimestampParserImpl, BPLib_CreationTimeStamp_t *, parsed);

    UT_GenStub_Execute(BPLib_QCBOR_TimestampParserImpl, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_TimestampParserImpl, BPLib_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_QCBOR_UInt64ParserImpl()
 * ----------------------------------------------------
 */
BPLib_Status_t BPLib_QCBOR_UInt64ParserImpl(QCBORDecodeContext *ctx, uint64_t *parsed)
{
    UT_GenStub_SetupReturnBuffer(BPLib_QCBOR_UInt64ParserImpl, BPLib_Status_t);

    UT_GenStub_AddParam(BPLib_QCBOR_UInt64ParserImpl, QCBORDecodeContext *, ctx);
    UT_GenStub_AddParam(BPLib_QCBOR_UInt64ParserImpl, uint64_t *, parsed);

    UT_GenStub_Execute(BPLib_QCBOR_UInt64ParserImpl, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_QCBOR_UInt64ParserImpl, BPLib_Status_t);
}

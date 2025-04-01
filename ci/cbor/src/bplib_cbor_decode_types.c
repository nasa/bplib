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

#include "bplib_cbor_internal.h"
#include "bplib_eid.h"


/*******************************************************************************
* Exported Parsing Helpers
*/

BPLib_Status_t BPLib_QCBOR_EnterDefiniteArray(QCBORDecodeContext* ctx, size_t* ArrayLen)
{
    QCBORError QStatus;
    QCBORItem Arr;

    if ((ctx == NULL) || (ArrayLen == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Ensure ctx is pointing to an array. */
    QCBORDecode_EnterArray(ctx, &Arr);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_ENTER_DEF_ARRAY_QCBOR_ERR;
    }

    /* Ensure the length isn't indicating this is an indefinite array 
    ** Note: According QCBOR docs, UINT16_MAX is used to indicate indefinte length
    */
    if (Arr.val.uCount > QCBOR_MAX_ITEMS_IN_ARRAY)
    {
        *ArrayLen = 0;
        return BPLIB_CBOR_DEC_TYPES_ENTER_DEF_ARRAY_COUNT_ERR;
    }

    /* Now it is safe to conclude this is a definite array */
    *ArrayLen = Arr.val.uCount;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QCBOR_ExitDefiniteArray(QCBORDecodeContext* ctx)
{
    QCBORError QStatus;

    if (ctx == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    QCBORDecode_ExitArray(ctx);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EXIT_DEF_ARRAY_QCBOR_ERR;
    }

    return BPLIB_SUCCESS;
}

/*******************************************************************************
* RFC-9171 Type Parsers (Implementation)
*/

BPLib_Status_t BPLib_QCBOR_UInt64ParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed)
{
    QCBORError QStatus;

    if ((ctx == NULL) || (parsed == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    QCBORDecode_GetUInt64(ctx, parsed);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_GET_UINT64_QCBOR_ERR;
    }
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QCBOR_EIDParserImpl(QCBORDecodeContext* ctx, BPLib_EID_t* parsed)
{
    BPLib_Status_t Status;
    size_t CurrArrLen;

    if ((ctx == NULL) || (parsed == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Enter EID Array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_ENTER_OUTER_ARRAY_ERR;
    }

    /* Parse URI Type: Note only IPN accepted */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Scheme);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }
    if (parsed->Scheme != BPLIB_EID_SCHEME_IPN)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_SCHEME_NOT_IMPL_ERR;
    }
    /* Jam these extra fields, while we only support IPN two-digit EIDs */
    parsed->IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
    parsed->Allocator = 0;

    /* Enter SSP Array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_SCHEME_NOT_IMPL_ERR;
    }

    /* Node Number */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Node);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_IPN_NODE_DEC_ERR;
    }

    /* Node Service */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Service);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_IPN_SERV_DEC_ERR;
    }

    /* Exit SSP Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_EXIT_SSP_ARRAY_ERR;
    }

    /* Exit EID Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_EID_EXIT_OUTER_ARRAY_ERR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_QCBOR_TimestampParserImpl(QCBORDecodeContext* ctx, BPLib_CreationTimeStamp_t* parsed)
{
    BPLib_Status_t Status;
    size_t CurrArrLen;

    if ((ctx == NULL) || (parsed == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* Enter Timestamp Array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_TIMESTAMP_ENTER_ARRAY_ERR;
    }

    /* First Element of the array should be creation timestamp */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->CreateTime);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_TIMESTAMP_CREATE_DEC_ERR;
    }

    /* Second Element of the array should be creation timestamp */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->SequenceNumber);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_TIMESTAMP_SEQ_NUM_DEC_ERR;
    }

    /* Exit timestamp Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_TIMESTAMP_EXIT_ARRAY_ERR;
    }

    return QCBOR_SUCCESS;
}

BPLib_Status_t BPLib_QCBOR_CRCParserImpl(QCBORDecodeContext* ctx, uint64_t* parsed, uint64_t crc_type)
{
    UsefulBufC TemporaryByteStringBuffer;
    QCBORError QStatus;

    if ((ctx == NULL) || (parsed == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (crc_type == BPLib_CRC_Type_None)
    {
        /* if the CRC is of type "none", assume there is no CRC and skip further decoding */
        *parsed = 0;
        return BPLIB_SUCCESS;
    }

    /* get the CRC as a byte string */
    QCBORDecode_GetByteString(ctx, &TemporaryByteStringBuffer);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_TYPES_CRC_ENTER_BYTE_STR_ERR;
    }

    if (crc_type == BPLib_CRC_Type_CRC16)
    {
        if (TemporaryByteStringBuffer.len != 2)
        {
            return BPLIB_CBOR_DEC_TYPES_CRC_16_LEN_ERR;
        }
        else
        {            
            /* unpack the network-byte-ordered 16-bit value, storing into the 64-bit destination */
            *parsed  = (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[0] <<  8u;
            *parsed |= (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[1];
            return BPLIB_SUCCESS;
        }
        
    }
    else if (crc_type == BPLib_CRC_Type_CRC32C)
    {
        if (TemporaryByteStringBuffer.len != 4)
        {
            return BPLIB_CBOR_DEC_TYPES_CRC_32_LEN_ERR;
        }
        else
        {
            /* unpack the network-byte-ordered 32-bit value, storing into the 64-bit destination */
            *parsed  = (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[0] << 24u;
            *parsed |= (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[1] << 16u;
            *parsed |= (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[2] <<  8u;
            *parsed |= (uint64_t) ((uint8_t*)TemporaryByteStringBuffer.ptr)[3];
            return BPLIB_SUCCESS;
        }
    }
    else
    {
        return BPLIB_CBOR_DEC_TYPES_CRC_UNSUPPORTED_TYPE_ERR;
    }
}

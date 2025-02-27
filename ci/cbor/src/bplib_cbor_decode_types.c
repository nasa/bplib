#include "bplib_cbor_internal.h"


#define BPLIB_IPN_URI              2

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
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Ensure the length isn't indicating this is an indefinite array 
    ** Note: According QCBOR docs, UINT16_MAX is used to indicate indefinte length
    */
    if (Arr.val.uCount > QCBOR_MAX_ITEMS_IN_ARRAY)
    {
        *ArrayLen = 0;
        return BPLIB_CBOR_DEC_ERR;
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
        return BPLIB_CBOR_DEC_ERR;
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
        return BPLIB_CBOR_DEC_ERR;
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
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Parse URI Type: Note only IPN accepted */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Scheme);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }
    if (parsed->Scheme != BPLIB_IPN_URI)
    {
        return BPLIB_CBOR_NOT_IMPL;
    }

    /* Enter SSP Array */
    Status = BPLib_QCBOR_EnterDefiniteArray(ctx, &CurrArrLen);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Node Number */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Node);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* Node Service */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->Service);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* Exit SSP Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* Exit EID Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
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
        return BPLIB_CBOR_DEC_ERR;
    }

    /* First Element of the array should be creation timestamp */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->CreateTime);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Second Element of the array should be creation timestamp */
    Status = BPLib_QCBOR_UInt64ParserImpl(ctx, &parsed->SequenceNumber);
    if (Status != BPLIB_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    /* Exit timestamp Array */
    Status = BPLib_QCBOR_ExitDefiniteArray(ctx);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
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

    /* get the CRC as a byte string */
    QCBORDecode_GetByteString(ctx, &TemporaryByteStringBuffer);
    QStatus = QCBORDecode_GetError(ctx);
    if (QStatus != QCBOR_SUCCESS)
    {
        return BPLIB_CBOR_DEC_ERR;
    }

    if (crc_type == BPLib_CRC_Type_CRC16)
    {
        if (TemporaryByteStringBuffer.len != 2)
        {
            printf("BPLib_QCBOR_CRCParserImpl error: expected 2-byte crc. Got %lu bytes instead!\n",
                TemporaryByteStringBuffer.len);
            return BPLIB_CBOR_DEC_ERR;
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
            printf("BPLib_QCBOR_CRCParserImpl error: expected 2-byte crc. Got %lu bytes instead!\n",
                TemporaryByteStringBuffer.len);
            return BPLIB_CBOR_DEC_ERR;
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
        return BPLIB_CBOR_DEC_ERR;
    }
}

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

/**
 * \brief Add an "open byte string" CBOR head
 *
 * Major Type: 2 (byte string)
 * Additional Info: number of bytes in the string
 *  - If number of bytes in the string is less than 24:
 *      - the next 5 bits describe the length
 *  - If number of bytes in the string is 24 to 0xFF
 *      - the next 5 bits should be set to 24
 *      - the next 1 byte should describe the length
 *  - If number of bytes in the string is 0x100 to 0xFFFF
 *      - the next 5 bits should be set to 25
 *      - the next 2 bytes should describe the length
 *  - If number of bytes in the string is 0x1.0000 to 0xFFFF.FFFF
 *      - the next 5 bits should be set to 26
 *      - the next 4 bytes should describe the length
 *  - If number of bytes in the string is 0x1.0000.0000 to 0xFFFF.FFFF.FFFF.FFFF
 *      - the next 5 bits should be set to 27
 *      - the next 8 bytes should describe the length
 *
 * \param[in] DataSize (uint64_t) Size of the ADU (determines how many bytes of additional info are in the CBOR head)
 * \param[in] CurrentOutputBufferAddr (uintptr_t) Destination buffer, to write the cbor head
 * \param[in] BytesLeftInOutputBuffer (size_t) Destination buffer, to write the cbor head
 *
 * \return The number of bytes written to the output buffer (0 returned on error)
 */
static size_t BPLib_CBOR_AddByteStringHead(uint64_t DataSize,
    uintptr_t CurrentOutputBufferAddr,
    size_t BytesLeftInOutputBuffer)
{
    size_t BytesWritten;
    size_t DataSizeBitShift;
    uint8_t DataSizeByte;
    uint8_t FirstByte;
    size_t BytesNeeded;

    if (DataSize < 24)
    {
        BytesNeeded = 1;
        FirstByte = (2 << 5) | DataSize;
    }
    else if (DataSize < 0x100)
    {
        BytesNeeded = 1 + 1;
        FirstByte = (2 << 5) | 24;
    }
    else if (DataSize < 0x10000)
    {
        BytesNeeded = 1 + 2;
        FirstByte = (2 << 5) | 25;
    }
    else if (DataSize < 0x100000000)
    {
        BytesNeeded = 1 + 4;
        FirstByte = (2 << 5) | 26;
    }
    else
    {
        BytesNeeded = 1 + 8;
        FirstByte = (2 << 5) | 27;
    }

    /* Make sure there is enough room in output buffer before writing anything */
    if (BytesNeeded <= BytesLeftInOutputBuffer)
    {
        /* write the first byte */
        *(uint8_t*)CurrentOutputBufferAddr = FirstByte;
        CurrentOutputBufferAddr++;
        /* optionally write any additional bytes */
        for (BytesWritten = 1; BytesWritten < BytesNeeded; BytesWritten++)
        {
            DataSizeBitShift = ((BytesNeeded - 1) * 8) - (BytesWritten * 8);
            DataSizeByte = (uint8_t) ((DataSize >> DataSizeBitShift) & 0xFF);
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            CurrentOutputBufferAddr++;
        }
    }
    else
    {
        /* error - not enough room in output buffer */
        BytesWritten = 0;
    }

    return BytesWritten;
}


BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Status_t PayloadDataCopyStatus;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;
    uintptr_t CurrentOutputBufferAddr;
    size_t TotalBytesCopied;
    size_t BytesLeftInOutputBuffer;
    size_t ByteStringCborHeadSize;

    if ((StoredBundle == NULL) ||
        (OutputBuffer == NULL) ||
        (NumBytesCopied == NULL))
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else
    {
        /*
        ** Jam in an "open definite array" character
        ** Major Type: 4 (array)
        ** Additional Info: number of data items in array (6 total)
        **  1. Block Type
        **  2. Block Num
        **  3. Block Processing Flags
        **  4. CRC Type
        **  5. Block-Specific Data (ADU)
        **  6. CRC Value
        ** 0b100_00110 == 0x86
        */
        CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer);
        *(uint8_t*)CurrentOutputBufferAddr = 0x86;
        TotalBytesCopied = 1;
        CurrentOutputBufferAddr++;
        BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

        /*
        ** Initialize the encoder (encoding just the initial items before the ADU)
        */
        InitStorage.ptr = (void*) CurrentOutputBufferAddr;
        InitStorage.len = BytesLeftInOutputBuffer;
        QCBOREncode_Init(&Context, InitStorage);

        /*
        ** Add our block header data
        */
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BlockType);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BlockNum);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BlockProcFlags);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.CrcType);

        /*
        ** Finish encoding, and check for errors
        */
        FinishBuffer.len = 0;
        FinishBuffer.ptr = NULL;
        QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);
        if (QcborStatus != QCBOR_SUCCESS)
        {
            *NumBytesCopied = 0;
            return BPLIB_CBOR_ENC_PAYL_QCBOR_FINISH_HEAD_ERR;
        }
        else
        {
            TotalBytesCopied += FinishBuffer.len;
            CurrentOutputBufferAddr += FinishBuffer.len;
            BytesLeftInOutputBuffer -= FinishBuffer.len;
        }

        /*
        ** Jam in our own "byte string" cbor encoding head
        */
        ByteStringCborHeadSize = BPLib_CBOR_AddByteStringHead(StoredBundle->blocks.PayloadHeader.DataSize,
                                                              CurrentOutputBufferAddr,
                                                              BytesLeftInOutputBuffer);
        if (ByteStringCborHeadSize > 0)
        {
            CurrentOutputBufferAddr += ByteStringCborHeadSize;
            TotalBytesCopied        += ByteStringCborHeadSize;
            BytesLeftInOutputBuffer -= ByteStringCborHeadSize;
        }
        else
        {
            *NumBytesCopied = 0;
            return BPLIB_CBOR_ENC_PAYL_ADD_BYTE_STR_HEAD_ERR;
        }

        /*
        ** Add the ADU data
        */
        PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
            StoredBundle->blocks.PayloadHeader.DataOffsetStart,
            StoredBundle->blocks.PayloadHeader.DataSize,
            (void*) CurrentOutputBufferAddr,
            BytesLeftInOutputBuffer);

        if (PayloadDataCopyStatus == BPLIB_SUCCESS)
        {
            CurrentOutputBufferAddr += StoredBundle->blocks.PayloadHeader.DataSize;
            BytesLeftInOutputBuffer -= StoredBundle->blocks.PayloadHeader.DataSize;
            TotalBytesCopied += StoredBundle->blocks.PayloadHeader.DataSize;
        }
        else
        {
            *NumBytesCopied = 0;
            return PayloadDataCopyStatus;
        }

        /*
        ** Initialize the encoder (to encode the CRC)
        */
        InitStorage.ptr = (void*) CurrentOutputBufferAddr;
        InitStorage.len = BytesLeftInOutputBuffer;
        QCBOREncode_Init(&Context, InitStorage);

        /*
        ** Add the CRC
        */
        /* Set CRC value to 0, real value will be jammed in after encoding is done */
        (void) BPLib_CBOR_EncodeCrcValue(&Context, 0, StoredBundle->blocks.PayloadHeader.CrcType);
        
        /*
        ** Finish encoding, and check for errors
        */
        FinishBuffer.len = 0;
        FinishBuffer.ptr = NULL;
        QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);
        if (QcborStatus != QCBOR_SUCCESS)
        {
            *NumBytesCopied = 0;
            return BPLIB_CBOR_ENC_PAYL_QCBOR_FINISH_TAIL_ERR;
        }
        else
        {
            TotalBytesCopied += FinishBuffer.len;

            /* Calculate new CRC for encoded block */
            BPLib_CBOR_GenerateBlockCrc(OutputBuffer, 
                                    StoredBundle->blocks.PayloadHeader.CrcType,
                                    0, TotalBytesCopied);

            *NumBytesCopied += TotalBytesCopied;
            CurrentOutputBufferAddr += FinishBuffer.len;
            BytesLeftInOutputBuffer -= FinishBuffer.len;
        }

        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}


BPLib_Status_t BPLib_CBOR_CopyOrEncodePayload(BPLib_Bundle_t* StoredBundle,
                                              void* OutputBuffer,
                                              size_t OutputBufferSize,
                                              size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    uint64_t TotalPayloadSize;
    BPLib_Status_t PayloadDataCopyStatus;

    if (StoredBundle->blocks.PayloadHeader.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePayload(StoredBundle,
                                                OutputBuffer,
                                                OutputBufferSize,
                                                NumBytesCopied);
    }
    /* Verify that the block offset values are reasonable */
    else if (StoredBundle->blocks.PayloadHeader.BlockOffsetStart >= 
             (StoredBundle->blocks.PayloadHeader.BlockOffsetEnd + 1))
    {
        *NumBytesCopied = 0;
        ReturnStatus = BPLIB_CBOR_ENC_PAYL_SIZES_CRRPTD_ERR;
    }
    else
    {
        /*
        ** Calculate the total payload size
        */
        TotalPayloadSize = StoredBundle->blocks.PayloadHeader.BlockOffsetEnd
                         - StoredBundle->blocks.PayloadHeader.BlockOffsetStart
                         + 1;

        /*
        ** Copy in the whole payload (header, data, and crc value)
        */
        if (TotalPayloadSize > OutputBufferSize)
        {
            *NumBytesCopied = 0;
            ReturnStatus = BPLIB_CBOR_ENC_PAYL_COPY_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                StoredBundle->blocks.PayloadHeader.BlockOffsetStart,
                TotalPayloadSize,
                OutputBuffer,
                OutputBufferSize);

            if (PayloadDataCopyStatus == BPLIB_SUCCESS)
            {
                *NumBytesCopied += TotalPayloadSize;
                ReturnStatus = BPLIB_SUCCESS;
            }
            else
            {
                *NumBytesCopied = 0;
                ReturnStatus = PayloadDataCopyStatus;
            }
        }
    }
    return ReturnStatus;
}

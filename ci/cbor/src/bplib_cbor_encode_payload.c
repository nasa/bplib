#include "bplib_cbor_internal.h"

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
    uint8_t DataSizeByte;

    if (StoredBundle == NULL)
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else if (OutputBuffer == NULL)
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else if (NumBytesCopied == NULL)
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
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BundleProcFlags);
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
            return BPLIB_ERROR;
        }
        else
        {
            TotalBytesCopied += FinishBuffer.len;
        }

        /*
        ** Jam in an "open byte string" character
        ** Major Type: 2 (byte string)
        ** Additional Info: number of bytes in the string
        **  - If number of bytes in the string is less than 24:
        **      - the next 5 bits describe the length
        **  - If number of bytes in the string is 25 to 0xFF
        **      - the next 5 bits should be set to 24
        **      - the next 1 byte should describe the length
        **  - If number of bytes in the string is 0x100 to 0xFFFF
        **      - the next 5 bits should be set to 25
        **      - the next 2 bytes should describe the length
        **  - If number of bytes in the string is 0x1.0000 to 0xFFFF.FFFF
        **      - the next 5 bits should be set to 26
        **      - the next 4 bytes should describe the length
        **  - If number of bytes in the string is 0x1.0000.0000 to 0xFFFF.FFFF.FFFF.FFFF
        **      - the next 5 bits should be set to 27
        **      - the next 8 bytes should describe the length
        */
        if (StoredBundle->blocks.PayloadHeader.DataOffsetSize < 24)
        {
            *(uint8_t*)CurrentOutputBufferAddr = (2 << 5) | StoredBundle->blocks.PayloadHeader.DataOffsetSize;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;
        }
        else if (StoredBundle->blocks.PayloadHeader.DataOffsetSize < 0x100)
        {
            *(uint8_t*)CurrentOutputBufferAddr = (2 << 5) | 24;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0xFF);
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;
        }
        else if (StoredBundle->blocks.PayloadHeader.DataOffsetSize < 0x10000)
        {
            *(uint8_t*)CurrentOutputBufferAddr = (2 << 5) | 25;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0xFF00) >> 8;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x00FF);
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;
        }
        else if (StoredBundle->blocks.PayloadHeader.DataOffsetSize < 0x100000000)
        {
            *(uint8_t*)CurrentOutputBufferAddr = (2 << 5) | 26;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0xFF000000) >> 24;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x00FF0000) >> 16;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x0000FF00) >> 8;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x000000FF);
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;
        }
        else
        {
            *(uint8_t*)CurrentOutputBufferAddr = (2 << 5) | 27;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0xFF00000000000000) >> 56;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x00FF000000000000) >> 48;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x0000FF0000000000) >> 40;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x000000FF00000000) >> 32;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x00000000FF000000) >> 24;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x0000000000FF0000) >> 16;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x000000000000FF00) >> 8;
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;

            DataSizeByte = (uint8_t) (StoredBundle->blocks.PayloadHeader.DataOffsetSize & 0x00000000000000FF);
            *(uint8_t*)CurrentOutputBufferAddr = DataSizeByte;
            TotalBytesCopied++;
            CurrentOutputBufferAddr++;
            BytesLeftInOutputBuffer--;
        }

        /*
        ** Add the ADU data
        */
        PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
            StoredBundle->blocks.PayloadHeader.DataOffsetStart,
            StoredBundle->blocks.PayloadHeader.DataOffsetSize,
            (void*) CurrentOutputBufferAddr,
            BytesLeftInOutputBuffer);

        if (PayloadDataCopyStatus == BPLIB_SUCCESS)
        {
            CurrentOutputBufferAddr += StoredBundle->blocks.PayloadHeader.DataOffsetSize;
            BytesLeftInOutputBuffer -= StoredBundle->blocks.PayloadHeader.DataOffsetSize;
            TotalBytesCopied += StoredBundle->blocks.PayloadHeader.DataOffsetSize;
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
        /* TODO: calculate the CRC first */
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
            return BPLIB_ERROR;
        }
        else
        {
            TotalBytesCopied += FinishBuffer.len;
            CurrentOutputBufferAddr += FinishBuffer.len;
            BytesLeftInOutputBuffer -= FinishBuffer.len;
        }

        *NumBytesCopied = TotalBytesCopied;
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
    else
    {
        /*
        ** Calculate the total payload size
        **
        ** TODO: Figure out why we don't have to add one here!
        **       This is likely due to how we're assigning BlockOffsetEnd during the payload decode
        */
        TotalPayloadSize = StoredBundle->blocks.PayloadHeader.BlockOffsetEnd
                         - StoredBundle->blocks.PayloadHeader.BlockOffsetStart;

        /*
        ** Copy in the whole payload (header, data, and crc value)
        */
        if (TotalPayloadSize > OutputBufferSize)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
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
                *NumBytesCopied = TotalPayloadSize;
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

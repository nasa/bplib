#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePayload(QCBOREncodeContext* Context,
                                        BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Status_t PayloadDataCopyStatus;
    size_t StartOffset;
    size_t EndOffset;
    size_t PayloadDataBytesCopied;
    UsefulBuf OutputSubBuffer;

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
        ** Grab the start offset, to be used to calculate the total encoded size
        */
        StartOffset = QCBOREncode_Tell(Context);

        /*
        ** Open Canonical/Payload Block Array
        */
        QCBOREncode_OpenArray(Context);

        /*
        ** Add our block header data
        */
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PayloadHeader.BlockType);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PayloadHeader.BlockNum);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PayloadHeader.BundleProcFlags);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PayloadHeader.CrcType);

        /*
        ** Add the ADU data
        */
        QCBOREncode_OpenBytes(Context, &OutputSubBuffer);

        PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
            StoredBundle->blocks.PayloadHeader.DataOffset,
            StoredBundle->blocks.PrimaryBlock.TotalAduLength,
            OutputSubBuffer.ptr,
            OutputSubBuffer.len);

        if (PayloadDataCopyStatus == BPLIB_SUCCESS)
        {
            PayloadDataBytesCopied = StoredBundle->blocks.PrimaryBlock.TotalAduLength;
        }
        else
        {
            PayloadDataBytesCopied = 0;
        }

        QCBOREncode_CloseBytes(Context, PayloadDataBytesCopied);

        /*
        ** Add the CRC
        */
        /* TODO: calculate the CRC first */
        (void) BPLib_CBOR_EncodeCrcValue(Context, 0, StoredBundle->blocks.PayloadHeader.CrcType);

        /*
        ** Close Array
        */
        QCBOREncode_CloseArray(Context);

        /*
        ** Calculate the total encoded size
        */
        EndOffset = QCBOREncode_Tell(Context);
        *NumBytesCopied = EndOffset - StartOffset;
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}


BPLib_Status_t BPLib_CBOR_CopyOutEncodedData(QCBOREncodeContext* Context,
    BPLib_Bundle_t* Bundle,
    uint64_t Offset,
    uint64_t NumBytesToCopy)
{
    BPLib_Status_t ReturnStatus = BPLIB_SUCCESS;
    BPLib_MEM_Block_t *CurrentBlock;
    uint64_t NumBytesLeftToSkip;
    uint64_t BytesLeftInThisBlock;
    uint64_t BytesToCopyInThisBlock;
    uint64_t RemainingBytesToCopy;
    uint64_t TotalBytesCopied;
    uintptr_t CurrentInputOffset;
    uint64_t ExpectedMemBlockNumber;
    uint64_t CurrentMemBlockNumber;
    UsefulBufC CurrentInfoToCopy;

    if (Bundle == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (Bundle->blob == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /* find the first blob that contains data after the offset */
    CurrentBlock = Bundle->blob;
    NumBytesLeftToSkip = Offset;
    ExpectedMemBlockNumber = Offset / BPLIB_MEM_CHUNKSIZE;
    for (CurrentMemBlockNumber = 0; CurrentMemBlockNumber < ExpectedMemBlockNumber; CurrentMemBlockNumber++)
    {
        NumBytesLeftToSkip -= BPLIB_MEM_CHUNKSIZE;
        CurrentBlock = CurrentBlock->next;
        if (CurrentBlock == NULL)
        {
            return BPLIB_ERROR;
        }
    }

    /* Start copying from the first block */
    CurrentInputOffset = (uintptr_t) &CurrentBlock->user_data.raw_bytes[NumBytesLeftToSkip];
    BytesLeftInThisBlock = BPLIB_MEM_CHUNKSIZE - NumBytesLeftToSkip;
    if (NumBytesToCopy <= BytesLeftInThisBlock)
    {
        CurrentInfoToCopy.len = NumBytesToCopy;
        CurrentInfoToCopy.ptr = CurrentInputOffset;
        /*
        ** TODO: apparently this is not the right way to add encoded data into the buffer
        */
        QCBOREncode_AddEncoded(Context, CurrentInfoToCopy);
        TotalBytesCopied = NumBytesToCopy;
    }
    else
    {
        CurrentInfoToCopy.len = BytesLeftInThisBlock;
        CurrentInfoToCopy.ptr = CurrentInputOffset;
        QCBOREncode_AddEncoded(Context, CurrentInfoToCopy);
        TotalBytesCopied = BytesLeftInThisBlock;
    }

    /* if we need to copy more data, loop through each next block */
    while ((TotalBytesCopied < NumBytesToCopy) && (CurrentBlock->next != NULL))
    {
        CurrentBlock = CurrentBlock->next;
        CurrentInputOffset = (uintptr_t) &CurrentBlock->user_data.raw_bytes[0];

        RemainingBytesToCopy = NumBytesToCopy - TotalBytesCopied;
        if (RemainingBytesToCopy >= BPLIB_MEM_CHUNKSIZE)
        {
            BytesToCopyInThisBlock = BPLIB_MEM_CHUNKSIZE;
        }
        else
        {
            BytesToCopyInThisBlock = RemainingBytesToCopy;
        }

        CurrentInfoToCopy.len = BytesToCopyInThisBlock;
        CurrentInfoToCopy.ptr = CurrentInputOffset;
        QCBOREncode_AddEncoded(Context, CurrentInfoToCopy);
        TotalBytesCopied += BytesToCopyInThisBlock;
    }

    if (TotalBytesCopied != NumBytesToCopy)
    {
        ReturnStatus = BPLIB_ERROR;
    }
    else
    {
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}



BPLib_Status_t BPLib_CBOR_CopyOrEncodePayload(QCBOREncodeContext* Context,
                                              BPLib_Bundle_t* StoredBundle,
                                              void* OutputBuffer,
                                              size_t OutputBufferSize,
                                              size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    uint64_t TotalPayloadSize;
    UsefulBuf OutputSubBuffer;
    size_t PayloadDataBytesCopied;
    BPLib_Status_t PayloadDataCopyStatus;

    if (StoredBundle->blocks.PayloadHeader.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePayload(Context,
                                                StoredBundle,
                                                OutputBuffer,
                                                OutputBufferSize,
                                                NumBytesCopied);
    }
    else
    {
        /*
        ** Calculate the total payload size
        */
        TotalPayloadSize = StoredBundle->blocks.PayloadHeader.EncodedCrcValOffset
                         - StoredBundle->blocks.PayloadHeader.HeaderOffset
                         + StoredBundle->blocks.PayloadHeader.EncodedCrcValSize;

        /*
        ** Open a byte array
        */
        QCBOREncode_OpenBytes(Context, &OutputSubBuffer);

        /*
        ** Copy in the whole payload (header, data, and crc value)
        */
        PayloadDataBytesCopied = 0;
        if (TotalPayloadSize > OutputBufferSize)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else if (TotalPayloadSize > OutputSubBuffer.len)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                StoredBundle->blocks.PayloadHeader.HeaderOffset,
                TotalPayloadSize,
                OutputSubBuffer.ptr,
                OutputSubBuffer.len);

            if (PayloadDataCopyStatus == BPLIB_SUCCESS)
            {
                PayloadDataBytesCopied = TotalPayloadSize;
                *NumBytesCopied = TotalPayloadSize;
                ReturnStatus = BPLIB_SUCCESS;
            }
            else
            {
                *NumBytesCopied = 0;
                ReturnStatus = PayloadDataCopyStatus;
            }
        }

        /*
        ** Close the byte array, telling CBOR how much data we just copied in
        */
        QCBOREncode_CloseBytes(Context, PayloadDataBytesCopied);
    }
    return ReturnStatus;
}

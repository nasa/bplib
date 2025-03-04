#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePayload(QCBOREncodeContext* Context,
                                        BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    size_t StartOffset;
    size_t EndOffset;

    /*
    ** TODO
    */
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
        ** Open Array (TODO: is this the right API?)
        ** Maybe this instead:
        ** QCBOREncode_AddBytes(QCBOREncodeContext *pCtx, UsefulBufC Bytes);
        ** or:
        ** QCBOREncode_BstrWrap(QCBOREncodeContext *pCtx);
        */
        // QCBOREncode_OpenArray(Context);


        /*
        ** Close Array
        ** Use this instead:
        ** static void
        ** QCBOREncode_CloseBstrWrap(QCBOREncodeContext *pCtx, UsefulBufC *pWrappedCBOR);
        */
        // QCBOREncode_CloseArray(Context);

        /*
        ** Calculate the total encoded size
        */
        EndOffset = QCBOREncode_Tell(Context);
        *NumBytesCopied = EndOffset - StartOffset;
        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}


BPLib_Status_t BPLib_CBOR_CopyOutEncodedPayload(QCBOREncodeContext* Context,
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
    uint64_t PayloadHeaderSize;
    uint64_t EncodedCrcValueSize;
    uint64_t TotalPayloadSize;

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
        PayloadHeaderSize = StoredBundle->blocks.PayloadHeader.DataOffset;
                          - StoredBundle->blocks.PayloadHeader.HeaderOffset;

        switch (StoredBundle->blocks.PayloadHeader.CrcType)
        {
            case BPLib_CRC_Type_CRC32C:
                EncodedCrcValueSize = 4; /* TODO: do we need to add 1 here, for the cbor byte string wrapper? */
                break;
            case BPLib_CRC_Type_CRC16:
                EncodedCrcValueSize = 2; /* TODO: do we need to add 1 here, for the cbor byte string wrapper? */
                break;
            default:
                EncodedCrcValueSize = 0;
        }

        TotalPayloadSize = PayloadHeaderSize
                         + StoredBundle->blocks.PrimaryBlock.TotalAduLength
                         + EncodedCrcValueSize;

        /*
        ** TODO: we can probably remove this length check,
        ** because QCBOR won't copy past the original buffer length
        */
        if (TotalPayloadSize > OutputBufferSize)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            /*
            ** copy adu data out of memory blocks
            */
            ReturnStatus = BPLib_CBOR_CopyOutEncodedPayload(Context,
                                                       StoredBundle,
                                                       StoredBundle->blocks.PayloadHeader.HeaderOffset,
                                                       TotalPayloadSize);

            if (ReturnStatus == BPLIB_SUCCESS)
            {
                *NumBytesCopied = TotalPayloadSize;
            }
        }
    }
    return ReturnStatus;
}

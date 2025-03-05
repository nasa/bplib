#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePrimary(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    // size_t StartOffset;
    // size_t EndOffset;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;


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
        ** Initialize the encoder.
        */
        InitStorage.ptr = OutputBuffer;
        InitStorage.len = OutputBufferSize;
        QCBOREncode_Init(&Context, InitStorage);

        /* Grab the start offset, to be used to calculate the total encoded size */
        // StartOffset = QCBOREncode_Tell(&Context);

        /*
        ** Open Array
        */
        QCBOREncode_OpenArray(&Context);

        /*
        ** Add Primary Block Fields
        */
        QCBOREncode_AddUInt64(&Context, (uint64_t) BPLIB_BUNDLE_PROTOCOL_VERSION);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.BundleProcFlags);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.CrcType);

        BPLib_CBOR_EncodeEID(&Context, &StoredBundle->blocks.PrimaryBlock.DestEID);
        BPLib_CBOR_EncodeEID(&Context, &StoredBundle->blocks.PrimaryBlock.SrcEID);
        BPLib_CBOR_EncodeEID(&Context, &StoredBundle->blocks.PrimaryBlock.ReportToEID);

        BPLib_CBOR_EncodeCreationTimeStamp(&Context, &StoredBundle->blocks.PrimaryBlock.Timestamp);

        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.Lifetime);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.FragmentOffset);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.TotalAduLength);

        /* start with CRC value 0. actual value to be filled in later. */
        StoredBundle->blocks.PrimaryBlock.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(&Context, StoredBundle->blocks.PrimaryBlock.CrcVal,
                                            StoredBundle->blocks.PrimaryBlock.CrcType);

        /*
        ** Close Array
        */
        QCBOREncode_CloseArray(&Context);

        /*
        ** Calculate the total encoded size
        */
        // EndOffset = QCBOREncode_Tell(&Context);

        /*
        ** Finish encoding, and check for errors
        */
        FinishBuffer.len = 0;
        FinishBuffer.ptr = NULL;
        QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);
        if (QcborStatus != QCBOR_SUCCESS)
        {
            *NumBytesCopied = 0;
        }
        else
        {
            *NumBytesCopied = FinishBuffer.len;
        }
        // *NumBytesCopied = EndOffset - StartOffset;

        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}



BPLib_Status_t BPLib_CBOR_CopyOrEncodePrimary(BPLib_Bundle_t* StoredBundle,
                                              void* OutputBuffer,
                                              size_t OutputBufferSize,
                                              size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    // UsefulBuf OutputSubBuffer;
    size_t PrimaryBlockSize;
    // size_t PrimaryDataBytesCopied;
    BPLib_Status_t PrimaryBlockCopyStatus;

    if (StoredBundle->blocks.PrimaryBlock.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePrimary(StoredBundle,
            OutputBuffer,
            OutputBufferSize,
            NumBytesCopied);
    }
    else
    {

        /*
        ** Open a byte array
        */
        // QCBOREncode_OpenBytes(Context, &OutputSubBuffer);

        PrimaryBlockSize = StoredBundle->blocks.PrimaryBlock.BlockOffsetEnd
                        - StoredBundle->blocks.PrimaryBlock.BlockOffsetStart
                        + 1;

        /*
        ** Check for input errors before copying the pre-encoded Primary Block to the output buffer
        ** This is important because BPLib_BI_BlobCopyOut doesn't do more than null ptr checks
        */
        if (PrimaryBlockSize > OutputBufferSize)
        {
            /*
            ** the encoded primary block (our source) exceeds the output buffer size
            */
            ReturnStatus = BPLIB_BI_COPY_PRIME_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            PrimaryBlockCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                StoredBundle->blocks.PrimaryBlock.BlockOffsetStart,
                PrimaryBlockSize,
                OutputBuffer,
                OutputBufferSize);
    
            if (PrimaryBlockCopyStatus == BPLIB_SUCCESS)
            {
                // PrimaryDataBytesCopied = PrimaryBlockSize;
                *NumBytesCopied = PrimaryBlockSize;
                ReturnStatus = PrimaryBlockCopyStatus;
            }
            else
            {
                // PrimaryDataBytesCopied = 0;
                *NumBytesCopied = 0;
                ReturnStatus = BPLIB_SUCCESS;
            }

            /*
            ** Close the byte array, telling CBOR how much data we just copied in
            */
            // QCBOREncode_CloseBytes(Context, PrimaryDataBytesCopied);

            /* don't forget to set our outputs! */
            // *NumBytesCopied = PrimaryDataBytesCopied;
            // ReturnStatus = BPLIB_SUCCESS;
        }
    }
    return ReturnStatus;
}


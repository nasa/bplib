#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePrimary(QCBOREncodeContext* Context,
                                        BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    size_t StartOffset;
    size_t EndOffset;

    if (Context == NULL)
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else if (StoredBundle == NULL)
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
        /* Grab the start offset, to be used to calculate the total encoded size */
        StartOffset = QCBOREncode_Tell(Context);

        /*
        ** Open Array
        */
        QCBOREncode_OpenArray(Context);

        /*
        ** Add Primary Block Fields
        */
        QCBOREncode_AddUInt64(Context, (uint64_t) BPLIB_BUNDLE_PROTOCOL_VERSION);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PrimaryBlock.BundleProcFlags);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PrimaryBlock.CrcType);

        BPLib_CBOR_EncodeEID(Context, &StoredBundle->blocks.PrimaryBlock.DestEID);
        BPLib_CBOR_EncodeEID(Context, &StoredBundle->blocks.PrimaryBlock.SrcEID);
        BPLib_CBOR_EncodeEID(Context, &StoredBundle->blocks.PrimaryBlock.ReportToEID);

        BPLib_CBOR_EncodeCreationTimeStamp(Context, &StoredBundle->blocks.PrimaryBlock.Timestamp);

        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PrimaryBlock.Lifetime);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PrimaryBlock.FragmentOffset);
        QCBOREncode_AddUInt64(Context, StoredBundle->blocks.PrimaryBlock.TotalAduLength);

        /* start with CRC value 0. actual value to be filled in later. */
        StoredBundle->blocks.PrimaryBlock.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(Context, StoredBundle->blocks.PrimaryBlock.CrcVal,
                                            StoredBundle->blocks.PrimaryBlock.CrcType);

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



BPLib_Status_t BPLib_CBOR_CopyOrEncodePrimary(QCBOREncodeContext* Context,
                                              BPLib_Bundle_t* StoredBundle,
                                              void* OutputBuffer,
                                              size_t OutputBufferSize,
                                              size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    if (StoredBundle->blocks.PrimaryBlock.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePrimary(Context,
            StoredBundle,
            OutputBuffer,
            OutputBufferSize,
            NumBytesCopied);
    }
    else
    {
        /*
        ** Check for input errors before copying the pre-encoded Primary Block to the output buffer
        ** This is important because BPLib_BI_BlobCopyOut doesn't do more than null ptr checks
        */
        if (StoredBundle->blocks.PrimaryBlock.EncodedSize > OutputBufferSize)
        {
            /*
            ** the encoded primary block (our source) exceeds the output buffer size
            */
            ReturnStatus = BPLIB_BI_COPY_PRIME_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else if (StoredBundle->blocks.PrimaryBlock.EncodedSize > StoredBundle->blob->used_len)
        {
            /*
            ** The primary block metadata conflicts with stored block user data length, or
            ** the primary block is larger than our chunk size.
            **
            ** Note:
            ** If it ever became true that an encoded Primary Block could be larger than
            ** our configured chunk size (`BPLIB_MEM_CHUNKSIZE`), we'd have to rework this logic
            ** to copy this out block-by-block.
            */
            ReturnStatus = BPLIB_BI_COPY_PRIME_ENC_SIZE_GT_USER_DATA_ERR;
        }
        else
        {
            /*
            ** TODO:
            ** This assume the encoded primary block starts at the very beginning of the first blob.
            ** Is that correct, or do we have
            */
            memcpy(OutputBuffer,
                (void*)StoredBundle->blob->user_data.raw_bytes,
                StoredBundle->blocks.PrimaryBlock.EncodedSize);

            /* don't forget to set our outputs! */
            *NumBytesCopied = StoredBundle->blocks.PrimaryBlock.EncodedSize;
            ReturnStatus = BPLIB_SUCCESS;
        }
    }
    return ReturnStatus;
}


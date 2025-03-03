#include "bplib_bi_internal.h"
#include "bplib_cbor.h"
#include "bplib_mem.h"


BPLib_Status_t BPLib_BI_CopyOrEncodePrimary(BPLib_Bundle_t* StoredBundle,
                                            void* OutputBuffer,
                                            size_t OutputBufferSize,
                                            size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
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


uint32_t BPLib_BI_GetNumExtensionBlocks(BPLib_Bundle_t* StoredBundle)
{
    /*
    ** TODO
    */
    return 0;
}


BPLib_Status_t BPLib_BI_CopyOrEncodePayload(BPLib_Bundle_t* StoredBundle,
                                            void* OutputBuffer,
                                            size_t OutputBufferSize,
                                            size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;

    if (StoredBundle->blocks.PayloadHeader.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePayload(StoredBundle,
                                                OutputBuffer,
                                                OutputBufferSize,
                                                NumBytesCopied);
    }
    else
    {
        if (StoredBundle->blocks.PrimaryBlock.TotalAduLength > OutputBufferSize)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            /*
            ** copy adu data out of memory blocks
            */
            ReturnStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                                                       StoredBundle->blocks.PayloadHeader.HeaderOffset,
                                                       StoredBundle->blocks.PrimaryBlock.TotalAduLength,
                                                       OutputBuffer,
                                                       OutputBufferSize);

            if (ReturnStatus == BPLIB_SUCCESS)
            {
                *NumBytesCopied = StoredBundle->blocks.PrimaryBlock.TotalAduLength;
            }
        }
    }
    return ReturnStatus;
}
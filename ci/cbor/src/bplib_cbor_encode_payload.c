#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePayload(QCBOREncodeContext* Context,
                                        BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;

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

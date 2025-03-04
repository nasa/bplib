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

            /*
            ** TODO: how do we keep QCBOR in the loop about this?
            ** Consider using this:
            **  void
            **  QCBOREncode_AddEncoded(QCBOREncodeContext *pCtx, UsefulBufC Encoded);
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

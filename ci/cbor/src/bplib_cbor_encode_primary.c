#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePrimary(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBuf FinishBuffer;
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
        **
        ** @param[in,out]  pCtx     The encoder context to initialize.
        ** @param[in]      Storage  The buffer into which the encoded result
        **                          will be written.
        ** A common encoding usage mode is to invoke the encoding twice. First
        ** with the output buffer as @ref SizeCalculateUsefulBuf to compute the
        ** length of the needed output buffer. The correct sized output buffer
        ** is allocated. The encoder is invoked a second time with the allocated
        ** output buffer.
        */
        InitStorage.len = 0;
        InitStorage.ptr = NULL;
        FinishBuffer.len = 0;
        FinishBuffer.ptr = OutputBuffer;

        /*
        ** TODO: Move this up-and-out, to BPLib_BI_BlobCopyOut()?
        */
        QCBOREncode_Init(&Context, InitStorage);

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

        StoredBundle->blocks.PrimaryBlock.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(&Context, StoredBundle->blocks.PrimaryBlock.CrcVal,
                                            StoredBundle->blocks.PrimaryBlock.CrcType);

        /*
        ** Close Array
        */
        QCBOREncode_CloseArray(&Context);

        /*
        ** Finish
        ** TODO: Move this up-and-out, to BPLib_BI_BlobCopyOut()?
        */
        QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);
        if (QcborStatus != QCBOR_SUCCESS)
        {
            *NumBytesCopied = 0;
            ReturnStatus = BPLIB_CBOR_ENC_PRIMARY_FINISH_ERR;
        }
        else
        {
            *NumBytesCopied = FinishBuffer.len;
            ReturnStatus = BPLIB_SUCCESS;
        }
    }

    return ReturnStatus;
}

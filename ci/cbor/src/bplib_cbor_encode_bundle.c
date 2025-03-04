#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodeBundle(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;

    /* TODO: these variables need to be scrubbed for usage (since I moved this from BI) */
    BPLib_Status_t PrimaryBlockReturnStatus;
    BPLib_Status_t ExtensionBlockReturnStatus = BPLIB_SUCCESS; // start with success, in case no ext blocks
    BPLib_Status_t PayloadBlockReturnStatus;
    size_t PrimaryBlockBytesCopied;
    size_t ExtBlockBytesCopied;
    size_t PayloadBytesCopied = 0;
    size_t TotalBytesCopied;
    uint32_t NumberOfExtensionBlocks;
    uint32_t CurrExtBlockIndex;
    uintptr_t CurrentOutputBufferAddr;
    size_t BytesLeftInOutputBuffer;


    if ((StoredBundle == NULL) || (StoredBundle->blob == NULL) || (OutputBuffer == NULL) || (NumBytesCopied == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /*
    ** Initialize the encoder.
    */
    InitStorage.ptr = OutputBuffer;
    InitStorage.len = OutputBufferSize;
    QCBOREncode_Init(&Context, InitStorage);

    /*
    ** Open Array
    */
    QCBOREncode_OpenArray(&Context);


    /*
    ** Begin encode
    */
    PrimaryBlockReturnStatus = BPLib_CBOR_CopyOrEncodePrimary(&Context,
                                                              StoredBundle,
                                                              OutputBuffer,
                                                              OutputBufferSize,
                                                              &PrimaryBlockBytesCopied);

    if (PrimaryBlockReturnStatus != BPLIB_SUCCESS)
    {
        *NumBytesCopied = 0;
        ReturnStatus = PrimaryBlockReturnStatus;
    }
    else
    {
        /* start accumulating the total bytes copied */
        TotalBytesCopied = PrimaryBlockBytesCopied;

        /* Copy or encode the canonical blocks */
        NumberOfExtensionBlocks = BPLib_CBOR_GetNumExtensionBlocks(StoredBundle);
        for (CurrExtBlockIndex = 0; CurrExtBlockIndex < NumberOfExtensionBlocks; CurrExtBlockIndex++)
        {
            CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
            BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

            /* Encode extension block */
            ExtensionBlockReturnStatus = BPLib_CBOR_EncodeExtensionBlock(&Context,
                                                                         StoredBundle,
                                                                         CurrExtBlockIndex,
                                                                         (void*)CurrentOutputBufferAddr,
                                                                         BytesLeftInOutputBuffer,
                                                                         &ExtBlockBytesCopied);
            if (ExtensionBlockReturnStatus != BPLIB_SUCCESS)
            {
                break;
            }
            else
            {
                TotalBytesCopied += ExtBlockBytesCopied;
            }
        }

        if (ExtensionBlockReturnStatus != BPLIB_SUCCESS)
        {
            ReturnStatus = ExtensionBlockReturnStatus;
        }
        else
        {
            CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
            BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

            PayloadBlockReturnStatus = BPLib_CBOR_CopyOrEncodePayload(&Context,
                                                                      StoredBundle,
                                                                      (void*)CurrentOutputBufferAddr,
                                                                      BytesLeftInOutputBuffer,
                                                                      &PayloadBytesCopied);
            if (PayloadBlockReturnStatus == BPLIB_SUCCESS)
            {
                TotalBytesCopied += PayloadBytesCopied;
            }

            /*
            ** Inform the caller of the successful number of bytes copied
            */
            ReturnStatus = PayloadBlockReturnStatus;
        }
    }

    /*
    ** Close Array
    */
    QCBOREncode_CloseArray(&Context);


    /*
    ** Finish encoding, and check for errors
    */
    FinishBuffer.len = 0;
    FinishBuffer.ptr = NULL;
    QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);

    /*
    ** Its possible for us to have a combination of errors here
    ** If there was a previous error along with a QCBOR error, preserve and return the original error
    ** If there were no other errors and we hit a QCBOR error, report the QCBOR error
    */
    if ((QcborStatus != QCBOR_SUCCESS) && (ReturnStatus != BPLIB_SUCCESS))
    {
        *NumBytesCopied = 0;
    }
    else if (QcborStatus != QCBOR_SUCCESS)
    {
        *NumBytesCopied = 0;
        ReturnStatus = BPLIB_CBOR_ENC_BUNDLE_FINISH_ERR;
    }
    else
    {
        *NumBytesCopied = FinishBuffer.len;
    }

    return ReturnStatus;
}


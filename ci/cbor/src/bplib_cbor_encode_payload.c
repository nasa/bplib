#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePayload(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    BPLib_Status_t PayloadDataCopyStatus;
    // size_t StartOffset;
    // size_t EndOffset;
    size_t PayloadDataBytesCopied;
    UsefulBuf OutputSubBuffer;
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
        ** TODO: move this after the openArray, and create the array ourselves
        */
        InitStorage.ptr = OutputBuffer;
        InitStorage.len = OutputBufferSize;
        QCBOREncode_Init(&Context, InitStorage);

        /*
        ** Grab the start offset, to be used to calculate the total encoded size
        */
        // StartOffset = QCBOREncode_Tell(&Context);

        /*
        ** Open Canonical/Payload Block Array
        */
        QCBOREncode_OpenArray(&Context);

        /*
        ** Add our block header data
        */
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BlockType);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BlockNum);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.BundleProcFlags);
        QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PayloadHeader.CrcType);

        /*
        ** Add the ADU data
        */
        QCBOREncode_OpenBytes(&Context, &OutputSubBuffer);

        PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
            StoredBundle->blocks.PayloadHeader.DataOffsetStart,
            StoredBundle->blocks.PayloadHeader.DataOffsetSize,
            OutputSubBuffer.ptr,
            OutputSubBuffer.len);

        if (PayloadDataCopyStatus == BPLIB_SUCCESS)
        {
            PayloadDataBytesCopied = StoredBundle->blocks.PayloadHeader.DataOffsetSize;
        }
        else
        {
            PayloadDataBytesCopied = 0;
        }

        QCBOREncode_CloseBytes(&Context, PayloadDataBytesCopied);

        /*
        ** Add the CRC
        */
        /* TODO: calculate the CRC first */
        (void) BPLib_CBOR_EncodeCrcValue(&Context, 0, StoredBundle->blocks.PayloadHeader.CrcType);

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
            ReturnStatus = BPLIB_SUCCESS;
        }
        else
        {
            *NumBytesCopied = FinishBuffer.len;
            ReturnStatus = BPLIB_SUCCESS;
        }

        // *NumBytesCopied = EndOffset - StartOffset;
        // ReturnStatus = BPLIB_SUCCESS;
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
    // UsefulBuf OutputSubBuffer;
    // size_t PayloadDataBytesCopied;
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
        */
        TotalPayloadSize = StoredBundle->blocks.PayloadHeader.BlockOffsetEnd
                         - StoredBundle->blocks.PayloadHeader.BlockOffsetStart
                         + 1;

        /*
        ** Open a byte array
        */
        // QCBOREncode_OpenBytes(Context, &OutputSubBuffer);

        /*
        ** Copy in the whole payload (header, data, and crc value)
        */
        // PayloadDataBytesCopied = 0;
        if (TotalPayloadSize > OutputBufferSize)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        #if 0
        else if (TotalPayloadSize > OutputSubBuffer.len)
        {
            ReturnStatus = BPLIB_BI_COPY_PAYLOAD_ENC_SIZE_GT_OUTPUT_ERR;
        }
        #endif
        else
        {
            PayloadDataCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                StoredBundle->blocks.PayloadHeader.BlockOffsetStart,
                TotalPayloadSize,
                OutputBuffer,
                OutputBufferSize);

            if (PayloadDataCopyStatus == BPLIB_SUCCESS)
            {
                // PayloadDataBytesCopied = TotalPayloadSize;
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
        // QCBOREncode_CloseBytes(Context, PayloadDataBytesCopied);
    }
    return ReturnStatus;
}

/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "bplib_cbor_internal.h"

BPLib_Status_t BPLib_CBOR_EncodePrimary(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;

    if ((StoredBundle == NULL) || (OutputBuffer == NULL) || (NumBytesCopied == NULL))
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

        // TODO these fields are only present in fragmented bundles, will add logic to
        // distinguish fragmented and unfragmented bundles later
        // QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.FragmentOffset);
        // QCBOREncode_AddUInt64(&Context, StoredBundle->blocks.PrimaryBlock.TotalAduLength);

        /* Set CRC value to 0, real value will be jammed in after encoding is done */
        StoredBundle->blocks.PrimaryBlock.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(&Context, StoredBundle->blocks.PrimaryBlock.CrcVal,
                                            StoredBundle->blocks.PrimaryBlock.CrcType);

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
        if (QcborStatus != QCBOR_SUCCESS)
        {
            ReturnStatus = BPLIB_CBOR_ENC_PRIM_QCBOR_FINISH_ERR;
        }
        else
        {
            /* Calculate new CRC for encoded bundle */
            BPLib_CBOR_GenerateBlockCrc(OutputBuffer, 
                                    StoredBundle->blocks.PrimaryBlock.CrcType,
                                    0, FinishBuffer.len);

            *NumBytesCopied += FinishBuffer.len;
            ReturnStatus = BPLIB_SUCCESS;
        }
    }

    return ReturnStatus;
}



BPLib_Status_t BPLib_CBOR_CopyOrEncodePrimary(BPLib_Bundle_t* StoredBundle,
                                              void* OutputBuffer,
                                              size_t OutputBufferSize,
                                              size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    size_t PrimaryBlockSize;
    BPLib_Status_t PrimaryBlockCopyStatus;

    if (StoredBundle->blocks.PrimaryBlock.RequiresEncode)
    {
        ReturnStatus = BPLib_CBOR_EncodePrimary(StoredBundle,
            OutputBuffer,
            OutputBufferSize,
            NumBytesCopied);
    }
    /* Verify that the block offset values are reasonable */
    else if (StoredBundle->blocks.PrimaryBlock.BlockOffsetStart >= StoredBundle->blocks.PrimaryBlock.BlockOffsetEnd)
    {
        *NumBytesCopied = 0;
        ReturnStatus = BPLIB_CBOR_ENC_PRIM_SIZES_CRRPTD_ERR;
    }
    else
    {
        /*
        ** Calculate the pre-encoded primary block size
        */
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
            *NumBytesCopied = 0;
            ReturnStatus = BPLIB_CBOR_ENC_PRIM_COPY_SIZE_GT_OUTPUT_ERR;
        }
        else
        {
            PrimaryBlockCopyStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                StoredBundle->blocks.PrimaryBlock.BlockOffsetStart,
                PrimaryBlockSize,
                OutputBuffer,
                OutputBufferSize);
    
            if (PrimaryBlockCopyStatus != BPLIB_SUCCESS)
            {
                *NumBytesCopied = 0;
                ReturnStatus = PrimaryBlockCopyStatus;
            }
            else
            {
                *NumBytesCopied += PrimaryBlockSize;
                ReturnStatus = BPLIB_SUCCESS;
            }
        }
    }
    return ReturnStatus;
}

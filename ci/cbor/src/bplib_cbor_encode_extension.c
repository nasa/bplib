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


uint32_t BPLib_CBOR_GetNumExtensionBlocks(BPLib_Bundle_t* StoredBundle)
{
    uint32_t ExtensionBlockIndex;
    for (ExtensionBlockIndex = 0; ExtensionBlockIndex < BPLIB_MAX_NUM_EXTENSION_BLOCKS; ExtensionBlockIndex++)
    {
        if (StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex].Header.BlockType == BPLib_BlockType_Reserved)
        {
            break;
        }
    }
    return ExtensionBlockIndex;
}

size_t BPLib_CBOR_EncodeBlockSpecificData(BPLib_ExtensionBlock_t* CurrExtBlock,
                                          UsefulBuf* CurrentContextPlace)
{
    QCBOREncodeContext Context;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;
    size_t SizeOfEncodedBlockSpecificData;

    if (CurrExtBlock == NULL)
    {
        SizeOfEncodedBlockSpecificData = 0;
    }
    else if (CurrentContextPlace == NULL)
    {
        SizeOfEncodedBlockSpecificData = 0;
    }
    else
    {
        /*
        ** Initialize the encoder.
        */
        QCBOREncode_Init(&Context, *CurrentContextPlace);

        /*
        ** Add the block-specific data
        */
        if (CurrExtBlock->Header.BlockType == BPLib_BlockType_PrevNode)
        {
            (void) BPLib_CBOR_EncodeEID(&Context, &CurrExtBlock->BlockData.PrevNodeBlockData.PrevNodeId);
        }
        else if (CurrExtBlock->Header.BlockType == BPLib_BlockType_Age)
        {
            QCBOREncode_AddUInt64(&Context, CurrExtBlock->BlockData.AgeBlockData.Age);
        }
        else if (CurrExtBlock->Header.BlockType == BPLib_BlockType_HopCount)
        {
            QCBOREncode_OpenArray(&Context);
            QCBOREncode_AddUInt64(&Context, CurrExtBlock->BlockData.HopCountData.HopLimit);
            QCBOREncode_AddUInt64(&Context, CurrExtBlock->BlockData.HopCountData.HopCount);
            QCBOREncode_CloseArray(&Context);
        }
        else
        {
            /* should we report an error here? */
        }

        /*
        ** Finish encoding, and check for errors
        */
        FinishBuffer.len = 0;
        FinishBuffer.ptr = NULL;
        QcborStatus = QCBOREncode_Finish(&Context, &FinishBuffer);
        if (QcborStatus != QCBOR_SUCCESS)
        {
            SizeOfEncodedBlockSpecificData = 0;
        }
        else
        {
            SizeOfEncodedBlockSpecificData = FinishBuffer.len;
        }
    }

    return SizeOfEncodedBlockSpecificData;
}



BPLib_Status_t BPLib_CBOR_EncodeExtensionBlock(BPLib_Bundle_t* StoredBundle,
                                               uint32_t ExtensionBlockIndex,
                                               void* OutputBuffer,
                                               size_t OutputBufferSize,
                                               size_t* NumBytesCopied)
{
    BPLib_Status_t ReturnStatus;
    QCBOREncodeContext Context;
    UsefulBuf InitStorage;
    UsefulBufC FinishBuffer;
    QCBORError QcborStatus;
    BPLib_ExtensionBlock_t* CurrExtBlock;
    UsefulBuf CurrentContextPlace;
    size_t SizeOfEncodedBlockSpecificData;
    size_t TotalBlockSize;

    if ((StoredBundle == NULL) ||
        (StoredBundle->blob == NULL) ||
        (OutputBuffer == NULL) ||
        (NumBytesCopied == NULL))
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else if (ExtensionBlockIndex >= BPLIB_MAX_NUM_EXTENSION_BLOCKS)
    {
        ReturnStatus = BPLIB_CBOR_ENC_EXT_INPUT_BLOCK_INDEX_ERR;
    }
    /* If block data couldn't be processed and we have to discard it, skip it */
    else if (StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex].Header.RequiresDiscard)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_UNPROCESSED_BLOCKS, 1);

        ReturnStatus = BPLIB_SUCCESS;
    }
    /* Unknown block - copy it all out */
    else if  (StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex].Header.BlockType == BPLib_BlockType_UNKNOWN)
    {
        if (CurrExtBlock->Header.BlockOffsetStart >= CurrExtBlock->Header.BlockOffsetEnd)
        {
            *NumBytesCopied = 0;
            ReturnStatus = BPLIB_CBOR_ENC_EXT_SIZES_CRRPTD_ERR;
        }
        else
        {
            CurrExtBlock = &StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex];
            TotalBlockSize = CurrExtBlock->Header.BlockOffsetEnd - CurrExtBlock->Header.BlockOffsetStart + 1;
            
            ReturnStatus = BPLib_MEM_CopyOutFromOffset(StoredBundle,
                        CurrExtBlock->Header.BlockOffsetStart, TotalBlockSize,
                        OutputBuffer, OutputBufferSize);
            if (ReturnStatus == BPLIB_SUCCESS)
            {
                *NumBytesCopied += TotalBlockSize;
            }
            else
            {
                *NumBytesCopied = 0;
            }
        }
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
        ** Open Definite Array
        */
        QCBOREncode_OpenArray(&Context);

        /* Select the extension block */
        CurrExtBlock = &StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex];

        /* Add the block header info */
        QCBOREncode_AddUInt64(&Context, CurrExtBlock->Header.BlockType);
        QCBOREncode_AddUInt64(&Context, CurrExtBlock->Header.BlockNum);
        QCBOREncode_AddUInt64(&Context, CurrExtBlock->Header.BlockProcFlags);
        QCBOREncode_AddUInt64(&Context, CurrExtBlock->Header.CrcType);

        /*
        ** Open block-specific data (byte string)
        */
        QCBOREncode_OpenBytes(&Context, &CurrentContextPlace);

        /*
        ** Add/encode the block-specific data
        */
        SizeOfEncodedBlockSpecificData = BPLib_CBOR_EncodeBlockSpecificData(CurrExtBlock, &CurrentContextPlace);

        /*
        ** Exit block-specific data (byte string)
        */
        QCBOREncode_CloseBytes(&Context, SizeOfEncodedBlockSpecificData);


        /* Set CRC value to 0, real value will be jammed in after encoding is done */
        CurrExtBlock->Header.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(&Context, CurrExtBlock->Header.CrcVal, CurrExtBlock->Header.CrcType);

        /*
        ** Close Definite Array
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
            ReturnStatus = BPLIB_CBOR_ENC_EXT_QCBOR_FINISH_ERR;
        }
        else
        {
            /* Calculate new CRC for encoded block */
            BPLib_CBOR_GenerateBlockCrc(OutputBuffer, 
                StoredBundle->blocks.ExtBlocks[ExtensionBlockIndex].Header.CrcType,
                0, FinishBuffer.len);
               
            *NumBytesCopied += FinishBuffer.len;
            ReturnStatus = BPLIB_SUCCESS;
        }
    }

    return ReturnStatus;
}

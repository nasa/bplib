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

    if (StoredBundle == NULL)
    {
        ReturnStatus = BPLIB_NULL_PTR_ERROR;
    }
    else if (StoredBundle->blob == NULL)
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
    else if (ExtensionBlockIndex >= BPLIB_MAX_NUM_EXTENSION_BLOCKS)
    {
        ReturnStatus = BPLIB_ERROR;
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


        /* start with CRC value 0. actual value to be filled in later. */
        CurrExtBlock->Header.CrcVal = 0;
        BPLib_CBOR_EncodeCrcValue(&Context, CurrExtBlock->Header.CrcVal, CurrExtBlock->Header.CrcType);

        /*
        ** Open Definite Array
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
            *NumBytesCopied = 0;
        }
        else
        {
            *NumBytesCopied = FinishBuffer.len;
        }

        ReturnStatus = BPLIB_SUCCESS;
    }

    return ReturnStatus;
}

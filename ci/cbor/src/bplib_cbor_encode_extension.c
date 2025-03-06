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


BPLib_Status_t BPLib_CBOR_EncodeExtensionBlock(BPLib_Bundle_t* StoredBundle,
                                               uint32_t ExtensionBlockIndex,
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

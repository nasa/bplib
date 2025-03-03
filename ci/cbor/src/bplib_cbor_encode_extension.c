#include "bplib_cbor_internal.h"

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

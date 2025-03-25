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

BPLib_Status_t BPLib_CBOR_EncodeBundle(BPLib_Bundle_t* StoredBundle,
                                        void* OutputBuffer,
                                        size_t OutputBufferSize,
                                        size_t* NumBytesCopied)
{
    BPLib_Status_t PrimaryBlockReturnStatus;
    BPLib_Status_t ExtensionBlockReturnStatus;
    BPLib_Status_t PayloadBlockReturnStatus;
    size_t TotalBytesCopied;
    uint32_t NumberOfExtensionBlocks;
    uint32_t CurrExtBlockIndex;
    uintptr_t CurrentOutputBufferAddr;
    size_t BytesLeftInOutputBuffer;

    if ((StoredBundle == NULL) || (StoredBundle->blob == NULL) || 
        (OutputBuffer == NULL) || (NumBytesCopied == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    /*
    ** The output buffer should at least have room for the CBOR indefinite array
    ** (meaning `0x9F` at the front, and `0xFF` at the end).
    ** We don't have a good way to predict the encoded block sizes.
    */
    if (OutputBufferSize <= 2)
    {
        return BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_1_ERR;
    }

    /*
    ** Jam in an "open indefinite array" character
    ** Major Type: 4 (array)
    ** Additional Info: indefinite length (31, or 0x1F)
    */
    CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer);
    *(uint8_t*)CurrentOutputBufferAddr = 0x9F;
    TotalBytesCopied = 1;
    CurrentOutputBufferAddr++;
    BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

    /*
    ** Begin encode
    */
    PrimaryBlockReturnStatus = BPLib_CBOR_CopyOrEncodePrimary(StoredBundle,
                                                              (void*)CurrentOutputBufferAddr,
                                                              BytesLeftInOutputBuffer,
                                                              &TotalBytesCopied);

    if (PrimaryBlockReturnStatus != BPLIB_SUCCESS)
    {
        *NumBytesCopied = 0;
        return PrimaryBlockReturnStatus;
    }

    /* Copy or encode the canonical blocks */
    NumberOfExtensionBlocks = BPLib_CBOR_GetNumExtensionBlocks(StoredBundle);
    for (CurrExtBlockIndex = 0; CurrExtBlockIndex < NumberOfExtensionBlocks; CurrExtBlockIndex++)
    {
        if (OutputBufferSize <= TotalBytesCopied)
        {
            *NumBytesCopied = 0;
            return BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_2_ERR;
        }

        CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
        BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

        /* Encode extension block */
        ExtensionBlockReturnStatus = BPLib_CBOR_EncodeExtensionBlock(StoredBundle,
                                        CurrExtBlockIndex, (void*)CurrentOutputBufferAddr,
                                        BytesLeftInOutputBuffer, &TotalBytesCopied);
        if (ExtensionBlockReturnStatus != BPLIB_SUCCESS)
        {
            *NumBytesCopied = 0;
            return ExtensionBlockReturnStatus;
        }
    }

    /* make sure we have more room in our output buffer, before we try to add the payload */
    if (OutputBufferSize <= TotalBytesCopied)
    {
        *NumBytesCopied = 0;
        return BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_3_ERR;
    }

    CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
    BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

    PayloadBlockReturnStatus = BPLib_CBOR_CopyOrEncodePayload(StoredBundle,
                                                                (void*)CurrentOutputBufferAddr,
                                                                BytesLeftInOutputBuffer,
                                                                &TotalBytesCopied);
    if (PayloadBlockReturnStatus != BPLIB_SUCCESS)
    {
        *NumBytesCopied = 0;
        return PayloadBlockReturnStatus;
    }

    if (OutputBufferSize <= TotalBytesCopied)
    {
        *NumBytesCopied = 0;
        return BPLIB_CBOR_ENC_BUNDLE_OUTPUT_BUF_LEN_4_ERR;
    }

    /*
    ** Close the indefinite-length array
    */
    CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
    *(uint8_t*)CurrentOutputBufferAddr = 0xFF;
    CurrentOutputBufferAddr++;
    TotalBytesCopied++;

    /*
    ** Set the output size
    */
    *NumBytesCopied = TotalBytesCopied;

    #if (BPLIB_CBOR_DEBUG_PRINTS_ENABLED)
    printf("Output encoded bundle generated with size %lu: \n", *NumBytesCopied);
    for (size_t i = 0 ; i < *NumBytesCopied; i++)
    {
        printf("0x%02x, ", ((uint8_t*)OutputBuffer)[i]);
        if (((i+1) % 8) == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
    #endif

    return BPLIB_SUCCESS;
}


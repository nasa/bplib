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
    BPLib_Status_t ReturnStatus;
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
                                                              &PrimaryBlockBytesCopied);

    if (PrimaryBlockReturnStatus != BPLIB_SUCCESS)
    {
        ReturnStatus = PrimaryBlockReturnStatus;
    }
    else
    {
        /* start accumulating the total bytes copied */
        TotalBytesCopied += PrimaryBlockBytesCopied;

        /* Copy or encode the canonical blocks */
        NumberOfExtensionBlocks = BPLib_CBOR_GetNumExtensionBlocks(StoredBundle);
        for (CurrExtBlockIndex = 0; CurrExtBlockIndex < NumberOfExtensionBlocks; CurrExtBlockIndex++)
        {
            CurrentOutputBufferAddr = (uintptr_t)(OutputBuffer) + TotalBytesCopied;
            BytesLeftInOutputBuffer = OutputBufferSize - TotalBytesCopied;

            /* Encode extension block */
            ExtensionBlockReturnStatus = BPLib_CBOR_EncodeExtensionBlock(StoredBundle,
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

            PayloadBlockReturnStatus = BPLib_CBOR_CopyOrEncodePayload(StoredBundle,
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
    ** Close the indefinte-length Array
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

    return ReturnStatus;
}


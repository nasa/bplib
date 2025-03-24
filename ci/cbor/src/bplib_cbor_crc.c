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
#include "bplib_crc.h"

BPLib_Status_t BPLib_CBOR_ValidateBlockCrc(const void *EncodedBundle, 
                    BPLib_CRC_Type_t CrcType, size_t CrcOffset, BPLib_CRC_Val_t ExpectedCrc, 
                    size_t BlockOffsetStart, size_t BlockOffsetEnd)
{
    BPLib_CRC_Val_t CalculatedCrc;
    size_t          BlockLength;
    uint8_t         CrcLen = 0;

    if (CrcType == BPLib_CRC_Type_CRC16)
    {
        CrcLen = 2;
    }
    else if (CrcType == BPLib_CRC_Type_CRC32C)
    {
        CrcLen = 4;
    }
    else 
    {
        return BPLIB_SUCCESS;
    }

    /* Zero out the CRC entry for calculation purposes */
    memset((void *) ((uintptr_t) EncodedBundle + CrcOffset), 0, CrcLen);

    /* Getting length of block, adding 1 to make last byte inclusive */
    BlockLength = BlockOffsetEnd - BlockOffsetStart + 1;

    /* Calculate the CRC of the block */
    CalculatedCrc = BPLib_CRC_Calculate((void *) ((uintptr_t) EncodedBundle + BlockOffsetStart), 
                                BlockLength, CrcType);

    printf("\nCalculated CRC is 0x%lx, blocklength is %ld, block offset start is %ld\n", 
            CalculatedCrc, BlockLength, BlockOffsetStart);

    /* Verify the calculated CRC matches the expected CRC */
    if (CalculatedCrc != ExpectedCrc)
    {
        return BPLIB_INVALID_CRC_ERROR;
    }

    /* Repopulate the byte array with the CRC value */
    memcpy((void *) ((uintptr_t)EncodedBundle + CrcOffset), &ExpectedCrc, CrcLen);

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_GenerateBlockCrc(const void *EncodedBundle, 
                                BPLib_CRC_Type_t CrcType, size_t CrcOffset,
                                size_t BlockOffsetStart, size_t BlockOffsetEnd)
{
    BPLib_CRC_Val_t CalculatedCrc;
    size_t          BlockLength;
    uint8_t         CrcLen = 0;

    if (CrcType == BPLib_CRC_Type_CRC16)
    {
        CrcLen = 2;
    }
    else if (CrcType == BPLib_CRC_Type_CRC32C)
    {
        CrcLen = 4;
    }
    else 
    {
        return;
    }

    /* Zero out the CRC entry for calculation purposes */
    memset((void *) ((uintptr_t) EncodedBundle + CrcOffset), 0, CrcLen);

    /* Getting length of block, adding 1 to make last byte inclusive */
    BlockLength = BlockOffsetEnd - BlockOffsetStart + 1;

    /* Calculate the CRC of the block */
    CalculatedCrc = BPLib_CRC_Calculate((void *) ((uintptr_t) EncodedBundle + BlockOffsetStart), 
                                BlockLength, CrcType);

    printf("\nGenerated CRC is 0x%lx, blocklength is %ld, block offset start is %ld\n", 
            CalculatedCrc, BlockLength, BlockOffsetStart);

    /* Repopulate the byte array with the CRC value */
    memcpy((void *) ((uintptr_t)EncodedBundle + CrcOffset), &CalculatedCrc, CrcLen);

    return;
}

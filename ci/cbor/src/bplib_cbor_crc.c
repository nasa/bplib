/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

#include "bplib_cbor_internal.h"
#include "bplib_crc.h"
#include "bplib_cfg.h"

/* On little-endian systems, copy the CRC into the buffer by swapping the bytes properly */
void BPLib_CBOR_CopyLittleEndianCrc(BPLib_CRC_Val_t CrcVal, BPLib_CRC_Type_t CrcType, uint8_t *Buf)
{
    if (CrcType == BPLib_CRC_Type_CRC16)
    {
        Buf[0] = (uint8_t) ((CrcVal >> 8)  & 0xFF);
        Buf[1] = (uint8_t) ((CrcVal >> 0)  & 0xFF);  
    }
    else if (CrcType == BPLib_CRC_Type_CRC32C)
    {
        Buf[0] = (uint8_t) ((CrcVal >> 24) & 0xFF);
        Buf[1] = (uint8_t) ((CrcVal >> 16) & 0xFF);
        Buf[2] = (uint8_t) ((CrcVal >> 8)  & 0xFF);
        Buf[3] = (uint8_t) ((CrcVal >> 0)  & 0xFF);
    }

    return;
}

BPLib_Status_t BPLib_CBOR_ValidateBlockCrc(const void *EncodedBundle, 
                    BPLib_CRC_Type_t CrcType, BPLib_CRC_Val_t ExpectedCrc, 
                    size_t BlockOffsetStart, size_t BlockLength)
{
    BPLib_CRC_Val_t CalculatedCrc;
    uint8_t         CrcLen = 0;
    size_t          CrcOffsetStart;

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

    CrcOffsetStart = BlockOffsetStart + BlockLength - CrcLen;

   /* Zero out the CRC entry for calculation purposes */
    memset((void *) ((uintptr_t) EncodedBundle + CrcOffsetStart), 0, CrcLen);

    /* Calculate the CRC of the block */
    CalculatedCrc = BPLib_CRC_Calculate((void *) ((uintptr_t) EncodedBundle + BlockOffsetStart), 
                                BlockLength, CrcType);

    /* Verify the calculated CRC matches the expected CRC */
    if (CalculatedCrc != ExpectedCrc)
    {
        return BPLIB_INVALID_CRC_ERROR;
    }

    /* Repopulate the byte array with the CRC value */
    #if BPLIB_SYS_BIG_ENDIAN == true
    memcpy((void *) ((uintptr_t)EncodedBundle + CrcOffsetStart), &CalculatedCrc, CrcLen);
    #else
    BPLib_CBOR_CopyLittleEndianCrc(CalculatedCrc, CrcType, (uint8_t *) ((uintptr_t)EncodedBundle + CrcOffsetStart));
    #endif

    return BPLIB_SUCCESS;
}

void BPLib_CBOR_GenerateBlockCrc(const void *EncodedBundle, BPLib_CRC_Type_t CrcType,
                                size_t BlockOffsetStart, size_t BlockLength)
{
    BPLib_CRC_Val_t CalculatedCrc;
    uint8_t         CrcLen = 0;
    size_t          CrcOffsetStart;

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

    CrcOffsetStart = BlockOffsetStart + BlockLength - CrcLen;

    /* Calculate the CRC of the block */
    CalculatedCrc = BPLib_CRC_Calculate((void *) ((uintptr_t) EncodedBundle + BlockOffsetStart), 
                                BlockLength, CrcType);

    /* Repopulate the byte array with the CRC value */
    #if BPLIB_SYS_BIG_ENDIAN == true
    memcpy((void *) ((uintptr_t)EncodedBundle + CrcOffsetStart), &CalculatedCrc, CrcLen);
    #else
    BPLib_CBOR_CopyLittleEndianCrc(CalculatedCrc, CrcType, (uint8_t *) ((uintptr_t)EncodedBundle + CrcOffsetStart));
    #endif
    
    return;
}

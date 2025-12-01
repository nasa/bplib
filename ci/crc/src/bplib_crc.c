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

/*
** Include
*/

#include "bplib_api_types.h"
#include "bplib_crc.h"
#include "bplib_crc_private.h"


/*
** Global data
*/

/*
 * Various lookup and translation tables for CRC calculation
 */
uint8_t BPLIB_CRC_REFLECT_TABLE[256];

static uint16_t BPLIB_CRC16_X25_TABLE[256];
static uint32_t BPLIB_CRC32_C_TABLE[256];


/*
 * Global definition of CRC-16/X.25 algorithm
 */
BPLib_CRC_Parameters_t BPLIB_CRC16_X25 = {
            .Length              = 16,
            .ShouldReflectOutput = true,
            .Digest              = (BPLib_CRC_DigestFunc_t) BPLib_CRC_GetCrc16X25,
            .FinalXor            = 0xFFFF
};

/*
 * Global definition of CRC-32/Castagnoli algorithm
 */
BPLib_CRC_Parameters_t BPLIB_CRC32_CASTAGNOLI = {
            .Length              = 32,
            .ShouldReflectOutput = true,
            .Digest              = (BPLib_CRC_DigestFunc_t) BPLib_CRC_GetCrc32Castagnoli,
            .FinalXor            = 0xFFFFFFFF
};


/*
** Internal Functions
*/

/* Calculates the CRC-16/X.25 of the provided data */
uint16_t BPLib_CRC_GetCrc16X25(const uint8_t *DataPtr, size_t DataLen)
{
    uint16_t CrcVal = 0xFFFF; /* Starting value */

    while (DataLen > 0)
    {
        CrcVal = BPLIB_CRC16_X25_TABLE[((CrcVal >> 8) ^ BPLIB_CRC_REFLECT_TABLE[*DataPtr]) & 0xFF] ^ (CrcVal << 8);
        DataPtr++;
        DataLen--;
    }

    return CrcVal;
}

/* Calculates the CRC-32/Castagnoli value of the provided data */
uint32_t BPLib_CRC_GetCrc32Castagnoli(const uint8_t *DataPtr, size_t DataLen)
{
    uint32_t CrcVal = 0xFFFFFFFF; /* Starting value */

    while (DataLen > 0)
    {
        CrcVal = BPLIB_CRC32_C_TABLE[((CrcVal >> 24) ^ BPLIB_CRC_REFLECT_TABLE[*DataPtr]) & 0xFF] ^ (CrcVal << 8);
        
        DataPtr++;
        DataLen--;
    }

    return CrcVal;
}

uint32_t BPLib_CRC_PrecomputeCrcByte(uint8_t Width, uint8_t Byte, uint32_t Polynomial)
{
    uint8_t  Mask = 0xFF;
    uint32_t NextBit;
    uint32_t CrcVal;

    /* Left-justifies the byte and polynomial in the registers */
    CrcVal = (uint32_t) Byte << 24;
    Polynomial <<= (32 - Width);

    while (Mask != 0)
    {
        Mask <<= 1;

        NextBit = CrcVal & 0x80000000U;
        CrcVal <<= 1;

        if (NextBit)
        {
            CrcVal ^= Polynomial;
        }
    }

    /* now align it and mask result based on the width of the CRC */
    if (Width < 32)
    {
        CrcVal >>= 32 - Width;
        CrcVal &= ((uint32_t) 1 << Width) - 1;
    }

    return CrcVal;
}

uint8_t BPLib_CRC_PrecomputeReflection(uint8_t Byte)
{
    uint8_t Mask = 0xFF;
    uint8_t Input = Byte;
    uint8_t Result = 0;

    while (Mask != 0)
    {
        Mask <<= 1;

        /*
         * This uses left shifts because technically
         * a right shift may not inject "0" bits, but
         * a left shift always will.
         */
        Result <<= 1;
        Result |= (Input & 1);
        Input >>= 1;
    }

    return Result;
}

/* Finalize the CRC value */
BPLib_CRC_Val_t BPLib_CRC_Finalize(BPLib_CRC_Parameters_t *Params, BPLib_CRC_Val_t Crc)
{
    BPLib_CRC_Val_t FinalCrc = 0;
    uint8_t     i;

    /* Reflect 8 bits at a time while possible */
    i = Params->Length;
    while (i >= 8)
    {
        FinalCrc <<= 8;
        FinalCrc |= BPLIB_CRC_REFLECT_TABLE[Crc & 0xFF];
        Crc >>= 8;
        i -= 8;
    }

    FinalCrc ^= Params->FinalXor;

    /* Return the CRC but mask out the significant bits */
    FinalCrc &= ((BPLib_CRC_Val_t)1 << Params->Length) - 1;

    return FinalCrc;
}


/*
** Exported functions
*/

/* Initialize the CRC tables */
void BPLib_CRC_Init(void)
{
    uint8_t Byte;

    Byte = 0;
    do
    {
        BPLIB_CRC_REFLECT_TABLE[Byte] = BPLib_CRC_PrecomputeReflection(Byte);

        /*
         * Note that the "width" passed to this function should indicate the
         * table data type, not necessarily the width of the CRC (although for the
         * two implemented algorithms, they are the same)
         */
        BPLIB_CRC16_X25_TABLE[Byte] = BPLib_CRC_PrecomputeCrcByte(16, Byte, BPLIB_CRC16_X25_POLY);
        BPLIB_CRC32_C_TABLE[Byte]   = BPLib_CRC_PrecomputeCrcByte(32, Byte, BPLIB_CRC32_C_POLY);

        Byte++;
    }
    while (Byte != 0);
}

/* Calculate the CRC on the provided data */
BPLib_CRC_Val_t BPLib_CRC_Calculate(const void *Data, size_t DataLen, BPLib_CRC_Type_t CrcType)
{
    BPLib_CRC_Parameters_t *Params;

    if (CrcType == BPLib_CRC_Type_CRC16)
    {
        Params = &BPLIB_CRC16_X25;
    }
    else if (CrcType == BPLib_CRC_Type_CRC32C)
    {
        Params = &BPLIB_CRC32_CASTAGNOLI;
    }
    /* Assume any other types are CRC-None and return 0 */
    else 
    {
        return 0;
    }

    /* Calculate CRC */
    return BPLib_CRC_Finalize(Params, Params->Digest(Data, DataLen));
}

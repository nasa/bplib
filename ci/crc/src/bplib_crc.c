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

/******************************************************************************
 INCLUDES
 ******************************************************************************/
#include "bplib_api_types.h"
#include "bplib_crc.h"
#include "bplib_crc_private.h"

#include <stdio.h>

/******************************************************************************
 FILE DATA
 ******************************************************************************/

/*
 * Various lookup and translation tables for CRC calculation
 * These can be mixed and matched for different algorithms
 */
uint8_t BPLIB_CRC_DIRECT_TABLE[256];
uint8_t BPLIB_CRC_REFLECT_TABLE[256];

static uint16_t BPLIB_CRC16_X25_TABLE[256];
static uint32_t BPLIB_CRC32_C_TABLE[256];

/*
 * Digest function/wrapper that does nothing
 */
static BPLib_CRC_Val_t BPLib_CRCDigestNOOP(BPLib_CRC_Val_t crc, const void *ptr, size_t size);

/*
 * Digest function/wrapper specific for CRC16 X.25 algorithm
 */
static BPLib_CRC_Val_t BPLib_CRCDigestCRC16X25(BPLib_CRC_Val_t crc, const void *ptr, size_t size);

/*
 * Digest function/wrapper specific for CRC32 Castagnoli algorithm
 */
static BPLib_CRC_Val_t BPLib_CRCDigestCRC32CASTAGNOLI(BPLib_CRC_Val_t crc, const void *ptr, size_t size);

/*
 * Global definition of "No CRC" algorithm
 * This is a placeholder that can be used when no CRC is desired, it provides a digest
 * function that does nothing.  It will always generate a CRC of "0".
 */
BPLib_CRC_Parameters_t BPLIB_CRC_NONE = {.name = "No CRC", .digest = BPLib_CRCDigestNOOP};

/*
 * Global definition of CRC16 X.25 algorithm
 */
BPLib_CRC_Parameters_t BPLIB_CRC16_X25 = {.length                = 16,
                                          .should_reflect_output = true,
                                          .digest                = BPLib_CRCDigestCRC16X25,
                                          .initial_value         = 0xFFFF,
                                          .final_xor             = 0xFFFF};

/*
 * Global definition of CRC32 Castagnoli algorithm
 */
BPLib_CRC_Parameters_t BPLIB_CRC32_CASTAGNOLI = {.name                  = "CRC-32 Castagnoli",
                                                 .length                = 32,
                                                 .should_reflect_output = true,
                                                 .digest                = BPLib_CRCDigestCRC32CASTAGNOLI,
                                                 .initial_value         = 0xFFFFFFFF,
                                                 .final_xor             = 0xFFFFFFFF};

/******************************************************************************
 STATIC FUNCTIONS
 ******************************************************************************/

// static uint16_t BPLib_CRCGeneric16Impl(const uint8_t *input_table, const uint16_t *xor_table, uint16_t crc,
//                                          const uint8_t *ptr, size_t size)
// {
//     printf("Calculating CRC\n");
//     while (size > 0)
//     {
//         printf("0x%02x ", *ptr);
//         crc = xor_table[((crc >> 8) ^ input_table[*ptr]) & 0xFF] ^ (crc << 8);
//         ++ptr;
//         --size;
//     }

//     printf("\nCRC = 0x%x\n", crc);

//     return crc;
// }

static uint16_t BPLib_CRC_GetCrc16(uint16_t CrcVal, const uint8_t *DataPtr, size_t DataLen)
{
    while (DataLen > 0)
    {
        printf("0x%02x ", *DataPtr);
        CrcVal = BPLIB_CRC16_X25_TABLE[((CrcVal >> 8) ^ BPLIB_CRC_REFLECT_TABLE[*DataPtr]) & 0xFF] ^ (CrcVal << 8);
        DataPtr++;
        DataLen--;
    }

    return CrcVal;
}

// static uint32_t BPLib_CRCGeneric32Impl(const uint8_t *input_table, const uint32_t *xor_table, uint32_t crc,
//                                          const uint8_t *ptr, size_t size)
// {
//     while (size > 0)
//     {
//         crc = xor_table[((crc >> 24) ^ input_table[*ptr]) & 0xFF] ^ (crc << 8);
//         ++ptr;
//         --size;
//     }

//     return crc;
// }

static uint32_t BPLib_CRC_GetCrc32(uint32_t CrcVal, const uint8_t *DataPtr, size_t DataLen)
{
    while (DataLen > 0)
    {
        CrcVal = BPLIB_CRC32_C_TABLE[((CrcVal >> 24) ^ BPLIB_CRC_REFLECT_TABLE[*DataPtr]) & 0xFF] ^ (CrcVal << 8);
        
        DataPtr++;
        DataLen--;
    }

    return CrcVal;
}

BPLib_CRC_Val_t BPLib_CRCDigestNOOP(BPLib_CRC_Val_t crc, const void *ptr, size_t size)
{
    return crc;
}

BPLib_CRC_Val_t BPLib_CRCDigestCRC16X25(BPLib_CRC_Val_t crc, const void *ptr, size_t size)
{
    return BPLib_CRC_GetCrc16(crc, ptr, size);
//    return BPLib_CRCGeneric16Impl(BPLIB_CRC_REFLECT_TABLE, BPLIB_CRC16_X25_TABLE, crc, ptr, size);
}

BPLib_CRC_Val_t BPLib_CRCDigestCRC32CASTAGNOLI(BPLib_CRC_Val_t crc, const void *ptr, size_t size)
{
    return BPLib_CRC_GetCrc32(crc, ptr, size);
//    return BPLib_CRCGeneric32Impl(BPLIB_CRC_REFLECT_TABLE, BPLIB_CRC32_C_TABLE, crc, ptr, size);
}

BPLib_CRC_Val_t PrecomputeCrcByte(uint8_t width, uint8_t byte, BPLib_CRC_Val_t polynomial)
{
    uint8_t     mask;
    BPLib_CRC_Val_t next_bit;
    BPLib_CRC_Val_t crcval;

    /* left-justifies the byte and polynomial in the registers */
    /* This is based on the "BPLib_CRC_Val_t" type being 32 bits wide */
    crcval = (BPLib_CRC_Val_t)byte << 24;
    polynomial <<= 32 - width;

    mask = 0xFF;
    while (mask != 0)
    {
        mask <<= 1;

        next_bit = crcval & 0x80000000U;
        crcval <<= 1;

        if (next_bit)
        {
            crcval ^= polynomial;
        }
    }

    /* now align it and mask result based on the width of the CRC */
    if (width < 32)
    {
        crcval >>= 32 - width;
        crcval &= ((BPLib_CRC_Val_t)1 << width) - 1;
    }

    return crcval;
}

uint8_t BPLib_STOR_CACHE_PrecomputeReflection(uint8_t byte)
{
    uint8_t mask;
    uint8_t input;
    uint8_t result;

    mask   = 0xFF;
    input  = byte;
    result = 0;
    while (mask != 0)
    {
        mask <<= 1;

        /*
         * This uses left shifts because technically
         * a right shift may not inject "0" bits, but
         * a left shift always will.
         */
        result <<= 1;
        result |= (input & 1);
        input >>= 1;
    }

    return result;
}

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * BPLib_CRCInit - Inits the subsystem by creating the xor table for all defined CRC types
 *-------------------------------------------------------------------------------------*/
void BPLib_CRCInit(void)
{
    uint8_t byte;

    byte = 0;
    do
    {
        BPLIB_CRC_DIRECT_TABLE[byte]  = byte;
        BPLIB_CRC_REFLECT_TABLE[byte] = BPLib_STOR_CACHE_PrecomputeReflection(byte);

        /*
         * note that the "width" passed to this function should indicate the
         * table data type, not necessarily the width of the CRC (although for the
         * two implemented algorithms, they are the same)
         */
        BPLIB_CRC16_X25_TABLE[byte] = PrecomputeCrcByte(16, byte, BPLIB_CRC16_X25_POLY);
        BPLIB_CRC32_C_TABLE[byte]   = PrecomputeCrcByte(32, byte, BPLIB_CRC32_C_POLY);

        ++byte;
    }
    while (byte != 0);
}

const char *BPLib_CRCGetName(BPLib_CRC_Parameters_t *params)
{
    return params->name;
}

uint8_t BPLib_CRCGetWidth(BPLib_CRC_Parameters_t *params)
{
    return params->length;
}

BPLib_CRC_Val_t BPLib_CRCInitialValue(BPLib_CRC_Parameters_t *params)
{
    return params->initial_value;
}

BPLib_CRC_Val_t BPLib_CRCUpdate(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc, const void *data, size_t size)
{
    return params->digest(crc, data, size);
}

BPLib_CRC_Val_t BPLib_CRCFinalize(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc)
{
    BPLib_CRC_Val_t crc_final;
    uint8_t     i;

    if (params->should_reflect_output)
    {
        crc_final = 0;

        /* Reflect 8 bits at a time while possible */
        i = params->length;
        while (i >= 8)
        {
            crc_final <<= 8;
            crc_final |= BPLIB_CRC_REFLECT_TABLE[crc & 0xFF];
            crc >>= 8;
            i -= 8;
        }

        /* Reflect any more bits */
        while (i > 0)
        {
            crc_final <<= 1;
            crc_final |= (crc & 1);
            crc >>= 1;
            --i;
        }
    }
    else
    {
        crc_final = crc;
    }

    crc_final ^= params->final_xor;

    /* Return the CRC but mask out the significant bits */
    if (params->length < 32)
    {
        crc_final &= ((BPLib_CRC_Val_t)1 << params->length) - 1;
    }
    return crc_final;
}

/*--------------------------------------------------------------------------------------
 * BPLib_CRCGet - Calculates the CRC from a byte array using the crc provided as params.
 *      crc_init must be called on the provided params before every calling this function.
 *
 * data: A ptr to a byte array containing data to calculate a CRC over. [INPUT]
 * length: The length of the provided data in bytes. [INPUT]
 * params: A ptr to a BPLib_CRC_Parameters_t struct defining how to calculate the crc and has
 *      an XOR lookup table. [INPUT]
 *
 * returns: A crc remainder of the provided data. If a crc length is used that is less
 *      than the returned data type size than expect it to be cast.
 *-------------------------------------------------------------------------------------*/
BPLib_CRC_Val_t BPLib_CRCGet(const void *data, const uint32_t length, BPLib_CRC_Parameters_t *params)
{
    return BPLib_CRCFinalize(params, params->digest(params->initial_value, data, length));
}

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

    return BPLib_CRCFinalize(Params, Params->digest(Params->initial_value, Data, DataLen));
}

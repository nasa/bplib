/************************************************************************
 * File: crc.c
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Alexander Meade, Code 582 NASA GSFC
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"
#include "crc.h"

/******************************************************************************
 FILE DATA
 ******************************************************************************/

#define BPLIB_CRC16_X25_POLY 0x1021U
#define BPLIB_CRC32_C_POLY   0x1EDC6F41U

/*
 * Various lookup and translation tables for CRC calculation
 * These can be mixed and matched for different algorithms
 */
static uint8_t BPLIB_CRC_DIRECT_TABLE[256];
static uint8_t BPLIB_CRC_REFLECT_TABLE[256];

static uint16_t BPLIB_CRC16_X25_TABLE[256];
static uint32_t BPLIB_CRC32_C_TABLE[256];

/*
 * Definition of generic-ish CRC data digest function.
 * Updates the CRC based on the data in the given buffer.
 */
typedef bp_crcval_t (*bplib_crc_digest_func_t)(bp_crcval_t crc, const void *data, size_t size);

/*
 * Digest function/wrapper that does nothing
 */
static bp_crcval_t bplib_crc_digest_NOOP(bp_crcval_t crc, const void *ptr, size_t size);

/*
 * Digest function/wrapper specific for CRC16 X.25 algorithm
 */
static bp_crcval_t bplib_crc_digest_CRC16_X25(bp_crcval_t crc, const void *ptr, size_t size);

/*
 * Digest function/wrapper specific for CRC32 Castagnoli algorithm
 */
static bp_crcval_t bplib_crc_digest_CRC32_CASTAGNOLI(bp_crcval_t crc, const void *ptr, size_t size);

/*
 * Actual definition of CRC parameters
 */
struct bplib_crc_parameters
{
    const char *name;                  /* Name of the CRC. */
    uint8_t     length;                /* The number of bits in the CRC. */
    bool        should_reflect_output; /* Whether to reflect the bits of the output crc. */

    const uint8_t *input_table; /* A ptr to a table for input translation (reflect or direct) */
    const void    *xor_table;   /* A ptr to a table with the precomputed XOR values. */

    bplib_crc_digest_func_t digest; /* externally-callable "digest" routine to update CRC with new data */

    bp_crcval_t initial_value; /* The value used to initialize a CRC (normalized). */
    bp_crcval_t final_xor;     /* The final value to xor with the crc before returning (normalized). */
};

/*
 * Global definition of "No CRC" algorithm
 * This is a placeholder that can be used when no CRC is desired, it provides a digest
 * function that does nothing.  It will always generate a CRC of "0".
 */
bplib_crc_parameters_t BPLIB_CRC_NONE = {
    .name   = "No CRC",
    .digest = bplib_crc_digest_NOOP
};

/*
 * Global definition of CRC16 X.25 algorithm
 */
bplib_crc_parameters_t BPLIB_CRC16_X25 = {
    .name                  = "CRC-16 X25",
    .length                = 16,
    .should_reflect_output = true,
    .digest                = bplib_crc_digest_CRC16_X25,
    .initial_value         = 0xFFFF,
    .final_xor             = 0xFFFF};

/*
 * Global definition of CRC32 Castagnoli algorithm
 */
bplib_crc_parameters_t BPLIB_CRC32_CASTAGNOLI = {
    .name                  = "CRC-32 Castagnoli",
    .length                = 32,
    .should_reflect_output = true,
    .digest                = bplib_crc_digest_CRC32_CASTAGNOLI,
    .initial_value         = 0xFFFFFFFF,
    .final_xor             = 0xFFFFFFFF
};

/******************************************************************************
 STATIC FUNCTIONS
 ******************************************************************************/

static uint16_t bplib_crc_generic16_impl(const uint8_t *input_table, const uint16_t *xor_table, uint16_t crc,
                                            const uint8_t *ptr, size_t size)
{
    while (size > 0)
    {
        crc = xor_table[((crc >> 8) ^ input_table[*ptr]) & 0xFF] ^ (crc << 8);
        ++ptr;
        --size;
    }

    return crc;
}

static uint32_t bplib_crc_generic32_impl(const uint8_t *input_table, const uint32_t *xor_table, uint32_t crc,
                                            const uint8_t *ptr, size_t size)
{
    while (size > 0)
    {
        crc = xor_table[((crc >> 24) ^ input_table[*ptr]) & 0xFF] ^ (crc << 8);
        ++ptr;
        --size;
    }

    return crc;
}

bp_crcval_t bplib_crc_digest_NOOP(bp_crcval_t crc, const void *ptr, size_t size)
{
    return crc;
}

bp_crcval_t bplib_crc_digest_CRC16_X25(bp_crcval_t crc, const void *ptr, size_t size)
{
    return bplib_crc_generic16_impl(BPLIB_CRC_REFLECT_TABLE, BPLIB_CRC16_X25_TABLE, crc, ptr, size);
}

bp_crcval_t bplib_crc_digest_CRC32_CASTAGNOLI(bp_crcval_t crc, const void *ptr, size_t size)
{
    return bplib_crc_generic32_impl(BPLIB_CRC_REFLECT_TABLE, BPLIB_CRC32_C_TABLE, crc, ptr, size);
}

bp_crcval_t bplib_precompute_crc_byte(uint8_t width, uint8_t byte, bp_crcval_t polynomial)
{
    uint8_t mask;
    bp_crcval_t next_bit;
    bp_crcval_t crcval;

    /* left-justifies the byte and polynomial in the registers */
    /* This is based on the "bp_crcval_t" type being 32 bits wide */
    crcval = (bp_crcval_t)byte << 24;
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
        crcval &= ((bp_crcval_t)1 << width) - 1;
    }

    return crcval;
}

uint8_t bplib_precompute_reflection(uint8_t byte)
{
    uint8_t mask;
    uint8_t input;
    uint8_t result;

    mask = 0xFF;
    input = byte;
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
 * bplib_crc_init - Inits the subsystem by creating the xor table for all defined CRC types
 *-------------------------------------------------------------------------------------*/
void bplib_crc_init(void)
{
    uint8_t  byte;

    byte = 0;
    do
    {
        BPLIB_CRC_DIRECT_TABLE[byte]  = byte;
        BPLIB_CRC_REFLECT_TABLE[byte] = bplib_precompute_reflection(byte);

        /*
         * note that the "width" passed to this function should indicate the
         * table data type, not necessarily the width of the CRC (although for the
         * two implemented algorithms, they are the same)
         */
        BPLIB_CRC16_X25_TABLE[byte] = bplib_precompute_crc_byte(16, byte, BPLIB_CRC16_X25_POLY);
        BPLIB_CRC32_C_TABLE[byte]   = bplib_precompute_crc_byte(32, byte, BPLIB_CRC32_C_POLY);

        ++byte;
    }
    while (byte != 0);
}

const char *bplib_crc_get_name(bplib_crc_parameters_t *params)
{
    return params->name;
}

uint8_t     bplib_crc_get_width(bplib_crc_parameters_t *params)
{
    return params->length;
}

bp_crcval_t bplib_crc_initial_value(bplib_crc_parameters_t *params)
{
    return params->initial_value;
}

bp_crcval_t bplib_crc_update(bplib_crc_parameters_t *params, bp_crcval_t crc, const void *data, size_t size)
{
    return params->digest(crc, data, size);
}

bp_crcval_t bplib_crc_finalize(bplib_crc_parameters_t *params, bp_crcval_t crc)
{
    bp_crcval_t crc_final;
    uint8_t     i;

    if (params->should_reflect_output)
    {
        crc_final = 0;

        /* Reflect 8 bits at a time while possible */
        i  = params->length;
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
    if(params->length < 32)
    {
        crc_final &= ((bp_crcval_t)1 << params->length) - 1;
    }
    return crc_final;
}

/*--------------------------------------------------------------------------------------
 * bplib_crc_get - Calculates the CRC from a byte array using the crc provided as params.
 *      crc_init must be called on the provided params before every calling this function.
 *
 * data: A ptr to a byte array containing data to calculate a CRC over. [INPUT]
 * length: The length of the provided data in bytes. [INPUT]
 * params: A ptr to a bplib_crc_parameters_t struct defining how to calculate the crc and has
 *      an XOR lookup table. [INPUT]
 *
 * returns: A crc remainder of the provided data. If a crc length is used that is less
 *      than the returned data type size than expect it to be cast.
 *-------------------------------------------------------------------------------------*/
bp_crcval_t bplib_crc_get(const uint8_t *data, const uint32_t length, bplib_crc_parameters_t *params)
{
    return bplib_crc_finalize(params, params->digest(params->initial_value, data, length));
}

/************************************************************************
 * File: crc.h
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

#ifndef _crc_h_
#define _crc_h_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BYTE_COMBOS 256 /* Number of different possible bytes. */

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/* Parameters specific to calculating 16 bit crc parameters. */
typedef struct crc16_parameters
{
    uint16_t generator_polynomial;   /* The generator polynomial used to compute the CRC. */
    uint16_t initial_value;          /* The value used to initialize a CRC. */
    uint16_t final_xor;              /* The final value to xor with the crc before returning. */
    uint16_t check_value;            /* The crc resulting from the input string "123456789". */
    uint16_t xor_table[BYTE_COMBOS]; /* A ptr to a table with the precomputed XOR values. */
} crc16_parameters_t;

/* Parameters specific to calculating 32 bit crcs. */
typedef struct crc32_parameters
{
    uint32_t generator_polynomial;   /* The generator polynomial used to compute the CRC. */
    uint32_t initial_value;          /* The value used to initialize a CRC. */
    uint32_t final_xor;              /* The final value to xor with the crc before returning. */
    uint32_t check_value;            /* The crc resulting from the input string "123456789". */
    uint32_t xor_table[BYTE_COMBOS]; /* A ptr to a table with the precomputed XOR values. */
} crc32_parameters_t;

/* Standard parameters for calculating a CRC. */
typedef struct crc_parameters
{
    const char* name;           /* Name of the CRC. */
    int length;                 /* The number of bits in the CRC. */
    bool should_reflect_input;  /* Whether to reflect the bits of the input bytes. */
    bool should_reflect_output; /* Whether to reflect the bits of the output crc. */
    /* Parameters specific to crc implementations of various lengths. The field that is populated
       within this union should directly coincide with the length member.
       Ex: If length == 16 crc16 should be popualted in this union below. */
    union {
        crc16_parameters_t crc16;
        crc32_parameters_t crc32;
    } n_bit_params;
} crc_parameters_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int         crc_init    (crc_parameters_t* params);
uint32_t    crc_get     (const uint8_t* data, const uint32_t length, const crc_parameters_t* params);

#endif /* _crc_h_ */

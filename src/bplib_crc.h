/************************************************************************
 * File: bplib_crc.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _BPLIB_CRC_H_
#define _BPLIB_CRC_H_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_crc16.h"
#include "bplib_crc32.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

// Number of different possible bytes.
#define BYTE_COMBOS 256

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

// Standard parameters for calculating a CRC.
typedef struct crc_parameters
{
    char* name;                      // Name of the CRC.
    int length;                      // The number of bits in the CRC.
    bool should_reflect_input;       // Whether to reflect the bits of the input bytes.
    bool should_reflect_output;      // Whether to reflect the bits of the output crc.
    uint32_t generator_polynomial;   // The generator polynomial used to compute the CRC.
    uint32_t initial_value;          // The value used to initialize a CRC.
    uint32_t final_xor;              // The final value to xor with the crc before returning.
    uint32_t check_value;            // The crc value resulting from the input string "123456789" used to check validity of the implementation.
    uint32_t xor_table[BYTE_COMBOS]; // A ptr to a lookup table containing the precomputed XOR values of each byte with the generator polynomial.
} crc_parameters;

// A generic return type for returning calculated crcs of various sizes.
typedef union crc_return
{
    uint16_t crc16; // 16 bit crc.
    uint32_t crc32; // 32 bit crc.
};

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/
// Creates an xor lookup table corresponding to the generator polynomial specified by params.
void init_crc_table(struct crc_parameters* params);
// Calulcates a CRC value for an array of bytes.
crc_return get_crc(uint8_t* data, int length, struct crc_parameters* params);
// Checks that a crc parameters properly computes its check value.
bool validate_crc_parameters(struct crc_parameters* params);

#endif /* _BPLIB_CRC_H_ */

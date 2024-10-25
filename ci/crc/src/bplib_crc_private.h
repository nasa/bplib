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

#ifndef BPLIB_CRC_PRIVATE_H
#define BPLIB_CRC_PRIVATE_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

#define BPLIB_CRC16_X25_POLY 0x1021U
#define BPLIB_CRC32_C_POLY   0x1EDC6F41U

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * Definition of generic-ish CRC data digest function.
 * Updates the CRC based on the data in the given buffer.
 */
typedef BPLib_CRC_Val_t (*BPLib_CRC_DigestFunc_t)(BPLib_CRC_Val_t crc, const void *data, size_t size);

/*
 * Actual definition of CRC parameters
 */
struct BPLib_CRC_Parameters
{
    const char *name;                  /* Name of the CRC. */
    uint8_t     length;                /* The number of bits in the CRC. */
    bool        should_reflect_output; /* Whether to reflect the bits of the output crc. */

    const uint8_t *input_table; /* A ptr to a table for input translation (reflect or direct) */
    const void    *xor_table;   /* A ptr to a table with the precomputed XOR values. */

    BPLib_CRC_DigestFunc_t digest; /* externally-callable "digest" routine to update CRC with new data */

    BPLib_CRC_Val_t initial_value; /* The value used to initialize a CRC (normalized). */
    BPLib_CRC_Val_t final_xor;     /* The final value to xor with the crc before returning (normalized). */
};

BPLib_CRC_Val_t PrecomputeCrcByte(uint8_t width, uint8_t byte, BPLib_CRC_Val_t polynomial);
uint8_t     BPLib_STOR_CACHE_PrecomputeReflection(uint8_t byte);

extern uint8_t BPLIB_CRC_DIRECT_TABLE[256];
extern uint8_t BPLIB_CRC_REFLECT_TABLE[256];

#endif /* BPLIB_CRC_PRIVATE_H */

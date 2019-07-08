/************************************************************************
 * File: bplib_crc32.h
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

#ifndef _BPLIB_CRC32_H_
#define _BPLIB_CRC32_H_

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_crc.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

// Creates an xor lookup table corresponding to the generator polynomial specified by params.
void  init_crc32_table(struct crc_parameters* params);
// Calulcates a CRC value for an array of bytes.
crc_return get_crc32(uint8_t* data, int length, struct crc_parameters* params);
// Checks that a crc parameters properly computes its check value.
bool validate_crc32_parameters(struct crc_parameters* params);
#endif /* _BPLIB_CRC32_H_ */

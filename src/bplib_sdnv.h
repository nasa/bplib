/************************************************************************
 * File: bplib_sdnv.h
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

#ifndef __BPLIB_SDNV_H__
#define __BPLIB_SDNV_H__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    uint32_t    value;
    uint32_t    index;
    uint32_t    width;
} bp_sdnv_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_sdnv_read(uint8_t* block, int size, bp_sdnv_t* sdnv, uint16_t* flags);
int bplib_sdnv_write(uint8_t* block, int size, bp_sdnv_t sdnv, uint16_t* flags);

#endif  /* __BPLIB_SDNV_H__ */

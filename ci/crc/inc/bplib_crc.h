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

#ifndef BPLIB_CRC_H
#define BPLIB_CRC_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/**
 * @brief Type of block CRC calculated by bplib
 *
 * @note the numeric values of this enumeration match the crctype values in the BPv7 spec.
 */
enum BPLib_CRC_Type
{
    /**
     * @brief No CRC is present.
     */
    BPLib_CRC_Type_None = 0,

    /**
     * @brief A standard X-25 CRC-16 is present.
     */
    BPLib_CRC_Type_CRC16 = 1,

    /**
     * @brief A CRC-32 (Castagnoli) is present.
     */
    BPLib_CRC_Type_CRC32C = 2

};

typedef uint8_t BPLib_CRC_Type_t;


/*
 * To keep the value consistent with CBOR, all CRCs are 64-bit
 * For CRC algorithms of lesser width, the value is right-justified (LSB/LSW)
 */
typedef uint64_t BPLib_CRC_Val_t;

/* Standard parameters for calculating a CRC. */
struct BPLib_CRC_Parameters;
typedef const struct BPLib_CRC_Parameters BPLib_CRC_Parameters_t;

/*
 * CRC algorithms that are implemented in BPLIB
 * These definitions are always fixed/const
 */
extern BPLib_CRC_Parameters_t BPLIB_CRC_NONE;
extern BPLib_CRC_Parameters_t BPLIB_CRC_16_X25;
extern BPLib_CRC_Parameters_t BPLIB_CRC_32_CASTAGNOLI;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

void BPLib_CRCInit(void);

const char *BPLib_CRC_GetName(BPLib_CRC_Parameters_t *params);
uint8_t     BPLib_CRC_GetWidth(BPLib_CRC_Parameters_t *params);
BPLib_CRC_Val_t BPLib_CRC_InitialValue(BPLib_CRC_Parameters_t *params);
BPLib_CRC_Val_t BPLib_CRC_Update(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc, const void *data, size_t size);
BPLib_CRC_Val_t BPLib_CRC_Finalize(BPLib_CRC_Parameters_t *params, BPLib_CRC_Val_t crc);

BPLib_CRC_Val_t BPLib_CRCGet(const void *data, const uint32_t length, BPLib_CRC_Parameters_t *params);

#endif /* BPLIB_CRC_H */

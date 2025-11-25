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

#ifndef BPLIB_CRC_H
#define BPLIB_CRC_H

/*
** Include
*/

#include "bplib_api_types.h"


/*
** Type Definitions 
*/

/**
 * @brief Types of CRC supported by BPLib
 *
 * @note The numeric values of this enumeration match the CRC type values in the BPv7 spec
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

/**
 * \brief CRC value
 * 
 * To keep the value consistent with CBOR, all CRCs are 64-bit
 * For CRC algorithms of lesser width, the value is right-justified (LSB/LSW)
 */
typedef uint64_t BPLib_CRC_Val_t;


/*
** Exported Functions
*/

/**
 * \brief     CRC Initialization
 * 
 *  \par Description
 *       Initializes the CRC tables
 */
void BPLib_CRC_Init(void);

/**
 * \brief Calculate CRC
 *
 *  \par Description
 *       Calculates the CRC of the provided data, using either the CRC-16/X25 or the
 *       CRC-32/Castagnoli algorithms. Any other CRC type will return 0.
 *
 *  \par Assumptions, External Events, and Notes:
 *       - The CRC tables have been initializes (see BPLib_CRC_Init)
 * 
 *  \param[in] Data Pointer to a byte array containing data to calculate the CRC over
 *  \param[in] DataLen Length of the data to CRC (in bytes)
 *  \param[in] CrcType Type of CRC (either NONE, CRC-16 or CRC-32)
 *
 *  \return CRC Value
 */
BPLib_CRC_Val_t BPLib_CRC_Calculate(const void *Data, size_t DataLen, BPLib_CRC_Type_t CrcType);

#endif /* BPLIB_CRC_H */

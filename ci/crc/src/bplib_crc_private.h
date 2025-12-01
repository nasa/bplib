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

#ifndef BPLIB_CRC_PRIVATE_H
#define BPLIB_CRC_PRIVATE_H

/*
** Include
*/

#include "bplib_api_types.h"


/*
** Macro Definitions
*/

#define BPLIB_CRC16_X25_POLY 0x1021U
#define BPLIB_CRC32_C_POLY   0x1EDC6F41U

/*
** Type Definitions
*/

/**
 * @brief Generic CRC digest function definition
 */
typedef BPLib_CRC_Val_t (*BPLib_CRC_DigestFunc_t)(const void *Data, size_t DataLen);

/**
 * @brief Parameters associated with each CRC implementation
 */
typedef struct
{
    uint8_t Length;                /** \brief The number of bits in the CRC */
    bool    ShouldReflectOutput;   /** \brief Whether to reflect the bits of the final CRC */
    BPLib_CRC_DigestFunc_t Digest; /** \brief Function to run particular CRC algorithm with */
    BPLib_CRC_Val_t FinalXor;      /** \brief The final value to XOR the CRC with before returning (normalized) */
} BPLib_CRC_Parameters_t;


/*
** Internal Functions 
*/

/**
 * \brief Get CRC-16/X.25
 *
 *  \par Description
 *       Calculates the CRC of the provided data using the CRC-16/X.25 algorithm
 * 
 *  \param[in] DataPtr Pointer to byte array to calculate CRC for
 *  \param[in] DataLen Length of data
 *
 *  \return CRC-16 Value
 */
uint16_t BPLib_CRC_GetCrc16X25(const uint8_t *DataPtr, size_t DataLen);

/**
 * \brief Get CRC-32/Castagnoli
 *
 *  \par Description
 *       Calculates the CRC of the provided data using the CRC-32/Castagnoli algorithm
 * 
 *  \param[in] DataPtr Pointer to byte array to calculate CRC for
 *  \param[in] DataLen Length of data
 *
 *  \return CRC-16 Value
 */
uint32_t BPLib_CRC_GetCrc32Castagnoli(const uint8_t *DataPtr, size_t DataLen);

/**
 * \brief Precompute CRC byte
 *
 *  \par Description
 *       Compute a byte for a CRC table
 * 
 *  \param[in] Width Width of the table data type
 *  \param[in] Byte Byte
 *  \param[in] Polynomial The polynomial associated with the CRC algorithm
 *
 *  \return CRC table byte value
 */
uint32_t BPLib_CRC_PrecomputeCrcByte(uint8_t Width, uint8_t Byte, uint32_t Polynomial);

/**
 * \brief Precompute reflection
 *
 *  \par Description
 *       Compute a byte for the reflection table.
 * 
 *  \param[in] Byte Byte
 *
 *  \return Reflection table byte value
 */
uint8_t BPLib_CRC_PrecomputeReflection(uint8_t Byte);

/**
 * \brief Finalize CRC
 *
 *  \par Description
 *       Takes a CRC value and finalizes it by optionally reflecting the value, XOR-ing
 *       it with the final XOR value, and ensuring its length is limited to the length of
 *       its CRC type
 * 
 *  \param[in] Params Pointer to parameters for the particular CRC algorithm
 *  \param[in] Crc Current value of the CRC
 *
 *  \return Final CRC Value
 */
BPLib_CRC_Val_t BPLib_CRC_Finalize(BPLib_CRC_Parameters_t *Params, BPLib_CRC_Val_t Crc);

#endif /* BPLIB_CRC_PRIVATE_H */

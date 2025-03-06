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

#ifndef BPLIB_BBLOCKS_H
#define BPLIB_BBLOCKS_H

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"
#include "bplib_crc.h"


/*
** Macros
*/

#define BPLIB_PRI_BLOCK_CRC_MASK 0xFF


/*
** Types
*/

/**
 * @brief Previous Node Extension Block Data
 */
typedef struct
{
    BPLib_EID_t PrevNodeId;
} BPLib_PrevNodeBlockData_t;

/**
 * @brief Age Block Extension Block Data
 */
typedef struct
{
    uint64_t Age;
} BPLib_AgeBlockData_t;

/**
 * @brief Hop Count Extension Block Data
 */
typedef struct
{
    uint64_t HopLimit;
    uint64_t HopCount;
} BPLib_HopCountData_t;

/**
 * @brief Creation timestamp of a bundle
 */
typedef struct 
{
    uint64_t CreateTime;
    uint64_t SequenceNumber;
} BPLib_CreationTimeStamp_t;

/**
 * @brief Represents an RFC-9171 primary block in the bundle.
 */
typedef struct 
{
    uint64_t                  CrcType;
    uint64_t                  BundleProcFlags;
    BPLib_EID_t               DestEID;
    BPLib_EID_t               SrcEID;
    BPLib_EID_t               ReportToEID;
    BPLib_CreationTimeStamp_t Timestamp;
    uint64_t                  Lifetime;
    uint64_t                  FragmentOffset;
    uint64_t                  TotalAduLength;
    BPLib_CRC_Val_t           CrcVal;
    /* Metadata */
    bool                      RequiresEncode;
    uint32_t                  BlockOffsetStart;
    uint32_t                  BlockOffsetEnd;
} BPLib_PrimaryBlock_t;

/**
 * @brief Union of all extension block data types
 */
typedef union 
{
    BPLib_HopCountData_t      HopCountData;
    BPLib_AgeBlockData_t      AgeBlockData;
    BPLib_PrevNodeBlockData_t PrevNodeBlockData;
} BPLib_ExtBlockData_t;

/**
 * @brief Canonical block header data
 */
typedef struct
{
    /* Header Data */
    uint64_t        BlockType;
    uint64_t        BlockNum;
    uint64_t        BundleProcFlags;
    uint64_t        CrcType;
    BPLib_CRC_Val_t CrcVal;
    /* Metadata */
    uint64_t        BlockOffsetStart; // Used when skipping re-encoding
    uint64_t        BlockOffsetEnd; // Used when skipping re-encoding
    uint64_t        DataOffsetStart; // Used for ADU Delivery
    uint64_t        DataOffsetSize; // Used for ADU Delivery
    bool            RequiresEncode;
} BPLib_CanBlockHeader_t;

/**
 * @brief Represents an RFC-9171 extension block in the bundle.
 */
typedef struct 
{
    BPLib_CanBlockHeader_t Header;
    BPLib_ExtBlockData_t   BlockData;
} BPLib_ExtensionBlock_t;

/**
 * @struct BPLib_BBlocks_t
 * @brief Represents the bundle blocks, including the primary block, extension blocks, and payload header.
 */
typedef struct
{
    BPLib_PrimaryBlock_t   PrimaryBlock;
    BPLib_ExtensionBlock_t ExtBlocks[BPLIB_MAX_NUM_EXTENSION_BLOCKS];
    BPLib_CanBlockHeader_t PayloadHeader;
} BPLib_BBlocks_t;


#endif /* BPLIB_BBLOCKS_H */

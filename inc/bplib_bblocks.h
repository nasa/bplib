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

#ifndef BPLIB_BBLOCKS_H
#define BPLIB_BBLOCKS_H

#include "bplib_api_types.h"
#include "bplib_cfg.h"
#include "bplib_eid.h"
#include "bplib_crc.h"
#include "bplib_time.h"

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
    size_t                    BlockOffsetStart;
    size_t                    BlockOffsetEnd;
    BPLib_TIME_MonotonicTime_t MonoTime;   /** \brief Creation *monotonic* time */
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
    uint64_t        BlockProcFlags;
    uint64_t        CrcType;
    BPLib_CRC_Val_t CrcVal;

    /* Metadata */
    size_t          BlockOffsetStart; // Used when skipping re-encoding
    size_t          BlockOffsetEnd; // Used when skipping re-encoding
    size_t          DataOffsetStart; // Used for ADU Delivery
    size_t          DataSize; // Used for ADU Delivery
    bool            RequiresEncode;
    bool            RequiresDiscard; // If discarding a block on re-encode
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

/**
 * @brief Represents the metadata needed for bundle processing
 */
typedef struct 
{
    uint16_t                   EgressID;   /** \brief For egressing bundles, ID of channel/contact to send to */
    size_t                     TotalBytes; /** \brief Size of this bundle in bytes */

    /* Additional metadata will likely get added here */

} BPLib_BundleMetaData_t;

#endif /* BPLIB_BBLOCKS_H */

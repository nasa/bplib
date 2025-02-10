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

#define MAX_EXT_BLOCKS 5 /**< Maximum number of decoded extension blocks that can be stored within Bundle metadata. */

/**
 * @struct EndpointIDSSP
 * @brief Represents the endpoint ID in the Bundle Protocol.
 */
typedef struct EndpointIDSSP {
    uint64_t node_number;
    uint64_t service_number;
} EndpointIDSSP;

/**
 * @struct CreationTimeStamp
 * @brief Represents the creation timestamp of a bundle.
 */
typedef struct CreationTimeStamp {
    uint64_t create_time;
    uint64_t sequence_number;
} CreationTimeStamp;

/**
 * @struct PrimaryBlock_t
 * @brief Represents an RFC-9171 primary block in the bundle.
 */
typedef struct PrimaryBlock {
    uint8_t version;
    uint8_t crc_type;
    uint8_t empty[6];
    uint64_t bundle_processing_control_flags;
    EndpointIDSSP dest_eid;
    EndpointIDSSP src_eid;
    EndpointIDSSP report_eid;
    CreationTimeStamp timestamp;
    uint64_t lifetime;
} PrimaryBlock_t;

/**
 * @struct ExtensionBlock_t
 * @brief Represents an RFC-9171 extension block in the bundle.
 */
typedef struct ExtensionBlock {
    uint64_t crc_type;
    uint64_t block_type;
    uint64_t block_processing_flags;
    uint64_t num_bytes;
    uint64_t data;
} ExtensionBlock_t;

/**
 * @struct PayloadHeader_t
 * @brief Represents the header of the payload section in the bundle.
 */
typedef struct PayloadHeader {
    uint64_t crc_type;
    uint64_t block_type;
    uint64_t block_processing_flags;
    uint64_t num_bytes;
} PayloadHeader_t;

/**
 * @struct BPLib_BBlocks_t
 * @brief Represents the bundle blocks, including the primary block, extension blocks, and payload header.
 */
typedef struct BPLib_BBlocks
{
    PrimaryBlock_t pri_blk;
    ExtensionBlock_t ext_blks[MAX_EXT_BLOCKS];
    PayloadHeader_t pay_hdr;
} BPLib_BBlocks_t;

#endif /* BPLIB_BBLOCKS_H */

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

#ifndef BPLIB_TYPES_BUNDLE_H
#define BPLIB_TYPES_BUNDLE_H

#include "bplib_mem.h"

#include <stdint.h>

#define MAX_EXT_BLOCKS 5

/* This is the current FPGA bundle definition. 
** We don't have to use this for software-only, but it's a good way to show a
** simple cross-compatible bundle definition I'm kicking around.
*/
typedef struct EndpointIDSSP {
	uint64_t node_number;
	uint64_t service_number;
} EndpointIDSSP;

typedef struct CreationTimeStamp {
	uint64_t create_time;
	uint64_t sequence_number;
} CreationTimeStamp;

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

typedef struct ExtensionBlock {
	uint64_t crc_type;
	uint64_t block_type;
	uint64_t block_processing_flags;
	uint64_t num_bytes;
	uint64_t data;
} ExtensionBlock_t;

typedef struct PayloadHeader {
	uint64_t crc_type;
	uint64_t block_type;
	uint64_t block_processing_flags;
	uint64_t num_bytes;
} PayloadHeader_t;

typedef struct BPLib_BBlocks
{
    PrimaryBlock_t pri_blk;
    ExtensionBlock_t ext_blks[MAX_EXT_BLOCKS];
    PayloadHeader_t pay_hdr;
} BPLib_BBlocks_t;

typedef struct BPLib_Bundle
{
    BPLib_BBlocks_t blocks; // "What BPLib cares about"
    // Here we eventually need to define a "channel" and a "contact".
    BPLib_MEM_Block_t* blob; // "Everything else"
} BPLib_Bundle_t;

void bplib_types_testbundle_init(BPLib_MEM_Pool_t* pool, BPLib_Bundle_t* bundle,
	int id);

#endif /* BPLIB_TYPES_BUNDLE_H */

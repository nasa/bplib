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

#ifndef BPLIB_MEM_INTERNAL_BBLOCKS_H
#define BPLIB_MEM_INTERNAL_BBLOCKS_H

typedef struct BPLib_MEM_PrimaryBlock
{
    uint8_t                                version;
    BPLib_MEM_BundleProcessingControlFlags_t controlFlags;
    BPLib_CRC_Type_t                       crctype; /* always present, indicates which CRC field is valid */
    BPLib_MEM_EidBuffer_t           destinationEID;
    BPLib_MEM_EidBuffer_t           sourceEID;
    BPLib_MEM_EidBuffer_t           reportEID;
    BPLib_MEM_CreationTimestamp_t   creationTimeStamp;
    BPLib_MEM_Lifetime_t            lifetime;
    BPLib_MEM_AduLength_t           fragmentOffset;
    BPLib_MEM_AduLength_t           totalADUlength;
    BPLib_CRC_Val_t                        crcval;

} BPLib_MEM_PrimaryBlock_t;

typedef struct BPLib_MEM_BblockPrimaryData
{
    BPLib_MEM_PrimaryBlock_t   logical;
    BPLib_MEM_BblockTracking_t delivery;
} BPLib_MEM_BblockPrimaryData_t;

typedef struct BPLib_MEM_BblockPrimary
{
    BPLib_MEM_Block_t cblock_list;
    BPLib_MEM_Block_t chunk_list;

    size_t block_encode_size_cache;
    size_t bundle_encode_size_cache;

    BPLib_MEM_BblockPrimaryData_t data;
} BPLib_MEM_BblockPrimary_t;

typedef struct BPLib_MEM_BblockPrimaryContent
{
    BPLib_MEM_BblockPrimary_t pblock;
    BPLib_MEM_AlignedData_t   user_data_start;
} BPLib_MEM_BblockPrimaryContent_t;

typedef uint8_t BPLib_MEM_Blocknum_t;

typedef struct BPLib_MEM_CanonicalBundleBlock
{
    BPLib_MEM_Blocktype_t              blockType;
    BPLib_MEM_Blocknum_t               blockNum;
    BPLib_CRC_Type_t                crctype; /* always present, indicates which CRC field is valid */
    BPLib_MEM_BlockProcessingFlags_t processingControlFlags;

    BPLib_CRC_Val_t crcval;

} BPLib_MEM_CanonicalBundleBlock_t;

typedef struct BPLib_MEM_PreviousNodeBlock
{
    BPLib_MEM_EidBuffer_t nodeId;
} BPLib_MEM_PreviousNodeBlock_t;

typedef struct BPLib_MEM_BundleAgeBlock
{
    uint64_t age;
} BPLib_MEM_BundleAgeBlock_t;

typedef struct BPLib_MEM_HopCountBlock
{
    uint64_t hopLimit;
    uint64_t hopCount;
} BPLib_MEM_HopCountBlock_t;

typedef struct BPLib_MEM_CustodyTrackingBlock
{
    BPLib_MEM_EidBuffer_t current_custodian;
} BPLib_MEM_CustodyTrackingBlock_t;

/* This reflects the payload block (1) of a bundle containing a custody block w/BPLib_MEM_CustodyOpAccept */
typedef struct BPLIB_MEM_AcceptPayloadBlock
{
    BPLib_MEM_EidBuffer_t duct_source_eid;
    uint64_t           num_entries;
    uint64_t           sequence_nums[BPLIB_DACS_MAX_SEQ_PER_PAYLOAD];
} BPLIB_MEM_AcceptPayloadBlock_t;

typedef union BPLib_MEM_CanonicalBlockData
{
    BPLib_MEM_PreviousNodeBlock_t          previous_node_block;
    BPLib_MEM_BundleAgeBlock_t             age_block;
    BPLib_MEM_HopCountBlock_t              hop_count_block;
    BPLib_MEM_CustodyTrackingBlock_t       custody_tracking_block;
    BPLIB_MEM_AcceptPayloadBlock_t custody_accept_payload_block;
} BPLib_MEM_CanonicalBlockData_t;

typedef struct BPLib_MEM_CanonicalBlockBuffer
{
    BPLib_MEM_CanonicalBundleBlock_t canonical_block; /* always present */
    BPLib_MEM_CanonicalBlockData_t   data;            /* variable data field, depends on type */
} BPLib_MEM_CanonicalBlockBuffer_t;

typedef struct BPLib_MEM_BblockCanonical
{
    BPLib_MEM_Block_t                  chunk_list;
    BPLib_MEM_BblockPrimary_t         *bundle_ref;
    size_t                             block_encode_size_cache;
    size_t                             encoded_content_offset;
    size_t                             encoded_content_length;
    BPLib_MEM_CanonicalBlockBuffer_t   canonical_logical_data;
} BPLib_MEM_BblockCanonical_t;

typedef struct BPLib_MEM_BblockCanonicalContent
{
    BPLib_MEM_BblockCanonical_t  cblock;
    BPLib_MEM_AlignedData_t      user_data_start;
} BPLib_MEM_BblockCanonicalContent_t;

typedef struct BPLib_MEM_GenericDataContent
{
    BPLib_MEM_AlignedData_t user_data_start;
} BPLib_MEM_GenericDataContent_t;

typedef struct BPLib_MEM_Job
{
    BPLib_MEM_Block_t        link;
    BPLib_MEM_CallbackFunc_t handler;
} BPLib_MEM_Job_t;

typedef struct BPLib_MEM_JobStateChange
{
    BPLib_MEM_Job_t base_job;

    /* JPHFIX: this single event handler may be separated into per-event-type handlers */
    BPLib_MEM_CallbackFunc_t event_handler;
} BPLib_MEM_JobStateChange_t;

#endif /* BPLIB_MEM_INTERNAL_BBLOCKS_H */

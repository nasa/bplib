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

#ifndef V7_TYPES_H
#define V7_TYPES_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#define BP_DACS_MAX_SEQ_PER_PAYLOAD 16

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef int (*v7_chunk_writer_func_t)(void *, const void *, size_t);
typedef int (*v7_chunk_reader_func_t)(void *, void *, size_t);

/*
 * Native Type for most BP integers.
 * TBD: does this need an option to be smaller?
 */
typedef uint64_t bp_integer_t;

typedef uint8_t bp_blocknum_t;

typedef enum bp_blocktype
{
    bp_blocktype_undefined                   = 0,
    bp_blocktype_payloadBlock                = 1,
    bp_blocktype_bundleAuthenicationBlock    = 2,
    bp_blocktype_payloadIntegrityBlock       = 3,
    bp_blocktype_payloadConfidentialityBlock = 4,
    bp_blocktype_previousHopInsertionBlock   = 5,
    bp_blocktype_previousNode                = 6,
    bp_blocktype_bundleAge                   = 7,
    bp_blocktype_metadataExtensionBlock      = 8,
    bp_blocktype_extensionSecurityBlock      = 9,
    bp_blocktype_hopCount                    = 10,
    bp_blocktype_bpsec_bib                   = 11,
    bp_blocktype_bpsec_bcb                   = 12,
    bp_blocktype_custodyTrackingBlock        = 13,
    bp_blocktype_MAX_NORMAL                  = 14,

    /*
     * These are internal block types - they exist only locally in this implementation
     * and the CBOR/RFC9171-compliant encoding uses a different type (possibly as 1 for
     * the payload block, or it may not appear in the RFC-compliant bundle at all).
     */
    bp_blocktype_SPECIAL_BLOCKS_START      = 100,
    bp_blocktype_adminRecordPayloadBlock   = bp_blocktype_SPECIAL_BLOCKS_START,
    bp_blocktype_ciphertextPayloadBlock    = 101,
    bp_blocktype_custodyAcceptPayloadBlock = 102,
    bp_blocktype_previousCustodianBlock    = 103,
    bp_blocktype_SPECIAL_BLOCKS_MAX
} bp_blocktype_t;

typedef enum bp_adminrectype
{
    bp_adminrectype_undefined              = 0,
    bp_adminrectype_statusReport           = 1,
    bp_adminrectype_custodyAcknowledgement = 4,

    bp_adminrectype_MAX = 5

} bp_adminrectype_t;

typedef bp_integer_t bp_dtntime_t;
typedef bp_integer_t bp_sequencenumber_t;

typedef enum bp_iana_uri_scheme
{
    bp_iana_uri_scheme_undefined = 0,
    bp_iana_uri_scheme_dtn       = 1,
    bp_iana_uri_scheme_ipn       = 2
} bp_iana_uri_scheme_t;

typedef enum bp_endpointid_scheme
{
    bp_endpointid_scheme_undefined = 0,
    bp_endpointid_scheme_dtn       = 1,
    bp_endpointid_scheme_ipn       = 2
} bp_endpointid_scheme_t;

typedef bp_integer_t bp_ipn_nodenumber_t;
typedef bp_integer_t bp_ipn_servicenumber_t;

typedef struct bp_ipn_uri_ssp
{
    bp_ipn_nodenumber_t    node_number;
    bp_ipn_servicenumber_t service_number;
} bp_ipn_uri_ssp_t;

typedef struct bp_dtn_uri_ssp
{
    bool is_none; /**< indicates the special address "dtn:none" per RFC9171 section 4.2.5.1.1 */
} bp_dtn_uri_ssp_t;

typedef struct bp_creation_timestamp
{
    bp_dtntime_t        time;
    bp_sequencenumber_t sequence_num;
} bp_creation_timestamp_t;

typedef struct bp_bundle_processing_control_flags
{

    /**
     * @brief Bundle deletion status reports are requested.
     */
    bool deletion_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle delivery status reports are requested.
     */
    bool delivery_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle forwarding status reports are requested.
     */
    bool forwarding_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Bundle reception status reports are requested.
     */
    bool reception_status_req; /* 1   bits/1   bytes */

    /**
     * @brief Status time is requested in all status reports.
     */
    bool statusTimeRequested; /* 1   bits/1   bytes */

    /**
     * @brief User application acknowledgement is requested.
     */
    bool acknowledgementRequested; /* 1   bits/1   bytes */

    /**
     * @brief Bundle must not be fragmented.
     */
    bool mustNotFragment; /* 1   bits/1   bytes */

    /**
     * @brief Payload is an administrative record.
     */
    bool isAdminRecord; /* 1   bits/1   bytes */

    /**
     * @brief Bundle is a fragment.
     */
    bool isFragment; /* 1   bits/1   bytes */
} bp_bundle_processing_control_flags_t;

typedef struct bp_block_processing_flags
{

    /**
     * @brief Block must be removed from bundle if it can't be processed.
     */
    bool must_remove; /* 1   bits/1   bytes */

    /**
     * @brief Bundle must be deleted if block can't be processed.
     */
    bool must_delete; /* 1   bits/1   bytes */

    /**
     * @brief Transmission of a status report is requested if block can't be processed.
     */
    bool xmit_status; /* 1   bits/1   bytes */

    /**
     * @brief Block must be replicated in every fragment.
     */
    bool must_replicate; /* 1   bits/1   bytes */

} bp_block_processing_flags_t;

typedef bp_integer_t bp_lifetime_t;
typedef bp_integer_t bp_adu_length_t;

typedef union bp_endpointid_ssp
{
    bp_dtn_uri_ssp_t dtn; /* present if scheme == bp_endpointid_scheme_dtn */
    bp_ipn_uri_ssp_t ipn; /* present if scheme == bp_endpointid_scheme_ipn */
} bp_endpointid_ssp_t;

typedef struct bp_endpointid_buffer
{
    bp_endpointid_scheme_t scheme; /* always present, indicates which union field is valid */
    bp_endpointid_ssp_t    ssp;
} bp_endpointid_buffer_t;

typedef struct bp_primary_block
{
    uint8_t                              version;
    bp_bundle_processing_control_flags_t controlFlags;
    bp_crctype_t                         crctype; /* always present, indicates which CRC field is valid */
    bp_endpointid_buffer_t               destinationEID;
    bp_endpointid_buffer_t               sourceEID;
    bp_endpointid_buffer_t               reportEID;
    bp_creation_timestamp_t              creationTimeStamp;
    bp_lifetime_t                        lifetime;
    bp_adu_length_t                      fragmentOffset;
    bp_adu_length_t                      totalADUlength;
    bp_crcval_t                          crcval;

} bp_primary_block_t;

typedef struct bp_canonical_bundle_block
{
    bp_blocktype_t              blockType;
    bp_blocknum_t               blockNum;
    bp_crctype_t                crctype; /* always present, indicates which CRC field is valid */
    bp_block_processing_flags_t processingControlFlags;

    bp_crcval_t crcval;

} bp_canonical_bundle_block_t;

typedef struct bp_previous_node_block
{
    bp_endpointid_buffer_t nodeId;
} bp_previous_node_block_t;

typedef struct bp_bundle_age_block
{
    bp_dtntime_t age;
} bp_bundle_age_block_t;

typedef struct bp_hop_count_block
{
    bp_integer_t hopLimit;
    bp_integer_t hopCount;
} bp_hop_count_block_t;

typedef struct bp_custody_tracking_block
{
    bp_endpointid_buffer_t current_custodian;
} bp_custody_tracking_block_t;

/* This reflects the payload block (1) of a bundle containing a custody block w/bp_custody_op_accept */
typedef struct bp_custody_accept_payload_block
{
    bp_endpointid_buffer_t flow_source_eid;
    bp_integer_t           num_entries;
    bp_integer_t           sequence_nums[BP_DACS_MAX_SEQ_PER_PAYLOAD];
} bp_custody_accept_payload_block_t;

typedef union bp_canonical_block_data
{
    bp_previous_node_block_t          previous_node_block;
    bp_bundle_age_block_t             age_block;
    bp_hop_count_block_t              hop_count_block;
    bp_custody_tracking_block_t       custody_tracking_block;
    bp_custody_accept_payload_block_t custody_accept_payload_block;
} bp_canonical_block_data_t;

typedef struct bp_canonical_block_buffer
{
    bp_canonical_bundle_block_t canonical_block; /* always present */
    bp_canonical_block_data_t   data;            /* variable data field, depends on type */
} bp_canonical_block_buffer_t;

#endif /* V7_TYPES_H */

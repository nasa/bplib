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

#ifndef BPLIB_STOR_CACHE_TYPES_H
#define BPLIB_STOR_CACHE_TYPES_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include <stdbool.h>

#include "bplib_api_types.h"

#include "bplib_mem.h"

// TODO Verify ifdef __cplusplus is on all public header files.
#ifdef __cplusplus
extern "C" {
#endif

#define BP_DACS_MAX_SEQ_PER_PAYLOAD 16

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

// TODO Belongs in BPLib_STOR_CACHE_ApiTypes.h

/**
 * @brief Abstract definition of bp_desc for external use
 */
struct bp_desc;

typedef struct bp_desc   bp_desc_t;
typedef struct bp_socket bp_socket_t;

typedef struct BPLib_STOR_CACHE_Block BPLib_STOR_CACHE_Block_t;
typedef struct BPLib_STOR_CACHE_ModuleApi BPLib_STOR_CACHE_ModuleApi_t;
typedef struct BPLib_STOR_CACHE_OffloadApi BPLib_STOR_CACHE_OffloadApi_t;

/*
 * The basic types of blocks which are cacheable in the mem
 */
typedef struct BPLib_STOR_CACHE_BblockPrimary          BPLib_STOR_CACHE_BblockPrimary_t;
typedef struct BPLib_STOR_CACHE_BblockPrimaryData      BPLib_STOR_CACHE_BblockPrimaryData_t;
typedef struct BPLib_STOR_CACHE_BblockTracking         BPLib_STOR_CACHE_BblockTracking_t;
typedef struct BPLib_STOR_CACHE_BblockCanonical        BPLib_STOR_CACHE_BblockCanonical_t;

typedef struct BPLib_STOR_CACHE_SubqBase               BPLib_STOR_CACHE_SubqBase_t;
typedef struct BPLib_STOR_CACHE_Duct                   BPLib_STOR_CACHE_Duct_t;
typedef struct BPLib_STOR_CACHE_DuctContent            BPLib_STOR_CACHE_DuctContent_t;

typedef struct BPLib_STOR_CACHE_BblockPrimaryContent   BPLib_STOR_CACHE_BBlockPrimaryContent_t;
typedef struct BPLib_STOR_CACHE_BblockCanonicalContent BPLib_STOR_CACHE_BblockCanonicalContent_t;
typedef struct BPLib_STOR_CACHE_BlockRefContent        BPLib_STOR_CACHE_BlockRefContent_t;

typedef struct BPLib_STOR_CACHE_Entry                  BPLib_STOR_CACHE_Entry_t;
typedef struct BPLib_STOR_CACHE_Pool                   BPLib_STOR_CACHE_Pool_t;

/**
 * @brief The content of a cache block
 *
 * @note At this scope the definition of this pointer is abstract.  Application
 * code should use BPLib_STOR_CACHE_Dereference() to obtain the pointer to the block
 * that this refers to.
 */
typedef struct BPLib_STOR_CACHE_BlockContent BPLib_STOR_CACHE_BlockContent_t;

/**
 * @brief A reference to another mpool block
 *
 * @note At this scope the definition of this pointer is abstract.  Application
 * code should use BPLib_STOR_CACHE_Dereference() to obtain the pointer to the block
 * that this refers to.
 */
typedef BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_Ref_t;

/**
 * @brief Callback frunction for various mpool block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*BPLib_STOR_CACHE_CallbackFunc_t)(void *, BPLib_STOR_CACHE_Block_t *);

/**
 * @brief Type of block CRC calculated by bplib
 *
 * @note the numeric values of this enumeration match the crctype values in the BPv7 spec.
 */
typedef enum bp_crctype
{

    /**
     * @brief No CRC is present.
     */
    bp_crctype_none = 0,

    /**
     * @brief A standard X-25 CRC-16 is present.
     */
    bp_crctype_CRC16 = 1,

    /**
     * @brief A CRC-32 (Castagnoli) is present.
     */
    bp_crctype_CRC32C = 2

} bp_crctype_t;

/*
 * To keep the interface consistent the digest functions do I/O as 32 bit values.
 * For CRC algorithms of lesser width, the value is right-justified (LSB/LSW)
 */
typedef uint32_t bp_crcval_t;

typedef enum
{
   BPLib_STOR_CACHE_PolicyDeliveryNone, /**< best effort handling only, bundle may be forward directly to CLA, no need to store
                                 */
   BPLib_STOR_CACHE_PolicyDeliveryLocalAck, /**< use local storage of bundle, locally acknowledge but no node-to-node custody
                                        transfer */
   BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking /**< enable full custody transfer signals and acknowledgement (not yet
                                              implemented) */
}BPLib_STOR_CACHE_PolicyDelivery_t;

typedef struct BPLib_STOR_QM_QueueTbl BPLib_STOR_QM_QueueTbl_t;

/* Storage service - reserved for future use */
typedef struct bp_store
{
    uint32_t reserved;
} bp_store_t;

/* Channel Statistics */
typedef struct
{
    /* Errors */
    uint32_t lost;         /* storage failure (load, process, accept) */
    uint32_t expired;      /* bundles, dacs, and payloads with expired lifetime (load, process, accept) */
    uint32_t unrecognized; /* unable to parse input OR bundle type not supported (process) */
    /* Data Duct */
    uint32_t transmitted_bundles;   /* bundles sent for first time, does not includes re-sends (load) */
    uint32_t transmitted_dacs;      /* dacs sent (load) */
    uint32_t retransmitted_bundles; /* bundles timed-out and resent (load) */
    uint32_t delivered_payloads;    /* payloads delivered to application (accept) */
    uint32_t received_bundles;      /* bundles destined for local node (process) */
    uint32_t forwarded_bundles;     /* bundles received by local node but destined for another node (process) */
    uint32_t received_dacs;         /* dacs destined for local node (process) */
    /* Storage */
    uint32_t stored_bundles;  /* number of data bundles currently in storage */
    uint32_t stored_payloads; /* number of payloads currently in storage */
    uint32_t stored_dacs;     /* number of dacs bundles currently in storage */
    /* Active */
    uint32_t acknowledged_bundles; /* freed by custody signal - process */
    uint32_t active_bundles;       /* number of slots in active table in use */
} bp_stats_t;

typedef enum
{
   BPLib_STOR_CACHE_VariableNone,            /**< reserved value, keep first */
   BPLib_STOR_CACHE_VariableMemCurrentUse, /**< replaces BPLib_STOR_CACHE_OsMemused() for external API use */
   BPLib_STOR_CACHE_VariableMemHighUse,    /**< replaces BPLib_STOR_CACHE_OsMemhigh() for external API use */
   BPLib_STOR_CACHE_VariableMax              /**< reserved value, keep last */
}BPLib_STOR_CACHE_Variable_t;

// TODO End of block that belongs in BPLib_STOR_CACHE_ModuleApiTypes.h

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
    bp_blocktype_custodyTrackingBlock        = 73,

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
    uint64_t            time;
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

typedef struct BPLib_STOR_CACHE_ServiceEndpt BPLib_STOR_CACHE_ServiceEndpt_t;
typedef struct BPLib_STOR_CACHE_SocketInfo BPLib_STOR_CACHE_SocketInfo_t;

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

typedef struct BPLib_STOR_CACHE_PrimaryBlock
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

} BPLib_STOR_CACHE_PrimaryBlock_t;

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
    uint64_t age;
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
typedef struct BPLIB_CT_AcceptPayloadBlock
{
    bp_endpointid_buffer_t duct_source_eid;
    bp_integer_t           num_entries;
    bp_integer_t           sequence_nums[BP_DACS_MAX_SEQ_PER_PAYLOAD];
} BPLIB_CT_AcceptPayloadBlock_t;

typedef union bp_canonical_block_data
{
    bp_previous_node_block_t          previous_node_block;
    bp_bundle_age_block_t             age_block;
    bp_hop_count_block_t              hop_count_block;
    bp_custody_tracking_block_t       custody_tracking_block;
    BPLIB_CT_AcceptPayloadBlock_t custody_accept_payload_block;
} bp_canonical_block_data_t;

typedef struct bp_canonical_block_buffer
{
    bp_canonical_bundle_block_t canonical_block; /* always present */
    bp_canonical_block_data_t   data;            /* variable data field, depends on type */
} bp_canonical_block_buffer_t;

void BPLib_STOR_CACHE_DebugScanPool(BPLib_STOR_CACHE_Pool_t *pool);
void BPLib_STOR_CACHE_DebugScanQueue(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id);
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label);

#define BPLIB_TIME_TO_INT(t)   ((t).Time)
#define BPLIB_TIME_FROM_INT(t) { (t) }
#define BPLIB_TIME_IS_VALID(t) (BPLIB_TIME_TO_INT(t) != 0)
#define BPLIB_TIME_IS_INFINITE(t) (BPLIB_TIME_TO_INT(t) == UINT64_MAX)

void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk);
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockCanonical_t *cblk);
void BPLib_STOR_CACHE_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk);
void BPLib_STOR_CACHE_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Duct_t *fblk);

int BPLib_STOR_QM_QueueIngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_CACHE_TYPES_H */

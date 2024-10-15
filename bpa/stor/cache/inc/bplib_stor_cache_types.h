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

// #include "bplib_stor_qm.h"

// TODO Verify ifdef __cplusplus is on all public header files.
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 DEFINES
 ******************************************************************************/

// TODO Should be in bplib_time.h?
/* time constants */
#define BPLIB_DTNTIME_UNDEFINED  0
#define BPLIB_DTNTIME_INFINITE   UINT64_MAX

#define BPLIB_MONOTIME_ZERO ((BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(0))
#define BP_MONOTIME_INFINITE ((BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(UINT64_MAX))
// TODO End of block that should be in bplib_time.h

#define BPLIB_MPOOL_ALLOC_PRI_LO  0
#define BPLIB_MPOOL_ALLOC_PRI_MLO 63
#define BPLIB_MPOOL_ALLOC_PRI_MED 127
#define BPLIB_MPOOL_ALLOC_PRI_MHI 191
#define BPLIB_MPOOL_ALLOC_PRI_HI  255

// TODO Belongs in bplib.h

/* Event Flags */
#define BPLIB_FLAG_DIAGNOSTIC              0x00000000
#define BPLIB_FLAG_NONCOMPLIANT            0x00000001 /* valid bundle but agent not able to comply with standard */
#define BPLIB_FLAG_INCOMPLETE              0x00000002 /* block in bundle was not recognized */
#define BPLIB_FLAG_UNRELIABLE_TIME         0x00000004 /* the os call to get time return a suspicious value */
#define BPLIB_FLAG_DROPPED                 0x00000008 /* bundle dropped because a required extension block could not be processed */
#define BPLIB_FLAG_FAILED_INTEGRITY_CHECK  0x00000010 /* bundle with BIB failed the integrity check on the payload */
#define BPLIB_FLAG_BUNDLE_TOO_LARGE        0x00000020 /* size of bundle exceeds capacity allowed by library */
#define BPLIB_FLAG_ROUTE_NEEDED            0x00000040 /* the bundle returned should be queued before transmission */
#define BPLIB_FLAG_STORE_FAILURE           0x00000080 /* storage service failed to deliver data */
#define BPLIB_FLAG_UNKNOWN_CID             0x00000100 /* received CID in acknowledgment for which no bundle was found */
#define BPLIB_FLAG_SDNV_OVERFLOW           0x00000200 /* insufficient room in variable to read/write value */
#define BPLIB_FLAG_SDNV_INCOMPLETE         0x00000400 /* insufficient room in block to read/write value */
#define BPLIB_FLAG_ACTIVE_TABLE_WRAP       0x00000800 /* the active table wrapped */
#define BPLIB_FLAG_DUPLICATES              0x00001000 /* multiple bundles on the network have the same custody id */
#define BPLIB_FLAG_CUSTODY_FULL            0x00002000 /* the dacs rb_tree was full */
#define BPLIB_FLAG_UNKNOWNREC              0x00004000 /* bundle contained unknown adminstrative record */
#define BPLIB_FLAG_INVALID_CIPHER_SUITEID  0x00008000 /* invalid cipher suite ID found in BIB */
#define BPLIB_FLAG_INVALID_BIB_RESULT_TYPE 0x00010000 /* invalid result type found in BIB */
#define BPLIB_FLAG_INVALID_BIB_TARGET_TYPE 0x00020000 /* invalid target type found in BIB */
#define BPLIB_FLAG_FAILED_TO_PARSE         0x00040000 /* unable to parse bundle due to internal inconsistencies in bundle */
#define BPLIB_FLAG_API_ERROR               0x00080000 /* calling code incorrectly used library */
#define BPLIB_FLAG_OUT_OF_MEMORY           0x00100000 /* memory pool exhausted */
#define BPLIB_FLAG_UNIMPLEMENTED           0x80000000 /* request requires a feature not implemented in BPLib */

// TODO End of block that belongs in bplib.h

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

typedef struct BPLib_STOR_CACHE_Block BPLib_STOR_CACHE_Block_t;
typedef struct BPLib_STOR_CACHE_ModuleApi BPLib_STOR_CACHE_ModuleApi_t;

/*
 * The basic types of blocks which are cacheable in the mem
 */
typedef struct BPLib_STOR_CACHE_BblockPrimary          BPLib_STOR_CACHE_BblockPrimary_t;
typedef struct BPLib_STOR_CACHE_BblockPrimaryData      BPLib_STOR_CACHE_BblockPrimaryData_t;
typedef struct BPLib_STOR_CACHE_BblockTracking         BPLib_STOR_CACHE_BblockTracking_t;
typedef struct BPLib_STOR_CACHE_BblockCanonical        BPLib_STOR_CACHE_BblockCanonical_t;

typedef struct BPLib_STOR_CACHE_SubqBase               BPLib_STOR_CACHE_SubqBase_t;
typedef struct BPLib_STOR_QM_Duct                      BPLib_STOR_QM_Duct_t;
typedef struct BPLib_STOR_QM_DuctContent               BPLib_STOR_QM_DuctContent_t;

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

typedef bp_integer_t bp_lifetime_t;
typedef bp_integer_t bp_adu_length_t;

typedef union bp_endpointid_ssp
{
    bp_dtn_uri_ssp_t dtn; /* present if scheme == bp_endpointid_scheme_dtn */
    bp_ipn_uri_ssp_t ipn; /* present if scheme == bp_endpointid_scheme_ipn */
} bp_endpointid_ssp_t;

typedef struct BPLib_STOR_CACHE_EidBuffer
{
    bp_endpointid_scheme_t scheme; /* always present, indicates which union field is valid */
    bp_endpointid_ssp_t    ssp;
} BPLib_STOR_CACHE_EidBuffer_t;

typedef struct BPLib_STOR_CACHE_PrimaryBlock
{
    uint8_t                              version;
    bp_bundle_processing_control_flags_t controlFlags;
    bp_crctype_t                         crctype; /* always present, indicates which CRC field is valid */
    BPLib_STOR_CACHE_EidBuffer_t               destinationEID;
    BPLib_STOR_CACHE_EidBuffer_t               sourceEID;
    BPLib_STOR_CACHE_EidBuffer_t               reportEID;
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
    BPLib_STOR_CACHE_EidBuffer_t nodeId;
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
    BPLib_STOR_CACHE_EidBuffer_t current_custodian;
} bp_custody_tracking_block_t;

/* This reflects the payload block (1) of a bundle containing a custody block w/bp_custody_op_accept */
typedef struct BPLIB_CT_AcceptPayloadBlock
{
    BPLib_STOR_CACHE_EidBuffer_t duct_source_eid;
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

// TODO bp_sid_t belongs in higher level.

/* Storage ID */
typedef unsigned long bp_sid_t;

typedef enum BPLib_STOR_CACHE_Blocktype
{
    BPLib_STOR_CACHE_BlocktypeUndefined = 0,

    /*
     * Note, the enum value here does matter -- these block types
     * are all refcount-capable, and thus are grouped together so this
     * can be implemented as a range check.  Do not change the
     * order of enum values without also updating the checks.
     */
    BPLib_STOR_CACHE_BlocktypeApi       = 1,
    BPLib_STOR_CACHE_BlocktypeGeneric   = 2,
    BPLib_STOR_CACHE_BlocktypePrimary   = 4,
    BPLib_STOR_CACHE_BlocktypeCanonical = 5,
    BPLib_STOR_CACHE_BlocktypeDuct      = 6,
    BPLib_STOR_CACHE_BlocktypeRef       = 7,
    BPLib_STOR_CACHE_BlocktypeMax       = 8, /* placeholder for the max "regular" block type */

    /*
     * All of these block types are _not_ at the beginning of the structure,
     * these are members within the structures.  Their position within the parent
     * is indicated in the parent_offset field so the parent block pointer can be
     * reconstituted from one of these secondary indices.
     */

    BPLib_STOR_CACHE_BlocktypeSecondaryGeneric = 100,
    BPLib_STOR_CACHE_BlocktypeListHead         = 101,
    BPLib_STOR_CACHE_BlocktypeJob              = 102, /* a job or pending work item to do */
    BPLib_STOR_CACHE_BlocktypeSecondaryMax     = 103,

    /* The administrative block will be marked with 0xFF, this still permits the
     * type to be stored as a uint8_t if needed to save bits */
    BPLib_STOR_CACHE_BlocktypeAdmin = 255

} BPLib_STOR_CACHE_Blocktype_t;

struct BPLib_STOR_CACHE_Block
{
    /* note that if it becomes necessary to recover bits here,
     * the offset could be reduced in size
     */
    BPLib_STOR_CACHE_Blocktype_t   type;
    uint32_t                       parent_offset;
    struct BPLib_STOR_CACHE_Block *next;
    struct BPLib_STOR_CACHE_Block *prev;
};

typedef struct BPLib_STOR_CACHE_BlockHeader
{
    BPLib_STOR_CACHE_Block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} BPLib_STOR_CACHE_BlockHeader_t;

struct BPLib_STOR_CACHE_BblockTracking
{
   BPLib_STOR_CACHE_PolicyDelivery_t delivery_policy;
    bp_handle_t                 ingress_intf_id;
    BPLib_TIME_MonotonicTime_t  ingress_time;
    bp_handle_t                 egress_intf_id;
    BPLib_TIME_MonotonicTime_t  egress_time;
    bp_handle_t                 storage_intf_id;
    bp_sid_t                    committed_storage_id;

    /* JPHFIX: this is here for now, but really it belongs on the egress CLA intf based on its RTT */
    uint64_t local_retx_interval;
};

struct BPLib_STOR_CACHE_BblockPrimaryData
{
    BPLib_STOR_CACHE_PrimaryBlock_t logical;
    BPLib_STOR_CACHE_BblockTracking_t     delivery;
};

struct BPLib_STOR_CACHE_BblockPrimary
{
    BPLib_STOR_CACHE_Block_t cblock_list;
    BPLib_STOR_CACHE_Block_t chunk_list;
    size_t              block_encode_size_cache;
    size_t              bundle_encode_size_cache;

    BPLib_STOR_CACHE_BblockPrimaryData_t data;
};

struct BPLib_STOR_CACHE_BblockPrimaryContent
{
    BPLib_STOR_CACHE_BblockPrimary_t    pblock;
    BPLib_MEM_AlignedData_t             user_data_start;
};

struct BPLib_STOR_CACHE_BblockCanonical
{
    BPLib_STOR_CACHE_Block_t           chunk_list;
    BPLib_STOR_CACHE_BblockPrimary_t *bundle_ref;
    size_t                        block_encode_size_cache;
    size_t                        encoded_content_offset;
    size_t                        encoded_content_length;
    bp_canonical_block_buffer_t   canonical_logical_data;
};

typedef struct BPLib_STOR_CACHE_Job
{
    BPLib_STOR_CACHE_Block_t        link;
    BPLib_STOR_CACHE_CallbackFunc_t handler;
} BPLib_STOR_CACHE_Job_t;

typedef struct BPLib_STOR_CACHE_JobStatechange
{
    BPLib_STOR_CACHE_Job_t base_job;

    /* JPHFIX: this single event handler may be separated into per-event-type handlers */
    BPLib_STOR_CACHE_CallbackFunc_t event_handler;
} BPLib_STOR_CACHE_JobStatechange_t;

struct BPLib_STOR_CACHE_SubqBase
{
    BPLib_STOR_CACHE_Block_t block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
};

typedef struct BPLib_STOR_CACHE_SubqWorkitem
{
    BPLib_STOR_CACHE_Job_t       job_header;
    BPLib_STOR_CACHE_SubqBase_t         base_subq;
    unsigned int                 current_depth_limit;
} BPLib_STOR_CACHE_SubqWorkitem_t;


struct BPLib_STOR_QM_Duct
{
    uint32_t pending_state_flags;
    uint32_t current_state_flags;

    BPLib_STOR_CACHE_JobStatechange_t statechange_job;
    BPLib_STOR_CACHE_Ref_t             parent;

    BPLib_STOR_CACHE_SubqWorkitem_t ingress;
    BPLib_STOR_CACHE_SubqWorkitem_t egress;
};

/*
 * Enumeration that defines the various possible queueing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    BPLib_STOR_CACHE_EventidUndefined,
    BPLib_STOR_CACHE_EventidRecycle,
    BPLib_STOR_CACHE_EventidMax

} BPLib_STOR_CACHE_Eventid_t;

/* IPN Schema Endpoint ID Integer Definition */
typedef bp_val_t bp_ipn_t;

/* combine IPN node+service */
typedef struct bp_ipn_addr
{
    bp_ipn_t node_number;
    bp_ipn_t service_number;
} bp_ipn_addr_t;

typedef struct BPLib_STOR_CACHE_Connection
{
    bp_ipn_addr_t local_ipn;
    bp_ipn_addr_t remote_ipn;
    bp_ipn_addr_t report_ipn;

    bp_val_t lifetime;
    bp_val_t local_retx_interval;

    bool is_admin_service;
    bool allow_fragmentation;

    bp_crctype_t crctype;

   BPLib_STOR_CACHE_PolicyDelivery_t local_delivery_policy;

}BPLib_STOR_CACHE_Connection_t;

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_STOR_CACHE_ListIter
{
    BPLib_STOR_CACHE_Block_t *position;
    BPLib_STOR_CACHE_Block_t *pending_entry;
} BPLib_STOR_CACHE_ListIter_t;

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_STOR_CACHE_BlocktypeApi
{
    BPLib_STOR_CACHE_CallbackFunc_t construct; /**< Initialize a newly-created block */
    BPLib_STOR_CACHE_CallbackFunc_t destruct;  /**< De-initialize a recycled block */
} BPLib_STOR_CACHE_BlocktypeApi_t;

typedef struct BPLib_STOR_CACHE_GenericDataContent
{
    BPLib_MEM_AlignedData_t             user_data_start;
} BPLib_STOR_CACHE_GenericDataContent_t;

typedef struct BPLib_STOR_CACHE_ModuleApiContent
{
    BPLib_MEM_RBT_Link_t                rbt_link;
    BPLib_STOR_CACHE_BlocktypeApi_t     api;
    size_t                              user_content_size;
    BPLib_MEM_AlignedData_t             user_data_start;
} BPLib_STOR_CACHE_ModuleApiContent_t;

typedef struct BPLib_STOR_CACHE_BlockAdminContent
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t bblock_alloc_threshold;   /**< threshold at which new bundles will no longer be allocatable */
    uint32_t internal_alloc_threshold; /**< threshold at which internal blocks will no longer be allocatable */
    uint32_t max_alloc_watermark;

    BPLib_MEM_RBT_Root_t          blocktype_registry; /**< registry of block signature values */
    BPLib_STOR_CACHE_ModuleApiContent_t blocktype_basic;    /**< a fixed entity in the registry for type 0 */
    BPLib_STOR_CACHE_ModuleApiContent_t blocktype_cbor;     /**< a fixed entity in the registry for CBOR blocks */

    BPLib_STOR_CACHE_SubqBase_t free_blocks;    /**< blocks which are available for use */
    BPLib_STOR_CACHE_SubqBase_t recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    BPLib_STOR_CACHE_Block_t active_list; /**< a list of flows/queues that need processing */

} BPLib_STOR_CACHE_BlockAdminContent_t;

void BPLib_STOR_CACHE_DebugScanPool(BPLib_STOR_CACHE_Pool_t *pool);
void BPLib_STOR_CACHE_DebugScanQueue(void *tbl, bp_handle_t intf_id);
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label);

#define BPLIB_TIME_TO_INT(t)   ((t).Time)
#define BPLIB_TIME_FROM_INT(t) { (t) }
#define BPLIB_TIME_IS_VALID(t) (BPLIB_TIME_TO_INT(t) != 0)
#define BPLIB_TIME_IS_INFINITE(t) (BPLIB_TIME_TO_INT(t) == UINT64_MAX)

void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk);
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockCanonical_t *cblk);
void BPLib_STOR_CACHE_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk);
void BPLib_STOR_QM_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_QM_Duct_t *fblk);

int BPLib_STOR_QM_IngressToParent(void *arg, BPLib_STOR_CACHE_Block_t *subq_src);

// TODO Reconcile heritage time functions with bplib_time.h TIME module.

static inline uint64_t BPLib_STOR_CACHE_GetDtnTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return BPLib_TIME_GetDtnTime(monotime);
}

static inline BPLib_TIME_MonotonicTime_t BPLib_STOR_CACHE_GetMonotonicTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return monotime;
}

static inline BPLib_TIME_MonotonicTime_t BPLib_STOR_CACHE_TimeRelativeToAbsolute(uint32_t ms)
{
    BPLib_TIME_MonotonicTime_t monotime;
    uint64_t result;
    if (ms == UINT32_MAX)
    {
        /* effectively forever */
        result = UINT64_MAX;
    }
    else
    {
        BPLib_TIME_GetMonotonicTime(&monotime);
        result = monotime.Time + ms;
    }

    return (BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(result);
}

// TODO Verify TimeCompare is still needed when we have BPLib_TIME_GetTimeDelta.
static inline int BPLib_STOR_CACHE_TimeCompare(BPLib_TIME_MonotonicTime_t t1, BPLib_TIME_MonotonicTime_t t2)
{
    return (t1.Time > t2.Time) - (t1.Time < t2.Time);
}

static inline BPLib_TIME_MonotonicTime_t BPLib_STOR_CACHE_TimeAddMs(BPLib_TIME_MonotonicTime_t t1, uint64_t msec)
{
    return (BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(BPLIB_TIME_TO_INT(t1) + msec);
}

static inline BPLib_TIME_MonotonicTime_t BPLib_STOR_CACHE_TimeSubtractMs(BPLib_TIME_MonotonicTime_t t1, uint64_t msec)
{
    return (BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(BPLIB_TIME_TO_INT(t1) - msec);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_CACHE_TYPES_H */

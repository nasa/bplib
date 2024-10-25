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
#include "bplib_crc.h"
#include "bplib_mem.h"
#include "bplib_rbt.h"

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
#define BPLIB_MONOTIME_INFINITE ((BPLib_TIME_MonotonicTime_t) BPLIB_TIME_FROM_INT(UINT64_MAX))
// TODO End of block that should be in bplib_time.h

#define BPLIB_MPOOL_ALLOC_PRI_LO  0
#define BPLIB_MPOOL_ALLOC_PRI_MLO 63
#define BPLIB_MPOOL_ALLOC_PRI_MED 127
#define BPLIB_MPOOL_ALLOC_PRI_MHI 191
#define BPLIB_MPOOL_ALLOC_PRI_HI  255

// TODO Belongs in bplib.h

#define BPLIB_CACHE_STATE_FLAG_ADMIN_UP 0x01
#define BPLIB_CACHE_STATE_FLAG_OPER_UP  0x02
#define BPLIB_CACHE_STATE_FLAG_STORAGE  0x04
#define BPLIB_CACHE_STATE_FLAG_ENDPOINT 0x08
#define BPLIB_CACHE_STATE_FLAG_POLL     0x10

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

#define BPLIB_DACS_MAX_SEQ_PER_PAYLOAD 16

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

// TODO Belongs in BPLib_STOR_CACHE_ApiTypes.h

/**
 * @brief Abstract definition of BPLib_STOR_CACHE_Desc for external use
 */
struct BPLib_STOR_CACHE_Desc;

typedef struct BPLib_STOR_CACHE_Desc   BPLib_STOR_CACHE_Desc_t;

typedef struct BPLib_STOR_CACHE_Block BPLib_STOR_CACHE_Block_t;

/*
 * The basic types of blocks which are cacheable in the mem
 */
typedef struct BPLib_STOR_CACHE_BblockPrimary          BPLib_STOR_CACHE_BblockPrimary_t;
typedef struct BPLib_STOR_CACHE_BblockPrimaryData      BPLib_STOR_CACHE_BblockPrimaryData_t;
typedef struct BPLib_STOR_CACHE_BblockTracking         BPLib_STOR_CACHE_BblockTracking_t;
typedef struct BPLib_STOR_CACHE_BblockCanonical        BPLib_STOR_CACHE_BblockCanonical_t;

typedef struct BPLib_STOR_CACHE_SubqBase               BPLib_STOR_CACHE_SubqBase_t;

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
 * @brief A reference to another CACHE block
 *
 * @note At this scope the definition of this pointer is abstract.  Application
 * code should use BPLib_STOR_CACHE_Dereference() to obtain the pointer to the block
 * that this refers to.
 */
typedef BPLib_STOR_CACHE_BlockContent_t *BPLib_STOR_CACHE_Ref_t;

/**
 * @brief Callback function for various CACHE block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*BPLib_STOR_CACHE_CallbackFunc_t)(void *, BPLib_STOR_CACHE_Block_t *);

typedef enum
{
   BPLib_STOR_CACHE_PolicyDeliveryNone, /**< best effort handling only, bundle may be forward directly to CLA, no need to store
                                 */
   BPLib_STOR_CACHE_PolicyDeliveryLocalAck, /**< use local storage of bundle, locally acknowledge but no node-to-node custody
                                        transfer */
   BPLib_STOR_CACHE_PolicyDeliveryCustodyTracking /**< enable full custody transfer signals and acknowledgement (not yet
                                              implemented) */
}BPLib_STOR_CACHE_PolicyDelivery_t;

typedef int (*chunk_writer_func_t)(void *, const void *, size_t);
typedef int (*chunk_reader_func_t)(void *, void *, size_t);

typedef uint8_t BPLib_STOR_CACHE_Blocknum_t;

typedef enum BPLib_STOR_CACHE_Blocktype
{
    BPLib_STOR_CACHE_BlocktypeUndefined                   = 0,
    BPLib_STOR_CACHE_BlocktypePayloadBlock                = 1,
    BPLib_STOR_CACHE_BlocktypeBundleAuthenicationBlock    = 2,
    BPLib_STOR_CACHE_BlocktypePayloadIntegrityBlock       = 3,
    BPLib_STOR_CACHE_BlocktypePayloadConfidentialityBlock = 4,
    BPLib_STOR_CACHE_BlocktypePreviousHopInsertionBlock   = 5,
    BPLib_STOR_CACHE_BlocktypePreviousNode                = 6,
    BPLib_STOR_CACHE_BlocktypeBundleAge                   = 7,
    BPLib_STOR_CACHE_BlocktypeMetadataExtensionBlock      = 8,
    BPLib_STOR_CACHE_BlocktypeExtensionSecurityBlock      = 9,
    BPLib_STOR_CACHE_BlocktypeHopCount                    = 10,
    BPLib_STOR_CACHE_BlocktypeBpsecBib                    = 11,
    BPLib_STOR_CACHE_BlocktypeBpsecBcb                    = 12,
    BPLib_STOR_CACHE_BlocktypeCustodyTrackingBlock        = 73,
    BPLib_STOR_CACHE_BlocktypeMax,

    /*
     * These are internal block types - they exist only locally in this implementation
     * and the CBOR/RFC9171-compliant encoding uses a different type (possibly as 1 for
     * the payload block, or it may not appear in the RFC-compliant bundle at all).
     */
    BPLib_STOR_CACHE_BlocktypeSpecialBlocksStart        = 100,
    BPLib_STOR_CACHE_BlocktypeAdminRecordPayloadBlock   = BPLib_STOR_CACHE_BlocktypeSpecialBlocksStart,
    BPLib_STOR_CACHE_BlocktypeCiphertextPayloadBlock    = 101,
    BPLib_STOR_CACHE_BlocktypeCustodyAcceptPayloadBlock = 102,
    BPLib_STOR_CACHE_BlocktypePreviousCustodianBlock    = 103,
    /**
     * These are block types that correspond to MEM block types with similar names.
     */
    BPLib_STOR_CACHE_BlocktypeListHead                  = 104,
    BPLib_STOR_CACHE_BlocktypeGeneric                   = 105,
    BPLib_STOR_CACHE_BlocktypeRef                       = 106,
    BPLib_STOR_CACHE_BlocktypePrimary                   = 107,
    BPLib_STOR_CACHE_BlocktypeCanonical                 = 108,
    BPLib_STOR_CACHE_BlocktypeAdmin                     = 109,
    BPLib_STOR_CACHE_BlocktypeApi                       = 110,
    BPLib_STOR_CACHE_BlocktypeDuct                      = 111,
    BPLib_STOR_CACHE_BlocktypeJob                       = 112,
    BPLib_STOR_CACHE_BlocktypeSpecialBlocksMax
} BPLib_STOR_CACHE_Blocktype_t;

typedef enum BPLib_STOR_CACHE_Adminrectype
{
    BPLib_STOR_CACHE_AdminrectypeUndefined              = 0,
    BPLib_STOR_CACHE_AdminrectypeStatusreport           = 1,
    BPLib_STOR_CACHE_AdminrectypeCustodyacknowledgement = 4,

    BPLib_STOR_CACHE_AdminrectypeMax = 5

} BPLib_STOR_CACHE_Adminrectype_t;

typedef uint64_t BPLib_STOR_CACHE_SequenceNumber_t;

typedef enum BPLib_STOR_CACHE_IanaUriScheme
{
    BPLib_STOR_CACHE_IanaUriSchemeUndefined = 0,
    BPLib_STOR_CACHE_IanaUriSchemeDtn       = 1,
    BPLib_STOR_CACHE_IanaUriSchemeIpn       = 2
} BPLib_STOR_CACHE_IanaUriScheme_t;

typedef enum BPLib_STOR_CACHE_EndpointidScheme
{
    BPLib_STOR_CACHE_EndpointidSchemeUndefined = 0,
    BPLib_STOR_CACHE_EndpointidSchemeDtn       = 1,
    BPLib_STOR_CACHE_EndpointidSchemeIpn       = 2
} BPLib_STOR_CACHE_EndpointidScheme_t;

typedef uint64_t BPLib_STOR_CACHE_IpnNodenumber_t;
typedef uint64_t BPLib_STOR_CACHE_IpnServicenumber_t;

typedef struct BPLib_STOR_CACHE_IpnUriSsp
{
    BPLib_STOR_CACHE_IpnNodenumber_t    node_number;
    BPLib_STOR_CACHE_IpnServicenumber_t service_number;
} BPLib_STOR_CACHE_IpnUriSsp_t;

typedef struct BPLib_STOR_CACHE_DtnUriSsp
{
    bool is_none; /**< indicates the special address "dtn:none" per RFC9171 section 4.2.5.1.1 */
} BPLib_STOR_CACHE_DtnUriSsp_t;

typedef struct BPLib_STOR_CACHE_CreationTimestamp
{
    uint64_t            time;
    BPLib_STOR_CACHE_SequenceNumber_t sequence_num;
} BPLib_STOR_CACHE_CreationTimestamp_t;

typedef struct BPLib_STOR_CACHE_BundleProcessingControlFlags
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
} BPLib_STOR_CACHE_BundleProcessingControlFlags_t;

typedef struct BPLib_STOR_CACHE_BlockProcessingFlags
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

} BPLib_STOR_CACHE_BlockProcessingFlags_t;

typedef uint64_t BPLib_STOR_CACHE_Lifetime_t;
typedef uint64_t BPLib_STOR_CACHE_AduLength_t;

typedef union BPLib_STOR_CACHE_EndpointidSsp
{
    BPLib_STOR_CACHE_DtnUriSsp_t dtn; /* present if scheme == BPLib_STOR_CACHE_EndpointidSchemeDtn */
    BPLib_STOR_CACHE_IpnUriSsp_t ipn; /* present if scheme == BPLib_STOR_CACHE_EndpointidSchemeIpn */
} BPLib_STOR_CACHE_EndpointidSsp_t;

typedef struct BPLib_STOR_CACHE_EidBuffer
{
    BPLib_STOR_CACHE_EndpointidScheme_t scheme; /* always present, indicates which union field is valid */
    BPLib_STOR_CACHE_EndpointidSsp_t    ssp;
} BPLib_STOR_CACHE_EidBuffer_t;

typedef struct BPLib_STOR_CACHE_PrimaryBlock
{
    uint8_t                                version;
    BPLib_STOR_CACHE_BundleProcessingControlFlags_t controlFlags;
    BPLib_CRC_Type_t                       crctype; /* always present, indicates which CRC field is valid */
    BPLib_STOR_CACHE_EidBuffer_t           destinationEID;
    BPLib_STOR_CACHE_EidBuffer_t           sourceEID;
    BPLib_STOR_CACHE_EidBuffer_t           reportEID;
    BPLib_STOR_CACHE_CreationTimestamp_t   creationTimeStamp;
    BPLib_STOR_CACHE_Lifetime_t            lifetime;
    BPLib_STOR_CACHE_AduLength_t           fragmentOffset;
    BPLib_STOR_CACHE_AduLength_t           totalADUlength;
    BPLib_CRC_Val_t                        crcval;

} BPLib_STOR_CACHE_PrimaryBlock_t;

typedef struct BPLib_STOR_CACHE_CanonicalBundleBlock
{
    BPLib_STOR_CACHE_Blocktype_t              blockType;
    BPLib_STOR_CACHE_Blocknum_t               blockNum;
    BPLib_CRC_Type_t                crctype; /* always present, indicates which CRC field is valid */
    BPLib_STOR_CACHE_BlockProcessingFlags_t processingControlFlags;

    BPLib_CRC_Val_t crcval;

} BPLib_STOR_CACHE_CanonicalBundleBlock_t;

typedef struct BPLib_STOR_CACHE_PreviousNodeBlock
{
    BPLib_STOR_CACHE_EidBuffer_t nodeId;
} BPLib_STOR_CACHE_PreviousNodeBlock_t;

typedef struct BPLib_STOR_CACHE_BundleAgeBlock
{
    uint64_t age;
} BPLib_STOR_CACHE_BundleAgeBlock_t;

typedef struct BPLib_STOR_CACHE_HopCountBlock
{
    uint64_t hopLimit;
    uint64_t hopCount;
} BPLib_STOR_CACHE_HopCountBlock_t;

typedef struct BPLib_STOR_CACHE_CustodyTrackingBlock
{
    BPLib_STOR_CACHE_EidBuffer_t current_custodian;
} BPLib_STOR_CACHE_CustodyTrackingBlock_t;

/* This reflects the payload block (1) of a bundle containing a custody block w/BPLib_STOR_CACHE_CustodyOpAccept */
typedef struct BPLIB_CT_AcceptPayloadBlock
{
    BPLib_STOR_CACHE_EidBuffer_t duct_source_eid;
    uint64_t           num_entries;
    uint64_t           sequence_nums[BPLIB_DACS_MAX_SEQ_PER_PAYLOAD];
} BPLIB_CT_AcceptPayloadBlock_t;

typedef union BPLib_STOR_CACHE_CanonicalBlockData
{
    BPLib_STOR_CACHE_PreviousNodeBlock_t          previous_node_block;
    BPLib_STOR_CACHE_BundleAgeBlock_t             age_block;
    BPLib_STOR_CACHE_HopCountBlock_t              hop_count_block;
    BPLib_STOR_CACHE_CustodyTrackingBlock_t       custody_tracking_block;
    BPLIB_CT_AcceptPayloadBlock_t                 custody_accept_payload_block;
} BPLib_STOR_CACHE_CanonicalBlockData_t;

typedef struct BPLib_STOR_CACHE_CanonicalBlockBuffer
{
    BPLib_STOR_CACHE_CanonicalBundleBlock_t canonical_block; /* always present */
    BPLib_STOR_CACHE_CanonicalBlockData_t   data;            /* variable data field, depends on type */
} BPLib_STOR_CACHE_CanonicalBlockBuffer_t;

// TODO BPLib_STOR_CACHE_Sid_t belongs in higher level.

/* Storage ID */
typedef unsigned long BPLib_STOR_CACHE_Sid_t;

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

typedef struct BPLib_STOR_CACHE_Connection
{
    BPLib_IpnAddr_t local_ipn;
    BPLib_IpnAddr_t remote_ipn;
    BPLib_IpnAddr_t report_ipn;

    BPLib_Val_t lifetime;
    BPLib_Val_t local_retx_interval;

    bool is_admin_service;
    bool allow_fragmentation;

    BPLib_CRC_Type_t crctype;

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

typedef struct BPLib_STOR_CACHE_Duct BPLib_STOR_CACHE_Duct_t;
typedef struct BPLib_STOR_CACHE_DuctContent BPLib_STOR_CACHE_DuctContent_t;

void BPLib_STOR_CACHE_DebugScanPool(BPLib_STOR_CACHE_Pool_t *pool);
void BPLib_STOR_CACHE_DebugScanQueue(void *tbl, BPLib_Handle_t intf_id);
void BPLib_STOR_CACHE_DebugPrintListStats(BPLib_STOR_CACHE_Block_t *list, const char *label);

void BPLib_STOR_CACHE_BblockPrimaryInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockPrimary_t *pblk);
void BPLib_STOR_CACHE_BblockCanonicalInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_BblockCanonical_t *cblk);
void BPLib_STOR_CACHE_DuctInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_Duct_t *cblk);
void BPLib_STOR_CACHE_SubqInit(BPLib_STOR_CACHE_Block_t *base_block, BPLib_STOR_CACHE_SubqBase_t *qblk);

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

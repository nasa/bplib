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

#ifndef BPLIB_MEM_INTERNAL_H
#define BPLIB_MEM_INTERNAL_H

#include "bplib_api_types.h"
#include "bplib.h"

#include "bplib_stor_cache.h"

#include "bplib_time.h"

#include "bplib_mem.h"
#include "bplib_mem_rbtree.h"

#include "osapi.h"


// Global MEM Mutex
extern osal_id_t BPLib_MEM_OS_FileDataLock;

#define BPLIB_CACHE_STATE_FLAG_ADMIN_UP 0x01
#define BPLIB_CACHE_STATE_FLAG_OPER_UP  0x02
#define BPLIB_CACHE_STATE_FLAG_STORAGE  0x04
#define BPLIB_CACHE_STATE_FLAG_ENDPOINT 0x08
#define BPLIB_CACHE_STATE_FLAG_POLL     0x10

typedef enum
{
   BPLib_MEM_PolicyDeliveryNone, /**< best effort handling only, bundle may be forward directly to CLA, no need to store
                                 */
   BPLib_MEM_PolicyDeliveryLocalAck, /**< use local storage of bundle, locally acknowledge but no node-to-node custody
                                        transfer */
   BPLib_MEM_PolicyDeliveryCustodyTracking /**< enable full custody transfer signals and acknowledgement (not yet
                                              implemented) */
}BPLib_MEM_PolicyDelivery_t;

// TODO Return BPlib_MEM_Block_t to an abstract type. The abstract type belongs in bplib_api_types.h
struct BPLib_MEM_Block
{
    /* note that if it becomes necessary to recover bits here,
     * the offset could be reduced in size
     */
    BPLib_MEM_Blocktype_t     type;
    uint32_t                  parent_offset;
    BPLib_MEM_Block_t *next;
    BPLib_MEM_Block_t *prev;
};

typedef struct BPLib_MEM_BlockHeader
{
    BPLib_MEM_Block_t base_link; /* must be first - this is the pointer used in the application */

    uint32_t content_type_signature; /* a "signature" (sanity check) value for identifying the data */
    uint16_t user_content_length;    /* actual length of user content (does not include fixed fields) */
    uint16_t refcount;               /* number of active references to the object */

} BPLib_MEM_BlockHeader_t;

/**
 * @brief Callback function for various memory pool block actions
 *
 * This is a generic API for a function to handle various events/conditions
 * that occur at the block level.  The generic argument supplies the context
 * information.
 *
 * The return value depends on the context and may or may not be used, it
 * should should return 0 unless otherwise specified.
 */
typedef int (*BPLib_MEM_CallbackFunc_t)(void *, BPLib_MEM_Block_t *);

/**
 * @brief Blocktype API
 *
 * Specifies functions for module-specific block operations,
 * including construction and destruction
 */
typedef struct BPLib_MEM_BlocktypeApi
{
    BPLib_MEM_CallbackFunc_t construct; /**< Initialize a newly-created block */
    BPLib_MEM_CallbackFunc_t destruct;  /**< De-initialize a recycled block */

} BPLib_MEM_BlocktypeApi_t;

/**
 * BPLib_MEM_ApiContent is a specialized variant of BPLib_STOR_CACHE_ApiContent.
 * Blocks must have a non-NULL pointer to API Content to be allocated.
 */
typedef struct BPLib_MEM_ApiContent
{
    BPLib_MEM_RBT_Link_t     rbt_link;
    BPLib_MEM_BlocktypeApi_t api;
    size_t                   user_content_size;
    BPLib_MEM_AlignedData_t  user_data_start;
} BPLib_MEM_ApiContent_t;

typedef struct BPLib_MEM_SubqBase
{
    BPLib_MEM_Block_t block_list;

    /* note - "unsigned int" is chosen here as it is likely to be
     * a single-cycle read in most CPUs.  The range is not as critical
     * because what matters is the difference between these values.
     * The "volatile" qualification helps ensure the values are read as they
     * appear in code and are not rearranged by the compiler, as they could
     * be changed by other threads.  */
    volatile unsigned int push_count;
    volatile unsigned int pull_count;
} BPLib_MEM_SubqBase_t;

typedef struct BPLib_MEM_BlockAdminContent
{
    size_t   buffer_size;
    uint32_t num_bufs_total;
    uint32_t bblock_alloc_threshold;   /**< threshold at which new bundles will no longer be allocatable */
    uint32_t internal_alloc_threshold; /**< threshold at which internal blocks will no longer be allocatable */
    uint32_t max_alloc_watermark;

    BPLib_MEM_RBT_Root_t   blocktype_registry; /**< registry of block signature values */
    BPLib_MEM_ApiContent_t blocktype_basic;    /**< a fixed entity in the registry for type 0 */
    BPLib_MEM_ApiContent_t blocktype_cbor;     /**< a fixed entity in the registry for CBOR blocks */

    BPLib_MEM_SubqBase_t free_blocks;    /**< blocks which are available for use */
    BPLib_MEM_SubqBase_t recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    BPLib_MEM_Block_t active_list; /**< a list of ducts/queues that need processing */

} BPLib_MEM_BlockAdminContent_t;

/* Storage ID */
typedef unsigned long BPLib_MEM_Sid_t;


typedef struct BPLib_MEM_BblockTracking
{
    BPLib_MEM_PolicyDelivery_t  delivery_policy;
    BPLib_Handle_t              ingress_intf_id;
    BPLib_TIME_MonotonicTime_t  ingress_time;
    BPLib_Handle_t              egress_intf_id;
    BPLib_TIME_MonotonicTime_t  egress_time;
    BPLib_Handle_t              storage_intf_id;
    BPLib_MEM_Sid_t             committed_storage_id;

    /* JPHFIX: this is here for now, but really it belongs on the egress CLA intf based on its RTT */
    uint64_t local_retx_interval;
} BPLib_MEM_BblockTracking_t;

typedef enum BPLib_MEM_Adminrectype
{
    BPLib_MEM_AdminrectypeUndefined              = 0,
    BPLib_MEM_AdminrectypeStatusreport           = 1,
    BPLib_MEM_AdminrectypeCustodyacknowledgement = 4,

    BPLib_MEM_AdminrectypeMax = 5

} BPLib_MEM_Adminrectype_t;

typedef uint64_t BPLib_MEM_SequenceNumber_t;

typedef enum BPLib_MEM_IanaUriScheme
{
    BPLib_MEM_IanaUriSchemeUndefined = 0,
    BPLib_MEM_IanaUriSchemeDtn       = 1,
    BPLib_MEM_IanaUriSchemeIpn       = 2
} BPLib_MEM_IanaUriScheme_t;

typedef enum BPLib_MEM_EndpointidScheme
{
    BPLib_MEM_EndpointidSchemeUndefined = 0,
    BPLib_MEM_EndpointidSchemeDtn       = 1,
    BPLib_MEM_EndpointidSchemeIpn       = 2
} BPLib_MEM_EndpointidScheme_t;

typedef uint64_t BPLib_MEM_IpnNodenumber_t;
typedef uint64_t BPLib_MEM_IpnServicenumber_t;

typedef struct BPLib_MEM_IpnUriSsp
{
    BPLib_MEM_IpnNodenumber_t    node_number;
    BPLib_MEM_IpnServicenumber_t service_number;
} BPLib_MEM_IpnUriSsp_t;

typedef struct BPLib_MEM_DtnUriSsp
{
    bool is_none; /**< indicates the special address "dtn:none" per RFC9171 section 4.2.5.1.1 */
} BPLib_MEM_DtnUriSsp_t;

typedef struct BPLib_MEM_CreationTimestamp
{
    uint64_t            time;
    BPLib_MEM_SequenceNumber_t sequence_num;
} BPLib_MEM_CreationTimestamp_t;

typedef struct BPLib_MEM_BundleProcessingControlFlags
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
} BPLib_MEM_BundleProcessingControlFlags_t;

typedef struct BPLib_MEM_BlockProcessingFlags
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

} BPLib_MEM_BlockProcessingFlags_t;

typedef uint64_t BPLib_MEM_Lifetime_t;
typedef uint64_t BPLib_MEM_AduLength_t;

typedef union BPLib_MEM_EndpointidSsp
{
    BPLib_MEM_DtnUriSsp_t dtn; /* present if scheme == BPLib_MEM_EndpointidSchemeDtn */
    BPLib_MEM_IpnUriSsp_t ipn; /* present if scheme == BPLib_MEM_EndpointidSchemeIpn */
} BPLib_MEM_EndpointidSsp_t;

typedef struct BPLib_MEM_EidBuffer
{
    BPLib_MEM_EndpointidScheme_t scheme; /* always present, indicates which union field is valid */
    BPLib_MEM_EndpointidSsp_t    ssp;
} BPLib_MEM_EidBuffer_t;

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

typedef struct BPLib_MEM_SubqWorkitem
{
    BPLib_MEM_Job_t               job_header;
    BPLib_MEM_SubqBase_t          base_subq;
    unsigned int                  current_depth_limit;
} BPLib_MEM_SubqWorkitem_t;

/**
 * @brief A reference to another MEM block
 *
 * @note At this scope the definition of this pointer is abstract.  Application
 * code should use BPLib_MEM_Dereference() to obtain the pointer to the block
 * that this refers to.
 */
typedef struct BPLib_MEM_BlockContent_t *BPLib_MEM_Ref_t;

typedef struct BPLib_MEM_Duct
{
    uint32_t pending_state_flags;
    uint32_t current_state_flags;

    BPLib_MEM_JobStateChange_t statechange_job;
    BPLib_MEM_Ref_t    parent;

    BPLib_MEM_SubqWorkitem_t ingress;
    BPLib_MEM_SubqWorkitem_t egress;
} BPLib_MEM_Duct_t;

typedef struct BPLib_MEM_DuctContent
{
    BPLib_MEM_Duct_t              dblock;
    BPLib_MEM_AlignedData_t       user_data_start;
} BPLib_MEM_DuctContent_t;

/*
 * Enumeration that defines the various possible queueing table events.  This enum
 * must always appear first in the structure that is the argument to the event handler,
 * and indicates the actual event that has occurred
 */
typedef enum
{
    BPLib_MEM_DuctEventUndefined,
    BPLib_MEM_DuctEventPoll,
    BPLib_MEM_DuctEventUp,
    BPLib_MEM_DuctEventDown,
    BPLib_MEM_DuctEventMax

} BPLib_MEM_DuctEvent_t;

typedef struct BPLib_MEM_StateEvent
{
    BPLib_MEM_DuctEvent_t event_type; /* must be first */
    BPLib_Ipn_t                 dest;
    BPLib_Ipn_t                 mask;
} BPLib_MEM_StateEvent_t;

typedef struct BPLib_MEM_DuctStatechangeEvent
{
    BPLib_MEM_DuctEvent_t event_type; /* must be first */
    BPLib_Handle_t              intf_id;
} BPLib_MEM_DuctStatechangeEvent_t;

typedef union BPLib_MEM_DuctGenericEvent
{
    BPLib_MEM_DuctEvent_t             event_type;
    BPLib_MEM_DuctStatechangeEvent_t intf_state;
} BPLib_MEM_DuctGenericEvent_t;

/**
 * @brief Allocate a duct block
 *
 * @param pool
 * @param magic_number
 * @param init_arg Opaque pointer passed to initializer (may be NULL)
 * @return BPLib_MEM_Block_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_DuctAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg);

typedef struct BPLib_MEM_BlockRefContent
{
    BPLib_MEM_Ref_t              pref_target;
    BPLib_MEM_AlignedData_t      user_data_start;
} BPLib_MEM_BlockRefContent_t;

typedef union BPLib_MEM_BlockBuffer
{
    BPLib_MEM_ApiContent_t             api;
    BPLib_MEM_GenericDataContent_t     generic_data;
    BPLib_MEM_BblockPrimaryContent_t   primary;
    BPLib_MEM_BblockCanonicalContent_t canonical;
    BPLib_MEM_DuctContent_t            duct;
    BPLib_MEM_BlockRefContent_t        ref;
    BPLib_MEM_BlockAdminContent_t      admin;

    /* guarantees a minimum size of the generic data blocks, also determines the amount
     * of extra space available for user objects in other types of blocks. */
    uint8_t content_bytes[BPLIB_MEM_MIN_USER_BLOCK_SIZE];
} BPLib_MEM_BlockBuffer_t;

typedef struct BPLib_MEM_BlockContent
{
    BPLib_MEM_BlockHeader_t header; /* must be first */
    BPLib_MEM_BlockBuffer_t u;
} BPLib_MEM_BlockContent_t;

struct BPLib_MEM_Pool
{
    BPLib_MEM_BlockContent_t admin_block; /**< Start of first real block (see num_bufs_total) */
};

// TODO MEM-local Time helpers, also in the cache code separately. Should be in bplib_api_types.h or TIME.
// TODO Reconcile heritage time functions with bplib_time.h TIME module.

static inline uint64_t BPLib_MEM_GetDtnTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return BPLib_TIME_GetDtnTime(monotime);
}

static inline BPLib_TIME_MonotonicTime_t BPLib_MEM_GetMonotonicTime(void)
{
    BPLib_TIME_MonotonicTime_t monotime;
    BPLib_TIME_GetMonotonicTime(&monotime);
    return monotime;
}

/*
 * Randomly-chosen 32-bit static values that can be put into
 * data structures to help positively identify those structs later.
 */
#define BPLIB_MEM_SIGNATURE_STATE    0x6833597a
#define BPLIB_MEM_SIGNATURE_ENTRY    0xf223ff9f
#define BPLIB_MEM_SIGNATURE_BLOCKREF 0x77e961b1

/**
 * Some MEM declarations now reside in STOR CACHE.
 * They are also declared here until the migration to STOR CACHE is complete.
 */
typedef enum
{
    BPLib_MEM_EntryStateUndefined,
    BPLib_MEM_EntryStateIdle,
    BPLib_MEM_EntryStateQueue,
    BPLib_MEM_EntryStateDelete,
    BPLib_MEM_EntryStateGenerateDacs,
    BPLib_MEM_EntryStateMax
} BPLib_MEM_EntryState_t;

typedef struct BPLib_MEM_Entry BPLib_MEM_Entry_t;

struct BPLib_MEM_Entry
{
    BPLib_MEM_Entry_t      *parent;
    BPLib_MEM_EntryState_t state;
    uint32_t               flags;
};

/**
 * @brief Gets the next block in a list of blocks
 *
 * @param cb
 * @return BPLib_MEM_Block_t*
 */
static inline BPLib_MEM_Block_t *BPLib_MEM_GetNextBlock(BPLib_MEM_Block_t *cb)
{
    return cb->next;
}

/**
 * @brief Gets the previous block in a list of blocks
 *
 * @param cb
 * @return BPLib_MEM_Block_t*
 */
static inline BPLib_MEM_Block_t *BPLib_MEM_GetPrevBlock(BPLib_MEM_Block_t *cb)
{
    return cb->prev;
}

/**
 * @brief Checks if this block is part of a list
 *
 * @param list
 * @return true If this is in a list
 * @return false If the block is a singleton
 */
static inline bool BPLib_MEM_IsLinkAttached(const BPLib_MEM_Block_t *list)
{
    return (list->next != list);
}

/**
 * @brief Checks if this block is a singleton
 *
 * @param list
 * @return true If the block is a singleton
 * @return false If the block is part of a list
 */
static inline bool BPLib_MEM_IsLinkUnattached(const BPLib_MEM_Block_t *list)
{
    return (list->next == list);
}

/**
 * @brief Checks if this block is the head of a list
 *
 * This is both a start condition and an end condition when iterating a list
 *
 * @param list
 * @return true If the block is a head node
 * @return false If the block is not a head node
 */
static inline bool BPLib_MEM_IsListHead(const BPLib_MEM_Block_t *list)
{
    return (list->type == BPLib_MEM_BlocktypeListHead);
}

/**
 * @brief Checks if this block is an empty list
 *
 * @param list
 * @return true If the list is empty
 * @return false If the list is not empty, or not a list head node
 */
static inline bool BPLib_MEM_IsEmptyListHead(const BPLib_MEM_Block_t *list)
{
    return (BPLib_MEM_IsListHead(list) && BPLib_MEM_IsLinkUnattached(list));
}

/**
 * @brief Checks if this block is a non-empty list
 *
 * @param list
 * @return true If the list is not empty
 * @return false If the list is empty, or not a list head node
 */
static inline bool BPLib_MEM_IsNonEmptyListHead(const BPLib_MEM_Block_t *list)
{
    return (BPLib_MEM_IsListHead(list) && BPLib_MEM_IsLinkAttached(list));
}

/**
 * @brief Gets the size of the user buffer associated with a data block
 *
 * @param cb pointer to block
 * @return size_t
 */
size_t BPLib_MEM_GetUserContentSize(const BPLib_MEM_Block_t *cb);

/**
 * @brief Reads the reference count of the object
 *
 * Primary and canonical blocks have a reference count, allowing them to be quickly
 * duplicated (such as to keep one copy in a storage, while another sent to a CLA) without
 * actually copying the data itself.  The content blocks will be kept in the pool until
 * the refcount reaches zero, and then the memory blocks will be recycled.
 *
 * If this returns "1" it means that the given block pointer is currently the only reference to
 * this particular block (that is, it is not also present in an interface queue somewhere else)
 *
 * @param cb
 * @return size_t
 */
size_t BPLib_MEM_ReadRefCount(const BPLib_MEM_Block_t *cb);

/**
 * @brief Checks if the block is indirect (a reference)
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_MEM_IsIndirectBlock(const BPLib_MEM_Block_t *cb)
{
    // return (cb->type == BPLib_MEM_BlocktypeRef);

    // There are no Refs in MEM.
    return false;
}

/**
 * @brief Checks if the block is any valid content type
 *
 * This indicates blocks that have actual content
 * This is any block type other than a list head, free block, or
 * secondary index.
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_MEM_IsAnyContentNode(const BPLib_MEM_Block_t *cb)
{
    return (cb->type > BPLib_MEM_BlocktypeUndefined && cb->type < BPLib_MEM_BlocktypeMax);
}

/**
 * @brief Checks if the block is any secondary index type
 *
 * These blocks are members of a larger block
 *
 * @param cb
 * @return true
 * @return false
 */
static inline bool BPLib_MEM_IsSecondaryIndexNode(const BPLib_MEM_Block_t *cb)
{
    return (cb->type >= BPLib_MEM_BlocktypeSecondaryGeneric && cb->type <=  BPLib_MEM_BlocktypeSecondaryMax);
}

BPLib_MEM_Block_t *BPLib_MEM_BlockFromExternalId(BPLib_MEM_Pool_t *pool, BPLib_Handle_t handle);

static inline BPLib_Handle_t BPLib_MEM_GetExternalId(const BPLib_MEM_Block_t *cb)
{
    return BPLib_HandleFromSerial(cb->parent_offset, BPLIB_HANDLE_MPOOL_BASE);
}

/* basic list ops */

/**
 * @brief Sets the node to be an empty list head node
 *
 * Initialize a new list head object.
 *
 * Any existing content will be discarded, so this should typically only
 * be used on new blocks (such as a temporary list created on the stack)
 * where it is guaranteed to NOT have any valid content, and the object
 * is in an unknown/undefined state.
 *
 * To clear a list that has already been initialized once, use
 * BPLib_STOR_CACHE_RecycleAllBlocksInList()
 *
 * @param base_block The parent/container of the list
 * @param list_head  The list to initialize
 */
void BPLib_MEM_InitListHead(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *list_head);

/**
 * @brief Creates a memory pool object using a preallocated memory block
 *
 * @param pool_mem  Pointer to pool memory
 * @param pool_size Size of pool memory
 * @return bplib_mpool_t*
 */
BPLib_MEM_Pool_t *BPLib_MEM_PoolCreate(void *pool_mem, size_t pool_size);

/*----------------------------------------------------------------
 *
 * Function: BPLib_MEM_RecycleBlockInternal
 *
 *-----------------------------------------------------------------*/
void BPLib_MEM_RecycleBlockInternal(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *blk);

/**
 * @brief Recycle an entire list of blocks which are no longer needed
 *
 * The blocks will be returned to the pool.  All blocks in the list must have
 * been allocated from the same pool.
 *
 * The pool pointer may also be passed as NULL if the list object is known to be a
 * member of a block that was also allocted from the pool.  In this case, the pool will
 * be inferred from the list pointer.  However, if the list is a temporary object
 * and not connected to the originating pool, then the actual pool pointer must be
 * passed in.
 *
 * @param pool Originating pool, or NULL to infer/determine from list
 * @param list List of objects to recycle
 */
void BPLib_MEM_RecycleAllBlocksInList(BPLib_MEM_Pool_t *pool, BPLib_MEM_Block_t *list);

/**
 * @brief Recycle a single block which is no longer needed
 *
 * The block will be returned to the pool it originated from.
 *
 * @param blk
 */
void BPLib_MEM_RecycleBlock(BPLib_MEM_Block_t *blk);

/**
 * @brief Registers a given block type signature
 *
 * The api contains a constructor and destructor function, which will be invoked on newly allocated
 * and recycled blocks, respectively.  This should be invoked during startup/initialization for all
 * the services types being used.
 *
 * @note At the current time there is no defined method to unregister a block type, as types of services
 * in use are not expected to change dynamically at runtime.  It is also somewhat difficult to ensure that
 * there are no instances of the block type in existence in the pool.  If reconfiguration is required, the
 * entire pool should be reinitialized.
 *
 * @param pool
 * @param magic_number 32-bit Block identifier/signature
 * @param api Structure containing op callbacks
 * @param user_content_size Maximum size of user content associated with blocktype
 * @returns status code
 * @retval BPLIB_SUCCESS if registration successful
 * @retval BPLIB_DUPLICATE if the block type is already registered.
 */
int BPLib_MEM_RegisterBlocktype (BPLib_MEM_Pool_t *pool, uint32_t magic_number, const BPLib_MEM_BlocktypeApi_t *api,
                                   size_t user_content_size);

/**
 * Initialize the MEM module's internal calls to OSAL.
 * Most of the internal MEM OS code came from the heritage bplib/os library.
 * It's only a subset of that library.
 */
BPLib_Status_t BPLib_MEM_OS_Init(void);

/*--------------------------------------------------------------------------------------
 * BPLib_MEM_OS_NextSerial -
 *-------------------------------------------------------------------------------------*/
unsigned int BPLib_MEM_OS_NextSerial(void);

#define BPLIB_MEM_DTNTIME_INFINITE UINT64_MAX // TODO Should be in bplib.h as BPLIB_DTNTIME_INFINITE.

int BPLib_MEM_OS_WaitUntilMs(BPLib_Handle_t h, uint64_t abs_dtntime_ms);

/**
 * @brief Gets the administrative block for the given pool
 *
 * This is always the first block in the pool.
 *
 * @param pool
 * @return BPLib_MEM_BlockAdminContent_t*
 */

static inline BPLib_MEM_BlockAdminContent_t *BPLib_MEM_GetAdmin(BPLib_MEM_Pool_t *pool)
{
    /* this just confirms that the passed-in pointer looks OK */
    return &pool->admin_block.u.admin;
}

// TODO Add brief for SubqInit and BPLib_MEM_InsertBefore.
void BPLib_MEM_SubqInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_SubqBase_t *qblk);

void BPLib_MEM_InsertBefore(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node);

uint64_t BPLib_MEM_OS_GetDtnTimeMs(void);

/**
 * @brief Get the current depth of a given subq
 *
 * @note If this is called outside of a lock, the results may be indeterminate because
 * the state may change by the time the result is returned.  However, since 32-bit reads
 * are generally atomic on most CPUs, it should be sufficiently safe to call while unlocked
 * if the caller ensures that the calling task is the only thread currently during
 * pushes or pulls - thereby ensuring that at least one of the values will be stable
 * between the check and the actual push/pull.  In the event that the value changes
 * between the time of check and time of use, it will only be "better" (that is, if the
 * caller is pulling, the depth can only go up if another thread pushes, and if the
 * caller is pushing, the depth can only go down if another thread pulls).
 *
 * @param subq
 * @returns Current depth of queue
 */
static inline uint32_t BPLib_MEM_SubqGetDepth(const BPLib_MEM_SubqBase_t *subq)
{
    return (subq->push_count - subq->pull_count);
}

/**
 * @brief Cast a block to a duct type
 *
 * @param cb
 * @return BPLib_MEM_Duct_t*
 */
BPLib_MEM_Duct_t *BPLib_MEM_DuctCast(BPLib_MEM_Block_t *cb);

typedef struct BPLib_MEM_State
{
    int placeholder;

    BPLib_IpnAddr_t self_addr;

    #ifdef QM_JOB
    BPLib_STOR_QM_Job_t pending_job;
    #endif // QM_JOB

    /*
     * pending_list holds bundle refs that are currently actionable in some way,
     * such as one of its timers getting reached, or the state flags changed.
     *
     * This may simply be re-classifying it into one of the other lists or indices.
     */
    BPLib_MEM_Block_t pending_list;

    BPLib_TIME_MonotonicTime_t action_time; /**< Monotonic time when the pending_list was last checked */

    /*
     * idle_list holds the items which do not fit into the other two lists.
     * They are not currently actionable, and placed here for holding.
     *
     * NOTE: this is just a sequential flat list for simplicity.  It can be
     * iterated, but that does not scale well.  One of the secondary indices
     * may be used for more efficient lookup.
     */
    BPLib_MEM_Block_t idle_list;

    BPLib_MEM_RBT_Root_t bundle_index;
    BPLib_MEM_RBT_Root_t dacs_index;
    #ifdef jphfix
    BPLib_MEM_RBT_Root_t dest_eid_jphfix_index;
    BPLib_MEM_RBT_Root_t time_jphfix_index;
    #else // jphfix
    BPLib_MEM_RBT_Root_t dest_eid_index;
    BPLib_MEM_RBT_Root_t time_index;
    #endif // jphfix

    // TODO const BPLib_STOR_PS_OffloadApi_t *offload_api;
    BPLib_MEM_Block_t         *offload_blk;


    uint32_t generated_dacs_seq;

    uint32_t fsm_state_enter_count[BPLib_MEM_EntryStateMax];
    uint32_t fsm_state_exit_count[BPLib_MEM_EntryStateMax];
    uint32_t discard_count;

} BPLib_MEM_State_t;

/* Allows reconstitution of the base block from a cache state pointer */
static inline BPLib_MEM_Block_t *BPLib_MEM_StateSeldblock(BPLib_MEM_State_t *state)
{
    /* any of the sub-lists can be used here, they should all trace back to the same parent */
    return BPLib_MEM_GetBlockFromLink(&state->pending_list);
}

/* Allows reconstitution of the duct object from a cache state pointer */
static inline BPLib_MEM_Duct_t *BPLib_MEM_GetDuct(BPLib_MEM_State_t *state)
{
    return BPLib_MEM_DuctCast(BPLib_MEM_StateSeldblock(state));
}

/**
 * @brief Gets the offset of the block user content section
 *
 * Some block types have an area that may be used for general purpose data storage.
 * The size and offset of this area depends on the block type.  This gets the offset
 * of the start of the area for the given a block type.
 *
 * @param bt Block type
 * @return size_t
 */
size_t BPLib_MEM_GetUserDataOffsetByBlocktype(BPLib_MEM_Blocktype_t bt);

/**
 * @brief Garbage collection routine
 *
 * Processes the list of recycled blocks and performing any additional cleanup work
 *
 * @param pool The mpool object
 * @param limit The maximum number of entries to process
 *
 * @returns The number of blocks collected
 */
uint32_t BPLib_MEM_CollectBlocks(BPLib_MEM_Pool_t *pool, uint32_t limit);

/**
 * @brief Inserts a node after the given position or list head
 *
 * Note when inserting at a list head, it is essentially a mirror image (outside looking in)
 * Therefore this call will place a node at the _beginning_ of the list (prepend)
 *
 * @param list
 * @param node
 */
void BPLib_MEM_InsertAfter(BPLib_MEM_Block_t *list, BPLib_MEM_Block_t *node);

/**
 * Subq-as-list iterator functions for operating on subqs with list-type operations.
 * CACHE uses subqs more extensively, but they are used in MEM for subqs
 * like the free_blocks and recycle_blocks subqs.
 */
typedef struct BPLib_MEM_ListIter
{
    BPLib_MEM_Block_t *position;
    BPLib_MEM_Block_t *pending_entry;
} BPLib_MEM_ListIter_t;

/* basic list iterators (forward or reverse) */

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the first value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterGotoFirst(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Position an iterator at the last value in the list
 *
 * @param iter          The iterator object to position
 * @returns integer status code
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterGotoLast(const BPLib_MEM_Block_t *list, BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator forward by one step in the tree
 *
 * Sets the iterator to the immediate successor of the current node.
 * This allows the caller to perform a ascending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterForward(BPLib_MEM_ListIter_t *iter);

/*--------------------------------------------------------------------------------------*/
/**
 * @brief Move an iterator backward by one step in the tree
 *
 * Sets the iterator to the immediate predecessor of the current node.
 * This allows the caller to perform a descending-order tree traversal.
 *
 * @param iter          The iterator object to move
 * @retval BPLIB_SUCCESS if iterator is valid
 */
int BPLib_MEM_ListIterReverse(BPLib_MEM_ListIter_t *iter);

/**
 * @brief Process every item in the list in sequential order
 *
 * The callback function will be invoked for every item in the list, except for the head node.
 *
 * If "always_remove" is true, the item will be removed from the list prior to invoking the call.
 * In this case, the callback function must guarantee to place the block onto another list (or
 * some other tracking facility) to not leak blocks.
 *
 * @param list
 * @param always_remove
 * @param callback_fn
 * @param callback_arg
 * @return int Number of items that were in the list
 */
int BPLib_MEM_ForeachItemInList(BPLib_MEM_Block_t *list, bool always_remove,
                                BPLib_MEM_CallbackFunc_t callback_fn, void *callback_arg);

/**
 * @brief Search a list in sequential order
 *
 * The match function will be invoked for every entry in the list, and the supplied argument
 * will be passed to it. If the function returns 0, then the search stops and the node is returned.
 *
 * @param list The list to search
 * @param match_fn A function that should return 0 if a match is found, nonzero otherwise
 * @param match_arg An opaque argument passed to the match_fn, typically the match reference object
 * @return bplib_mpool_block_t* The matching list entry
 * @retval NULL if no match was found
 */
BPLib_MEM_Block_t *BPLib_MEM_SearchList(const BPLib_MEM_Block_t *list,
                                        BPLib_MEM_CallbackFunc_t match_fn,
                                        void *match_arg);

/**
 * @brief Append a single bundle to the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @param cpb
 */
void BPLib_MEM_SubqPushSingle(BPLib_MEM_SubqBase_t *subq, BPLib_MEM_Block_t *cpb);

/**
 * @brief Get the next bundle from the given sub-queue
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq
 * @return BPLib_MEM_Block_t*
 */
BPLib_MEM_Block_t *BPLib_MEM_SubqPullSingle(BPLib_MEM_SubqBase_t *subq);

/**
 * @brief Counts the number of blocks in a list
 *
 * For record-keeping or stats-reporting purposes, this counts the number of blocks in a list
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param list the subject list head
 * @return uint32_t number of blocks
 */
uint32_t BPLib_MEM_ListCountBlocks(BPLib_MEM_Block_t *list);

/**
 * @brief Pushes an entire list of blocks into a subq
 *
 * Similar to BPLib_MEM_SubqMoveAll() but the source is just a simple list rather
 * than a subq FIFO object.
 *
 * Because simple lists do not track the number of entries, this counts the blocks in the
 * list prior to moving the items, so the dest queue push stats will remain correct.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst the destination queue
 * @param list  the source list
 * @return uint32_t number of blocks moved
 */
uint32_t BPLib_MEM_SubqMergeList(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_Block_t *list);

/**
 * @brief Merges the entire contents of the source subq into the destination
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param subq_dst
 * @param subq_src
 * @return The number of queue entries moved
 */
uint32_t BPLib_MEM_SubqMoveAll(BPLib_MEM_SubqBase_t *subq_dst, BPLib_MEM_SubqBase_t *subq_src);

/**
 * @brief Drops the entire contents of a subq
 *
 * This purges the contents of a queue, such as when the interface is set to a "down" state,
 * this removes any pending items that were in it.
 *
 * @note This should only be called from internal contexts where a lock is held
 *
 * @param pool
 * @param subq
 * @return uint32_t The number of queue entries dropped
 */
uint32_t BPLib_MEM_SubqDropAll(BPLib_MEM_Pool_t *pool, BPLib_MEM_SubqBase_t *subq);

/* gets to the underlying block content (which may be a ref block) */
BPLib_MEM_BlockContent_t       *BPLib_MEM_GetBlockContent(BPLib_MEM_Block_t *cb);
const BPLib_MEM_BlockContent_t *BPLib_MEM_GetBlockContentConst(const BPLib_MEM_Block_t *cb);

BPLib_MEM_BlockContent_t *BPLib_MEM_AllocBlockInternal(BPLib_MEM_Pool_t *pool,
    BPLib_MEM_Blocktype_t blocktype, uint32_t content_type_signature, void *init_arg, uint8_t priority);

void BPLib_MEM_InitBaseObject(BPLib_MEM_BlockHeader_t *block_hdr, uint16_t user_content_length,
                                  uint32_t content_type_signature);

void BPLib_MEM_BblockPrimaryInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockPrimary_t *pblk);

void BPLib_MEM_BblockCanonicalInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_BblockCanonical_t *cblk);

BPLib_MEM_Block_t *BPLib_MEM_BblockPrimaryAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number,
                                                void *init_arg, uint8_t priority, BPLib_TIME_MonotonicTime_t timeout);

BPLib_MEM_Block_t *BPLib_MEM_BblockCanonicalAlloc(BPLib_MEM_Pool_t *pool, uint32_t magic_number, void *init_arg);

void BPLib_MEM_DuctInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Duct_t *fblk);

void BPLib_MEM_JobInit(BPLib_MEM_Block_t *base_block, BPLib_MEM_Job_t *jblk);

static inline bool BPLib_MEM_DuctIsUp(const BPLib_MEM_Duct_t *duct)
{
    return (duct->current_state_flags & (BPLIB_CACHE_STATE_FLAG_ADMIN_UP | BPLIB_CACHE_STATE_FLAG_OPER_UP)) == 0;
}

void BPLib_MEM_InitSecondaryLink(BPLib_MEM_Block_t *base_block, BPLib_MEM_Block_t *secondary_link,
                                 BPLib_MEM_Blocktype_t block_type);

#endif /* BPLIB_MEM_INTERNAL_H */

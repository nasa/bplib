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

#ifndef BPLIB_MEM_INTERNAL_SUBQS_H
#define BPLIB_MEM_INTERNAL_SUBQS_H

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

    BPLib_RBT_Root_t   blocktype_registry; /**< registry of block signature values */
    BPLib_MEM_ApiContent_t blocktype_basic;    /**< a fixed entity in the registry for type 0 */
    BPLib_MEM_ApiContent_t blocktype_cbor;     /**< a fixed entity in the registry for CBOR blocks */

    BPLib_MEM_SubqBase_t free_blocks;    /**< blocks which are available for use */
    BPLib_MEM_SubqBase_t recycle_blocks; /**< blocks which can be garbage-collected */

    /* note that the active_list and managed_block_list are not FIFO in nature, as blocks
     * can be removed from the middle of the list or otherwise rearranged. Therefore a subq
     * is not used for these, because the push_count and pull_count would not remain accurate. */

    BPLib_MEM_Block_t active_list; /**< a list of ducts/queues that need processing */

} BPLib_MEM_BlockAdminContent_t;

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

#endif /* BPLIB_MEM_INTERNAL_H */

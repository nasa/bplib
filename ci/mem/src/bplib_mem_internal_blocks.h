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

#ifndef BPLIB_MEM_INTERNAL_BLOCKS_H
#define BPLIB_MEM_INTERNAL_BLOCKS_H

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
    BPLib_RBT_Link_t     rbt_link;
    BPLib_MEM_BlocktypeApi_t api;
    size_t                   user_content_size;
    BPLib_MEM_AlignedData_t  user_data_start;
} BPLib_MEM_ApiContent_t;

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

#endif /* BPLIB_MEM_INTERNAL_BLOCKS_H */

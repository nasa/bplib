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

#ifndef BPLIB_STOR_QM_MODULE_API_H
#define BPLIB_STOR_QM_MODULE_API_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/
typedef enum BPLib_STOR_QM_ModuleType
{
    BPLib_STOR_QM_ModuleTypeNone,
    BPLib_STOR_QM_ModuleTypeInternal,
    BPLib_STOR_QM_ModuleTypeOffload
} BPLib_STOR_QM_ModuleType_t;

typedef enum BPLib_STOR_QM_ModuleValtype
{
    BPLib_STOR_QM_ModuleValtypeNone,
    BPLib_STOR_QM_ModuleValtypeInteger,
    BPLib_STOR_QM_ModuleValtypeString
} BPLib_STOR_QM_ModuleValtype_t;

typedef enum BPLib_STOR_QM_Confkey
{
    BPLib_STOR_QM_ConfkeyNone,
    BPLib_STOR_QM_ConfkeyOffloadBaseDir,
} BPLib_STOR_QM_Confkey_t;

struct BPLib_STOR_QM_ModuleApi
{
    BPLib_STOR_QM_ModuleType_t module_type;
    BPLib_STOR_CACHE_Block_t *(*instantiate)(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg);
    int (*configure)(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t cache_intf_id, int key,
                     BPLib_STOR_QM_ModuleValtype_t vt, const void *val);
    int (*query)(BPLib_STOR_CACHE_Pool_t *tbl, BPLib_Handle_t module_intf_id, int key,
                 BPLib_STOR_QM_ModuleValtype_t vt, const void **val);
    int (*start)(BPLib_STOR_CACHE_Block_t *tbl, BPLib_Handle_t module_intf_id);
    int (*stop)(BPLib_STOR_CACHE_Block_t *tbl, BPLib_Handle_t module_intf_id);
};

/* Storage service - reserved for future use */
typedef struct BPLib_STOR_QM_Stor
{
    uint32_t reserved;
} BPLib_STOR_QM_Stor_t;

/* Channel Statistics */
typedef struct BPLib_STOR_QM_Stats
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
} BPLib_STOR_QM_Stats_t;

typedef enum
{
   BPLib_STOR_CACHE_VariableNone,            /**< reserved value, keep first */
   BPLib_STOR_CACHE_VariableMemCurrentUse, /**< replaces BPLib_STOR_CACHE_OsMemused() for external API use */
   BPLib_STOR_CACHE_VariableMemHighUse,    /**< replaces BPLib_STOR_CACHE_OsMemhigh() for external API use */
   BPLib_STOR_CACHE_VariableMax              /**< reserved value, keep last */
}BPLib_STOR_CACHE_Variable_t;

// TODO End of block that belongs in BPLib_STOR_QM_ModuleApiTypes.h
/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

BPLib_STOR_CACHE_Block_t *BPLib_STOR_QM_Instantiate(BPLib_STOR_CACHE_Ref_t parent, void *init_arg);

/* Service API */
BPLib_Handle_t BPLib_STOR_QM_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const BPLib_IpnAddr_t *service_addr);
int         BPLib_STOR_QM_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const BPLib_IpnAddr_t *service_addr);

BPLib_Handle_t BPLib_STOR_QM_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t cache_intf_id,
                                                const BPLib_STOR_QM_ModuleApi_t *api, void *init_arg);

int BPLib_STOR_QM_Configure(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t module_intf_id, int key,
                               BPLib_STOR_QM_ModuleValtype_t vt, const void *val);
int BPLib_STOR_QM_Query(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t module_intf_id, int key,
                           BPLib_STOR_QM_ModuleValtype_t vt, const void **val);
int BPLib_STOR_QM_Start(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t module_intf_id);
int BPLib_STOR_QM_Stop(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t module_intf_id);

void BPLib_STOR_QM_DebugScan(BPLib_STOR_QM_QueueTbl_t *tbl, BPLib_Handle_t intf_id);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_QM_MODULE_API_H */

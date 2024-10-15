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

#ifndef BPLIB_STOR_CACHE_MODULE_API_H
#define BPLIB_STOR_CACHE_MODULE_API_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#include "bplib_stor_qm.h"

#include "bplib_stor_ps_file_offload.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/
typedef enum BPLib_STOR_CACHE_Type
{
    BPLib_STOR_CACHE_ModuleTypeNone,
    BPLib_STOR_CACHE_ModuleTypeInternal,
    BPLib_STOR_CACHE_ModuleTypeOffload
} BPLib_STOR_CACHE_ModuleType_t;

typedef enum BPLib_STOR_CACHE_ModuleValtype
{
    BPLib_STOR_CACHE_ModuleValtypeNone,
    BPLib_STOR_CACHE_ModuleValtypeInteger,
    BPLib_STOR_CACHE_ModuleValtypeString
} BPLib_STOR_CACHE_ModuleValtype_t;

typedef enum BPLib_STOR_CACHE_Confkey
{
    BPLib_STOR_CACHE_ConfkeyNone,
    BPLib_STOR_CACHE_ConfkeyOffloadBaseDir,
} BPLib_STOR_CACHE_Confkey_t;

// TODO Differentiate between CACHE Api and File Offload API.
/**
 * Heritage File Offload API  Now in bplib_stor_ps_file_offload.h.
 * int BPLib_STOR_CACHE_FileOffloadConstructBlock(void *arg, BPLib_STOR_CACHE_MpoolBlock_t *blk)
 * int BPLib_STOR_CACHE_FileOffloadConfigure(BPLib_STOR_CACHE_MpoolBlock_t *svc, int key, BPLib_STOR_CACHE_CacheModuleValtype_t vt, const void *val)
 * int BPLib_STOR_CACHE_FileOffloadQuery(BPLib_STOR_CACHE_MpoolBlock_t *svc, int key, BPLib_STOR_CACHE_CacheModuleValtype_t vt, const void **val)
 * int BPLib_STOR_CACHE_FileOffloadStart(BPLib_STOR_CACHE_MpoolBlock_t *svc)
 * int BPLib_STOR_CACHE_FileOffloadStop(BPLib_STOR_CACHE_MpoolBlock_t *svc)
 *
 * Heritage CACHE API
 * int BPLib_STOR_CACHE_CacheConfigure(BPLib_STOR_CACHE_Routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_CacheModuleValtype_t vt,
 * int BPLib_STOR_CACHE_CacheQuery(BPLib_STOR_CACHE_Routetbl_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_CacheModuleValtype_t vt,
 * int BPLib_STOR_CACHE_CacheStart(BPLib_STOR_CACHE_Routetbl_t *tbl, bp_handle_t module_intf_id);
 * int BPLib_STOR_CACHE_CacheStop(BPLib_STOR_CACHE_Routetbl_t *tbl, bp_handle_t module_intf_id)
 */

// Declarations must match the declarations in BPLib_STOR_CACHE_StorCacheModuleApi.h.
struct BPLib_STOR_CACHE_ModuleApi
{
    BPLib_STOR_CACHE_ModuleType_t module_type;
    BPLib_STOR_CACHE_Block_t *(*instantiate)(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg);
    int (*configure)(BPLib_STOR_CACHE_Pool_t *svc, bp_handle_t cache_intf_id, int key,
                     BPLib_STOR_CACHE_ModuleValtype_t vt, const void *val);
    int (*query)(BPLib_STOR_CACHE_Pool_t *tbl, bp_handle_t module_intf_id, int key,
                 BPLib_STOR_CACHE_ModuleValtype_t vt, const void **val);
    int (*start)(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id);
    int (*stop)(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id);
};

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

BPLib_STOR_CACHE_Block_t *BPLib_STOR_CACHE_Instantiate(BPLib_STOR_CACHE_Ref_t parent, void *init_arg);

/* Service API */
bp_handle_t BPLib_STOR_CACHE_Attach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr);
int         BPLib_STOR_CACHE_Detach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *service_addr);

bp_handle_t BPLib_STOR_CACHE_RegisterModuleService(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t cache_intf_id,
                                                const BPLib_STOR_CACHE_ModuleApi_t *api, void *init_arg);

int BPLib_STOR_CACHE_Configure(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key,
                               BPLib_STOR_CACHE_ModuleValtype_t vt, const void *val);
int BPLib_STOR_CACHE_Query(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id, int key,
                           BPLib_STOR_CACHE_ModuleValtype_t vt, const void **val);
int BPLib_STOR_CACHE_Start(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id);
int BPLib_STOR_CACHE_Stop(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t module_intf_id);

void BPLib_STOR_CACHE_DebugScan(BPLib_STOR_QM_QueueTbl_t *tbl, bp_handle_t intf_id);

#ifdef NEED_CACHE_INTERNAL_API
// TODO Is this "CACHE_INTERNAL_API" model needed? File offload has its own.
const BPLib_STOR_CACHE_ModuleApi_t BPLIB_STOR_CACHE_INTERNAL_API = {
    .module_type = BPLib_STOR_CACHE_ModuleTypeOffload,
    .instantiate = BPLib_STOR_CACHE_Instantiate,
    .configure   = BPLib_STOR_CACHE_Configure,
    .query       = BPLib_STOR_CACHE_Query,
    .start       = BPLib_STOR_CACHE_Start,
    .stop        = BPLib_STOR_CACHE_Stop,
};

const BPLib_STOR_CACHE_ModuleApi_t *BPLIB_FILE_OFFLOAD_API =
    (const BPLib_STOR_CACHE_ModuleApi_t *)&BPLIB_STOR_CACHE_INTERNAL_API;
#endif // NEED_CACHE_INTERNAL_API

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_CACHE_MODULE_API_H */

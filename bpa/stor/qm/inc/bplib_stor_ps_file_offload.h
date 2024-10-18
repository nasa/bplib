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

#ifndef BPLIB_STOR_PS_FILE_OFFLOAD_H
#define BPLIB_STOR_PS_FILE_OFFLOAD_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_api_types.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

// TODO PS extern const BPLib_STOR_QM_ModuleApi_t *BPLIB_FILE_OFFLOAD_API;

/**
 * The Heritage CACHE API is in bplib_stor_cache_module_api.h.
 * Heritage File Offload API
 * int bplib_file_offload_construct_block(void *arg, bplib_mpool_block_t *blk)
 * int bplib_file_offload_configure(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void *val)
 * int bplib_file_offload_query(bplib_mpool_block_t *svc, int key, bplib_cache_module_valtype_t vt, const void **val)
 * int bplib_file_offload_start(bplib_mpool_block_t *svc)
 * int bplib_file_offload_stop(bplib_mpool_block_t *svc)
 */

#ifdef QM_MODULE_API
struct BPLib_STOR_QM_ModuleApi
{
    BPLib_STOR_QM_ModuleType_t module_type;
    BPLib_STOR_CACHE_Block_t *(*instantiate)(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg);
    int (*configure)(BPLib_STOR_CACHE_Pool_t *svc, BPLib_Handle_t ps_intf_id, int key,
                     BPLib_STOR_QM_ModuleValtype_t vt, const void *val);
    int (*query)(BPLib_STOR_CACHE_Pool_t *tbl, BPLib_Handle_t module_intf_id, int key,
                 BPLib_STOR_QM_ModuleValtype_t vt, const void **val);
    int (*start)(BPLib_STOR_CACHE_Block_t *tbl, BPLib_Handle_t module_intf_id);
    int (*stop)(BPLib_STOR_CACHE_Block_t *tbl, BPLib_Handle_t module_intf_id);
};

typedef struct BPLib_STOR_PS_OffloadApi
{
    BPLib_STOR_QM_ModuleApi_t std;
    int (*offload)(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk);
    int (*restore)(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid, BPLib_STOR_CACHE_Block_t **pblk);
    int (*release)(BPLib_STOR_CACHE_Block_t *svc, BPLib_STOR_CACHE_Sid_t sid);
} BPLib_STOR_PS_OffloadApi_t;

static inline int BPLib_STOR_CACHE_FileOffloadConstructBlock(void *arg, BPLib_STOR_CACHE_Block_t *blk)
{
    return BPLIB_SUCCESS;
}
#endif // QM_MODULE_API

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_PS_FILE_OFFLOAD_H */

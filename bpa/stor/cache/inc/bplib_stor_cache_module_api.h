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

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

struct BPLib_STOR_CACHE_ModuleApi
{
    BPLib_STOR_CACHE_ModuleType_t module_type;
    BPLib_STOR_CACHE_Block_t *(*instantiate)(BPLib_STOR_CACHE_Ref_t parent_ref, void *init_arg);
    int (*configure)(BPLib_STOR_CACHE_Block_t *svc, bp_handle_t cache_intf_id, int key,
                     BPLib_STOR_CACHE_ModuleValtype_t vt, const void *val);
    int (*query)(BPLib_STOR_CACHE_Block_t *svc, int key, BPLib_STOR_CACHE_ModuleValtype_t vt,
                 const void **val);
    int (*start)(BPLib_STOR_CACHE_Block_t *svc);
    int (*stop)(BPLib_STOR_CACHE_Block_t *svc);
};

struct BPLib_STOR_CACHE_OffloadApi
{
    BPLib_STOR_CACHE_ModuleApi_t std;
    int (*offload)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t *sid, BPLib_STOR_CACHE_Block_t *pblk);
    int (*restore)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid, BPLib_STOR_CACHE_Block_t **pblk);
    int (*release)(BPLib_STOR_CACHE_Block_t *svc, bp_sid_t sid);
};

extern const BPLib_STOR_CACHE_ModuleApi_t *BPLIB_FILE_OFFLOAD_API;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STOR_CACHE_MODULE_API_H */

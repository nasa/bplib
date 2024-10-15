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

#ifdef __cplusplus
extern "C" {
#endif

#include "bplib_mem.h"

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"
#include "bplib_stor_cache_module_api.h"

bp_handle_t BPLib_STOR_CACHE_CreateFileStorage(BPLib_STOR_CACHE_Block_t *rtbl, const bp_ipn_addr_t *storage_addr)
{
    bp_handle_t intf_id;
    bp_handle_t svc_id;
    char        storage_path[64];

    intf_id = BPLib_STOR_CACHE_Attach(rtbl, storage_addr);
    if (bp_handle_is_valid(intf_id))
    {
        svc_id = BPLib_STOR_CACHE_RegisterModuleService(rtbl, intf_id, BPLIB_FILE_OFFLOAD_API, NULL);

        if (bp_handle_is_valid(svc_id))
        {
            snprintf(storage_path, sizeof(storage_path), "./storage/%u.%u", (unsigned int)storage_addr->node_number,
                     (unsigned int)storage_addr->service_number);
            BPLib_STOR_CACHE_Configure(rtbl, intf_id, BPLib_STOR_CACHE_ConfkeyOffloadBaseDir,
                                       BPLib_STOR_CACHE_ModuleValtypeString, storage_path);
            BPLib_STOR_CACHE_Start(rtbl, intf_id);
        }
    }

    return intf_id;
}

bp_handle_t BPLib_STOR_CACHE_CreateNodeIntf(BPLib_STOR_CACHE_Block_t *rtbl, bp_ipn_t node_num)
{
    bp_handle_t intf_id;

    intf_id = BPLib_STOR_CACHE_DataserviceAddBaseIntf(rtbl, node_num);
    if (!bp_handle_is_valid(intf_id))
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "BPLib_STOR_CACHE_AddIntf failed\n");
        return BP_INVALID_HANDLE;
    }

    if (BPLib_STOR_CACHE_Add(rtbl, node_num, ~(bp_ipn_t)0, intf_id) < 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "BPLib_STOR_CACHE_Add failed\n");
        return BP_INVALID_HANDLE;
    }

    return intf_id;
}

#ifdef __cplusplus
} // extern "C"
#endif

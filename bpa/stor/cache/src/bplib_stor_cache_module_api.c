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

/******************************************************************************
 INCLUDES
 ******************************************************************************/
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "bplib.h"
#include "crc.h"
#include "bplib_stor_qm_eid.h"
#include "bplib_mem.h"

#include "bplib_stor_cache.h"
#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_codec.h"
#include "bplib_stor_cache.h"
#include "bplib_stor_cache_dataservice.h"
#include "bplib_stor_cache_module_api.h"

#include "bplib_stor_qm.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

static const BPLib_STOR_CACHE_OffloadApi_t BPLIB_FILE_OFFLOAD_INTERNAL_API = {
    .std.module_type = BPLib_STOR_CACHE_TypeOffload,
    .std.instantiate = BPLib_STOR_PS_Instantiate,
    .std.configure   = BPLib_STOR_PS_Configure,
    .std.query       = BPLib_STOR_PS_Query,
    .std.start       = BPLib_STOR_PS_Start,
    .std.stop        = BPLib_STOR_PS_Stop,
    .offload         = BPLib_STOR_PS_Offload,
    .restore         = BPLib_STOR_PS_Restore,
    .release         = BPLib_STOR_PS_Release};

const BPLib_STOR_CACHE_ModuleApi_t *BPLIB_FILE_OFFLOAD_API =
    (const BPLib_STOR_CACHE_ModuleApi_t *)&BPLIB_FILE_OFFLOAD_INTERNAL_API;

int BPLib_STOR_PS_Configure(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_ModuleValtype_t vt,
                          const void *val)
{
    BPLib_STOR_CACHE_Block_t *cblk;
    BPLib_STOR_CACHE_State_t *state;
    int                  result;

    result = BPLIB_ERROR;
    cblk   = BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_QM_GetMpool(tbl), module_intf_id);
    state  = BPLib_STOR_CACHE_GenericDataCast(cblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        /* currently the only module is offload, so all keys are passed here */
        if (state->offload_blk != NULL)
        {
            result = state->offload_api->std.configure(state->offload_blk, key, vt, val);
        }
    }

    return result;
}
int BPLib_STOR_PS_Query(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id, int key, BPLib_STOR_CACHE_ModuleValtype_t vt,
                      const void **val)
{
    BPLib_STOR_CACHE_Block_t *cblk;
    BPLib_STOR_CACHE_State_t *state;
    int                  result;

    result = BPLIB_ERROR;
    cblk   = BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_QM_GetMpool(tbl), module_intf_id);
    state  = BPLib_STOR_CACHE_GenericDataCast(cblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        /* currently the only module is offload, so all keys are passed here */
        if (state->offload_blk != NULL)
        {
            result = state->offload_api->std.query(state->offload_blk, key, vt, val);
        }
    }

    return result;
}

int BPLib_STOR_PS_Start(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id)
{
    BPLib_STOR_CACHE_Block_t *cblk;
    BPLib_STOR_CACHE_State_t *state;
    int                  result;

    result = BPLIB_ERROR;
    cblk   = BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_QM_GetMpool(tbl), module_intf_id);
    state  = BPLib_STOR_CACHE_GenericDataCast(cblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        /* currently the only module is offload, so all keys are passed here */
        if (state->offload_blk != NULL)
        {
            result = state->offload_api->std.start(state->offload_blk);
        }
    }

    return result;
}

int BPLib_STOR_PS_Stop(BPLib_STOR_CACHE_Block_t *tbl, bp_handle_t module_intf_id)
{
    BPLib_STOR_CACHE_Block_t *cblk;
    BPLib_STOR_CACHE_State_t *state;
    int                  result;

    result = BPLIB_ERROR;
    cblk   = BPLib_STOR_CACHE_BlockFromExternalId(BPLib_STOR_QM_GetMpool(tbl), module_intf_id);
    state  = BPLib_STOR_CACHE_GenericDataCast(cblk, BPLIB_STORE_SIGNATURE_STATE);
    if (state != NULL)
    {
        /* currently the only module is offload, so all keys are passed here */
        if (state->offload_blk != NULL)
        {
            result = state->offload_api->std.stop(state->offload_blk);
        }
    }

    return result;
}

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
            BPLib_STOR_PS_Configure(rtbl, intf_id, BPLib_STOR_CACHE_ConfkeyOffloadBaseDir, storage_path);
            BPLib_STOR_PS_Start(rtbl, intf_id);
        }
    }

    return intf_id;
}

bp_handle_t BPLib_STOR_CACHE_CreateNodeIntf(BPLib_STOR_CACHE_Block_t *rtbl, bp_ipn_t node_num)
{
    bp_handle_t intf_id;

    intf_id =BPLib_STOR_CACHE_DataserviceAddBaseIntf(rtbl, node_num);
    if (!bp_handle_is_valid(intf_id))
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_OUT_OF_MEMORY, "BPLib_STOR_QM_AddIntf failed\n");
        return BP_INVALID_HANDLE;
    }

    if (BPLib_STOR_QM_Add(rtbl, node_num, ~(bp_ipn_t)0, intf_id) < 0)
    {
        // TODO remove bplog(NULL, BPLIB_FLAG_DIAGNOSTIC, "BPLib_STOR_QM_Add failed\n");
    }

    return intf_id;
}

/*--------------------------------------------------------------------------------------
 *BPLib_STOR_CACHE_Display -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  flags -                 processing flags [OUTPUT]
 *  Returns:                BPLIB_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int BPLib_STOR_CACHE_Display(const void *bundle, size_t size, uint32_t *flags)
{
    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 *BPLib_STOR_CACHE_Eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  service -               service number as read from eid [OUTPUT]
 *  Returns:                BPLIB_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int BPLib_STOR_CACHE_Eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service)
{
    char          eidtmp[BP_MAX_EID_STRING];
    int           tmplen;
    char         *node_ptr;
    char         *service_ptr;
    char         *endptr;
    unsigned long node_result;
    unsigned long service_result;

    /* Sanity Check EID Pointer */
    if (eid == NULL)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if (len < 7)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING,
                     len);
    }

    /* Check IPN Scheme */
    if (eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID (%s) must start with 'ipn:'\n", eid);
    }

    /* Copy EID to Temporary Buffer and Set Pointers */
    tmplen = len - 4;
    memcpy(eidtmp, &eid[4], tmplen);
    eidtmp[tmplen] = '\0';
    node_ptr       = eidtmp;
    service_ptr    = strchr(node_ptr, '.');
    if (service_ptr)
    {
        *service_ptr = '\0';
        service_ptr++;
    }
    else
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno       = 0;
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == node_ptr) || ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE))
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result =
        strtoul(service_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == service_ptr) || ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE))
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "Unable to parse EID (%s) service number\n", eid);
    }

    /* Set Outputs */
    *node    = (bp_ipn_t)node_result;
    *service = (bp_ipn_t)service_result;
    return BPLIB_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 *BPLib_STOR_CACHE_Ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  service -               service number to be written into eid [INPUT]
 *  Returns:                BPLIB_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int BPLib_STOR_CACHE_Ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service)
{
    /* Sanity Check EID Buffer Pointer */
    if (eid == NULL)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if (len < 7)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID buffer must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return BPLIB_ERROR;  // TODO remove bplog(NULL, BPLIB_FLAG_API_ERROR, "EID buffer cannot exceed %d bytes in length, act: %d\n",
                     BP_MAX_EID_STRING, len);
    }

    /* Write EID */
    snprintf(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)service);

    return BPLIB_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function:BPLib_STOR_CACHE_QueryInteger
 *
 * Public API function
 * See description in header for argument/return detail
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_QueryInteger(BPLib_STOR_CACHE_Block_t *rtbl, bp_handle_t intf_id,BPLib_STOR_CACHE_Variable_t var_id, bp_sval_t *value)
{
    int retval;

    retval = BPLIB_ERROR;

    switch (var_id)
    {
        case BPLib_STOR_CACHE_VariableMemCurrentUse:
            *value = BPLib_STOR_CACHE_QueryMemCurrentUse(BPLib_STOR_QM_GetMpool(rtbl));
            retval = BPLIB_SUCCESS;
            break;

        case BPLib_STOR_CACHE_VariableMemHighUse:
            *value = BPLib_STOR_CACHE_QueryMemMaxUse(BPLib_STOR_QM_GetMpool(rtbl));
            retval = BPLIB_SUCCESS;
            break;

        default:
            /* non-readable variable */
            *value = 0;
            break;
    }

    return retval;
}

/*----------------------------------------------------------------
 *
 * Function:BPLib_STOR_CACHE_ConfigInteger
 *
 * Public API function
 * See description in header for argument/return detail
 *
 *-----------------------------------------------------------------*/
int BPLib_STOR_CACHE_ConfigInteger(BPLib_STOR_CACHE_Block_t *rtbl, bp_handle_t intf_id,BPLib_STOR_CACHE_Variable_t var_id, bp_sval_t value)
{
    int retval;

    retval = BPLIB_ERROR;

    switch (var_id)
    {
        default:
            /* non-writable variable */
            break;
    }

    return retval;
}

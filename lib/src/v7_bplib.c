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

#include "bplib.h"
#include "bplib_os.h"
#include "crc.h"
#include "v7.h"
#include "v7_mpool.h"
#include "v7_codec.h"
#include "v7_cache.h"
#include "bplib_routing.h"
#include "bplib_dataservice.h"
#include "bplib_file_offload.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/*
 * Define the external wrapper type (bp_desc).
 * This is an abstract structure externally, so the contents
 * are hidden from external entities.
 */
struct bp_socket
{
    bplib_mpool_block_t fblk;
};

struct bplib_storage
{
    bp_ipn_addr_t           storage_ipn;
    const bplib_routetbl_t *parent_tbl;
};

/******************************************************************************
 LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

/*--------------------------------------------------------------------------------------
 * bplib_init - initializes bp library
 *-------------------------------------------------------------------------------------*/
int bplib_init(void)
{
    /* Initialize OS Interface */
    bplib_os_init();

    /* Initialize CRC algorithms */
    bplib_crc_init();

    /* Return Success */
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_deinit - deinitializes bp library
 *-------------------------------------------------------------------------------------*/
void bplib_deinit(void) {}

bp_handle_t bplib_create_ram_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
{
    bp_handle_t intf_id;

    intf_id = bplib_cache_attach(rtbl, storage_addr);

    return intf_id;
}

bp_handle_t bplib_create_file_storage(bplib_routetbl_t *rtbl, const bp_ipn_addr_t *storage_addr)
{
    bp_handle_t intf_id;
    bp_handle_t svc_id;
    char        storage_path[64];

    intf_id = bplib_cache_attach(rtbl, storage_addr);
    if (bp_handle_is_valid(intf_id))
    {
        svc_id = bplib_cache_register_module_service(rtbl, intf_id, BPLIB_FILE_OFFLOAD_API, NULL);

        if (bp_handle_is_valid(svc_id))
        {
            snprintf(storage_path, sizeof(storage_path), "./storage/%u.%u", (unsigned int)storage_addr->node_number,
                     (unsigned int)storage_addr->service_number);
            bplib_cache_configure(rtbl, intf_id, bplib_cache_confkey_offload_base_dir,
                                  bplib_cache_module_valtype_string, storage_path);
            bplib_cache_start(rtbl, intf_id);
        }
    }

    return intf_id;
}

bp_handle_t bplib_create_node_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_num)
{
    bp_handle_t intf_id;

    intf_id = bplib_dataservice_add_base_intf(rtbl, node_num);
    if (!bp_handle_is_valid(intf_id))
    {
        bplog(NULL, BP_FLAG_OUT_OF_MEMORY, "bplib_route_add_intf failed\n");
        return BP_INVALID_HANDLE;
    }

    if (bplib_route_add(rtbl, node_num, ~(bp_ipn_t)0, intf_id) < 0)
    {
        bplog(NULL, BP_FLAG_DIAGNOSTIC, "bplib_route_add failed\n");
    }

    return intf_id;
}

/*--------------------------------------------------------------------------------------
 * bplib_display -
 *
 *  bundle -                pointer to a bundle (byte array) [INPUT]
 *  size -                  size of bundle being pointed to [INPUT]
 *  flags -                 processing flags [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_display(const void *bundle, size_t size, uint32_t *flags)
{
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_eid2ipn -
 *
 *  eid -                   null-terminated string representation of End Point ID [INPUT]
 *  len -                   size in bytes of above string [INPUT]
 *  node -                  node number as read from eid [OUTPUT]
 *  service -               service number as read from eid [OUTPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_eid2ipn(const char *eid, size_t len, bp_ipn_t *node, bp_ipn_t *service)
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
        return bplog(NULL, BP_FLAG_API_ERROR, "EID is null\n");
    }

    /* Sanity Check Length of EID */
    if (len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID cannot exceed %d bytes in length, act: %d\n", BP_MAX_EID_STRING,
                     len);
    }

    /* Check IPN Scheme */
    if (eid[0] != 'i' || eid[1] != 'p' || eid[2] != 'n' || eid[3] != ':')
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID (%s) must start with 'ipn:'\n", eid);
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
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to find dotted notation in EID (%s)\n", eid);
    }

    /* Parse Node Number */
    errno       = 0;
    node_result = strtoul(node_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == node_ptr) || ((node_result == ULONG_MAX || node_result == 0) && errno == ERANGE))
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) node number\n", eid);
    }

    /* Parse Service Number */
    errno = 0;
    service_result =
        strtoul(service_ptr, &endptr, 10); /* assume IPN node and service numbers always written in base 10 */
    if ((endptr == service_ptr) || ((service_result == ULONG_MAX || service_result == 0) && errno == ERANGE))
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "Unable to parse EID (%s) service number\n", eid);
    }

    /* Set Outputs */
    *node    = (bp_ipn_t)node_result;
    *service = (bp_ipn_t)service_result;
    return BP_SUCCESS;
}

/*--------------------------------------------------------------------------------------
 * bplib_ipn2eid -
 *
 *  eid -                   buffer that will hold null-terminated string representation of End Point ID [OUTPUT]
 *  len -                   size in bytes of above buffer [INPUT]
 *  node -                  node number to be written into eid [INPUT]
 *  service -               service number to be written into eid [INPUT]
 *  Returns:                BP_SUCCESS or error code
 *-------------------------------------------------------------------------------------*/
int bplib_ipn2eid(char *eid, size_t len, bp_ipn_t node, bp_ipn_t service)
{
    /* Sanity Check EID Buffer Pointer */
    if (eid == NULL)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer is null\n");
    }

    /* Sanity Check Length of EID Buffer */
    if (len < 7)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer must be at least 7 characters, act: %d\n", len);
    }
    else if (len > BP_MAX_EID_STRING)
    {
        return bplog(NULL, BP_FLAG_API_ERROR, "EID buffer cannot exceed %d bytes in length, act: %d\n",
                     BP_MAX_EID_STRING, len);
    }

    /* Write EID */
    snprintf(eid, len, "ipn:%lu.%lu", (unsigned long)node, (unsigned long)service);

    return BP_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: bplib_query_integer
 *
 * Public API function
 * See description in header for argument/return detail
 *
 *-----------------------------------------------------------------*/
int bplib_query_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t *value)
{
    int retval;

    retval = BP_ERROR;

    switch (var_id)
    {
        case bplib_variable_mem_current_use:
            *value = bplib_mpool_query_mem_current_use(bplib_route_get_mpool(rtbl));
            retval = BP_SUCCESS;
            break;

        case bplib_variable_mem_high_use:
            *value = bplib_mpool_query_mem_max_use(bplib_route_get_mpool(rtbl));
            retval = BP_SUCCESS;
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
 * Function: bplib_config_integer
 *
 * Public API function
 * See description in header for argument/return detail
 *
 *-----------------------------------------------------------------*/
int bplib_config_integer(bplib_routetbl_t *rtbl, bp_handle_t intf_id, bplib_variable_t var_id, bp_sval_t value)
{
    int retval;

    retval = BP_ERROR;

    switch (var_id)
    {
        default:
            /* non-writable variable */
            break;
    }

    return retval;
}

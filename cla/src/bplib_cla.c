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

/*
** Include
*/
#include <stdio.h>

#include "bplib_cla.h"
#include "bplib_cla_internal.h"
#include "bplib_bi.h"
#include "bplib_qm.h"


/*
** Function Definitions
*/

BPLib_Status_t BPLib_CLA_Init(void) {
    return BPLIB_SUCCESS;
}

/* BPLib_CLA_Ingress - Received candidate bundles from CL */
BPLib_Status_t BPLib_CLA_Ingress(BPLib_QM_QueueTable_t* tbl, uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    const uint8_t *InBundle = Bundle;

    if (tbl == NULL)
    {
        return BPLIB_ERROR;
    }

    if (Bundle != NULL)
    {
        /* Not a RFC 9171 bundle. Can be a control message or junk*/
        if (BPLib_CLA_IsAControlMsg(InBundle))
        {
            /* Processes the control message and pass to BI*/
            BPLib_CLA_ProcessControlMessage((BPLib_CLA_CtrlMsg_t*)Bundle);
            
        }
        else
        {
            /* Receive a RFC 9171 bundle and pass it to BI */
            Status = BPLib_BI_RecvFullBundleIn(tbl, Bundle, Size);
        }
    }
    
    return Status;    
}

/* BPLib_CLA_Egress - Receive bundles from BI and send bundles out to CL */
BPLib_Status_t BPLib_CLA_Egress(BPLib_QM_QueueTable_t* tbl, uint8_t ContId, void *Bundle, size_t *Size, uint32_t Timeout)
{
    BPLib_Bundle_t* bundle;
    size_t bytes_copied;
    BPLib_MEM_Block_t* curr_block;

    if (tbl == NULL)
    {
        return BPLIB_ERROR;
    }

    if (BPLib_CI_WaitQueueTryPull(&tbl->cla_out, &bundle, Timeout))
    {
        bytes_copied = 0;
        curr_block = bundle->blob;
        while (curr_block != NULL)
        {
            memcpy((uint8_t*)Bundle + bytes_copied, curr_block->chunk, curr_block->chunk_len);
            bytes_copied += curr_block->chunk_len;
            curr_block = curr_block->next;
        }
        BPLib_MEM_BlockListFree(&tbl->pool, bundle->blob);
        BPLib_MEM_BlockFree(&tbl->pool, (BPLib_MEM_Block_t*)bundle);
        *Size = bytes_copied;
        printf("Egressing packet of %lu bytes to CLA\n", *Size);
        return BPLIB_SUCCESS;
    }
    return BPLIB_CLA_TIMEOUT;
}

/* Validate Contacts table data */
BPLib_Status_t BPLib_CLA_ContactsTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;
    BPLib_CLA_ContactsTable_t *TblDataPtr = (BPLib_CLA_ContactsTable_t *)TblData;

    /* Validate data values are within allowed range */
    if (TblDataPtr[0].ContactSet->PortNum <= 0)
    {
        /* element is out of range, return an appropriate error code */
        ReturnCode = BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}


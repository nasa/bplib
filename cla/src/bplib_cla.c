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

#include "bplib_cla.h"
#include "bplib_cla_internal.h"


/*
** Function Definitions
*/

int BPLib_CLA_Init(void) {
    return BPLIB_SUCCESS;
}

/* BPLib_CLA_Ingress - Received candidate bundles from CL */
int BPLib_CLA_Ingress(uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout)
{
    const uint8_t *InBundle = Bundle;
    if (Bundle != NULL)
    {
        /* Count candidate bundles received from CL*/
        if (*InBundle != 0x9F) /*Check CBOR indefinite-length array*/
        {
            /* Not a RFC 9171 bundle. Can be a control message or junk*/
            if (BPLib_CLA_IsAControlMsg(InBundle))
            {
                /* Processes the control message and pass to BI*/
                BPLib_CLA_ProcessControlMessage();
                
                /*For now just count the message and drop here*/
            }
        }
        else
        {
            /* Receive a RFC 9171 bundle and pass it to BI*/
            
            /*For now drop the bundles here and count them */
        }
        
    }
    
    return BPLIB_SUCCESS;    
}

/* BPLib_CLA_Egress - Receive bundles from BI and send bundles out to CL */
int BPLib_CLA_Egress(uint8_t ContId, void *Bundle, size_t *Size, uint32_t Timeout)
{
    const uint8_t *InBundle = Bundle;
    if (InBundle != NULL)
    {
        /* Receive a RFC9171 bundle from BI */
    }
    return BPLIB_SUCCESS;    
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


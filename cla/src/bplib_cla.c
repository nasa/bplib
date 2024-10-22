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
#include "bplib_bi.h"


/*
** Function Definitions
*/

BPLib_Status_t BPLib_CLA_Init(void) {
    return BPLIB_SUCCESS;
}

/* BPLib_CLA_Ingress - Received candidate bundles from CL */
BPLib_Status_t BPLib_CLA_Ingress(uint8_t ContId, const void *Bundle, size_t Size, uint32_t Timeout)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
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
                BPLib_CLA_ProcessControlMessage((BPLib_CLA_CtrlMsg_t*)Bundle);
                
            }
        }
        else
        {
            /* Receive a RFC 9171 bundle and pass it to BI*/
            Status = BPLib_BI_RecvFullBundleIn(Bundle, Size);
            if (Status != BPLIB_SUCCESS)
            {
                
            }
        }
        
    }
    
    return Status;    
}

/* BPLib_CLA_Egress - Receive bundles from BI and send bundles out to CL */
BPLib_Status_t BPLib_CLA_Egress(uint8_t ContId, void *Bundle, size_t *Size, uint32_t Timeout)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    const uint8_t *InBundle = Bundle;
    if (InBundle != NULL)
    {
        /* Receive a RFC9171 deserialized bundle from BI and send it to CL */
        Status = BPLib_BI_SendFullBundleOut(Bundle, Size);
        if (Status != BPLIB_SUCCESS)
        {

        }
    }
    return Status;    
}


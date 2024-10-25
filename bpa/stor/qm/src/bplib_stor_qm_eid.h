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

#ifndef BPLIB_STOR_QM_EID_H
#define BPLIB_STOR_QM_EID_H

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib_stor_cache_types.h"
#include "bplib_stor_cache_block.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

void         BPLib_STOR_QM_SetEid(BPLib_STOR_CACHE_EidBuffer_t *eid, const BPLib_IpnAddr_t *bundle_addr);
void         BPLib_STOR_QM_GetEid(BPLib_IpnAddr_t *bundle_addr, const BPLib_STOR_CACHE_EidBuffer_t *eid);

static inline int compare_numeric(BPLib_Val_t n1, BPLib_Val_t n2)
{
    /**
     * The comments on the return values state that they are not BPLib_Status_t
     * values for code reviews that look for invalid return values.
     */
    if (n1 == n2)
    {
        return 0;  // n1 == n2
    }
    if (n1 > n2)
    {
        return 1;  // n1 > n2
    }
    return -1;     // n1 < n2
}

/* A generic strcmp-like call to compare an IPN address to a BP endpoint ID value */
int BPLib_STOR_QM_CompareIpn2Eid(const BPLib_IpnAddr_t *ipn, const BPLib_STOR_CACHE_EidBuffer_t *eid);
/* A generic strcmp-like call to compare two IPN addresses */
int BPLib_STOR_QM_CompareIpn2Ipn(const BPLib_IpnAddr_t *ipn1, const BPLib_IpnAddr_t *ipn2);

#endif /* BPLIB_STOR_QM_EID_H */

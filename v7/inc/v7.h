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

#ifndef v7_h
#define v7_h

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "v7_types.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 EXPORTED FUNCTIONS
 ******************************************************************************/

bp_dtntime_t v7_get_current_time(void);
void         v7_set_eid(bp_endpointid_buffer_t *eid, const bp_ipn_addr_t *bp_addr);
void         v7_get_eid(bp_ipn_addr_t *bp_addr, const bp_endpointid_buffer_t *eid);

static inline int v7_compare_numeric(bp_val_t n1, bp_val_t n2)
{
    if (n1 == n2)
    {
        return 0;
    }
    if (n1 > n2)
    {
        return 1;
    }
    return -1;
}

/* A generic strcmp-like call to compare an IPN address to a BP endpoint ID value */
int v7_compare_ipn2eid(const bp_ipn_addr_t *ipn, const bp_endpointid_buffer_t *eid);
/* A generic strcmp-like call to compare two IPN addresses */
int v7_compare_ipn2ipn(const bp_ipn_addr_t *ipn1, const bp_ipn_addr_t *ipn2);

#ifdef __cplusplus
}
#endif

#endif /* v7_h */

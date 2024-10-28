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

#include "bplib_stor_qm_eid.h"

/******************************************************************************
 DEFINES
 ******************************************************************************/

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 FUNCTIONS
 ******************************************************************************/

void init_ipn_eid(BPLib_STOR_CACHE_EidBuffer_t *eid, BPLib_Ipn_t node, BPLib_Ipn_t service)
{
    eid->scheme                 = bundle_endpointid_scheme_ipn;
    eid->ssp.ipn.node_number    = node;
    eid->ssp.ipn.service_number = service;
}

void BPLib_STOR_QM_SetEid(BPLib_STOR_CACHE_EidBuffer_t *eid, const BPLib_IpnAddr_t *bundle_addr)
{
    eid->scheme                 = bundle_endpointid_scheme_ipn;
    eid->ssp.ipn.node_number    = bundle_addr->node_number;
    eid->ssp.ipn.service_number = bundle_addr->service_number;
}

void BPLib_STOR_QM_GetEid(BPLib_IpnAddr_t *bundle_addr, const BPLib_STOR_CACHE_EidBuffer_t *eid)
{
    if (eid->scheme == bundle_endpointid_scheme_ipn)
    {
        bundle_addr->node_number    = eid->ssp.ipn.node_number;
        bundle_addr->service_number = eid->ssp.ipn.service_number;
    }
    else
    {
        bundle_addr->node_number    = 0;
        bundle_addr->service_number = 0;
    }
}

int BPLib_STOR_QM_CompareIpn2Eid(const BPLib_IpnAddr_t *ipn, const BPLib_STOR_CACHE_EidBuffer_t *eid)
{
    int result;

    result = compare_numeric(eid->scheme, bundle_endpointid_scheme_ipn);
    if (result == 0)
    {
        result = compare_numeric(ipn->node_number, eid->ssp.ipn.node_number);
        if (result == 0)
        {
            result = compare_numeric(ipn->service_number, eid->ssp.ipn.service_number);
        }
    }

    return result;
}

int BPLib_STOR_QM_CompareIpn2Ipn(const BPLib_IpnAddr_t *ipn1, const BPLib_IpnAddr_t *ipn2)
{
    int result;

    result = compare_numeric(ipn1->node_number, ipn2->node_number);
    if (result == 0)
    {
        result = compare_numeric(ipn1->service_number, ipn2->service_number);
    }

    return result;
}

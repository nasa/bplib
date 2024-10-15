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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in v7 header
 */

#include "bplib_stor_cache_eid.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_CompareIpn2Eid()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_CompareIpn2Eid(const bp_ipn_addr_t *ipn, const BPLib_STOR_CACHE_EidBuffer_t *eid)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_CompareIpn2Eid, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_CompareIpn2Eid, const bp_ipn_addr_t *, ipn);
    UT_GenStub_AddParam(BPLib_STOR_QM_CompareIpn2Eid, const BPLib_STOR_CACHE_EidBuffer_t *, eid);

    UT_GenStub_Execute(BPLib_STOR_QM_CompareIpn2Eid, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_CompareIpn2Eid, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_CompareIpn2Ipn()
 * ----------------------------------------------------
 */
int BPLib_STOR_QM_CompareIpn2Ipn(const bp_ipn_addr_t *ipn1, const bp_ipn_addr_t *ipn2)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_QM_CompareIpn2Ipn, int);

    UT_GenStub_AddParam(BPLib_STOR_QM_CompareIpn2Ipn, const bp_ipn_addr_t *, ipn1);
    UT_GenStub_AddParam(BPLib_STOR_QM_CompareIpn2Ipn, const bp_ipn_addr_t *, ipn2);

    UT_GenStub_Execute(BPLib_STOR_QM_CompareIpn2Ipn, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_QM_CompareIpn2Ipn, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_GetEid()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_GetEid(bp_ipn_addr_t *bp_addr, const BPLib_STOR_CACHE_EidBuffer_t *eid)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_GetEid, bp_ipn_addr_t *, bp_addr);
    UT_GenStub_AddParam(BPLib_STOR_QM_GetEid, const BPLib_STOR_CACHE_EidBuffer_t *, eid);

    UT_GenStub_Execute(BPLib_STOR_QM_GetEid, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_QM_SetEid()
 * ----------------------------------------------------
 */
void BPLib_STOR_QM_SetEid(BPLib_STOR_CACHE_EidBuffer_t *eid, const bp_ipn_addr_t *bp_addr)
{
    UT_GenStub_AddParam(BPLib_STOR_QM_SetEid, BPLib_STOR_CACHE_EidBuffer_t *, eid);
    UT_GenStub_AddParam(BPLib_STOR_QM_SetEid, const bp_ipn_addr_t *, bp_addr);

    UT_GenStub_Execute(BPLib_STOR_QM_SetEid, Basic, NULL);
}

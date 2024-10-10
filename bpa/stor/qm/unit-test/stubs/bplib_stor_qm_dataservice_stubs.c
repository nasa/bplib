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
 * Auto-Generated stub implementations for functions defined in bplib_stor_qm_dataservice header
 */

#include "bplib_stor_qm_dataservice.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DataserviceAddBaseIntf()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_CACHE_DataserviceAddBaseIntf(BPLib_STOR_QM_QueueTbl_t *rtbl, bp_ipn_t node_number)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_DataserviceAddBaseIntf, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAddBaseIntf, BPLib_STOR_QM_QueueTbl_t *, rtbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAddBaseIntf, bp_ipn_t, node_number);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DataserviceAddBaseIntf, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_DataserviceAddBaseIntf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DataserviceAttach()
 * ----------------------------------------------------
 */
bp_handle_t BPLib_STOR_CACHE_DataserviceAttach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn,
                                               BPLib_STOR_CACHE_DataserviceType_t type, BPLib_STOR_CACHE_Ref_t blkref)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_DataserviceAttach, bp_handle_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAttach, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAttach, const bp_ipn_addr_t *, ipn);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAttach, BPLib_STOR_CACHE_DataserviceType_t, type);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceAttach, BPLib_STOR_CACHE_Ref_t, blkref);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DataserviceAttach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_DataserviceAttach, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for BPLib_STOR_CACHE_DataserviceDetach()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Ref_t BPLib_STOR_CACHE_DataserviceDetach(BPLib_STOR_QM_QueueTbl_t *tbl, const bp_ipn_addr_t *ipn)
{
    UT_GenStub_SetupReturnBuffer(BPLib_STOR_CACHE_DataserviceDetach, BPLib_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceDetach, BPLib_STOR_QM_QueueTbl_t *, tbl);
    UT_GenStub_AddParam(BPLib_STOR_CACHE_DataserviceDetach, const bp_ipn_addr_t *, ipn);

    UT_GenStub_Execute(BPLib_STOR_CACHE_DataserviceDetach, Basic, NULL);

    return UT_GenStub_GetReturnValue(BPLib_STOR_CACHE_DataserviceDetach, BPLib_STOR_CACHE_Ref_t);
}

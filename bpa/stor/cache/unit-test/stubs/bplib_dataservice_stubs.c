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
 * Auto-Generated stub implementations for functions defined in bplib_dataservice header
 */

#include "bplib_dataservice.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_dataservice_add_base_intf()
 * ----------------------------------------------------
 */
bp_handle_t bplib_dataservice_add_base_intf(bplib_routetbl_t *rtbl, bp_ipn_t node_number)
{
    UT_GenStub_SetupReturnBuffer(bplib_dataservice_add_base_intf, bp_handle_t);

    UT_GenStub_AddParam(bplib_dataservice_add_base_intf, bplib_routetbl_t *, rtbl);
    UT_GenStub_AddParam(bplib_dataservice_add_base_intf, bp_ipn_t, node_number);

    UT_GenStub_Execute(bplib_dataservice_add_base_intf, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_dataservice_add_base_intf, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_dataservice_attach()
 * ----------------------------------------------------
 */
bp_handle_t bplib_dataservice_attach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn, bplib_dataservice_type_t type,
                                     BPLib_STOR_CACHE_Ref_t blkref)
{
    UT_GenStub_SetupReturnBuffer(bplib_dataservice_attach, bp_handle_t);

    UT_GenStub_AddParam(bplib_dataservice_attach, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_dataservice_attach, const bp_ipn_addr_t *, ipn);
    UT_GenStub_AddParam(bplib_dataservice_attach, bplib_dataservice_type_t, type);
    UT_GenStub_AddParam(bplib_dataservice_attach, BPLib_STOR_CACHE_Ref_t, blkref);

    UT_GenStub_Execute(bplib_dataservice_attach, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_dataservice_attach, bp_handle_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for bplib_dataservice_detach()
 * ----------------------------------------------------
 */
BPLib_STOR_CACHE_Ref_t bplib_dataservice_detach(bplib_routetbl_t *tbl, const bp_ipn_addr_t *ipn)
{
    UT_GenStub_SetupReturnBuffer(bplib_dataservice_detach, BPLib_STOR_CACHE_Ref_t);

    UT_GenStub_AddParam(bplib_dataservice_detach, bplib_routetbl_t *, tbl);
    UT_GenStub_AddParam(bplib_dataservice_detach, const bp_ipn_addr_t *, ipn);

    UT_GenStub_Execute(bplib_dataservice_detach, Basic, NULL);

    return UT_GenStub_GetReturnValue(bplib_dataservice_detach, BPLib_STOR_CACHE_Ref_t);
}

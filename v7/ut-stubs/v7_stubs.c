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

#include "v7.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for v7_compare_ipn2eid()
 * ----------------------------------------------------
 */
int v7_compare_ipn2eid(const bp_ipn_addr_t *ipn, const bp_endpointid_buffer_t *eid)
{
    UT_GenStub_SetupReturnBuffer(v7_compare_ipn2eid, int);

    UT_GenStub_AddParam(v7_compare_ipn2eid, const bp_ipn_addr_t *, ipn);
    UT_GenStub_AddParam(v7_compare_ipn2eid, const bp_endpointid_buffer_t *, eid);

    UT_GenStub_Execute(v7_compare_ipn2eid, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_compare_ipn2eid, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_compare_ipn2ipn()
 * ----------------------------------------------------
 */
int v7_compare_ipn2ipn(const bp_ipn_addr_t *ipn1, const bp_ipn_addr_t *ipn2)
{
    UT_GenStub_SetupReturnBuffer(v7_compare_ipn2ipn, int);

    UT_GenStub_AddParam(v7_compare_ipn2ipn, const bp_ipn_addr_t *, ipn1);
    UT_GenStub_AddParam(v7_compare_ipn2ipn, const bp_ipn_addr_t *, ipn2);

    UT_GenStub_Execute(v7_compare_ipn2ipn, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_compare_ipn2ipn, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_get_current_time()
 * ----------------------------------------------------
 */
bp_dtntime_t v7_get_current_time(void)
{
    UT_GenStub_SetupReturnBuffer(v7_get_current_time, bp_dtntime_t);

    UT_GenStub_Execute(v7_get_current_time, Basic, NULL);

    return UT_GenStub_GetReturnValue(v7_get_current_time, bp_dtntime_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_get_eid()
 * ----------------------------------------------------
 */
void v7_get_eid(bp_ipn_addr_t *bp_addr, const bp_endpointid_buffer_t *eid)
{
    UT_GenStub_AddParam(v7_get_eid, bp_ipn_addr_t *, bp_addr);
    UT_GenStub_AddParam(v7_get_eid, const bp_endpointid_buffer_t *, eid);

    UT_GenStub_Execute(v7_get_eid, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for v7_set_eid()
 * ----------------------------------------------------
 */
void v7_set_eid(bp_endpointid_buffer_t *eid, const bp_ipn_addr_t *bp_addr)
{
    UT_GenStub_AddParam(v7_set_eid, bp_endpointid_buffer_t *, eid);
    UT_GenStub_AddParam(v7_set_eid, const bp_ipn_addr_t *, bp_addr);

    UT_GenStub_Execute(v7_set_eid, Basic, NULL);
}

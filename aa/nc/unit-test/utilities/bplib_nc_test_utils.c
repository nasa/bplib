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

/* ======== */
/* Includes */
/* ======== */

#include "bplib_nc_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t Test_BPA_ADUP_AddApplication_Success(uint8 ChanId)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t Test_BPA_ADUP_AddApplication_ChanErr(uint8 ChanId)
{
    return BPLIB_ADU_ADD_CHAN_ERR;
}

BPLib_Status_t Test_BPA_ADUP_AddApplication_StateErr(uint8 ChanId)
{
    return BPLIB_ADU_ADD_STAT_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StartApplication_Success(uint8 ChanId)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t Test_BPA_ADUP_StartApplication_ChanErr(uint8 ChanId)
{
    return BPLIB_ADU_START_CHAN_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StartApplication_StateErr(uint8 ChanId)
{
    return BPLIB_ADU_START_STAT_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StartApplication_SubErr(uint8 ChanId)
{
    return BPLIB_ADU_START_SUB_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StopApplication_Success(uint8 ChanId)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t Test_BPA_ADUP_StopApplication_ChanErr(uint8 ChanId)
{
    return BPLIB_ADU_STOP_CHAN_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StopApplication_StateErr(uint8 ChanId)
{
    return BPLIB_ADU_STOP_STAT_ERR;
}

BPLib_Status_t Test_BPA_ADUP_StopApplication_UnsubErr(uint8 ChanId)
{
    return BPLIB_ADU_STOP_UNSUB_ERR;
}

void BPLib_NC_Test_Setup(void)
{
    /* Initialize test environment to default state for every test */
    UT_ResetState(0);
}

void BPLib_NC_Test_Teardown(void)
{
    /* Clean up test environment */
}

void UtTest_Setup(void)
{
    TestBplibNc_Register();
}

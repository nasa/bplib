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
 * Include
 */

#include "bplib_pi_test_utils.h"

/* Test nominal add application function */
void Test_BPLib_PI_AddApplication_Nominal(void)
{
    uint8_t ChanId = 0;

    UtAssert_INT32_EQ(BPLib_PI_AddApplication(ChanId), BPLIB_SUCCESS);
}

/* Test nominal remove application function */
void Test_BPLib_PI_RemoveApplication_Nominal(void)
{
    uint8_t ChanId = 0;

    UtAssert_INT32_EQ(BPLib_PI_RemoveApplication(ChanId), BPLIB_SUCCESS);
}

/* Test nominal validate configs function */
void Test_BPLib_PI_ValidateConfigs_Nominal(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_SUCCESS);
}

/* Test validate configs function when it fails */
void Test_BPLib_PI_ValidateConfigs_Failure(void)
{
    BPLib_PI_ChannelTable_t ChanTbl;

    memset(&ChanTbl, 0, sizeof(ChanTbl));

    ChanTbl.Configs[0].MaxBundlePayloadSize = -1;

    UtAssert_INT32_EQ(BPLib_PI_ValidateConfigs(&ChanTbl), BPLIB_PI_INVALID_CONFIG_ERROR);
}

/* Test nominal ingress function */
void Test_BPLib_PI_Ingress_Nominal(void)
{
    uint8_t ChanId = 0;
    void *AduPtr = NULL;
    size_t AduSize = 0;

    UtAssert_INT32_EQ(BPLib_PI_Ingress(ChanId, AduPtr, AduSize), BPLIB_SUCCESS);
}

/* Test nominal egress function */
void Test_BPLib_PI_Egress_Nominal(void)
{
    uint8_t ChanId = 0;
    void *BundlePtr = NULL;
    size_t BundleSize = 0;

    UtAssert_INT32_EQ(BPLib_PI_Egress(ChanId, BundlePtr, BundleSize), BPLIB_SUCCESS);
}


void TestBplibPi_Register(void)
{
    ADD_TEST(Test_BPLib_PI_AddApplication_Nominal);

    ADD_TEST(Test_BPLib_PI_RemoveApplication_Nominal);

    ADD_TEST(Test_BPLib_PI_ValidateConfigs_Nominal);
    ADD_TEST(Test_BPLib_PI_ValidateConfigs_Failure);

    ADD_TEST(Test_BPLib_PI_Ingress_Nominal);

    ADD_TEST(Test_BPLib_PI_Egress_Nominal);
}

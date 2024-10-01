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

#include "bplib_nc.h"
#include "bplib_nc_test_utils.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_NC_Init(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_NoopCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_NoopCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAllApplicationsCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_AddAllApplicationsCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartAllApplicationsCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_StartAllApplicationsCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleStorageCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleStorageCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_InitBundleStorageCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_InitBundleStorageCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleMetadataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleMetadataCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RebuildBundleMetadataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_RebuildBundleMetadataCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ClearVolatileCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ClearVolatileCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ReloadSavedDataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ReloadSavedDataCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetAllCountersCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ResetAllCountersCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetCounterCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ResetCounterCmd_Payload_t Payload;

    Payload.ExampleParameter = 42;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ResetCounterCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetSourceCountersCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ResetSourceCountersCmd_Payload_t Payload;

    Payload.ExampleParameter = 19;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ResetSourceCountersCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetBundleCountersCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ResetBundleCountersCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetErrorCountersCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ResetErrorCountersCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddApplicationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplicationCmd_Payload_t Payload;

    Payload.ExampleParameter = 1;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddApplicationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveApplicationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveApplicationCmd_Payload_t Payload;

    Payload.ExampleParameter = 2;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveApplicationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetRegistrationStateCmd(void)
{
    BPLib_Status_t Status;
    BPLib_SetRegistrationStateCmd_Payload_t Payload;

    Payload.ExampleParameter = 3;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_SetRegistrationStateCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartApplicationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplicationCmd_Payload_t Payload;

    Payload.ExampleParameter = 4;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_StartApplicationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StopApplicationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplicationCmd_Payload_t Payload;

    Payload.ExampleParameter = 5;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_StopApplicationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthSourcesCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthSourcesCmd_Payload_t Payload;

    Payload.ExampleParameter = 6;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddAuthSourcesCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthSourcesCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthSourcesCmd_Payload_t Payload;

    Payload.ExampleParameter = 7;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveAuthSourcesCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodySourcesCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySourcesCmd_Payload_t Payload;

    Payload.ExampleParameter = 8;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddAuthCustodySourcesCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodySourcesCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySourcesCmd_Payload_t Payload;

    Payload.ExampleParameter = 9;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveAuthCustodySourcesCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodiansCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodiansCmd_Payload_t Payload;

    Payload.ExampleParameter = 10;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddAuthCustodiansCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodiansCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodiansCmd_Payload_t Payload;

    Payload.ExampleParameter = 11;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveAuthCustodiansCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthReportToEidCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEidCmd_Payload_t Payload;

    Payload.ExampleParameter = 12;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddAuthReportToEidCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthReportToEidCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEidCmd_Payload_t Payload;

    Payload.ExampleParameter = 13;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveAuthReportToEidCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddLatencyCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddLatencyCmd_Payload_t Payload;

    Payload.ExampleParameter = 14;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddLatencyCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveLatencyCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveLatencyCmd_Payload_t Payload;

    Payload.ExampleParameter = 15;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveLatencyCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactSetupCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ContactSetupCmd_Payload_t Payload;

    Payload.ExampleParameter = 16;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ContactSetupCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStartCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStartCmd_Payload_t Payload;

    Payload.ExampleParameter = 17;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ContactStartCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStopCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStopCmd_Payload_t Payload;

    Payload.ExampleParameter = 18;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ContactStopCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactTeardownCmd(void)
{
    BPLib_Status_t Status;
    BPLib_ContactTeardownCmd_Payload_t Payload;

    Payload.ExampleParameter = 19;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_ContactTeardownCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddMibArrayKeyCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddMibArrayKeyCmd_Payload_t Payload;

    Payload.ExampleParameter = 20;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddMibArrayKeyCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveMibArrayKeyCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKeyCmd_Payload_t Payload;

    Payload.ExampleParameter = 21;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveMibArrayKeyCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetMibItemCmd(void)
{
    BPLib_Status_t Status;
    BPLib_SetMibItemCmd_Payload_t Payload;

    Payload.ExampleParameter = 22;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_SetMibItemCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddStorageAllocationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_AddStorageAllocationCmd_Payload_t Payload;

    Payload.ExampleParameter = 23;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_AddStorageAllocationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveStorageAllocationCmd(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocationCmd_Payload_t Payload;

    Payload.ExampleParameter = 24;

    Status = BPLIB_UNKNOWN;
    Status = BPLib_NC_RemoveStorageAllocationCmd(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_PerformSelfTestCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_PerformSelfTestCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibConfigHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendNodeMibConfigHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibConfigHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendSourceMibConfigHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibCountersHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendNodeMibCountersHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibCountersHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendSourceMibCountersHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendStorageHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendStorageHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendChannelContactStatHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendChannelContactStatHkCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void TestBplibNc_Register(void)
{
    ADD_TEST(Test_BPLib_NC_Init);
    ADD_TEST(Test_BPLib_NC_NoopCmd);
    ADD_TEST(Test_BPLib_NC_AddAllApplicationsCmd);
    ADD_TEST(Test_BPLib_NC_StartAllApplicationsCmd);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorageCmd);
    ADD_TEST(Test_BPLib_NC_InitBundleStorageCmd);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadataCmd);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadataCmd);
    ADD_TEST(Test_BPLib_NC_ClearVolatileCmd);
    ADD_TEST(Test_BPLib_NC_ReloadSavedDataCmd);
    ADD_TEST(Test_BPLib_NC_ResetAllCountersCmd);
    ADD_TEST(Test_BPLib_NC_ResetCounterCmd);
    ADD_TEST(Test_BPLib_NC_ResetSourceCountersCmd);
    ADD_TEST(Test_BPLib_NC_ResetBundleCountersCmd);
    ADD_TEST(Test_BPLib_NC_ResetErrorCountersCmd);
    ADD_TEST(Test_BPLib_NC_AddApplicationCmd);
    ADD_TEST(Test_BPLib_NC_RemoveApplicationCmd);
    ADD_TEST(Test_BPLib_NC_SetRegistrationStateCmd);
    ADD_TEST(Test_BPLib_NC_StartApplicationCmd);
    ADD_TEST(Test_BPLib_NC_StopApplicationCmd);
    ADD_TEST(Test_BPLib_NC_AddAuthSourcesCmd);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSourcesCmd);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySourcesCmd);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySourcesCmd);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodiansCmd);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodiansCmd);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEidCmd);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEidCmd);
    ADD_TEST(Test_BPLib_NC_AddLatencyCmd);
    ADD_TEST(Test_BPLib_NC_RemoveLatencyCmd);
    ADD_TEST(Test_BPLib_NC_ContactSetupCmd);
    ADD_TEST(Test_BPLib_NC_ContactStartCmd);
    ADD_TEST(Test_BPLib_NC_ContactStopCmd);
    ADD_TEST(Test_BPLib_NC_ContactTeardownCmd);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKeyCmd);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKeyCmd);
    ADD_TEST(Test_BPLib_NC_SetMibItemCmd);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocationCmd);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocationCmd);
    ADD_TEST(Test_BPLib_NC_PerformSelfTestCmd);
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHkCmd);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHkCmd);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHkCmd);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHkCmd);
    ADD_TEST(Test_BPLib_NC_SendStorageHkCmd);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHkCmd);
}

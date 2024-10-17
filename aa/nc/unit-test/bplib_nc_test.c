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
#include "bplib_eventids.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_NC_Init_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_NoopCmd_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_NoopCmd();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAllApplicationsCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_ALL_APPS_SUCCESS_EID,
    //                     "Add all applications command is unimplemented");

    Status = BPLib_NC_AddAllApplicationsCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartAllApplicationsCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_ALL_APPS_SUCCESS_EID,
    //                     "Start all applications command not implemented");

    Status = BPLib_NC_StartAllApplicationsCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleStorageCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_SUCCESS_EID,
    //                     "Verify bundle storage command not implemented");

    Status = BPLib_NC_VerifyBundleStorageCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_InitBundleStorageCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_INIT_BNDL_STOR_SUCCESS_EID,
    //                     "Initialize bundle storage command not implemented");

    Status = BPLib_NC_InitBundleStorageCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleMetadataCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_META_SUCCESS_EID,
    //                     "Verify bundle metadata (7.2) command not implemented");

    Status = BPLib_NC_VerifyBundleMetadataCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RebuildBundleMetadataCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_REBUILD_BNDL_META_SUCCESS_EID,
    //                     "Rebuild bundle metadata command not implemented");

    Status = BPLib_NC_RebuildBundleMetadataCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ClearVolatileCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CLR_VOLATILE_SUCCESS_EID,
    //                    "Clear volatile command not implemented");

    Status = BPLib_NC_ClearVolatileCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ReloadSavedDataCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RELOAD_SVD_DATA_SUCCESS_EID,
    //                    "Reload saved data (7.2) command not implemented");

    Status = BPLib_NC_ReloadSavedDataCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetAllCountersCmd_Nominal(void)
{
    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ALL_CTRS_SUCCESS_EID,
    //                    "Reset all counters command not implemented");

    BPLib_NC_ResetAllCountersCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
}

void Test_BPLib_NC_ResetCounterCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetCounter_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_CTR_SUCCESS_EID,
    //                    "Reset counter command not implemented, received 42 in payload");

    Payload.ExampleParameter = 42;
    Status = BPLib_NC_ResetCounterCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetSourceCountersCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetSourceCounters_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_SRC_CTRS_SUCCESS_EID,
    //                    "Reset source counters command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ResetSourceCountersCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetBundleCountersCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_BNDL_CTRS_SUCCESS_EID,
    //                    "Reset bundle counters command not implemented");

    Status = BPLib_NC_ResetBundleCountersCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetErrorCountersCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ERR_CTRS_SUCCESS_EID,
    //                    "Reset error counters command not implemented");

    Status = BPLib_NC_ResetErrorCountersCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddApplicationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_APP_SUCCESS_EID,
    //                    "Add application command not implemented, received 1 in payload");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication = Test_BPA_ADUP_AddApplication;

    Payload.ChanId = 1;
    Status = BPLib_NC_AddApplicationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveApplicationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_APP_SUCCESS_EID,
    //                    "Remove application command not implemented, received 2 in payload");

    Payload.ExampleParameter = 2;
    Status = BPLib_NC_RemoveApplicationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetRegistrationStateCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetRegistrationState_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_REGI_STAT_SUCCESS_EID,
    //                    "Set registration state command not implemented, received 3 in payload");

    Payload.ExampleParameter = 3;
    Status = BPLib_NC_SetRegistrationStateCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartApplicationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_APP_SUCCESS_EID,
    //                    "Start application command not implemented, received 4 in payload");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = Test_BPA_ADUP_StartApplication;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplicationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StopApplicationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_STOP_APP_SUCCESS_EID,
    //                    "Stop application command not implemented, received 5 in payload");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = Test_BPA_ADUP_StopApplication;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplicationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthSourcesCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthSources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_SRCS_SUCCESS_EID,
    //                    "Add authorized sources command not implemented, received 6 in payload");

    Payload.ExampleParameter = 6;
    Status = BPLib_NC_AddAuthSourcesCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthSourcesCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthSources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_SRCS_SUCCESS_EID,
    //                    "Remove authorized sources command not implemented, received 7 in payload");

    Payload.ExampleParameter = 7;
    Status = BPLib_NC_RemoveAuthSourcesCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodySourcesCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID,
    //                    "Add authorized custody sources command not implemented, received 8 in payload");

    Payload.ExampleParameter = 8;
    Status = BPLib_NC_AddAuthCustodySourcesCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodySourcesCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID,
    //                    "Remove authorized custody sources command not implemented, received 9 in payload");

    Payload.ExampleParameter = 9;
    Status = BPLib_NC_RemoveAuthCustodySourcesCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodiansCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodians_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID,
    //                    "Add authorized custodians command not implemented, received 10 in payload");

    Payload.ExampleParameter = 10;
    Status = BPLib_NC_AddAuthCustodiansCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodiansCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID,
    //                    "Remove authorized custodians command not implemented, received 11 in payload");

    Payload.ExampleParameter = 11;
    Status = BPLib_NC_RemoveAuthCustodiansCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthReportToEidCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEid_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID,
    //                    "Add authorized report-to eid command not implemented, received 12 in payload");

    Payload.ExampleParameter = 12;
    Status = BPLib_NC_AddAuthReportToEidCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthReportToEidCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID,
    //                    "Remove authorized report-to eid command not implemented, received 13 in payload");

    Payload.ExampleParameter = 13;
    Status = BPLib_NC_RemoveAuthReportToEidCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddLatencyCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddLatency_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_LATENCY_SUCCESS_EID,
    //                    "Add latency command not implemented, received 14 in payload");

    Payload.ExampleParameter = 14;
    Status = BPLib_NC_AddLatencyCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveLatencyCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveLatency_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_LATENCY_SUCCESS_EID,
    //                    "Remove latency command not implemented, received 15 in payload");

    Payload.ExampleParameter = 15;
    Status = BPLib_NC_RemoveLatencyCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactSetupCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactSetup_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_SETUP_SUCCESS_EID,
    //                    "Contact setup command not implemented, received 16 in payload");

    Payload.ExampleParameter = 16;
    Status = BPLib_NC_ContactSetupCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStartCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStart_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_START_SUCCESS_EID,
    //                    "Contact start command not implemented, received 17 in payload");

    Payload.ExampleParameter = 17;
    Status = BPLib_NC_ContactStartCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStopCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStop_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_STOP_SUCCESS_EID,
    //                    "Contact stop command not implemented, received 18 in payload");

    Payload.ExampleParameter = 18;
    Status = BPLib_NC_ContactStopCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactTeardownCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactTeardown_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_TEARDOWN_SUCCESS_EID,
    //                    "Contact teardown command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ContactTeardownCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddMibArrayKeyCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddMibArrayKey_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID,
    //                    "Add mib array key command not implemented, received 20 in payload");

    Payload.ExampleParameter = 20;
    Status = BPLib_NC_AddMibArrayKeyCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveMibArrayKeyCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID,
    //                    "Remove mib array key command not implemented, received 21 in payload");

    Payload.ExampleParameter = 21;
    Status = BPLib_NC_RemoveMibArrayKeyCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetMibItemCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetMibItem_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_MIB_ITEM_SUCCESS_EID,
    //                    "Set mib item command not implemented, received 22 in payload");

    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItemCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddStorageAllocationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddStorageAllocation_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_STOR_ALLOC_SUCCESS_EID,
    //                    "Add storage allocation command not implemented, received 23 in payload");

    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveStorageAllocationCmd_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_STOR_ALLOC_SUCCESS_EID,
    //                    "Remove storage allocation command not implemented, received 24 in payload");

    Payload.ExampleParameter = 24;
    Status = BPLib_NC_RemoveStorageAllocationCmd(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_PerformSelfTestCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_PERFORM_SELF_TEST_SUCCESS_EID,
    //                    "Perform self test (7.2) command not implemented");

    Status = BPLib_NC_PerformSelfTestCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibConfigHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CFG_HK_SUCCESS_EID,
    //                    "Send node mib config hk command not implemented");

    Status = BPLib_NC_SendNodeMibConfigHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibConfigHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CFG_HK_SUCCESS_EID,
    //                    "Send per-source mib config hk command not implemented");

    Status = BPLib_NC_SendSourceMibConfigHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibCountersHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CTRS_HK_SUCCESS_EID,
    //                    "Send node mib counters HK command not implemented");

    Status = BPLib_NC_SendNodeMibCountersHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibCountersHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CTRS_HK_SUCCESS_EID,
    //                    "Send per-source mib counter hk command not implemented");

    Status = BPLib_NC_SendSourceMibCountersHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendStorageHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_STOR_HK_SUCCESS_EID,
    //                    "Send storage hk command not implemented");

    Status = BPLib_NC_SendStorageHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendChannelContactStatHkCmd_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_CHAN_CONTACT_STAT_HK_SUCCESS_EID,
    //                    "Send channel/contact status hk command not implemented");

    Status = BPLib_NC_SendChannelContactStatHkCmd();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void TestBplibNc_Register(void)
{
    ADD_TEST(Test_BPLib_NC_Init_Nominal);
    ADD_TEST(Test_BPLib_NC_NoopCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAllApplicationsCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_StartAllApplicationsCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorageCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_InitBundleStorageCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadataCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadataCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ClearVolatileCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ReloadSavedDataCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetAllCountersCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetCounterCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetSourceCountersCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetBundleCountersCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetErrorCountersCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddApplicationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveApplicationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SetRegistrationStateCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_StartApplicationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_StopApplicationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthSourcesCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSourcesCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySourcesCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySourcesCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodiansCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodiansCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEidCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEidCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddLatencyCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveLatencyCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactSetupCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStartCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStopCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactTeardownCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKeyCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKeyCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SetMibItemCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocationCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_PerformSelfTestCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHkCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHkCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHkCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHkCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendStorageHkCmd_Nominal);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHkCmd_Nominal);
}

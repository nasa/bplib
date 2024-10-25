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

/* ======= */
/* Globals */
/* ======= */

// UT_CheckEvent_t EventTest;

/* ==================== */
/* Function Definitions */
/* ==================== */

void Test_BPLib_NC_Init_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_Init_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_Noop_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_Noop();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_Noop_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_Noop();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAllApplications_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_ALL_APPS_SUCCESS_EID,
    //                     "Add all applications command is unimplemented");

    Status = BPLib_NC_AddAllApplications();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAllApplications_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_ALL_APPS_ERR_EID,
                        "Add all applications directive is unimplemented");

    Status = BPLib_NC_AddAllApplications();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_StartAllApplications_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_ALL_APPS_SUCCESS_EID,
    //                     "Start all applications command not implemented");

    Status = BPLib_NC_StartAllApplications();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartAllApplications_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_ALL_APPS_ERR_EID,
                        "Start all applications command not implemented");

    Status = BPLib_NC_StartAllApplications();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_VerifyBundleStorage_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_SUCCESS_EID,
    //                     "Verify bundle storage command not implemented");

    Status = BPLib_NC_VerifyBundleStorage();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleStorage_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_ERR_EID,
                        "Verify bundle storage command not implemented");

    Status = BPLib_NC_VerifyBundleStorage();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_InitBundleStorage_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_INIT_BNDL_STOR_SUCCESS_EID,
    //                     "Initialize bundle storage command not implemented");

    Status = BPLib_NC_InitBundleStorage();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_InitBundleStorage_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_INIT_BNDL_STOR_ERR_EID,
                        "Initialize bundle storage command not implemented");

    Status = BPLib_NC_InitBundleStorage();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_VerifyBundleMetadata_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_META_SUCCESS_EID,
    //                     "Verify bundle metadata (7.2) command not implemented");

    Status = BPLib_NC_VerifyBundleMetadata();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleMetadata_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_VERIF_BNDL_META_ERR_EID,
                        "Verify bundle metadata (7.2) command not implemented");

    Status = BPLib_NC_VerifyBundleMetadata();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RebuildBundleMetadata_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_REBUILD_BNDL_META_SUCCESS_EID,
    //                     "Rebuild bundle metadata command not implemented");

    Status = BPLib_NC_RebuildBundleMetadata();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RebuildBundleMetadata_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_REBUILD_BNDL_META_ERR_EID,
                        "Rebuild bundle metadata command not implemented");

    Status = BPLib_NC_RebuildBundleMetadata();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ClearVolatile_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CLR_VOLATILE_SUCCESS_EID,
    //                     "Clear volatile command not implemented");

    Status = BPLib_NC_ClearVolatile();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ClearVolatile_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CLR_VOLATILE_ERR_EID,
                        "Clear volatile command not implemented");

    Status = BPLib_NC_ClearVolatile();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ReloadSavedData_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RELOAD_SVD_DATA_SUCCESS_EID,
    //                     "Reload saved data (7.2) command not implemented");

    Status = BPLib_NC_ReloadSavedData();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ReloadSavedData_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RELOAD_SVD_DATA_ERR_EID,
                        "Reload saved data (7.2) command not implemented");

    Status = BPLib_NC_ReloadSavedData();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetAllCounters_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ALL_CTRS_SUCCESS_EID,
    //                     "Reset all counters command not implemented");

    Status = BPLib_NC_ResetAllCounters();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetAllCounters_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ALL_CTRS_ERR_EID,
                        "Reset all counters command not implemented");

    Status = BPLib_NC_ResetAllCounters();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetCounter_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetCounter_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_CTR_SUCCESS_EID,
    //                     "Reset counter command not implemented, received 42 in payload");

    Payload.ExampleParameter = 42;
    Status = BPLib_NC_ResetCounter(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetCounter_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ResetCounter_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_CTR_ERR_EID,
                        "Reset counter command not implemented, received 42 in payload");

    Payload.ExampleParameter = 42;
    Status = BPLib_NC_ResetCounter(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetSourceCounters_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetSourceCounters_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_SRC_CTRS_SUCCESS_EID,
    //                     "Reset source counters command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ResetSourceCounters(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetSourceCounters_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ResetSourceCounters_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_SRC_CTRS_ERR_EID,
                        "Reset source counters command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ResetSourceCounters(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetBundleCounters_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_BNDL_CTRS_SUCCESS_EID,
    //                     "Reset bundle counters command not implemented");

    Status = BPLib_NC_ResetBundleCounters();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetBundleCounters_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_BNDL_CTRS_ERR_EID,
                        "Reset bundle counters command not implemented");

    Status = BPLib_NC_ResetBundleCounters();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetErrorCounters_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ERR_CTRS_SUCCESS_EID,
    //                     "Reset error counters command not implemented");

    Status = BPLib_NC_ResetErrorCounters();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetErrorCounters_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RESET_ERR_CTRS_ERR_EID,
                        "Reset error counters command not implemented");

    Status = BPLib_NC_ResetErrorCounters();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_APP_SUCCESS_EID,
    //                     "Successful add-application directive for ChanId=%d");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication = Test_BPA_ADUP_AddApplication_Success;

    Payload.ChanId = 1;
    Status = BPLib_NC_AddApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_APP_SUCCESS_EID,
    //                     "Successful add-application directive for ChanId=%d");

    /* Check channel error return code */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication = Test_BPA_ADUP_AddApplication_ChanErr;

    Payload.ChanId = 2;
    Status = BPLib_NC_AddApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_ADD_CHAN_ERR);
    
    /* Check state error return code */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication = Test_BPA_ADUP_AddApplication_StateErr;
    
    Payload.ChanId = 2;
    Status = BPLib_NC_AddApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_ADD_STAT_ERR);
}

void Test_BPLib_NC_RemoveApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_APP_SUCCESS_EID,
    //                     "Remove application command not implemented, received 2 in payload");

    Payload.ExampleParameter = 2;
    Status = BPLib_NC_RemoveApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveApplication_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveApplication_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_APP_ERR_EID,
                        "Remove application command not implemented, received 2 in payload");

    Payload.ExampleParameter = 2;
    Status = BPLib_NC_RemoveApplication(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SetRegistrationState_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetRegistrationState_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_REGI_STAT_SUCCESS_EID,
    //                     "Set registration state command not implemented, received 3 in payload");

    Payload.ExampleParameter = 3;
    Status = BPLib_NC_SetRegistrationState(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetRegistrationState_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_SetRegistrationState_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_REGI_STAT_ERR_EID,
                        "Set registration state command not implemented, received 3 in payload");

    Payload.ExampleParameter = 3;
    Status = BPLib_NC_SetRegistrationState(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_StartApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_APP_SUCCESS_EID,
    //                     "Successful start-application directive for ChanId=1");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = Test_BPA_ADUP_StartApplication_Success;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_START_APP_SUCCESS_EID,
    //                     "Successful start-application directive for ChanId=1");

    /* Channel error return code check */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = Test_BPA_ADUP_StartApplication_ChanErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_CHAN_ERR);

    /* State error return code check */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = Test_BPA_ADUP_StartApplication_StateErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_STAT_ERR);

    /* Subscribe error return code check */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication = Test_BPA_ADUP_StartApplication_SubErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_SUB_ERR);
}

void Test_BPLib_NC_StopApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_STOP_APP_SUCCESS_EID,
    //                     "Successful stop-application directive for ChanId=5");

    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = Test_BPA_ADUP_StopApplication_Success;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StopApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplication_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_STOP_APP_SUCCESS_EID,
    //                     "Successful stop-application directive for ChanId=5");

    /* Invalid channel return code test */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = Test_BPA_ADUP_StopApplication_ChanErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_CHAN_ERR);

    /* Invalid state return code test */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = Test_BPA_ADUP_StopApplication_StateErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_STAT_ERR);

    /* Unsubscribe error return code test */
    BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication = Test_BPA_ADUP_StopApplication_UnsubErr;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_UNSUB_ERR);
}

void Test_BPLib_NC_AddAuthSources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthSources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_SRCS_SUCCESS_EID,
    //                     "Add authorized sources command not implemented, received 6 in payload");

    Payload.ExampleParameter = 6;
    Status = BPLib_NC_AddAuthSources(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthSources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthSources_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_SRCS_ERR_EID,
                        "Add authorized sources command not implemented, received 6 in payload");

    Payload.ExampleParameter = 6;
    Status = BPLib_NC_AddAuthSources(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthSources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthSources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_SRCS_SUCCESS_EID,
    //                     "Remove authorized sources command not implemented, received 7 in payload");

    Payload.ExampleParameter = 7;
    Status = BPLib_NC_RemoveAuthSources(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthSources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthSources_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_SRCS_ERR_EID,
                        "Remove authorized sources command not implemented, received 7 in payload");

    Payload.ExampleParameter = 7;
    Status = BPLib_NC_RemoveAuthSources(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthCustodySources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUST_SRCS_SUCCESS_EID,
    //                     "Add authorized custody sources command not implemented, received 8 in payload");

    Payload.ExampleParameter = 8;
    Status = BPLib_NC_AddAuthCustodySources(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodySources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySources_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUST_SRCS_ERR_EID,
                        "Add authorized custody sources command not implemented, received 8 in payload");

    Payload.ExampleParameter = 8;
    Status = BPLib_NC_AddAuthCustodySources(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthCustodySources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID,
    //                     "Remove authorized custody sources command not implemented, received 9 in payload");

    Payload.ExampleParameter = 9;
    Status = BPLib_NC_RemoveAuthCustodySources(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodySources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUST_SRCS_ERR_EID,
                        "Remove authorized custody sources command not implemented, received 9 in payload");

    Payload.ExampleParameter = 9;
    Status = BPLib_NC_RemoveAuthCustodySources(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthCustodians_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodians_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID,
    //                     "Add authorized custodians command not implemented, received 10 in payload");

    Payload.ExampleParameter = 10;
    Status = BPLib_NC_AddAuthCustodians(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodians_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthCustodians_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_CUSTODIANS_ERR_EID,
                        "Add authorized custodians command not implemented, received 10 in payload");

    Payload.ExampleParameter = 10;
    Status = BPLib_NC_AddAuthCustodians(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthCustodians_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID,
    //                     "Remove authorized custodians command not implemented, received 11 in payload");

    Payload.ExampleParameter = 11;
    Status = BPLib_NC_RemoveAuthCustodians(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodians_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_CUSTODIANS_ERR_EID,
                        "Remove authorized custodians command not implemented, received 11 in payload");

    Payload.ExampleParameter = 11;
    Status = BPLib_NC_RemoveAuthCustodians(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthReportToEid_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEid_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID,
    //                     "Add authorized report-to eid command not implemented, received 12 in payload");

    Payload.ExampleParameter = 12;
    Status = BPLib_NC_AddAuthReportToEid(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthReportToEid_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEid_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_AUTH_RPT_EID_ERR_EID,
                        "Add authorized report-to eid command not implemented, received 12 in payload");

    Payload.ExampleParameter = 12;
    Status = BPLib_NC_AddAuthReportToEid(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthReportToEid_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID,
    //                     "Remove authorized report-to eid command not implemented, received 13 in payload");

    Payload.ExampleParameter = 13;
    Status = BPLib_NC_RemoveAuthReportToEid(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthReportToEid_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_AUTH_RPT_EID_ERR_EID,
                        "Remove authorized report-to eid command not implemented, received 13 in payload");

    Payload.ExampleParameter = 13;
    Status = BPLib_NC_RemoveAuthReportToEid(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddLatency_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddLatency_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_LATENCY_SUCCESS_EID,
    //                     "Add latency command not implemented, received 14 in payload");

    Payload.ExampleParameter = 14;
    Status = BPLib_NC_AddLatency(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddLatency_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddLatency_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_LATENCY_ERR_EID,
                        "Add latency command not implemented, received 14 in payload");

    Payload.ExampleParameter = 14;
    Status = BPLib_NC_AddLatency(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveLatency_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveLatency_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_LATENCY_SUCCESS_EID,
    //                     "Remove latency command not implemented, received 15 in payload");

    Payload.ExampleParameter = 15;
    Status = BPLib_NC_RemoveLatency(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveLatency_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveLatency_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_LATENCY_ERR_EID,
                        "Remove latency command not implemented, received 15 in payload");

    Payload.ExampleParameter = 15;
    Status = BPLib_NC_RemoveLatency(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactSetup_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactSetup_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_SETUP_SUCCESS_EID,
    //                     "Contact setup command not implemented, received 16 in payload");

    Payload.ExampleParameter = 16;
    Status = BPLib_NC_ContactSetup(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactSetup_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactSetup_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_SETUP_ERR_EID,
                        "Contact setup command not implemented, received 16 in payload");

    Payload.ExampleParameter = 16;
    Status = BPLib_NC_ContactSetup(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactStart_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStart_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_START_SUCCESS_EID,
    //                     "Contact start command not implemented, received 17 in payload");

    Payload.ExampleParameter = 17;
    Status = BPLib_NC_ContactStart(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStart_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactStart_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_START_ERR_EID,
                        "Contact start command not implemented, received 17 in payload");

    Payload.ExampleParameter = 17;
    Status = BPLib_NC_ContactStart(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactStop_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStop_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_STOP_SUCCESS_EID,
    //                     "Contact stop command not implemented, received 18 in payload");

    Payload.ExampleParameter = 18;
    Status = BPLib_NC_ContactStop(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStop_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactStop_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_STOP_ERR_EID,
                        "Contact stop command not implemented, received 18 in payload");

    Payload.ExampleParameter = 18;
    Status = BPLib_NC_ContactStop(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactTeardown_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactTeardown_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_TEARDOWN_SUCCESS_EID,
    //                     "Contact teardown command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ContactTeardown(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactTeardown_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactTeardown_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_CONTACT_TEARDOWN_ERR_EID,
                        "Contact teardown command not implemented, received 19 in payload");

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ContactTeardown(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddMibArrayKey_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID,
    //                     "Add mib array key command not implemented, received 20 in payload");

    Payload.ExampleParameter = 20;
    Status = BPLib_NC_AddMibArrayKey(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddMibArrayKey_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddMibArrayKey_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_MIB_ARR_KEY_ERR_EID,
                        "Add mib array key command not implemented, received 20 in payload");

    Payload.ExampleParameter = 20;
    Status = BPLib_NC_AddMibArrayKey(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID,
    //                     "Remove mib array key command not implemented, received 21 in payload");

    Payload.ExampleParameter = 21;
    Status = BPLib_NC_RemoveMibArrayKey(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveMibArrayKey_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_MIB_ARR_KEY_ERR_EID,
                        "Remove mib array key command not implemented, received 21 in payload");

    Payload.ExampleParameter = 21;
    Status = BPLib_NC_RemoveMibArrayKey(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SetMibItem_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetMibItem_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_MIB_ITEM_SUCCESS_EID,
    //                     "Set mib item command not implemented, received 22 in payload");

    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetMibItem_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_SetMibItem_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SET_MIB_ITEM_SUCCESS_EID,
                        "Set mib item command not implemented, received 22 in payload");

    // Invalid item index error return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MIB_ITEM_INVALID_INDEX_ERR_EID);

    // Invalid item value return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MID_ITEM_INVALID_VALUE_ERR_EID);
    
    // Table update failure return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MIB_ITEM_TBL_UPDATE_FAIL);
    */
}

void Test_BPLib_NC_AddStorageAllocation_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddStorageAllocation_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_STOR_ALLOC_SUCCESS_EID,
    //                     "Add storage allocation command not implemented, received 23 in payload");

    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddStorageAllocation_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddStorageAllocation_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_ADD_STOR_ALLOC_SUCCESS_EID,
                        "Add storage allocation command not implemented, received 23 in payload");

    // Storage allocation error
    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    // Table update error
    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 0);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveStorageAllocation_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_STOR_ALLOC_SUCCESS_EID,
    //                     "Remove storage allocation command not implemented, received 24 in payload");

    Payload.ExampleParameter = 24;
    Status = BPLib_NC_RemoveStorageAllocation(Payload);

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveStorageAllocation_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_RM_STOR_ALLOC_ERR_EID,
                        "Remove storage allocation command not implemented, received 24 in payload");

    Payload.ExampleParameter = 24;
    Status = BPLib_NC_RemoveStorageAllocation(Payload);

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_PerformSelfTest_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_PERFORM_SELF_TEST_SUCCESS_EID,
    //                     "Perform self test (7.2) command not implemented");

    Status = BPLib_NC_PerformSelfTest();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_PerformSelfTest_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_PERFORM_SELF_TEST_ERR_EID,
                        "Perform self test (7.2) command not implemented");

    Status = BPLib_NC_PerformSelfTest();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendNodeMibConfigHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CFG_HK_SUCCESS_EID,
    //                     "Send node mib config hk command not implemented");

    Status = BPLib_NC_SendNodeMibConfigHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibConfigHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CFG_HK_ERR_EID,
                        "Send node mib config hk command not implemented");

    Status = BPLib_NC_SendNodeMibConfigHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendSourceMibConfigHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CFG_HK_SUCCESS_EID,
    //                     "Send per-source mib config hk command not implemented");

    Status = BPLib_NC_SendSourceMibConfigHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibConfigHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CFG_HK_ERR_EID,
                        "Send per-source mib config hk command not implemented");

    Status = BPLib_NC_SendSourceMibConfigHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendNodeMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CTRS_HK_SUCCESS_EID,
    //                     "Send node mib counters HK command not implemented");

    Status = BPLib_NC_SendNodeMibCountersHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibCountersHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_NODE_MIB_CTRS_HK_SUCCESS_EID,
                        "Send node mib counters HK command not implemented");

    Status = BPLib_NC_SendNodeMibCountersHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendSourceMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CTRS_HK_SUCCESS_EID,
    //                     "Send per-source mib counter hk command not implemented");

    Status = BPLib_NC_SendSourceMibCountersHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibCountersHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_SRC_MIB_CTRS_ERR_EID,
                        "Send per-source mib counter hk command not implemented");

    Status = BPLib_NC_SendSourceMibCountersHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendStorageHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_STOR_HK_SUCCESS_EID,
    //                     "Send storage hk command not implemented");

    Status = BPLib_NC_SendStorageHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendStorageHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_STOR_HK_ERR_EID,
                        "Send storage hk command not implemented");

    Status = BPLib_NC_SendStorageHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendChannelContactStatHk_Nominal(void)
{
    BPLib_Status_t Status;

    // UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_CHAN_CONTACT_STAT_HK_SUCCESS_EID,
    //                     "Send channel/contact status hk command not implemented");

    Status = BPLib_NC_SendChannelContactStatHk();

    // UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendChannelContactStatHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    UT_CHECKEVENT_SETUP(&EventTest, BPLIB_SEND_CHAN_CONTACT_STAT_HK_SUCCESS_EID,
                        "Send channel/contact status hk command not implemented");

    Status = BPLib_NC_SendChannelContactStatHk();

    UtAssert_INT32_EQ(EventTest.MatchCount, 1);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_MIBConfigPNTblValidateFunc_Nominal(void)
{
    BPLib_NC_MIBConfigPNTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    TestTblData.BundleAgentNum = 10;
    UtAssert_INT32_EQ((int32) BPLib_NC_MIBConfigPNTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_NC_MIBConfigPNTblValidateFunc_Invalid(void)
{
    BPLib_NC_MIBConfigPNTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.BundleAgentNum = 0;

    UtAssert_INT32_EQ(BPLib_NC_MIBConfigPNTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_BPLib_NC_MIBConfigPSTblValidateFunc_Nominal(void)
{
    BPLib_NC_MIBConfigPSTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));
    TestTblData.MIB_PS_Set[0].ParamSetMaxLifetime = 10;
    UtAssert_INT32_EQ((int32) BPLib_NC_MIBConfigPSTblValidateFunc(&TestTblData), (int32) BPLIB_SUCCESS);     
}

void Test_BPLib_NC_MIBConfigPSTblValidateFunc_Invalid(void)
{
    BPLib_NC_MIBConfigPSTable_t TestTblData;
    memset(&TestTblData, 0, sizeof(TestTblData));

    /* Error case should return BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.MIB_PS_Set[0].ParamSetMaxLifetime = 0;

    UtAssert_INT32_EQ(BPLib_NC_MIBConfigPSTblValidateFunc(&TestTblData), 
                                                BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void TestBplibNc_Register(void)
{
    ADD_TEST(Test_BPLib_NC_AddAllApplications_Error);
    ADD_TEST(Test_BPLib_NC_AddAllApplications_Nominal);
    ADD_TEST(Test_BPLib_NC_AddApplication_Error);
    ADD_TEST(Test_BPLib_NC_AddApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodians_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodians_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySources_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySources_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEid_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEid_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthSources_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthSources_Nominal);
    ADD_TEST(Test_BPLib_NC_AddLatency_Error);
    ADD_TEST(Test_BPLib_NC_AddLatency_Nominal);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKey_Error);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKey_Nominal);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocation_Error);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocation_Nominal);
    ADD_TEST(Test_BPLib_NC_ClearVolatile_Error);
    ADD_TEST(Test_BPLib_NC_ClearVolatile_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactSetup_Error);
    ADD_TEST(Test_BPLib_NC_ContactSetup_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStart_Error);
    ADD_TEST(Test_BPLib_NC_ContactStart_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStop_Error);
    ADD_TEST(Test_BPLib_NC_ContactStop_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactTeardown_Error);
    ADD_TEST(Test_BPLib_NC_ContactTeardown_Nominal);
    ADD_TEST(Test_BPLib_NC_InitBundleStorage_Error);
    ADD_TEST(Test_BPLib_NC_InitBundleStorage_Nominal);
    ADD_TEST(Test_BPLib_NC_Init_Error);
    ADD_TEST(Test_BPLib_NC_Init_Nominal);
    ADD_TEST(Test_BPLib_NC_Noop_Error);
    ADD_TEST(Test_BPLib_NC_Noop_Nominal);
    ADD_TEST(Test_BPLib_NC_PerformSelfTest_Error);
    ADD_TEST(Test_BPLib_NC_PerformSelfTest_Nominal);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadata_Error);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadata_Nominal);
    ADD_TEST(Test_BPLib_NC_ReloadSavedData_Error);
    ADD_TEST(Test_BPLib_NC_ReloadSavedData_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveApplication_Error);
    ADD_TEST(Test_BPLib_NC_RemoveApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodians_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodians_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySources_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySources_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEid_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEid_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSources_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSources_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveLatency_Error);
    ADD_TEST(Test_BPLib_NC_RemoveLatency_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKey_Error);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKey_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocation_Error);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocation_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetAllCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetAllCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetBundleCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetBundleCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetCounter_Error);
    ADD_TEST(Test_BPLib_NC_ResetCounter_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetErrorCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetSourceCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHk_Error);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHk_Error);
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHk_Error);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHk_Error);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHk_Error);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendStorageHk_Error);
    ADD_TEST(Test_BPLib_NC_SendStorageHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SetMibItem_Error);
    ADD_TEST(Test_BPLib_NC_SetMibItem_Nominal);
    ADD_TEST(Test_BPLib_NC_SetRegistrationState_Error);
    ADD_TEST(Test_BPLib_NC_SetRegistrationState_Nominal);
    ADD_TEST(Test_BPLib_NC_StartAllApplications_Error);
    ADD_TEST(Test_BPLib_NC_StartAllApplications_Nominal);
    ADD_TEST(Test_BPLib_NC_StartApplication_Error);
    ADD_TEST(Test_BPLib_NC_StartApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_StopApplication_Error);
    ADD_TEST(Test_BPLib_NC_StopApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadata_Error);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadata_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorage_Error);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorage_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Invalid);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Invalid);
}

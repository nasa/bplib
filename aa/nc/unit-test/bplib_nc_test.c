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

void Test_BPLib_NC_Init_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_Init();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAllApplications_Nominal(void)
{
    BPLib_NC_AddAllApplications();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_ALL_APPS_SUCCESS_EID,
                                "Add all applications directive is unimplemented");
}

void Test_BPLib_NC_AddAllApplications_Error(void)
{
    /* 
    BPLib_NC_AddAllApplications();

    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_StartAllApplications_Nominal(void)
{
    BPLib_NC_StartAllApplications();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0,BPLIB_NC_START_ALL_APPS_SUCCESS_EID,
                                "Start all applications directive not implemented");
}

void Test_BPLib_NC_StartAllApplications_Error(void)
{
    /* BPLib_NC_StartAllApplications();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_VerifyBundleStorage_Nominal(void)
{
    BPLib_NC_VerifyBundleStorage();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_VERIF_BNDL_SUCCESS_EID,
                                "Verify bundle storage directive not implemented");
}

void Test_BPLib_NC_VerifyBundleStorage_Error(void)
{
    /* BPLib_NC_VerifyBundleStorage();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_InitBundleStorage_Nominal(void)
{
    BPLib_NC_InitBundleStorage();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_INIT_BNDL_STOR_SUCCESS_EID,
                                "Initialize bundle storage directive not implemented");
}

void Test_BPLib_NC_InitBundleStorage_Error(void)
{
    /* BPLib_NC_InitBundleStorage();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_VerifyBundleMetadata_Nominal(void)
{
    BPLib_NC_VerifyBundleMetadata();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_VERIF_BNDL_META_SUCCESS_EID,
                                "Verify bundle metadata (7.2) directive not implemented");
}

void Test_BPLib_NC_VerifyBundleMetadata_Error(void)
{
    /* BPLib_NC_VerifyBundleMetadata();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_RebuildBundleMetadata_Nominal(void)
{
    BPLib_NC_RebuildBundleMetadata();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_REBUILD_BNDL_META_SUCCESS_EID,
                                "Rebuild bundle metadata directive not implemented");
}

void Test_BPLib_NC_RebuildBundleMetadata_Error(void)
{
    /* BPLib_NC_RebuildBundleMetadata();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_ClearVolatile_Nominal(void)
{
    BPLib_NC_ClearVolatile();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_CLR_VOLATILE_SUCCESS_EID,
                                "Clear volatile directive not implemented");
}

void Test_BPLib_NC_ClearVolatile_Error(void)
{
    /* BPLib_NC_ClearVolatile();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_ReloadSavedData_Nominal(void)
{
    BPLib_NC_ReloadSavedData();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RELOAD_SVD_DATA_SUCCESS_EID,
                                "Reload saved data (7.2) directive not implemented");
}

void Test_BPLib_NC_ReloadSavedData_Error(void)
{
    /* BPLib_NC_ReloadSavedData();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1); */
}

void Test_BPLib_NC_ResetAllCounters_Nominal(void)
{
    BPLib_NC_ResetAllCounters();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_ALL_CTRS_SUCCESS_EID,
                                "Successful reset-all-counters directive");

    UtAssert_STUB_COUNT(BPLib_AS_ResetAllCounters, 1);
}

void Test_BPLib_NC_ResetAllCounters_Error(void)
{
    /* Not applicable as of right now */
}

void Test_BPLib_NC_ResetCounter_Nominal(void)
{
    BPLib_ResetCounter_Payload_t Payload;

    Payload.SourceEid = 4;
    Payload.Counter   = BUNDLE_COUNT_DELETED_CANCELLED;
    Payload.Spare     = 0;

    BPLib_NC_ResetCounter(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetCounter, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_CTR_SUCCESS_EID,
                                "Successfully reset counter %d for source EID %d");
}

void Test_BPLib_NC_ResetCounter_Error(void)
{
    BPLib_ResetCounter_Payload_t Payload;

    /* Put dummy values into the payload */
    Payload.Counter   = 0;
    Payload.SourceEid = 0;
    Payload.Spare     = 0;

    /* === BPLIB_AS_INVALID_EID returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetCounter), BPLIB_AS_INVALID_EID);

    /* Run the function under test */
    BPLib_NC_ResetCounter(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetCounter, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_CTR_ERR_EID,
                                "Could not reset counter %d with source EID %d, RC = %d");

    /* === BPLIB_AS_UNKNOWN_NODE_CNTR returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetCounter), BPLIB_AS_UNKNOWN_NODE_CNTR);

    /* Run the function under test */
    BPLib_NC_ResetCounter(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetCounter, 2);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(1, BPLIB_NC_RESET_CTR_ERR_EID,
                                "Could not reset counter %d with source EID %d, RC = %d");

    /* === BPLIB_AS_UNKNOWN_SRC_CNTR returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetCounter), BPLIB_AS_UNKNOWN_SRC_CNTR);

    /* Run the function under test */
    BPLib_NC_ResetCounter(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetCounter, 3);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(2, BPLIB_NC_RESET_CTR_ERR_EID,
                                "Could not reset counter %d with source EID %d, RC = %d");
}

void Test_BPLib_NC_ResetSourceCounters_Nominal(void)
{
    BPLib_ResetSourceCounters_Payload_t Payload;

    Payload.Spare     = 0;
    Payload.SourceEid = 2;
    BPLib_NC_ResetSourceCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetSourceCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_SRC_CTRS_SUCCESS_EID,
                                "Successfully reset source counters for source EID %d");
}

void Test_BPLib_NC_ResetSourceCounters_Error(void)
{
    BPLib_ResetSourceCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetSourceCounters), BPLIB_AS_INVALID_EID);

    Payload.Spare     = 0;
    Payload.SourceEid = 19;
    BPLib_NC_ResetSourceCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetSourceCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_SRC_CTRS_ERR_EID,
                                "Could not reset source counters with source EID %d, RC = %d");
}

void Test_BPLib_NC_ResetBundleCounters_Nominal(void)
{
    BPLib_ResetBundleCounters_Payload_t Payload;

    Payload.Spare     = 0;
    Payload.SourceEid = 2;
    BPLib_NC_ResetBundleCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetBundleCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_BNDL_CTRS_SUCCESS_EID,
                                "Successfully reset bundle counters for source EID %d");
}

void Test_BPLib_NC_ResetBundleCounters_Error(void)
{
    BPLib_ResetBundleCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetBundleCounters), BPLIB_AS_INVALID_EID);

    Payload.Spare     = 0;
    Payload.SourceEid = 19;
    BPLib_NC_ResetBundleCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetBundleCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_BNDL_CTRS_ERR_EID,
                                "Could not reset bundle counters with source EID %d, RC = %d");
}

void Test_BPLib_NC_ResetErrorCounters_Nominal(void)
{
    BPLib_ResetErrorCounters_Payload_t Payload;

    Payload.Spare     = 0;
    Payload.SourceEid = 2;
    BPLib_NC_ResetErrorCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetErrorCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_ERR_CTRS_SUCCESS_EID,
                                "Successfully reset error counters for source EID %d");
}

void Test_BPLib_NC_ResetErrorCounters_Error(void)
{
    BPLib_ResetErrorCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetErrorCounters), BPLIB_AS_INVALID_EID);

    Payload.Spare     = 0;
    Payload.SourceEid = 19;
    BPLib_NC_ResetErrorCounters(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_ResetErrorCounters, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RESET_ERR_CTRS_ERR_EID,
                                "Could not reset error counters with source EID %d, RC = %d");
}

void Test_BPLib_NC_AddApplication_Nominal(void)
{
    BPLib_AddApplication_Payload_t Payload;

    Payload.ChanId = 1;
    BPLib_NC_AddApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_APP_SUCCESS_EID,
                                "Successful add-application directive for ChanId=%d");
}

void Test_BPLib_NC_AddApplication_Error(void)
{
    BPLib_AddApplication_Payload_t Payload;

    /* Check channel error return code */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_AddApplication), BPLIB_ADU_ADD_CHAN_ERR);

    Payload.ChanId = 2;
    BPLib_NC_AddApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    
    /* Check state error return code */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_AddApplication), BPLIB_ADU_ADD_STAT_ERR);
    
    Payload.ChanId = 2;
    BPLib_NC_AddApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);
}

void Test_BPLib_NC_RemoveApplication_Nominal(void)
{
    BPLib_RemoveApplication_Payload_t Payload;

    Payload.ExampleParameter = 2;
    BPLib_NC_RemoveApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_APP_SUCCESS_EID,
                                "Remove application directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveApplication_Error(void)
{
    /*
    BPLib_RemoveApplication_Payload_t Payload;

    Payload.ExampleParameter = 2;
    BPLib_NC_RemoveApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_SetRegistrationState_Nominal(void)
{
    BPLib_SetRegistrationState_Payload_t Payload;

    Payload.ExampleParameter = 3;
    BPLib_NC_SetRegistrationState(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_SET_REGI_STAT_SUCCESS_EID,
                                "Set registration state directive not implemented, received %d in payload");
}

void Test_BPLib_NC_SetRegistrationState_Error(void)
{
    /*
    BPLib_SetRegistrationState_Payload_t Payload;

    Payload.ExampleParameter = 3;
    BPLib_NC_SetRegistrationState(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_StartApplication_Nominal(void)
{
    BPLib_StartApplication_Payload_t Payload;

    Payload.ChanId = 1;
    BPLib_NC_StartApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_START_APP_SUCCESS_EID,
                                "Successful start-application directive for ChanId=%d");
}

void Test_BPLib_NC_StartApplication_Error(void)
{
    BPLib_StartApplication_Payload_t Payload;

    /* Channel error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_CHAN_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StartApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* State error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_STAT_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StartApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);

    /* Subscribe error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_SUB_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StartApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
}

void Test_BPLib_NC_StopApplication_Nominal(void)
{
    BPLib_StopApplication_Payload_t Payload;

    Payload.ChanId = 1;
    BPLib_NC_StopApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_STOP_APP_SUCCESS_EID,
                                "Successful stop-application directive for ChanId=%d");
}

void Test_BPLib_NC_StopApplication_Error(void)
{
    BPLib_StopApplication_Payload_t Payload;

    /* Invalid channel return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_CHAN_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StopApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Invalid state return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_STAT_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StopApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);

    /* Unsubscribe error return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_UNSUB_ERR);

    Payload.ChanId = 1;
    BPLib_NC_StopApplication(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
}

void Test_BPLib_NC_AddAuthSources_Nominal(void)
{
    BPLib_AddAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 6;
    BPLib_NC_AddAuthSources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_AUTH_SRCS_SUCCESS_EID,
                                "Add authorized sources directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddAuthSources_Error(void)
{
    /*
    BPLib_AddAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 6;
    BPLib_NC_AddAuthSources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveAuthSources_Nominal(void)
{
    BPLib_RemoveAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 7;
    BPLib_NC_RemoveAuthSources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_AUTH_SRCS_SUCCESS_EID,
                                "Remove authorized sources directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveAuthSources_Error(void)
{
    /*
    BPLib_RemoveAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 7;
    BPLib_NC_RemoveAuthSources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_AddAuthCustodySources_Nominal(void)
{
    BPLib_AddAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 8;
    BPLib_NC_AddAuthCustodySources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_AUTH_CUST_SRCS_SUCCESS_EID,
                                "Add authorized custody sources directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddAuthCustodySources_Error(void)
{
    /*
    BPLib_AddAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 8;
    BPLib_NC_AddAuthCustodySources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveAuthCustodySources_Nominal(void)
{
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 9;
    BPLib_NC_RemoveAuthCustodySources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_AUTH_CUST_SRCS_SUCCESS_EID,
                                "Remove authorized custody sources directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveAuthCustodySources_Error(void)
{
    /*
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 9;
    BPLib_NC_RemoveAuthCustodySources(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_AddAuthCustodians_Nominal(void)
{
    BPLib_AddAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 10;
    BPLib_NC_AddAuthCustodians(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_AUTH_CUSTODIANS_SUCCESS_EID,
                                "Add authorized custodians directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddAuthCustodians_Error(void)
{
    /*
    BPLib_AddAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 10;
    BPLib_NC_AddAuthCustodians(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveAuthCustodians_Nominal(void)
{
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 11;
    BPLib_NC_RemoveAuthCustodians(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_AUTH_CUSTODIANS_SUCCESS_EID,
                                "Remove authorized custodians directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveAuthCustodians_Error(void)
{
    /*
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 11;
    BPLib_NC_RemoveAuthCustodians(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_AddAuthReportToEid_Nominal(void)
{
    BPLib_AddAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 12;
    BPLib_NC_AddAuthReportToEid(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_AUTH_RPT_EID_SUCCESS_EID,
                                "Add authorized report-to eid directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddAuthReportToEid_Error(void)
{
    /*
    BPLib_AddAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 12;
    BPLib_NC_AddAuthReportToEid(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveAuthReportToEid_Nominal(void)
{
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 13;
    BPLib_NC_RemoveAuthReportToEid(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_AUTH_RPT_EID_SUCCESS_EID,
                                "Remove authorized report-to eid directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveAuthReportToEid_Error(void)
{
    /*
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 13;
    BPLib_NC_RemoveAuthReportToEid(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_AddLatency_Nominal(void)
{
    BPLib_AddLatency_Payload_t Payload;

    Payload.ExampleParameter = 14;
    BPLib_NC_AddLatency(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_LATENCY_SUCCESS_EID,
                                "Add latency directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddLatency_Error(void)
{
    /*
    BPLib_AddLatency_Payload_t Payload;

    Payload.ExampleParameter = 14;
    BPLib_NC_AddLatency(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveLatency_Nominal(void)
{
    BPLib_RemoveLatency_Payload_t Payload;

    Payload.ExampleParameter = 15;
    BPLib_NC_RemoveLatency(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_LATENCY_SUCCESS_EID,
                                "Remove latency directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveLatency_Error(void)
{
    /*
    BPLib_RemoveLatency_Payload_t Payload;

    Payload.ExampleParameter = 15;
    BPLib_NC_RemoveLatency(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_ContactSetup_Nominal(void)
{
    BPLib_ContactSetup_Payload_t Payload;

    Payload.ExampleParameter = 16;
    BPLib_NC_ContactSetup(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_CONTACT_SETUP_SUCCESS_EID,
                                "Contact setup directive not implemented, received %d in payload");
}

void Test_BPLib_NC_ContactSetup_Error(void)
{
    /*
    BPLib_ContactSetup_Payload_t Payload;

    Payload.ExampleParameter = 16;
    BPLib_NC_ContactSetup(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_ContactStart_Nominal(void)
{
    BPLib_ContactStart_Payload_t Payload;

    Payload.ExampleParameter = 17;
    BPLib_NC_ContactStart(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_CONTACT_START_SUCCESS_EID,
                                "Contact start directive not implemented, received %d in payload");
}

void Test_BPLib_NC_ContactStart_Error(void)
{
    /*
    BPLib_ContactStart_Payload_t Payload;

    Payload.ExampleParameter = 17;
    BPLib_NC_ContactStart(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_ContactStop_Nominal(void)
{
    BPLib_ContactStop_Payload_t Payload;

    Payload.ExampleParameter = 18;
    BPLib_NC_ContactStop(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_CONTACT_STOP_SUCCESS_EID,
                                "Contact stop directive not implemented, received %d in payload");
}

void Test_BPLib_NC_ContactStop_Error(void)
{
    /*
    BPLib_ContactStop_Payload_t Payload;

    Payload.ExampleParameter = 18;
    BPLib_NC_ContactStop(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_ContactTeardown_Nominal(void)
{
    BPLib_ContactTeardown_Payload_t Payload;

    Payload.ExampleParameter = 19;
    BPLib_NC_ContactTeardown(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_CONTACT_TEARDOWN_SUCCESS_EID,
                                "Contact teardown directive not implemented, received %d in payload");
}

void Test_BPLib_NC_ContactTeardown_Error(void)
{
    /*
    BPLib_ContactTeardown_Payload_t Payload;

    Payload.ExampleParameter = 19;
    BPLib_NC_ContactTeardown(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_AddMibArrayKey_Nominal(void)
{
    BPLib_AddMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 20;
    BPLib_NC_AddMibArrayKey(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_MIB_ARR_KEY_SUCCESS_EID,
                                "Add mib array key directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddMibArrayKey_Error(void)
{
    /*
    BPLib_AddMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 20;
    BPLib_NC_AddMibArrayKey(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_RemoveMibArrayKey_Nominal(void)
{
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 21;
    BPLib_NC_RemoveMibArrayKey(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_MIB_ARR_KEY_SUCCESS_EID,
                                "Remove mib array key directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveMibArrayKey_Error(void)
{
    /*
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 21;
    BPLib_NC_RemoveMibArrayKey(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_SetMibItem_Nominal(void)
{
    BPLib_SetMibItem_Payload_t Payload;

    Payload.ExampleParameter = 22;
    BPLib_NC_SetMibItem(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_SET_MIB_ITEM_SUCCESS_EID,
                                "Set mib item directive not implemented, received %d in payload");
}

void Test_BPLib_NC_SetMibItem_Error(void)
{
    /*
    BPLib_SetMibItem_Payload_t Payload;

    // Invalid item index error return code test
    Payload.ExampleParameter = 22;
    BPLib_NC_SetMibItem(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    // Invalid item value return code test
    Payload.ExampleParameter = 22;
    BPLib_NC_SetMibItem(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);

    // Table update failure return code test
    Payload.ExampleParameter = 22;
    BPLib_NC_SetMibItem(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 3);
    */
}

void Test_BPLib_NC_AddStorageAllocation_Nominal(void)
{
    BPLib_AddStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 23;
    BPLib_NC_AddStorageAllocation(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_ADD_STOR_ALLOC_SUCCESS_EID,
                                "Add storage allocation directive not implemented, received %d in payload");
}

void Test_BPLib_NC_AddStorageAllocation_Error(void)
{
    /*
    BPLib_AddStorageAllocation_Payload_t Payload;

    // Storage allocation error
    Payload.ExampleParameter = 23;
    BPLib_NC_AddStorageAllocation(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    // Table update error
    Payload.ExampleParameter = 23;
    BPLib_NC_AddStorageAllocation(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 2);
    */
}

void Test_BPLib_NC_RemoveStorageAllocation_Nominal(void)
{
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 24;
    BPLib_NC_RemoveStorageAllocation(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_RM_STOR_ALLOC_SUCCESS_EID,
                                "Remove storage allocation directive not implemented, received %d in payload");
}

void Test_BPLib_NC_RemoveStorageAllocation_Error(void)
{
    /*
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 24;
    BPLib_NC_RemoveStorageAllocation(Payload);
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_PerformSelfTest_Nominal(void)
{
    BPLib_NC_PerformSelfTest();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);

    /* Verify event */
    BPLib_NC_Test_Verify_Event(0, BPLIB_NC_PERFORM_SELF_TEST_SUCCESS_EID,
                                "Perform self test (7.2) directive not implemented");
}

void Test_BPLib_NC_PerformSelfTest_Error(void)
{
    /*
    BPLib_NC_PerformSelfTest();
    
    // Verify directive counter was incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 1);
    */
}

void Test_BPLib_NC_SendNodeMibConfigHk_Nominal(void)
{
    BPLib_NC_SendNodeMibConfigHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPA_TLMP_SendNodeMibConfigPkt, 1);
}

void Test_BPLib_NC_SendSourceMibConfigHk_Nominal(void)
{
    BPLib_NC_SendSourceMibConfigHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPA_TLMP_SendPerSourceMibConfigPkt, 1);
}

void Test_BPLib_NC_SendNodeMibCountersHk_Nominal(void)
{
    BPLib_NC_SendNodeMibCountersHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_SendNodeMibCountersHk, 1);
}


void Test_BPLib_NC_SendSourceMibCountersHk_Nominal(void)
{
    BPLib_NC_SendSourceMibCountersHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_SendSourceMibCountersHk, 1);
}

void Test_BPLib_NC_SendStorageHk_Nominal(void)
{
    BPLib_NC_SendStorageHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPA_TLMP_SendStoragePkt, 1);
}

void Test_BPLib_NC_SendChannelContactStatHk_Nominal(void)
{
    BPLib_NC_SendChannelContactStatHk();
    
    // Verify directive counter was not incremented
    UtAssert_STUB_COUNT(BPLib_AS_Increment, 0);

    /* Verify downstream function was called */
    UtAssert_STUB_COUNT(BPLib_AS_SendChannelContactStatHk, 1);
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
    ADD_TEST(Test_BPLib_NC_Init_Nominal);
    ADD_TEST(Test_BPLib_NC_Init_Error);
    ADD_TEST(Test_BPLib_NC_AddAllApplications_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAllApplications_Error);
    ADD_TEST(Test_BPLib_NC_StartAllApplications_Nominal);
    ADD_TEST(Test_BPLib_NC_StartAllApplications_Error);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorage_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleStorage_Error);
    ADD_TEST(Test_BPLib_NC_InitBundleStorage_Nominal);
    ADD_TEST(Test_BPLib_NC_InitBundleStorage_Error);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadata_Nominal);
    ADD_TEST(Test_BPLib_NC_VerifyBundleMetadata_Error);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadata_Nominal);
    ADD_TEST(Test_BPLib_NC_RebuildBundleMetadata_Error);
    ADD_TEST(Test_BPLib_NC_ClearVolatile_Nominal);
    ADD_TEST(Test_BPLib_NC_ClearVolatile_Error);
    ADD_TEST(Test_BPLib_NC_ReloadSavedData_Nominal);
    ADD_TEST(Test_BPLib_NC_ReloadSavedData_Error);
    ADD_TEST(Test_BPLib_NC_ResetAllCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetAllCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetCounter_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetCounter_Error);
    ADD_TEST(Test_BPLib_NC_ResetSourceCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetSourceCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetBundleCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetBundleCounters_Error);
    ADD_TEST(Test_BPLib_NC_ResetErrorCounters_Nominal);
    ADD_TEST(Test_BPLib_NC_ResetErrorCounters_Error);
    ADD_TEST(Test_BPLib_NC_AddApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_AddApplication_Error);
    ADD_TEST(Test_BPLib_NC_RemoveApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveApplication_Error);
    ADD_TEST(Test_BPLib_NC_SetRegistrationState_Nominal);
    ADD_TEST(Test_BPLib_NC_SetRegistrationState_Error);
    ADD_TEST(Test_BPLib_NC_StartApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_StartApplication_Error);
    ADD_TEST(Test_BPLib_NC_StopApplication_Nominal);
    ADD_TEST(Test_BPLib_NC_StopApplication_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthSources_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthSources_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSources_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthSources_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySources_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodySources_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySources_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodySources_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodians_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthCustodians_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodians_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthCustodians_Error);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEid_Nominal);
    ADD_TEST(Test_BPLib_NC_AddAuthReportToEid_Error);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEid_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveAuthReportToEid_Error);
    ADD_TEST(Test_BPLib_NC_AddLatency_Nominal);
    ADD_TEST(Test_BPLib_NC_AddLatency_Error);
    ADD_TEST(Test_BPLib_NC_RemoveLatency_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveLatency_Error);
    ADD_TEST(Test_BPLib_NC_ContactSetup_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactSetup_Error);
    ADD_TEST(Test_BPLib_NC_ContactStart_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStart_Error);
    ADD_TEST(Test_BPLib_NC_ContactStop_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactStop_Error);
    ADD_TEST(Test_BPLib_NC_ContactTeardown_Nominal);
    ADD_TEST(Test_BPLib_NC_ContactTeardown_Error);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKey_Nominal);
    ADD_TEST(Test_BPLib_NC_AddMibArrayKey_Error);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKey_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveMibArrayKey_Error);
    ADD_TEST(Test_BPLib_NC_SetMibItem_Nominal);
    ADD_TEST(Test_BPLib_NC_SetMibItem_Error);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocation_Nominal);
    ADD_TEST(Test_BPLib_NC_AddStorageAllocation_Error);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocation_Nominal);
    ADD_TEST(Test_BPLib_NC_RemoveStorageAllocation_Error);
    ADD_TEST(Test_BPLib_NC_PerformSelfTest_Nominal);
    ADD_TEST(Test_BPLib_NC_PerformSelfTest_Error);
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendStorageHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHk_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Invalid);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Invalid);
}

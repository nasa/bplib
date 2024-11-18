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

    Status = BPLib_NC_AddAllApplications();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_ALL_APPS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add all applications directive is unimplemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAllApplications_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_AddAllApplications();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_StartAllApplications_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_StartAllApplications();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_START_ALL_APPS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Start all applications directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartAllApplications_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_StartAllApplications();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_VerifyBundleStorage_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleStorage();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_VERIF_BNDL_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Verify bundle storage directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleStorage_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleStorage();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_InitBundleStorage_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_InitBundleStorage();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_INIT_BNDL_STOR_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Initialize bundle storage directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_InitBundleStorage_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_InitBundleStorage();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_VerifyBundleMetadata_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleMetadata();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_VERIF_BNDL_META_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Verify bundle metadata (7.2) directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_VerifyBundleMetadata_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_VerifyBundleMetadata();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RebuildBundleMetadata_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_RebuildBundleMetadata();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_REBUILD_BNDL_META_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Rebuild bundle metadata directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RebuildBundleMetadata_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_RebuildBundleMetadata();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ClearVolatile_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ClearVolatile();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_CLR_VOLATILE_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Clear volatile directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ClearVolatile_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_ClearVolatile();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ReloadSavedData_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_ReloadSavedData();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RELOAD_SVD_DATA_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Reload saved data (7.2) directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ReloadSavedData_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_ReloadSavedData();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ResetAllCounters_Nominal(void)
{
    BPLib_NC_ResetAllCounters();

    UtAssert_STUB_COUNT(BPLib_AS_ResetAllCounters, 1);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_ALL_CTRS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successful reset-all-counters directive", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_ResetAllCounters_Error(void)
{
    /* Not applicable as of right now */
}

void Test_BPLib_NC_ResetCounter_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetCounter_Payload_t Payload;

    Payload.SourceEid = 4;
    Payload.Counter   = BUNDLE_COUNT_DELETED_CANCELLED;

    Status = BPLib_NC_ResetCounter(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_CTR_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successfully reset counter %d for source EID %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_ResetCounter_Error(void)
{
    BPLib_Status_t Status;
    BPLib_ResetCounter_Payload_t Payload;

    /* Put dummy values into the payload */
    Payload.Counter   = 0;
    Payload.SourceEid = 0;

    /* === BPLIB_AS_INVALID_EID returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_Set), BPLIB_AS_INVALID_EID);

    /* Run the function under test */
    Status = BPLib_NC_ResetCounter(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_STUB_COUNT(BPLib_AS_Set, 1);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_CTR_SRC_EID_ERR_EID);
    UtAssert_STRINGBUF_EQ("Could not reset counter %d due to a source EID (%d) with unexpected pattern", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* === BPLIB_AS_UNKNOWN_NODE_CNTR returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_Set), BPLIB_AS_UNKNOWN_NODE_CNTR);

    /* Run the function under test */
    Status = BPLib_NC_ResetCounter(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_NODE_CNTR, Status);
    UtAssert_STUB_COUNT(BPLib_AS_Set, 2);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[1].EventID, BPLIB_NC_RESET_CTR_UNK_SRC_CNTR_ERR_EID);
    UtAssert_STRINGBUF_EQ("Could reset unrecognized node counter %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[1].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);

    /* === BPLIB_AS_UNKNOWN_SRC_CNTR returned === */

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_Set), BPLIB_AS_UNKNOWN_SRC_CNTR);

    /* Run the function under test */
    Status = BPLib_NC_ResetCounter(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_UNKNOWN_SRC_CNTR, Status);
    UtAssert_STUB_COUNT(BPLib_AS_Set, 3);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[2].EventID, BPLIB_NC_RESET_CTR_UNK_NODE_CNTR_ERR_EID);
    UtAssert_STRINGBUF_EQ("Could not reset unrecognized source counter %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[2].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_ResetSourceCounters_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetSourceCounters_Payload_t Payload;

    Payload.SourceEid = 2;

    Status = BPLib_NC_ResetSourceCounters(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_SRC_CTRS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successfully reset source counters for source EID %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ResetSourceCounters_Error(void)
{
    BPLib_Status_t Status;
    BPLib_ResetSourceCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetSourceCounters), BPLIB_AS_INVALID_EID);

    Payload.SourceEid = 19;
    Status = BPLib_NC_ResetSourceCounters(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_STUB_COUNT(BPLib_AS_ResetSourceCounters, 1);
}

void Test_BPLib_NC_ResetBundleCounters_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetBundleCounters_Payload_t Payload;

    Payload.SourceEid = 2;

    Status = BPLib_NC_ResetBundleCounters(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_BNDL_CTRS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successfully reset bundle counters for source EID %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_ResetBundleCounters_Error(void)
{
    BPLib_Status_t Status;
    BPLib_ResetBundleCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetBundleCounters), BPLIB_AS_INVALID_EID);

    Payload.SourceEid = 19;
    Status = BPLib_NC_ResetBundleCounters(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_STUB_COUNT(BPLib_AS_ResetBundleCounters, 1);
}

void Test_BPLib_NC_ResetErrorCounters_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ResetErrorCounters_Payload_t Payload;

    Payload.SourceEid = 2;

    Status = BPLib_NC_ResetErrorCounters(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RESET_ERR_CTRS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successfully reset error counters for source EID %d", BPLIB_EM_EXPANDED_EVENT_SIZE,
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
}

void Test_BPLib_NC_ResetErrorCounters_Error(void)
{
    BPLib_Status_t Status;
    BPLib_ResetErrorCounters_Payload_t Payload;

    UT_SetDefaultReturnValue(UT_KEY(BPLib_AS_ResetErrorCounters), BPLIB_AS_INVALID_EID);

    Payload.SourceEid = 19;
    Status = BPLib_NC_ResetErrorCounters(Payload);

    UtAssert_EQ(BPLib_Status_t, BPLIB_AS_INVALID_EID, Status);
    UtAssert_STUB_COUNT(BPLib_AS_ResetErrorCounters, 1);
}

void Test_BPLib_NC_AddApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplication_Payload_t Payload;

    Payload.ChanId = 1;
    Status = BPLib_NC_AddApplication(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_APP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successful add-application directive for ChanId=%d", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_AddApplication_Payload_t Payload;

    /* Check channel error return code */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_AddApplication), BPLIB_ADU_ADD_CHAN_ERR);

    Payload.ChanId = 2;
    Status = BPLib_NC_AddApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_ADD_CHAN_ERR);
    
    /* Check state error return code */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_AddApplication), BPLIB_ADU_ADD_STAT_ERR);
    
    Payload.ChanId = 2;
    Status = BPLib_NC_AddApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_ADD_STAT_ERR);
}

void Test_BPLib_NC_RemoveApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveApplication_Payload_t Payload;

    Payload.ExampleParameter = 2;
    Status = BPLib_NC_RemoveApplication(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_APP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove application directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveApplication_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveApplication_Payload_t Payload;

    Payload.ExampleParameter = 2;
    Status = BPLib_NC_RemoveApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SetRegistrationState_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetRegistrationState_Payload_t Payload;

    Payload.ExampleParameter = 3;
    Status = BPLib_NC_SetRegistrationState(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_SET_REGI_STAT_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Set registration state directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetRegistrationState_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_SetRegistrationState_Payload_t Payload;

    Payload.ExampleParameter = 3;
    Status = BPLib_NC_SetRegistrationState(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_StartApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t Payload;

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_START_APP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successful start-application directive for ChanId=%d", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StartApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t Payload;

    /* Channel error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_CHAN_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_CHAN_ERR);

    /* State error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_STAT_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_STAT_ERR);

    /* Subscribe error return code check */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StartApplication), BPLIB_ADU_START_SUB_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StartApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_START_SUB_ERR);
}

void Test_BPLib_NC_StopApplication_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplication_Payload_t Payload;

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_STOP_APP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Successful stop-application directive for ChanId=%d", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_StopApplication_Error(void)
{
    BPLib_Status_t Status;
    BPLib_StopApplication_Payload_t Payload;

    /* Invalid channel return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_CHAN_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_CHAN_ERR);

    /* Invalid state return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_STAT_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_STAT_ERR);

    /* Unsubscribe error return code test */
    UT_SetDefaultReturnValue(UT_KEY(BPA_ADUP_StopApplication), BPLIB_ADU_STOP_UNSUB_ERR);

    Payload.ChanId = 1;
    Status = BPLib_NC_StopApplication(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_ADU_STOP_UNSUB_ERR);
}

void Test_BPLib_NC_AddAuthSources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 6;
    Status = BPLib_NC_AddAuthSources(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_AUTH_SRCS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add authorized sources directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthSources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 6;
    Status = BPLib_NC_AddAuthSources(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthSources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 7;
    Status = BPLib_NC_RemoveAuthSources(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_AUTH_SRCS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove authorized sources directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthSources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthSources_Payload_t Payload;

    Payload.ExampleParameter = 7;
    Status = BPLib_NC_RemoveAuthSources(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthCustodySources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 8;
    Status = BPLib_NC_AddAuthCustodySources(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_AUTH_CUST_SRCS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add authorized custody sources directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodySources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 8;
    Status = BPLib_NC_AddAuthCustodySources(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthCustodySources_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 9;
    Status = BPLib_NC_RemoveAuthCustodySources(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_AUTH_CUST_SRCS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove authorized custody sources directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodySources_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodySources_Payload_t Payload;

    Payload.ExampleParameter = 9;
    Status = BPLib_NC_RemoveAuthCustodySources(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthCustodians_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 10;
    Status = BPLib_NC_AddAuthCustodians(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_AUTH_CUSTODIANS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add authorized custodians directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthCustodians_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 10;
    Status = BPLib_NC_AddAuthCustodians(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthCustodians_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 11;
    Status = BPLib_NC_RemoveAuthCustodians(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_AUTH_CUSTODIANS_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove authorized custodians directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthCustodians_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthCustodians_Payload_t Payload;

    Payload.ExampleParameter = 11;
    Status = BPLib_NC_RemoveAuthCustodians(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddAuthReportToEid_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 12;
    Status = BPLib_NC_AddAuthReportToEid(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_AUTH_RPT_EID_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add authorized report-to eid directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddAuthReportToEid_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 12;
    Status = BPLib_NC_AddAuthReportToEid(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveAuthReportToEid_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 13;
    Status = BPLib_NC_RemoveAuthReportToEid(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_AUTH_RPT_EID_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove authorized report-to eid directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveAuthReportToEid_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveAuthReportToEid_Payload_t Payload;

    Payload.ExampleParameter = 13;
    Status = BPLib_NC_RemoveAuthReportToEid(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddLatency_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddLatency_Payload_t Payload;

    Payload.ExampleParameter = 14;
    Status = BPLib_NC_AddLatency(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_LATENCY_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add latency directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddLatency_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddLatency_Payload_t Payload;

    Payload.ExampleParameter = 14;
    Status = BPLib_NC_AddLatency(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveLatency_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveLatency_Payload_t Payload;

    Payload.ExampleParameter = 15;
    Status = BPLib_NC_RemoveLatency(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_LATENCY_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove latency directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveLatency_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveLatency_Payload_t Payload;

    Payload.ExampleParameter = 15;
    Status = BPLib_NC_RemoveLatency(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactSetup_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactSetup_Payload_t Payload;

    Payload.ExampleParameter = 16;
    Status = BPLib_NC_ContactSetup(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_CONTACT_SETUP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Contact setup directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactSetup_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactSetup_Payload_t Payload;

    Payload.ExampleParameter = 16;
    Status = BPLib_NC_ContactSetup(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactStart_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStart_Payload_t Payload;

    Payload.ExampleParameter = 17;
    Status = BPLib_NC_ContactStart(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_CONTACT_START_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Contact start directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStart_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactStart_Payload_t Payload;

    Payload.ExampleParameter = 17;
    Status = BPLib_NC_ContactStart(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactStop_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactStop_Payload_t Payload;

    Payload.ExampleParameter = 18;
    Status = BPLib_NC_ContactStop(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_CONTACT_STOP_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Contact stop directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactStop_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactStop_Payload_t Payload;

    Payload.ExampleParameter = 18;
    Status = BPLib_NC_ContactStop(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_ContactTeardown_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_ContactTeardown_Payload_t Payload;

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ContactTeardown(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_CONTACT_TEARDOWN_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Contact teardown directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_ContactTeardown_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_ContactTeardown_Payload_t Payload;

    Payload.ExampleParameter = 19;
    Status = BPLib_NC_ContactTeardown(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_AddMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 20;
    Status = BPLib_NC_AddMibArrayKey(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_MIB_ARR_KEY_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add mib array key directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddMibArrayKey_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 20;
    Status = BPLib_NC_AddMibArrayKey(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveMibArrayKey_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 21;
    Status = BPLib_NC_RemoveMibArrayKey(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_MIB_ARR_KEY_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove mib array key directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveMibArrayKey_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveMibArrayKey_Payload_t Payload;

    Payload.ExampleParameter = 21;
    Status = BPLib_NC_RemoveMibArrayKey(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SetMibItem_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_SetMibItem_Payload_t Payload;

    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_SET_MIB_ITEM_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Set mib item directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SetMibItem_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_SetMibItem_Payload_t Payload;

    // Invalid item index error return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MIB_ITEM_INVALID_INDEX_ERR_EID);

    // Invalid item value return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MID_ITEM_INVALID_VALUE_ERR_EID);
    
    // Table update failure return code test
    Payload.ExampleParameter = 22;
    Status = BPLib_NC_SetMibItem(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SET_MIB_ITEM_TBL_UPDATE_FAIL);
    */
}

void Test_BPLib_NC_AddStorageAllocation_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_AddStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_ADD_STOR_ALLOC_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Add storage allocation directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_AddStorageAllocation_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_AddStorageAllocation_Payload_t Payload;

    // Storage allocation error
    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);

    // Table update error
    Payload.ExampleParameter = 23;
    Status = BPLib_NC_AddStorageAllocation(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_RemoveStorageAllocation_Nominal(void)
{
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 24;
    Status = BPLib_NC_RemoveStorageAllocation(Payload);

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_RM_STOR_ALLOC_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Remove storage allocation directive not implemented, received %d in payload", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_RemoveStorageAllocation_Error(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_RemoveStorageAllocation_Payload_t Payload;

    Payload.ExampleParameter = 24;
    Status = BPLib_NC_RemoveStorageAllocation(Payload);

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_PerformSelfTest_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_PerformSelfTest();

    UtAssert_INT32_EQ(context_BPLib_EM_SendEvent[0].EventID, BPLIB_NC_PERFORM_SELF_TEST_SUCCESS_EID);
    UtAssert_STRINGBUF_EQ("Perform self test (7.2) directive not implemented", BPLIB_EM_EXPANDED_EVENT_SIZE, 
                            context_BPLib_EM_SendEvent[0].Spec, BPLIB_EM_EXPANDED_EVENT_SIZE);
    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_PerformSelfTest_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_PerformSelfTest();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendNodeMibConfigHk_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendNodeMibConfigPkt), BPLIB_SUCCESS);

    Status = BPLib_NC_SendNodeMibConfigHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibConfigHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_SendNodeMibConfigHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendSourceMibConfigHk_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendPerSourceMibConfigPkt), BPLIB_SUCCESS);

    Status = BPLib_NC_SendSourceMibConfigHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibConfigHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_SendSourceMibConfigHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendNodeMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendNodeMibCounterPkt), BPLIB_SUCCESS);

    Status = BPLib_NC_SendNodeMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendNodeMibCountersHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_SendNodeMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendSourceMibCountersHk_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendPerSourceMibCounterPkt), BPLIB_SUCCESS);

    Status = BPLib_NC_SendSourceMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendSourceMibCountersHk_Error(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendPerSourceMibCounterPkt), BPLIB_UNKNOWN);

    Status = BPLib_NC_SendSourceMibCountersHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_UNKNOWN);
}

void Test_BPLib_NC_SendStorageHk_Nominal(void)
{
    BPLib_Status_t Status;

    Status = BPLib_NC_SendStorageHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendStorageHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_SendStorageHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
    */
}

void Test_BPLib_NC_SendChannelContactStatHk_Nominal(void)
{
    BPLib_Status_t Status;

    UT_SetDefaultReturnValue(UT_KEY(BPA_TLMP_SendChannelContactPkt), BPLIB_SUCCESS);

    Status = BPLib_NC_SendChannelContactStatHk();

    UtAssert_EQ(BPLib_Status_t, Status, BPLIB_SUCCESS);
}

void Test_BPLib_NC_SendChannelContactStatHk_Error(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_NC_SendChannelContactStatHk();

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
    ADD_TEST(Test_BPLib_NC_Init_Nominal);
    ADD_TEST(Test_BPLib_NC_Init_Error);
    ADD_TEST(Test_BPLib_NC_Noop_Nominal);
    ADD_TEST(Test_BPLib_NC_Noop_Error);
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
    ADD_TEST(Test_BPLib_NC_SendNodeMibConfigHk_Error);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibConfigHk_Error);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendNodeMibCountersHk_Error);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendSourceMibCountersHk_Error);
    ADD_TEST(Test_BPLib_NC_SendStorageHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendStorageHk_Error);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHk_Nominal);
    ADD_TEST(Test_BPLib_NC_SendChannelContactStatHk_Error);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPNTblValidateFunc_Invalid);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Nominal);
    ADD_TEST(Test_BPLib_NC_MIBConfigPSTblValidateFunc_Invalid);
}

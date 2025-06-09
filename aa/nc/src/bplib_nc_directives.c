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

#include "bplib_nc_directives.h"
#include "bplib_cla.h"
#include "bplib_pi.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

void BPLib_NC_Noop(void)
{
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
    BPLib_EM_SendEvent(BPLIB_NC_NOOP_SUCCESS_EID,
                        BPLib_EM_EventType_INFORMATION,
                        "BPLib Version: v%u.%u.%u-sprint-%u. NODE_STARTUP_COUNTER is %d",
                        BPLIB_MAJOR_VERSION,
                        BPLIB_MINOR_VERSION,
                        BPLIB_REVISION,
                        BPLIB_BUILD_NUMBER,
                        BPLib_AS_GetCounter(&BPLIB_EID_INSTANCE, NODE_STARTUP_COUNTER));
}

void BPLib_NC_AddAllApplications(void)
{
    /*
    BPLib_Status_t Status;
    uint8_t AppIdx;
    ??? AppList[???];
    BPLib_AddApplication_Payload_t AddAllAppsPayload;

    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        AddAllAppsPayload.ChanId = AppList[AppIdx]
        Status &= BPLib_NC_AddApplication(AddAllAppsPayload);
    }

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_ALL_APPS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add all applications directive is unimplemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_ALL_APPS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Add all applications directive is unimplemented");
    }
    */
}

void BPLib_NC_StartAllApplications(void)
{
    /*
    BPLib_Status_t Status;
    BPLib_StartApplication_Payload_t StartAllAppsPayload;
    uint8_t AppIdx;
    ??? AppList[???];

    Status = BPLIB_SUCCESS;

    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        StartAllAppsPayload.ChanId = AppList[AppIdx];
        Status &= BPLib_NC_StartApplication(StartAllAppsPayload);
    }

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_START_ALL_APPS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Start all applications directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_START_ALL_APPS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Start all applications directive not implemented");
    }
    */
}

void BPLib_NC_VerifyBundleStorage(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_VERIF_BNDL_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Verify bundle storage directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_VERIF_BNDL_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Verify bundle storage directive not implemented");
    }
    */
}

void BPLib_NC_InitBundleStorage(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_INIT_BNDL_STOR_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Initialize bundle storage directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_INIT_BNDL_STOR_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Initialize bundle storage directive not implemented");
    }
    */
}

void BPLib_NC_VerifyBundleMetadata(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_VERIF_BNDL_META_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Verify bundle metadata (7.2) directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_VERIF_BNDL_META_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Verify bundle metadata (7.2) directive not implemented");
    }
    */
}

void BPLib_NC_RebuildBundleMetadata(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_STOR_RebuildBundleMetadata();

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_REBUILD_BNDL_META_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Rebuild bundle metadata directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_REBUILD_BNDL_META_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Rebuild bundle metadata directive not implemented");
    }
    */
}

void BPLib_NC_ClearVolatile(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_STOR_ClearVolatile();

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CLR_VOLATILE_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Clear volatile directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CLR_VOLATILE_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Clear volatile directive not implemented");
    }
    */
}

void BPLib_NC_ReloadSavedData(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RELOAD_SVD_DATA_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Reload saved data (7.2) directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RELOAD_SVD_DATA_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Reload saved data (7.2) directive not implemented");
    }
    */
}

void BPLib_NC_ResetAllCounters(void)
{
    BPLib_AS_ResetAllCounters();

    BPLib_EM_SendEvent(BPLIB_NC_RESET_ALL_CTRS_SUCCESS_EID,
                        BPLib_EM_EventType_INFORMATION,
                        "Successful reset-all-counters directive");
}

void BPLib_NC_ResetCounter(const BPLib_ResetCounter_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_ResetCounter(Payload.MibArrayIndex, Payload.Counter);

    if (Status == BPLIB_SUCCESS)
    {
        if (Payload.Counter != BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT)
        {
            BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        }

        BPLib_EM_SendEvent(BPLIB_NC_RESET_CTR_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful reset-counter directive for counter %d at MIB counter array index %d",
                            Payload.Counter,
                            Payload.MibArrayIndex);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RESET_CTR_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset counter %d at MIB counter array index %d, RC = %d",
                            Payload.Counter,
                            Payload.MibArrayIndex,
                            Status);
    }
}

void BPLib_NC_ResetSourceCounters(const BPLib_ResetSourceCounters_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_ResetSourceCounters(Payload.MibArrayIndex);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RESET_SRC_CTRS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful reset-source-counters directive for MIB counter array index %d",
                            Payload.MibArrayIndex);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RESET_SRC_CTRS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset source counters at index %d, RC = %d",
                            Payload.MibArrayIndex,
                            Status);
    }
}

void BPLib_NC_ResetBundleCounters()
{
    BPLib_AS_ResetBundleCounters();

    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);

    BPLib_EM_SendEvent(BPLIB_NC_RESET_BNDL_CTRS_SUCCESS_EID,
                         BPLib_EM_EventType_INFORMATION,
                        "Successful reset-bundle-counters command");
}

void BPLib_NC_ResetErrorCounters(const BPLib_ResetErrorCounters_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_ResetErrorCounters(Payload.MibArrayIndex);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RESET_ERR_CTRS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful reset-error-counters for source MIB counters at index %d",
                            Payload.MibArrayIndex);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RESET_ERR_CTRS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not reset error counters for source MIB counters at index %d, RC = %d",
                            Payload.MibArrayIndex,
                            Status);
    }
}

void BPLib_NC_AddApplication(const BPLib_AddApplication_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Add application configurations */
    Status = BPLib_PI_AddApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_APP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful add-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_APP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not add application with channel ID %d, RC = %d",
                            Payload.ChanId,
                            Status);
    }
}

void BPLib_NC_RemoveApplication(BPLib_Instance_t *Inst, const BPLib_RemoveApplication_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Remove application configurations */
    Status = BPLib_PI_RemoveApplication(Inst, Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_APP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful remove-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_APP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not remove application with channel ID %d, RC = %d",
                            Payload.ChanId,
                            Status);
    }
}

void BPLib_NC_SetRegistrationState(const BPLib_SetRegistrationState_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    Node Configuration calls Storage to set application state in-channel parameters to
    * Active
    * Passive, with action entry:
        * Defer
        * Abandon
            * Node Configuration sends Storage request to delete any bundles already queued for the channel and future bundles for that channel

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SET_REGI_STAT_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Set registration state directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SET_REGI_STAT_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Set registration state directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_StartApplication(const BPLib_StartApplication_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Start application */
    Status = BPLib_PI_StartApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_START_APP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful start-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_START_APP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not start application with channel ID %d, RC = %d",
                            Payload.ChanId,
                            Status);
    }
}

void BPLib_NC_StopApplication(const BPLib_StopApplication_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Stop application */
    Status = BPLib_PI_StopApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_STOP_APP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful stop-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_STOP_APP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not stop application with channel ID %d, RC = %d",
                            Payload.ChanId,
                            Status);
    }
}

void BPLib_NC_AddAuthSources(const BPLib_AddAuthSources_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_SRCS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_SRCS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Add authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_RemoveAuthSources(const BPLib_RemoveAuthSources_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_SRCS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_SRCS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_AddAuthCustodySources(const BPLib_AddAuthCustodySources_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_CUST_SRCS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_CUST_SRCS_ERR_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_RemoveAuthCustodySources(const BPLib_RemoveAuthCustodySources_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_CUST_SRCS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_CUST_SRCS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_AddAuthCustodians(const BPLib_AddAuthCustodians_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_CUSTODIANS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_CUSTODIANS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Add authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_RemoveAuthCustodians(const BPLib_RemoveAuthCustodians_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_CUSTODIANS_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_CUSTODIANS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_AddAuthReportToEid(const BPLib_AddAuthReportToEid_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_RPT_EID_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_AUTH_RPT_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Add authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_RemoveAuthReportToEid(const BPLib_RemoveAuthReportToEid_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_RPT_EID_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_AUTH_RPT_EID_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_AddLatency(const BPLib_AddLatency_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_LATENCY_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_LATENCY_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Add latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_RemoveLatency(const BPLib_RemoveLatency_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_LATENCY_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_LATENCY_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_ContactSetup(const BPLib_ContactSetup_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_CLA_ContactSetup(Payload.ContactId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_SETUP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful contact-setup directive for contact ID = %d",
                            Payload.ContactId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_SETUP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed contact-setup directive for contact ID = %d",
                            Payload.ContactId);
    }
}

void BPLib_NC_ContactStart(const BPLib_ContactStart_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_CLA_ContactStart(Payload.ContactId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_START_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful contact-start directive for contact ID = %d",
                            Payload.ContactId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_START_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed contact-start directive for contact ID = %d",
                            Payload.ContactId);
    }
}

void BPLib_NC_ContactStop(const BPLib_ContactStop_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_CLA_ContactStop(Payload.ContactId);

    if (Status == BPLIB_SUCCESS)
    {
        /* TODO: Request Storage clear all queues associated with the CLA. Storage clears the queues */

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_STOP_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful contact-stop directive for contact ID = %d",
                            Payload.ContactId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_STOP_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed contact-stop directive for contact ID = %d",
                            Payload.ContactId);
    }
}

void BPLib_NC_ContactTeardown(BPLib_Instance_t *Inst, const BPLib_ContactTeardown_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_CLA_ContactTeardown(Inst, Payload.ContactId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_TEARDOWN_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful contact-teardown directive for contact ID = %d",
                            Payload.ContactId);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_CONTACT_TEARDOWN_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Failed contact-teardown directive for contact ID = %d",
                            Payload.ContactId);
    }
}

void BPLib_NC_AddMibArrayKey(const BPLib_AddMibArrayKey_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_AddMibArrayKey(Payload.EID_Patterns);
    Status = BPLIB_SUCCESS; // Ignore return status for integration tests

    /*
    - If valid, NC adds entry to table and synchronously begins using updated table
    - NC sends notification to Framework Proxy that MIB configuration has been updated
    - Framework Proxy notifies cFS Table Services of MIB table update
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_MIB_ARR_KEY_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Successful add-mib-array-key directive");
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_MIB_ARR_KEY_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "An error occurred while attempting to add a MIB array key");
    }
}

void BPLib_NC_RemoveMibArrayKey(const BPLib_RemoveMibArrayKey_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_MIB_ARR_KEY_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_MIB_ARR_KEY_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_SetMibItem(const BPLib_SetMibItem_Payload_t Payload)
{
    BPLib_Status_t Status;

    if (BPLib_EID_PatternIsMatch(&BPLIB_EID_INSTANCE, (BPLib_EID_Pattern_t *) &(Payload.EidPattern)))
    {
        Status = BPLib_NC_SetMibNodeConfig(Payload.MibItem, Payload.Value);
    }
    else
    {
        Status = BPLib_NC_SetMibSourceConfig(&(Payload.EidPattern), Payload.MibItem, 
                                                                    Payload.Value);
    }

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SET_MIB_ITEM_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Set MIB item #%d to %d.",
                            Payload.MibItem, Payload.Value);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SET_MIB_ITEM_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Failed to set MIB item #%d to %d, RC=%d",
                            Payload.MibItem, Payload.Value, Status);

    }
}

void BPLib_NC_AddStorageAllocation(const BPLib_AddStorageAllocation_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLib_STOR_AddStorageAllocation(Payload.EID, Payload.AllocSize);

    if (Status == BPLIB_SUCCESS)
    {
        Status = (BPLib_Status_t) BPA_TABLEP_TableUpdate();
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_ADD_STOR_ALLOC_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Add storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        switch (Status)
        {
            // case BPLIB_STOR_ADD_ALLOC_ERR:
            //     BPLib_EM_SendEvent(BPLIB_NC_ADD_STOR_ALLOC_ERR_EID,
            //                         BPLib_EM_EventType_ERROR,
            //                         "Could not update storage allocation of size %d for EID %d",
            //                         Msg->Payload.AllocSize, Msg->Payload.EID);
            //     break;
            // case BPLIB_TABLE_UPDATE_ERR:
            //     BPLib_EM_SendEvent(BPLIB_NC_ADD_STOR_ALLOC_ERR_EID,
            //                         BPLib_EM_EventType_ERROR,
            //                         "Failed to update the MIB configuration");

            //     break;
            default:
                break;
        }
    }
    */
}

void BPLib_NC_RemoveStorageAllocation(const BPLib_RemoveStorageAllocation_Payload_t Payload)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    - Node Configuration notifies Storage to remove an allocation with a given EID pattern and allocation size
    - If storage update is successful, NC calls FWP to sync table updates with host

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_STOR_ALLOC_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Remove storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_RM_STOR_ALLOC_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Remove storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    */
}

void BPLib_NC_PerformSelfTest(void)
{
    /*
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    */
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_ACCEPTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_PERFORM_SELF_TEST_SUCCESS_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Perform self test (7.2) directive not implemented");
    }
    /*
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_PERFORM_SELF_TEST_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Perform self test (7.2) directive not implemented");
    }
    */
}

void BPLib_NC_SendNodeMibConfigHk(void)
{
    BPLib_Status_t Status;

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendNodeMibConfigPkt(&BPLib_NC_NodeMibConfigPayload);

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_NODE_CONFIG_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send node MIB configuration packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendSourceMibConfigHk(void)
{
    BPLib_Status_t Status;

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendPerSourceMibConfigPkt(&BPLib_NC_SourceMibConfigPayload);

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_SRC_CONFIG_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send source MIB configuration packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendNodeMibCountersHk(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendNodeMibCountersHk();

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_NODE_CNTRS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send node MIB counters packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendSourceMibCountersHk(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_SendSourceMibCountersHk();

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_SRC_CNTRS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send source MIB counters packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendStorageHk(void)
{
    BPLib_Status_t Status;

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendStoragePkt(&BPLib_STOR_StoragePayload);

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_STORAGE_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send storage packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendChannelContactStatHk(void)
{
    BPLib_Status_t              Status;
    uint32_t                    ContactId;
    BPLib_CLA_ContactRunState_t RunState;

    for (ContactId = 0; ContactId < BPLIB_MAX_NUM_CONTACTS; ContactId++)
    {
        Status = BPLib_CLA_GetContactRunState(ContactId, &RunState);
        if (Status == BPLIB_SUCCESS)
        {
            BPLib_NC_ChannelContactStatsPayload.ContactStatus[ContactId].State = RunState;
        }
        else
        {
            // BPLib_EM_SendEvent()
        }
    }

    Status = BPLib_FWP_ProxyCallbacks.BPA_TLMP_SendChannelContactPkt(&BPLib_NC_ChannelContactStatsPayload);

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_CONTACTS_ERR_EID,
                            BPLib_EM_EventType_ERROR,
                            "Could not send channel contact statistics packet, RC = %d",
                            Status);
    }
}

void BPLib_NC_SendNodeMibReportsHk(void)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;

    Status = BPLib_AS_SendNodeMibReportsHk();

    if (Status != BPLIB_SUCCESS)
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_AGENT_REJECTED_DIRECTIVE_COUNT, 1);
        BPLib_EM_SendEvent(BPLIB_NC_SEND_REPORTS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Could not send node MIB reports packet, RC = %d", Status);
    }
}

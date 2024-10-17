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

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_NC_Init(void)
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_NC_Noop(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAllApplications(void)
{
    BPLib_Status_t Status;
    // uint8_t AppIdx;
    // ??? AppList[???];
    // BPLib_AddApplicationCmd_Payload_t AddAllAppsPayload;

    Status = BPLIB_SUCCESS;

    /*
    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        AddAllAppsPayload.ChanId = AppList[AppIdx]
        Status &= BPLib_NC_AddApplication(AddAllAppsPayload);
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
         BPLib_EM_SendEvent(BPLIB_ADD_ALL_APPS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add all applications directive is unimplemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_ALL_APPS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add all applications directive is unimplemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StartAllApplications(void)
{
    BPLib_Status_t Status;
    // BPLib_StartApplicationCmd_Payload_t StartAllAppsPayload;
    // uint8_t AppIdx;
    // ??? AppList[???];

    Status = BPLIB_SUCCESS;

    /*
    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        StartAllAppsPayload.ChanId = AppList[AppIdx];
        Status &= BPLib_NC_StartApplication(StartAllAppsPayload);
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_START_ALL_APPS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Start all applications directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_START_ALL_APPS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Start all applications directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleStorage(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Verify bundle storage directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Verify bundle storage directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_InitBundleStorage(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_INIT_BNDL_STOR_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Initialize bundle storage directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_INIT_BNDL_STOR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Initialize bundle storage directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleMetadata(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_META_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Verify bundle metadata (7.2) directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_META_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Verify bundle metadata (7.2) directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RebuildBundleMetadata(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_RebuildBundleMetadata();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_REBUILD_BNDL_META_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Rebuild bundle metadata directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_REBUILD_BNDL_META_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Rebuild bundle metadata directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ClearVolatile(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_ClearVolatile();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CLR_VOLATILE_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Clear volatile directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CLR_VOLATILE_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Clear volatile directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ReloadSavedData(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RELOAD_SVD_DATA_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reload saved data (7.2) directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RELOAD_SVD_DATA_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reload saved data (7.2) directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetAllCounters(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /* Status = BPLib_AS_ResetAllCounters(); */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ALL_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION, 
                            "Reset all counters directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ALL_CTRS_ERR_EID, BPLib_EM_EventType_INFORMATION, 
                            "Reset all counters directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetCounter(const BPLib_ResetCounterCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_CTR_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset counter directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_CTR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset counter directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetSourceCounters(const BPLib_ResetSourceCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetSourceCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_SRC_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset source counters directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_SRC_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset source counters directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetBundleCounters(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetBundleCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_BNDL_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset bundle counters directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_BNDL_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset bundle counters directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetErrorCounters(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetErrorCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ERR_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset error counters directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ERR_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset error counters directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddApplication(const BPLib_AddApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Add application configurations */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_AddApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Successful add-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        switch (Status)
        {
            case BPLIB_ADU_ADD_CHAN_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_ADD_CHAN_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with add-application directive, invalid ChanId=%d",
                                    Payload.ChanId);

                break;
            case BPLIB_ADU_ADD_STAT_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_ADD_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with add-application directive, invalid app state for ChanId=%d", 
                                    Payload.ChanId);

                break;
            default:
                break;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveApplication(const BPLib_RemoveApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove application directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_APP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove application directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SetRegistrationState(const BPLib_SetRegistrationStateCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Node Configuration calls Storage to set application state in-channel parameters to
    * Active
    * Passive, with action entry:
        * Defer
        * Abandon
            * Node Configuration sends Storage request to delete any bundles already queued for the channel and future bundles for that channel
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SET_REGI_STAT_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Set registration state directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SET_REGI_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Set registration state directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StartApplication(const BPLib_StartApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Start application */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_StartApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_START_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Successful start-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        switch (Status)
        {
            case BPLIB_ADU_START_CHAN_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_START_CHAN_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with start-application directive, invalid ChanId=%d",
                                    Payload.ChanId);

                break;
            case BPLIB_ADU_START_STAT_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_START_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with start-application directive, invalid app state for ChanId=%d", 
                                    Payload.ChanId);

                break;
            case BPLIB_ADU_START_SUB_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_START_SUB_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error subscribing to ADU on channel #%d",
                                    Payload.ChanId);

                break;
            default:
                break;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StopApplication(const BPLib_StopApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    /* Stop application */
    Status = BPLib_FWP_ProxyCallbacks.BPA_ADUP_StopApplication(Payload.ChanId);

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_START_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Successful stop-application directive for ChanId=%d",
                            Payload.ChanId);
    }
    else
    {
        switch (Status)
        {
            case BPLIB_ADU_STOP_CHAN_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_STOP_CHAN_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with stop-application directive, invalid ChanId=%d",
                                    Payload.ChanId);

                break;
            case BPLIB_ADU_STOP_STAT_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_STOP_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error with stop-application directive, invalid app state for ChanId=%d", 
                                    Payload.ChanId);

                break;
            case BPLIB_ADU_STOP_UNSUB_ERR:
                BPLib_EM_SendEvent(BPLIB_ADU_UNSUB_ERR_EID, BPLib_EM_EventType_ERROR,
                                    "Error unsubscribing from ADU on channel #%d", 
                                    Payload.ChanId);

                break;
            default:
                break;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthSources(const BPLib_AddAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthSources(const BPLib_RemoveAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodySources(const BPLib_AddAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodySources(const BPLib_RemoveAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUST_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized custody sources directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodians(const BPLib_AddAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUSTODIANS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodians(const BPLib_RemoveAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUSTODIANS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized custodians directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthReportToEid(const BPLib_AddAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_RPT_EID_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthReportToEid(const BPLib_RemoveAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_RPT_EID_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized report-to eid directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddLatency(const BPLib_AddLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_LATENCY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_LATENCY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveLatency(const BPLib_RemoveLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_LATENCY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_LATENCY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove latency directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactSetup(const BPLib_ContactSetupCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Verify that table was validated via cFS table upload.    
    Checks if path is available for assignment
    Assigns an instance of BI and CLA
    Configures assigned instances of BI, CT, EBP, CLA based on configuration
    Registers CLA with Storage given the table configuration (Path ID, EID map)
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_SETUP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact setup directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_SETUP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact setup directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactStart(const BPLib_ContactStartCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    identify contact
    Status = BPLib_CLA_ContactStart();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_START_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact start directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_START_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact start directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactStop(const BPLib_ContactStopCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    identify contact to be stopped

    Status = BPLib_CLA_ContactStop();

    Node Configuration requests Storage clear all queues associated with the CLA. Storage clears the queues    
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_STOP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact stop directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_STOP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact stop directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactTeardown(const BPLib_ContactTeardownCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    If contact has been stopped, Node Configuration deconfigures the CLA
    Node Configuration requests BI/CT/EBP/CLA instances to remove configuration
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_TEARDOWN_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact teardown directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_TEARDOWN_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact teardown directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddMibArrayKey(const BPLib_AddMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    NC verifies that specified EID (source) is valid (EID schema, table size, and duplicate checking)
    If valid, NC adds entry to table and synchronously begins using updated table
    NC sends notification to Framework Proxy that MIB configuration has been updated
    Framework Proxy notifies cFS Table Services of MIB table update
    NC calls AS to set the MIB array key
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_MIB_ARR_KEY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveMibArrayKey(const BPLib_RemoveMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_MIB_ARR_KEY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove mib array key directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SetMibItem(const BPLib_SetMibItemCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    // NC verifies that specified MIB item is valid index into the list of MIB items
    if (Payload.Index > MAX_MIB_ARR_SIZE || Payload.Index < 0)
    {
        return BPLIB_INVALID_MIB_ITEM_INDEX;
    }
    */

    /*
    // NC verifies that value provided is a valid value for the MIB item
    if (Payload.Value > MAX_MIB_VALUE || Payload.Value < MIN_MIB_VALUE)
    {
        return BPLIB_INVALID_MIB_VALUE;   
    }
    else
    {
        // If valid, NC sets the MIB item to specified value...
        MIB.Value = Payload.Value;

        // NC synchronously begins using updated table
    }
    */

    /*
    // Framework Proxy notifies cFS Table Services of MIB table update
    Status = (BPLib_Status_t) BPA_TABLEP_TableUpdate();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SET_MIB_ITEM_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Set mib item directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        switch (Status)
        {
            case BPLIB_NC_INVALID_MIB_ITEM_INDEX:
                // BPLib_EM_SendEvent(BPLIB_SET_MIB_ITEM_INVALID_INDEX_ERR_EID, BPLib_EM_EventType_ERROR,
                //                     "Given index (%d) was out of bounds, expected value in range [0, %d]",
                //                     Msg->Payload.Index, MAX_MIB_ARR_SIZE);

                break;
            case BPLIB_NC_INVALID_MID_VALUE:
                // BPLib_EM_SendEvent(BPLIB_SET_MID_ITEM_INVALID_VALUE_ERR_EID,
                //                     BPLib_EM_EventType_ERROR,
                //                     "Given MIB value (%d) was invalid, expected value in range [%d, %d]",
                //                     Msg->Payload.Value, MAX_MIB_VALUE, MIN_MID_VALUE);

                break;
            // case BPLIB_TABLE_UPDATE_ERR:
            //     BPLib_EM_SendEvent(BPLIB_SET_MIB_ITEM_TBL_UPDATE_FAIL, BPLib_EM_EventType_ERROR,
            //                         "Failed to update the MIB configuration")

            //     break;
            default:
                break;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddStorageAllocation(const BPLib_AddStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_AddStorageAllocation(Payload.EID, Payload.AllocSize);

    if (Status == BPLIB_SUCCESS)
    {
        Status = (BPLib_Status_t) BPA_TABLEP_TableUpdate();
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_STOR_ALLOC_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        switch (Status)
        {
            // case BPLIB_STOR_ADD_ALLOC_ERR:
            //     BPLib_EM_SendEvent(BPLIB_ADD_STOR_ALLOC_ERR_EID,
            //                         BPLib_EM_EventType_ERROR,
            //                         "Could not update storage allocation of size %d for EID %d",
            //                         Msg->Payload.AllocSize, Msg->Payload.EID);
            //     break;
            // case BPLIB_TABLE_UPDATE_ERR:
            //     BPLib_EM_SendEvent(BPLIB_ADD_STOR_ALLOC_ERR_EID,
            //                         BPLib_EM_EventType_ERROR,
            //                         "Failed to update the MIB configuration");

            //     break;
            default:
                break;
        }
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveStorageAllocation(const BPLib_RemoveStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Node Configuration notifies Storage to remove an allocation with a given EID pattern and allocation size
    If storage update is successful, NC calls FWP to sync table updates with host
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_STOR_ALLOC_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_STOR_ALLOC_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove storage allocation directive not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_PerformSelfTest(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_PERFORM_SELF_TEST_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Perform self test (7.2) directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PERFORM_SELF_TEST_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Perform self test (7.2) directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibConfigHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CFG_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send node mib config hk directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CFG_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send node mib config hk directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibConfigHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CFG_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send per-source mib config hk directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CFG_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send per-source mib config hk directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibCountersHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CTRS_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send node mib counters HK directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send node mib counters HK directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibCountersHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CTRS_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send per-source mib counter hk directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send per-source mib counter hk directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendStorageHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_STOR_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send storage hk directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_STOR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send storage hk directive not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendChannelContactStatHk(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_CHAN_CONTACT_STAT_HK_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send channel/contact status hk directive not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_CHAN_CONTACT_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send channel/contact status hk directive not implemented");
    }

    return Status;
}
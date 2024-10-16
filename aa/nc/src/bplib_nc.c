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

BPLib_Status_t BPLib_NC_NoopCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAllApplicationsCmd(void)
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
        Status &= BPLib_NC_AddApplicationCmd(AddAllAppsPayload);
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
         BPLib_EM_SendEvent(BPLIB_ADD_ALL_APPS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add all applications command is unimplemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_ALL_APPS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add all applications command is unimplemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StartAllApplicationsCmd(void)
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
        Status &= BPLib_NC_StartApplicationCmd(StartAllAppsPayload);
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_START_ALL_APPS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Start all applications command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_START_ALL_APPS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Start all applications command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleStorageCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Verify bundle storage command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Verify bundle storage command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_InitBundleStorageCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_INIT_BNDL_STOR_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Initialize bundle storage command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_INIT_BNDL_STOR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Initialize bundle storage command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleMetadataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_META_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Verify bundle metadata (7.2) command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_VERIF_BNDL_META_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Verify bundle metadata (7.2) command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RebuildBundleMetadataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_RebuildBundleMetadata();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_REBUILD_BNDL_META_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Rebuild bundle metadata command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_REBUILD_BNDL_META_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Rebuild bundle metadata command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ClearVolatileCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_ClearVolatile();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CLR_VOLATILE_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Clear volatile command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CLR_VOLATILE_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Clear volatile command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ReloadSavedDataCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RELOAD_SVD_DATA_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reload saved data (7.2) command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RELOAD_SVD_DATA_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reload saved data (7.2) command not implemented");
    }

    return Status;
}

void BPLib_NC_ResetAllCountersCmd(void)
{
    /* Call to other function(s) */
}

BPLib_Status_t BPLib_NC_ResetCounterCmd(const BPLib_ResetCounterCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_CTR_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset counter command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_CTR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset counter command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetSourceCountersCmd(const BPLib_ResetSourceCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetSourceCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_SRC_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset source counters command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_SRC_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset source counters command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetBundleCountersCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetBundleCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_BNDL_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset bundle counters command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_BNDL_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset bundle counters command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetErrorCountersCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetErrorCounters();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ERR_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset error counters command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ERR_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset error counters command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddApplicationCmd(const BPLib_AddApplicationCmd_Payload_t Payload)
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

BPLib_Status_t BPLib_NC_RemoveApplicationCmd(const BPLib_RemoveApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_APP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SetRegistrationStateCmd(const BPLib_SetRegistrationStateCmd_Payload_t Payload)
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
                            "Set registration state command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SET_REGI_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Set registration state command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StartApplicationCmd(const BPLib_StartApplicationCmd_Payload_t Payload)
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

BPLib_Status_t BPLib_NC_StopApplicationCmd(const BPLib_StopApplicationCmd_Payload_t Payload)
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

BPLib_Status_t BPLib_NC_AddAuthSourcesCmd(const BPLib_AddAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthSourcesCmd(const BPLib_RemoveAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodySourcesCmd(const BPLib_AddAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodySourcesCmd(const BPLib_RemoveAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodiansCmd(const BPLib_AddAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodiansCmd(const BPLib_RemoveAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthReportToEidCmd(const BPLib_AddAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthReportToEidCmd(const BPLib_RemoveAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddLatencyCmd(const BPLib_AddLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveLatencyCmd(const BPLib_RemoveLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_ContactSetupCmd(const BPLib_ContactSetupCmd_Payload_t Payload)
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

    return Status;
}

BPLib_Status_t BPLib_NC_ContactStartCmd(const BPLib_ContactStartCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    identify contact
    Status = BPLib_CLA_ContactStart();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ContactStopCmd(const BPLib_ContactStopCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    identify contact to be stopped

    Status = BPLib_CLA_ContactStop();

    Node Configuration requests Storage clear all queues associated with the CLA. Storage clears the queues    
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ContactTeardownCmd(const BPLib_ContactTeardownCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    If contact has been stopped, Node Configuration deconfigures the CLA
    Node Configuration requests BI/CT/EBP/CLA instances to remove configuration
    */

    return Status;
}

BPLib_Status_t BPLib_NC_AddMibArrayKeyCmd(const BPLib_AddMibArrayKeyCmd_Payload_t Payload)
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

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveMibArrayKeyCmd(const BPLib_RemoveMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SetMibItemCmd(const BPLib_SetMibItemCmd_Payload_t Payload)
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

    return Status;
}

BPLib_Status_t BPLib_NC_AddStorageAllocationCmd(const BPLib_AddStorageAllocationCmd_Payload_t Payload)
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

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveStorageAllocationCmd(const BPLib_RemoveStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Node Configuration notifies Storage to remove an allocation with a given EID pattern and allocation size
    If storage update is successful, NC calls FWP to sync table updates with host
    */

    return Status;
}

BPLib_Status_t BPLib_NC_PerformSelfTestCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibConfigHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibConfigHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibCountersHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibCountersHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendStorageHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendChannelContactStatHkCmd(void)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}
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
#include "bplib_fwp.h"
#include "bplib.h"
#include "bplib_nc_payloads.h"
#include "bplib_eventids.h"

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_NC_Init()
{
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_NC_NoopCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_AddAllApplicationsCmd()
{
    BPLib_Status_t Status;
    // uint8_t AppIdx;
    // ??? AppList[???];
    // BPLib_AddApplicationCmd_Payload_t AddPayload;

    Status = BPLIB_SUCCESS;

    /*
    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        AddPayload.App = AppList[AppIdx]
        BPLib_NC_AddApplicationCmd(AddPayload);
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

BPLib_Status_t BPLib_NC_StartAllApplicationsCmd()
{
    BPLib_Status_t Status;
    // uint8_t AppIdx;
    // ??? AppList[???];

    Status = BPLIB_SUCCESS;

    /*
    for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    {
        BPLib_NC_StartApplicationCmd(AppList[AppIdx]);
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

BPLib_Status_t BPLib_NC_VerifyBundleStorageCmd()
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

BPLib_Status_t BPLib_NC_InitBundleStorageCmd()
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

BPLib_Status_t BPLib_NC_VerifyBundleMetadataCmd()
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

BPLib_Status_t BPLib_NC_RebuildBundleMetadataCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

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

BPLib_Status_t BPLib_NC_ClearVolatileCmd()
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

BPLib_Status_t BPLib_NC_ReloadSavedDataCmd()
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

BPLib_Status_t BPLib_NC_ResetAllCountersCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetAllCounters();

    BPLib_AS_ResetAllCounters()
    {
        BPNode_AppData.NodeMibCountersHkTlm.Payload.AcceptedDirectiveCount = 0;
        BPNode_AppData.NodeMibCountersHkTlm.Payload.RejectedDirectiveCount = 0;
        BPNode_AppData.NodeMibCountersHkTlm.Payload.AduCountDelivered = 0;
        BPNode_AppData.NodeMibCountersHkTlm.Payload.AduCountReceived = 0;
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ALL_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Reset all counters command");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RESET_ALL_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Reset all counters command");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ResetCounterCmd(const BPLib_ResetCounterCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetCounter();
    */

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

BPLib_Status_t BPLib_NC_ResetBundleCountersCmd()
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

BPLib_Status_t BPLib_NC_ResetErrorCountersCmd()
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

    Status = BPLIB_SUCCESS;

    /*
    if (path available for assignment)
    {
        assign path
        pull configuration from channel configuration table
        set configuration of payload interface instance
        set configuration of ADU proxy instance
        Use FWP to establish mapping with client application
        register channel with storage
        Use STOR to create output queue for the channel
    }
    else
    {
        Status = BPLIB_ERROR;
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_APP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
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

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_START_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Start application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_START_APP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Start application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_StopApplicationCmd(const BPLib_StopApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOP_APP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Stop application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_STOP_APP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Stop application command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthSourcesCmd(const BPLib_AddAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthSourcesCmd(const BPLib_RemoveAuthSourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodySourcesCmd(const BPLib_AddAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUST_SRCS_SUCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custody sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodySourcesCmd(const BPLib_RemoveAuthCustodySourcesCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUST_SRCS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custody sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUST_SRCS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized custody sources command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthCustodiansCmd(const BPLib_AddAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUSTODIANS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized custodians command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_CUSTODIANS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized custodians command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthCustodiansCmd(const BPLib_RemoveAuthCustodiansCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUSTODIANS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized custodians command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_CUSTODIANS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized custodians command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddAuthReportToEidCmd(const BPLib_AddAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_RPT_EID_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add authorized report-to eid command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_AUTH_RPT_EID_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add authorized report-to eid command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveAuthReportToEidCmd(const BPLib_RemoveAuthReportToEidCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_RPT_EID_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove authorized report-to eid command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_AUTH_RPT_EID_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove authorized report-to eid command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddLatencyCmd(const BPLib_AddLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_LATENCY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add latency command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_LATENCY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add latency command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveLatencyCmd(const BPLib_RemoveLatencyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_LATENCY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove latency command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_LATENCY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove latency command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactSetupCmd(const BPLib_ContactSetupCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Configure CLA based on configuration
    Status = BPLib_CLA_ConactSetup();
    */

    // or

    /*
    if (path available)
    {
        assign path
        config = <pull configuration from channel configuration table>
        switch (config)
        {
            case A:
                assign bundle interface instance for config A
                configure custody transfer instance for config A
                configure extension block processor for config A
                configure CLA for config A
                break;
            case B:
                assign bundle interface instance for config B
                configure custody transfer instance for config B
                configure extension block processor for config B
                configure CLA for config B
            default:
                Status = BPLIB_ERROR;
        }

        register CLA with storage
    }
    else
    {
        Stauts = BPLIB_ERROR;
    }
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_SETUP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact setup command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_SETUP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact setup command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

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

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_START_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact start command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_START_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact start command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactStopCmd(const BPLib_ContactStopCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    identify contact to be stopped

    if (LTP)
    {
        send cancel request
    }

    Status = BPLib_CLA_ContactStop();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_STOP_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact stop command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_STOP_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact stop command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_ContactTeardownCmd(const BPLib_ContactTeardownCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_CLA_ContactTeardown();
    */

    // or

    /*
    deconfigure the CLA
    deconfigure bundle interface
    deconfigure custody transfer
    deconfigure extension block processor
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_TEARDOWN_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Contact teardown command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_CONTACT_TEARDOWN_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Contact teardown command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddMibArrayKeyCmd(const BPLib_AddMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    BPLib_AS_AddMibArrayKey();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_MIB_ARR_KEY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add mib array key command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_MIB_ARR_KEY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add mib array key command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveMibArrayKeyCmd(const BPLib_RemoveMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_MIB_ARR_KEY_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove mib array key command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_MIB_ARR_KEY_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove mib array key command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SetMibItemCmd(const BPLib_SetMibItemCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SET_MIB_ITEM_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Set mib item command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SET_MIB_ITEM_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Set mib item command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_AddStorageAllocationCmd(const BPLib_AddStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    BPLib_STOR_AddStorageAllocation();
    BPA_TABLEP_TableUpdate();
    */

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_ADD_STOR_ALLOC_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Add storage allocation command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_ADD_STOR_ALLOC_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Add storage allocation command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveStorageAllocationCmd(const BPLib_RemoveStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_RM_STOR_ALLOC_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Remove storage allocation command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_RM_STOR_ALLOC_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Remove storage allocation command not implemented, received %d in payload",
                            Payload.ExampleParameter);
    }

    return Status;
}

BPLib_Status_t BPLib_NC_PerformSelfTestCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_PERFORM_SELF_TEST_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Perform self test (7.2) command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_PERFORM_SELF_TEST_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Perform self test (7.2) command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibConfigHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CFG_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send node mib config hk command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CFG_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send node mib config hk command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibConfigHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CFG_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send per-source mib config hk command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CFG_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send per-source mib config hk command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibCountersHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send node mib counters HK command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_NODE_MIB_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send node mib counters HK command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibCountersHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CTRS_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send per-source mib counter hk command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_SRC_MIB_CTRS_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send per-source mib counter hk command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendStorageHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_STOR_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send storage hk command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_STOR_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send storage hk command not implemented");
    }

    return Status;
}

BPLib_Status_t BPLib_NC_SendChannelContactStatHkCmd()
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    if (Status == BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_SEND_CHAN_CONTACT_STAT_SUCCESS_EID, BPLib_EM_EventType_INFORMATION,
                            "Send channel/contact status hk command not implemented");
    }
    else
    {
        BPLib_EM_SendEvent(BPLIB_SEND_CHAN_CONTACT_STAT_ERR_EID, BPLib_EM_EventType_ERROR,
                            "Send channel/contact status hk command not implemented");
    }

    return Status;
}
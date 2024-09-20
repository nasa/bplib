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

BPLib_Status_t BPLib_NC_AddAllApplicationsCmd(const BPLib_AddAllApplicationsCmd_Payload_t Payload)
{
    BPLib_Status_t Status;
    // uint8_t AppIdx;
    // ??? AppList[???];

    Status = BPLIB_SUCCESS;

    /* for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    { */
        BPLib_NC_AddApplicationCmd(/*AppList[AppIdx]*/);
    // }

    return Status;
}

BPLib_Status_t BPLib_NC_StartAllApplicationsCmd(const BPLib_StartAllApplicationsCmd_Payload_t Payload)
{
    BPLib_Status_t Status;
    // uint8_t AppIdx;
    // ??? AppList[???];

    Status = BPLIB_SUCCESS;

    /* for (AppIdx = 0; AppIdx < <num apps>; AppIdx++)
    { */
        BPLib_NC_StartApplicationCmd(/*AppList[AppIdx]*/);
    // }

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleStorageCmd(const BPLib_VerifyBundleStorageCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_InitBundleStorageCmd(const BPLib_InitBundleStorageCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_VerifyBundleMetadataCmd(const BPLib_VerifyBundleMetadataCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_RebuildBundleMetadataCmd(const BPLib_RebuildBundleMetadataCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_ClearVolatileCmd(const BPLib_ClearVolatileCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_STOR_ClearVolatile();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ReloadSavedDataCmd(const BPLib_ReloadSavedDataCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_ResetAllCountersCmd(const BPLib_ResetAllCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetAllCounters();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ResetCounterCmd(const BPLib_ResetCounterCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetCounter();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ResetSourceCountersCmd(const BPLib_ResetSourceCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetSourceCounters();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ResetBundleCountersCmd(const BPLib_ResetBundleCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetBundleCounters();
    */

    return Status;
}

BPLib_Status_t BPLib_NC_ResetErrorCountersCmd(const BPLib_ResetErrorCountersCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    Status = BPLib_AS_ResetErrorCounters();
    */

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

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveApplicationCmd(const BPLib_RemoveApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SetRegistrationStateCmd(const BPLib_SetRegistrationStateCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_StartApplicationCmd(const BPLib_StartApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_StopApplicationCmd(const BPLib_StopApplicationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

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

    if (LTP)
    {
        send cancel request
    }

    Status = BPLib_CLA_ContactStop();
    */

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

    return Status;
}

BPLib_Status_t BPLib_NC_AddMibArrayKeyCmd(const BPLib_AddMibArrayKeyCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    /*
    BPLib_AS_AddMibArrayKey();
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

    return Status;
}

BPLib_Status_t BPLib_NC_RemoveStorageAllocationCmd(const BPLib_RemoveStorageAllocationCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_PerformSelfTestCmd(const BPLib_PerformSelfTestCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibConfigHkCmd(const BPLib_SendNodeMibConfigHkCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibConfigHkCmd(const BPLib_SendSourceMibConfigHkCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendNodeMibCountersHkCmd(const BPLib_SendNodeMibCountersHkCmd_Payload_t Payload)
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendSourceMibCountersHkCmd(const BPLib_SendSourceMibCountersHkCmd_Payload_t Payload);
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendStorageHkCmd(const BPLib_SendStorageHkCmd_Payload_t Payload);
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}

BPLib_Status_t BPLib_NC_SendChannelContactStatHkCmd(const BPLib_SendChannelContactStatHkCmd_Payload_t Payload);
{
    BPLib_Status_t Status;

    Status = BPLIB_SUCCESS;

    return Status;
}
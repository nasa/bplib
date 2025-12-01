/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */

/* ======== */
/* Includes */
/* ======== */

#include "bplib_nc.h"
#include "bplib_fwp.h"
#include "bplib_crc.h"
#include "bplib_nc_rwlock.h"
#include "bplib_nc_internal.h"

/* ======= */
/* Globals */
/* ======= */
static BPLib_NC_RWLock_t BPLib_NC_CfgLock;
BPLib_NC_ConfigPtrs_t    BPLib_NC_ConfigPtrs;

/* MIB configuration validation functions */
static MibConfigValidateFunc_t MibConfigValidate[] = {
    BPLib_NC_ValidParamBundleSizeNoFragment,
    BPLib_NC_ValidParamSetMaxSequenceNum,
    BPLib_NC_ValidParamMaxPayloadLength,
    BPLib_NC_ValidParamMaxBundleLength,
    BPLib_NC_ValidParamSetNodeDtnTime,
    BPLib_NC_ValidParamSetBehaviorEventReporting,
    BPLib_NC_ValidParamSetMaxLifetime
};

/* ==================== */
/* Prototypes           */
/* ==================== */
static BPLib_Status_t BPLib_NC_ConfigUpdateUnlocked(void);

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_NC_Init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    BPLib_Status_t Status;

    memset((void*) &BPLib_NC_SourceMibConfigPayload,     0, sizeof(BPLib_NC_SourceMibConfigPayload));
    memset((void*) &BPLib_NC_NodeMibConfigPayload,       0, sizeof(BPLib_NC_NodeMibConfigPayload));
    memset((void*) &BPLib_NC_ChannelContactStatsPayload, 0, sizeof(BPLib_NC_ChannelContactStatsPayload));

    /* Initialize the configuration lock */
    Status = BPLib_NC_RWLock_Init(&BPLib_NC_CfgLock);
    if (Status != BPLIB_SUCCESS)
    {
        return Status;
    }

    /* Capture configuration pointers in the global configuration struct */
    if (ConfigPtrs                     == NULL ||
        ConfigPtrs->ChanConfigPtr      == NULL ||
        ConfigPtrs->ContactsConfigPtr  == NULL ||
        ConfigPtrs->CrsConfigPtr       == NULL ||
        ConfigPtrs->CustodianConfigPtr == NULL ||
        ConfigPtrs->CustodyConfigPtr   == NULL ||
        ConfigPtrs->MibPnConfigPtr     == NULL ||
        ConfigPtrs->MibPsConfigPtr     == NULL ||
        ConfigPtrs->ReportConfigPtr    == NULL ||
        ConfigPtrs->AuthConfigPtr      == NULL ||
        ConfigPtrs->LatConfigPtr       == NULL ||
        ConfigPtrs->StorConfigPtr      == NULL)
    {
        Status = BPLIB_NC_INIT_CONFIG_PTRS_ERROR;
    }
    else
    {
        /* Initialize configurations */
        BPLib_NC_ConfigPtrs.ChanConfigPtr      = ConfigPtrs->ChanConfigPtr;
        BPLib_NC_ConfigPtrs.ContactsConfigPtr  = ConfigPtrs->ContactsConfigPtr;
        BPLib_NC_ConfigPtrs.CrsConfigPtr       = ConfigPtrs->CrsConfigPtr;
        BPLib_NC_ConfigPtrs.CustodianConfigPtr = ConfigPtrs->CustodianConfigPtr;
        BPLib_NC_ConfigPtrs.CustodyConfigPtr   = ConfigPtrs->CustodyConfigPtr;
        BPLib_NC_ConfigPtrs.MibPnConfigPtr     = ConfigPtrs->MibPnConfigPtr;
        BPLib_NC_ConfigPtrs.MibPsConfigPtr     = ConfigPtrs->MibPsConfigPtr;
        BPLib_NC_ConfigPtrs.ReportConfigPtr    = ConfigPtrs->ReportConfigPtr;
        BPLib_NC_ConfigPtrs.AuthConfigPtr      = ConfigPtrs->AuthConfigPtr;
        BPLib_NC_ConfigPtrs.LatConfigPtr       = ConfigPtrs->LatConfigPtr;
        BPLib_NC_ConfigPtrs.StorConfigPtr      = ConfigPtrs->StorConfigPtr;

        /* Set the instance EID */
        BPLib_EID_CopyEids(&BPLIB_EID_INSTANCE, BPLib_NC_ConfigPtrs.MibPnConfigPtr->InstanceEID);

        /* Set telemetry values */
        memcpy(&(BPLib_NC_NodeMibConfigPayload.Values), BPLib_NC_ConfigPtrs.MibPnConfigPtr, 
                                                            sizeof(BPLib_NC_MibPerNodeConfig_t));

        /* Initialize contact/channel status telemetry with table values */
        BPLib_NC_UpdateContactHkTlm();
        BPLib_NC_UpdateChannelHkTlm();

        /* Initialize CRC tables */
        BPLib_CRC_Init();

        /* Initialize AS */
        Status = BPLib_AS_Init();
    }

    return Status;
}

/*******************************************************************************
* Locked Wrapper Functions
*/
void BPLib_NC_ReaderLock()
{
    BPLib_NC_RWLock_RLock(&BPLib_NC_CfgLock);
}

void BPLib_NC_ReaderUnlock()
{
    BPLib_NC_RWLock_RUnlock(&BPLib_NC_CfgLock);
}

BPLib_Status_t BPLib_NC_ConfigUpdate()
{
    BPLib_Status_t Status;

    BPLib_NC_RWLock_WLock(&BPLib_NC_CfgLock);
    Status = BPLib_NC_ConfigUpdateUnlocked();
    BPLib_NC_RWLock_WUnlock(&BPLib_NC_CfgLock);

    return Status;
}

/* Set a node MIB configuration to a new value */
BPLib_Status_t BPLib_NC_SetMibNodeConfig(uint32_t MibItem, uint32_t Value)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    uint32_t OldValue;

    BPLib_NC_RWLock_WLock(&BPLib_NC_CfgLock);

    /* Validate MIB item index */
    if (MibItem < BPLIB_NC_NODE_MIB_CONFIG_NUM)
    {
        OldValue = BPLib_NC_ConfigPtrs.MibPnConfigPtr->Configs[MibItem];

        /* Update table with new value and try to validate new state */
        BPLib_NC_ConfigPtrs.MibPnConfigPtr->Configs[MibItem] = Value;
        if (MibConfigValidate[MibItem](BPLib_NC_ConfigPtrs.MibPnConfigPtr))
        {
            Status = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_MIB_PER_NODE, 
                                                (void **) &(BPLib_NC_ConfigPtrs.MibPnConfigPtr));
            if (Status == BPLIB_SUCCESS)
            {
                /* 
                ** Table update was successful, update the corresponding telemetry value
                ** as well
                */
                BPLib_NC_NodeMibConfigPayload.Values.Configs[MibItem] = Value;
            }
            else
            {
                /*
                ** Table update failed, revert the table value to its original value
                */
                BPLib_NC_ConfigPtrs.MibPnConfigPtr->Configs[MibItem] = OldValue;
            }
        }
        else
        {
            /* Validation failed, revert the table value to its original value */
            Status = BPLIB_NC_INVALID_MIB_VALUE;
            BPLib_NC_ConfigPtrs.MibPnConfigPtr->Configs[MibItem] = OldValue;
        }
    }
    else
    {
        Status = BPLIB_NC_INVALID_MIB_ITEM_INDEX;
    }

    BPLib_NC_RWLock_WUnlock(&BPLib_NC_CfgLock);

    return Status;
}

/* Set a source MIB configuration to a new value */
BPLib_Status_t BPLib_NC_SetMibSourceConfig(const BPLib_EID_Pattern_t *EidPattern, 
                                                        uint32_t MibItem, uint32_t Value)
{
    // TODO implement
    printf("Source configs for set-mib-item are unimplemented\n");

    return BPLIB_UNIMPLEMENTED;
}                                                        

/* Validate MIB Config PN configuration data */
BPLib_Status_t BPLib_NC_MIBConfigPNTblValidateFunc(void* TblData)
{
    BPLib_NC_MibPerNodeConfig_t *TblDataPtr = (BPLib_NC_MibPerNodeConfig_t *) TblData; 

    /* Validate the EID doesn't use the DTN scheme and is a valid EID */
    if (TblDataPtr->InstanceEID.Scheme == BPLIB_EID_SCHEME_DTN ||
        !BPLib_EID_IsValid(&(TblDataPtr->InstanceEID)))
    {
        return BPLIB_INVALID_CONFIG_ERR;
    }

    /* Validate each MIB configuration item */
    if (!BPLib_NC_ValidParamBundleSizeNoFragment(TblDataPtr) ||
        !BPLib_NC_ValidParamSetMaxSequenceNum(TblDataPtr) ||
        !BPLib_NC_ValidParamMaxPayloadLength(TblDataPtr) ||
        !BPLib_NC_ValidParamMaxBundleLength(TblDataPtr) ||
        !BPLib_NC_ValidParamSetNodeDtnTime(TblDataPtr) ||
        !BPLib_NC_ValidParamSetBehaviorEventReporting(TblDataPtr) ||
        !BPLib_NC_ValidParamSetMaxLifetime(TblDataPtr))
    {
        return BPLIB_INVALID_CONFIG_ERR;
    }

    return BPLIB_SUCCESS;
}

/* Validate MIB Config PS configuration data */
BPLib_Status_t BPLib_NC_MIBConfigPSTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;
    BPLib_NC_MIBConfigPSTable_t *TblDataPtr = (BPLib_NC_MIBConfigPSTable_t *)TblData;

    /* Validate data values are within allowed range */
    if (TblDataPtr[0].Sources->Configs[0] <= 0)
    {
        /* element is out of range, return an appropriate error code */
        ReturnCode = BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

void BPLib_NC_SetAppState(uint8_t ChanId, BPLib_NC_ApplicationState_t State)
{
    BPLib_NC_ChannelContactStatsPayload.ChannelStatus[ChanId].State = State;
}

BPLib_NC_ApplicationState_t BPLib_NC_GetAppState(uint8_t ChanId)
{
    return BPLib_NC_ChannelContactStatsPayload.ChannelStatus[ChanId].State;
}

static BPLib_Status_t BPLib_NC_ConfigUpdateUnlocked(void)
{
    BPLib_Status_t FWP_UpdateStatus;
    // BPLib_Status_t ModuleStatus;

    /* Update Channel Configurations configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_CHANNEL,
                                                                        (void**) &BPLib_NC_ConfigPtrs.ChanConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_NC_ChannelConfigTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Channel configuration");
        }
        else
        */
        {
            /* Update channel telemetry with new table values */
            BPLib_NC_UpdateChannelHkTlm();

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Channel configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Contacts configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_CONTACTS,
                                                                        (void**) &BPLib_NC_ConfigPtrs.ContactsConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_NC_ContactsTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Contacts configuration");
        }
        else
        */
        {
            /* Update contact telemetry with new table values */
            BPLib_NC_UpdateContactHkTlm();
            
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Contacts configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Compressed Reporting configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_COMPRESSED_REPORTING,
                                                                        (void**) &BPLib_NC_ConfigPtrs.CrsConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_ARP_CompressedReportingTblUpdate();
        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Compressed Reporting configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Compressed Reporting configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Custodian Authorization configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_CUSTODIAN_AUTH_POLICY,
                                                                        (void**) &BPLib_NC_ConfigPtrs.CustodianConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_PD_CustodianAuthorizationTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Custodian Authorization Policy configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Custodian Authorization Policy configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Custody Authorization configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_CUSTODY_AUTH_POLICY,
                                                                        (void**) &BPLib_NC_ConfigPtrs.CustodyConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_PD_CustodyAuthorizationTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Custody Authorization Policy configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Custody Authorization Policy configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update MIB Configuration per Node configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_MIB_PER_NODE,
                                                                        (void**) &BPLib_NC_ConfigPtrs.MibPnConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /* Update the instance EID */
        BPLib_EID_CopyEids(&BPLIB_EID_INSTANCE, BPLib_NC_ConfigPtrs.MibPnConfigPtr->InstanceEID);

        /* Update telemetry values */
        memcpy(&(BPLib_NC_NodeMibConfigPayload.Values), BPLib_NC_ConfigPtrs.MibPnConfigPtr, 
                                                            sizeof(BPLib_NC_MibPerNodeConfig_t));

        BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                            BPLib_EM_EventType_INFORMATION,
                            "Updated MIB Configuration per Node configuration");
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update MIB Configuration per Source configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_MIB_PER_SRC,
                                                                        (void**) &BPLib_NC_ConfigPtrs.MibPsConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_NC_SourceConfigTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update MIB Configuration per Source configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated MIB Configuration per Source configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Report-to-EID Authorization Policy configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_REPORT_TO_EID_AUTH_POLICY,
                                                                        (void**) &BPLib_NC_ConfigPtrs.ReportConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_PD_ReportTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Report-to-EID Authorization Policy configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Report-to-EID Authorization Policy configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Source Authorization Policy configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_SRC_AUTH_POLICY,
                                                                        (void**) &BPLib_NC_ConfigPtrs.AuthConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_PD_SrcAuthTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Source Authorization Policy configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Source Authorization Policy configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Source Latency Policy configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_SRC_LATENCY_POLICY,
                                                                        (void**) &BPLib_NC_ConfigPtrs.LatConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_PD_SrcLatencyTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Source Latency Policy configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Source Latency Policy configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    /* Update Storage configuration with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(BPLIB_STORAGE,
                                                                        (void**) &BPLib_NC_ConfigPtrs.StorConfigPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        ModuleStatus = BPLib_STOR_StorageTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Storage configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated Storage configuration");
        }
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

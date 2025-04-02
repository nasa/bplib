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
#include "bplib_crc.h"

/* ======= */
/* Globals */
/* ======= */

BPLib_SourceMibConfigHkTlm_Payload_t    BPLib_NC_SourceMibConfigPayload;
BPLib_NodeMibConfigHkTlm_Payload_t      BPLib_NC_NodeMibConfigPayload;
BPLib_ChannelContactStatHkTlm_Payload_t BPLib_NC_ChannelContactStatsPayload; /** \brief Global channel contact statistics payload */
BPLib_NC_ConfigPtrs_t                   BPLib_NC_ConfigPtrs;

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_NC_Init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    BPLib_Status_t Status;

    memset((void*) &BPLib_NC_SourceMibConfigPayload,     0, sizeof(BPLib_NC_SourceMibConfigPayload));
    memset((void*) &BPLib_NC_NodeMibConfigPayload,       0, sizeof(BPLib_NC_NodeMibConfigPayload));
    memset((void*) &BPLib_NC_ChannelContactStatsPayload, 0, sizeof(BPLib_NC_ChannelContactStatsPayload));

    /* Set bundle protocol version */
    BPLib_NC_NodeMibConfigPayload.Version = BPLIB_BUNDLE_PROTOCOL_VERSION;

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

        /* Initialize CRC tables */
        BPLib_CRC_Init();

        /* Initialize AS */
        Status = BPLib_AS_Init();
    }

    return Status;
}

/* Validate MIB Config PN configuration data */
BPLib_Status_t BPLib_NC_MIBConfigPNTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;
    BPLib_NC_MIBConfigPNTable_t *TblDataPtr = (BPLib_NC_MIBConfigPNTable_t *)TblData;

    /* Validate data values are within allowed range */
    if (TblDataPtr->BundleAgentNum <= 0)
    {
        /* element is out of range, return an appropriate error code */
        ReturnCode = BPLIB_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

/* Validate MIB Config PS configuration data */
BPLib_Status_t BPLib_NC_MIBConfigPSTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;
    BPLib_NC_MIBConfigPSTable_t *TblDataPtr = (BPLib_NC_MIBConfigPSTable_t *)TblData;

    /* Validate data values are within allowed range */
    if (TblDataPtr[0].MIB_PS_Set->ParamSetMaxLifetime <= 0)
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

BPLib_Status_t BPLib_NC_ConfigUpdate(void)
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
        /*
        ModuleStatus = BPLib_NC_NodeConfigTblUpdate();

        if (ModuleStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_ERROR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update MIB Configuration per Node configuration");
        }
        else
        */
        {
            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_INF_EID,
                                BPLib_EM_EventType_INFORMATION,
                                "Updated MIB Configuration per Node configuration");
        }
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

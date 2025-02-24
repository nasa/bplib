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

/* ======= */
/* Globals */
/* ======= */

BPLib_SourceMibConfigHkTlm_Payload_t    BPLib_NC_SourceMibConfigPayload;
BPLib_NodeMibConfigHkTlm_Payload_t      BPLib_NC_NodeMibConfigPayload;
BPLib_ChannelContactStatHkTlm_Payload_t BPLib_NC_ChannelContactStatsPayload; /** \brief Global channel contact statistics payload */

/* ==================== */
/* Function Definitions */
/* ==================== */

BPLib_Status_t BPLib_NC_Init(BPLib_FWP_ConfigPtrs_t* ConfigPtrs)
{
    BPLib_Status_t Status;

    memset((void*) &BPLib_NC_SourceMibConfigPayload,     0, sizeof(BPLib_NC_SourceMibConfigPayload));
    memset((void*) &BPLib_NC_NodeMibConfigPayload,       0, sizeof(BPLib_NC_NodeMibConfigPayload));
    memset((void*) &BPLib_NC_ChannelContactStatsPayload, 0, sizeof(BPLib_NC_ChannelContactStatsPayload));

    /* Capture configuration pointers in the global configuration struct */
    if (ConfigPtrs                  == NULL ||
        ConfigPtrs->ChanTblPtr      == NULL ||
        ConfigPtrs->ContactsTblPtr  == NULL ||
        ConfigPtrs->CrsTblPtr       == NULL ||
        ConfigPtrs->CustodianTblPtr == NULL ||
        ConfigPtrs->CustodyTblPtr   == NULL ||
        ConfigPtrs->MibPnTblPtr     == NULL ||
        ConfigPtrs->MibPsTblPtr     == NULL ||
        ConfigPtrs->ReportTblPtr    == NULL ||
        ConfigPtrs->AuthTblPtr      == NULL ||
        ConfigPtrs->LatTblPtr       == NULL ||
        ConfigPtrs->StorTblPtr      == NULL)
    {
        Status = BPLIB_NC_INIT_CONFIG_PTRS_ERROR;
    }
    else
    {
        /* Initialize configurations (AKA tables) */
        BPLib_FWP_ConfigPtrs.ChanTblPtr      = ConfigPtrs->ChanTblPtr;
        BPLib_FWP_ConfigPtrs.ContactsTblPtr  = ConfigPtrs->ContactsTblPtr;
        BPLib_FWP_ConfigPtrs.CrsTblPtr       = ConfigPtrs->CrsTblPtr;
        BPLib_FWP_ConfigPtrs.CustodianTblPtr = ConfigPtrs->CustodianTblPtr;
        BPLib_FWP_ConfigPtrs.CustodyTblPtr   = ConfigPtrs->CustodyTblPtr;
        BPLib_FWP_ConfigPtrs.MibPnTblPtr     = ConfigPtrs->MibPnTblPtr;
        BPLib_FWP_ConfigPtrs.MibPsTblPtr     = ConfigPtrs->MibPsTblPtr;
        BPLib_FWP_ConfigPtrs.ReportTblPtr    = ConfigPtrs->ReportTblPtr;
        BPLib_FWP_ConfigPtrs.AuthTblPtr      = ConfigPtrs->AuthTblPtr;
        BPLib_FWP_ConfigPtrs.LatTblPtr       = ConfigPtrs->LatTblPtr;
        BPLib_FWP_ConfigPtrs.StorTblPtr      = ConfigPtrs->StorTblPtr;

        /* Initialize AS */
        Status = BPLib_AS_Init();
    }

    return Status;
}

/* Validate MIB Config PN table data */
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

/* Validate MIB Config PS table data */
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

BPLib_Status_t BPLib_NC_TableWakeUp(void)
{
    BPLib_Status_t FWP_UpdateStatus;
    BPLib_Status_t BPLibUpdateStatus;
    BPLib_Status_t Status;

    /* Default to reporting successful table wake ups */
    Status = BPLIB_SUCCESS;

    /* Update Channel Configurations table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(CHANNEL_CONFIG,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.ChanTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_NC_ChannelConfigTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Channel Configuration table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Contacts table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(CONTACTS,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.ContactsTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_NC_ContactsTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Contacts table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Compressed Reporting table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(COMPRESSED_REPORTING,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.CrsTblPtr);

    
    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_ARP_CompressedReportingTblUpdate();
        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                            BPLib_EM_EventType_ERROR,
                                            "Failed to update Compressed Reporting table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }
    
    /* Update Custodian Authorization Table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(CUSTODIAN_AUTH_POLICY,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.CustodianTblPtr);
                                                                    
    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_PD_CustodianAuthorizationTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Custodian Authorization Policy table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Custody Authorization table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(CUSTODY_AUTH_POLICY,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.CustodyTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_PD_CustodyAuthorizationTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Custody Authorization Policy table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update MIB Configuration per Node table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(MIB_CONFIG_PER_NODE,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.MibPnTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_NC_NodeConfigTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update MIB Configuration per Node table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update MIB Configuration per Source table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(MIB_CONFIG_PER_SRC,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.MibPsTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_NC_SourceConfigTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update MIB Configuration per Source table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Report-to-EID Authorization Policy table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(REPORT_TO_EID_AUTH_POLICY,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.ReportTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_PD_ReportTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Report-to-EID Authorization Policy table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Source Authorization Policy table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(SRC_AUTH_POLICY,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.AuthTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_PD_SrcAuthTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Source Authorization Policy table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Source Latency Policy table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(SRC_LATENCY_POLICY,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.LatTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_PD_SrcLatencyTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Source Latency Policy table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    /* Update Storage table with TABLEP */
    FWP_UpdateStatus = BPLib_FWP_ProxyCallbacks.BPA_TABLEP_TableUpdate(STORAGE,
                                                                        (void**) &BPLib_FWP_ConfigPtrs.StorTblPtr);

    if (FWP_UpdateStatus == BPLIB_TBL_UPDATED)
    {
        /*
        BPLibUpdateStatus = BPLib_STOR_StorageTblUpdate();

        if (BPLibUpdateStatus != BPLIB_SUCCESS)
        {
            Status = BPLIB_NC_TBL_WAKEUP_ERR;

            BPLib_EM_SendEvent(BPLIB_NC_TBL_UPDATE_ERR_EID,
                                BPLib_EM_EventType_ERROR,
                                "Failed to update Storage table");
        }
        */
    }
    else if (FWP_UpdateStatus != BPLIB_SUCCESS)
    {
        Status = BPLIB_NC_FWP_TBL_WAKEUP_ERR;
    }

    return Status;
}
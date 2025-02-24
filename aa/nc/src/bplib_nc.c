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

BPLib_Status_t BPLib_NC_Init(void)
{
    BPLib_Status_t Status;

    Status = BPLib_AS_Init();

    memset((void*) &BPLib_NC_SourceMibConfigPayload,     0, sizeof(BPLib_NC_SourceMibConfigPayload));
    memset((void*) &BPLib_NC_NodeMibConfigPayload,       0, sizeof(BPLib_NC_NodeMibConfigPayload));
    memset((void*) &BPLib_NC_ChannelContactStatsPayload, 0, sizeof(BPLib_NC_ChannelContactStatsPayload));

    BPLib_NC_NodeMibConfigPayload.Version = BPLIB_BUNDLE_PROTOCOL_VERSION;

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
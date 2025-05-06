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

#include "bplib_nc_internal.h"
#include "bplib_nc.h"
#include "bplib_eid.h"


/*
** Global Data
*/

BPLib_SourceMibConfigHkTlm_Payload_t    BPLib_NC_SourceMibConfigPayload;
BPLib_NodeMibConfigHkTlm_Payload_t      BPLib_NC_NodeMibConfigPayload;
BPLib_ChannelContactStatHkTlm_Payload_t BPLib_NC_ChannelContactStatsPayload;


/*
** Function Definitions
*/

/* Update the contact state telemetry with latest table values */
void BPLib_NC_UpdateContactHkTlm(void)
{
    uint32_t ContId;
    uint32_t EidIdx;

    for (ContId = 0; ContId < BPLIB_MAX_NUM_CONTACTS; ContId++)
    {
        for (EidIdx = 0; EidIdx < BPLIB_MAX_CONTACT_DEST_EIDS; EidIdx++)
        {
            BPLib_EID_CopyEidPatterns(&(BPLib_NC_ChannelContactStatsPayload.ContactStatus[ContId].DestEIDs[EidIdx]),
                BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[ContId].DestEIDs[EidIdx]);
        }
    }
}

/* Update the channel state telemetry with latest table values */
void BPLib_NC_UpdateChannelHkTlm(void)
{
    uint32_t ChanId;

    for (ChanId = 0; ChanId < BPLIB_MAX_NUM_CHANNELS; ChanId++)
    {
        BPLib_NC_ChannelContactStatsPayload.ChannelStatus[ChanId].LocalServiceNum = 
            BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[ChanId].LocalServiceNumber;
    }
}

bool BPLib_NC_ValidParamBundleSizeNoFragment(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    return TblDataPtr->Configs[PARAM_BUNDLE_SIZE_NO_FRAGMENT] <= BPLIB_MAX_BUNDLE_LEN;
}

bool BPLib_NC_ValidParamSetMaxSequenceNum(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    /* All values accepted */
    return true;
}

bool BPLib_NC_ValidParamMaxPayloadLength(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    return (TblDataPtr->Configs[PARAM_SET_MAX_PAYLOAD_LENGTH] <= BPLIB_MAX_PAYLOAD_SIZE) &&
           (TblDataPtr->Configs[PARAM_SET_MAX_PAYLOAD_LENGTH] < TblDataPtr->Configs[PARAM_SET_MAX_BUNDLE_LENGTH]);
}

bool BPLib_NC_ValidParamMaxBundleLength(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    return (TblDataPtr->Configs[PARAM_SET_MAX_BUNDLE_LENGTH] <= BPLIB_MAX_BUNDLE_LEN) &&
           (TblDataPtr->Configs[PARAM_SET_MAX_PAYLOAD_LENGTH] < TblDataPtr->Configs[PARAM_SET_MAX_BUNDLE_LENGTH]);
}

bool BPLib_NC_ValidParamSetNodeDtnTime(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    /* All values accepted */
    return true;
}

bool BPLib_NC_ValidParamSetBehaviorEventReporting(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    /* All values accepted */
    return true;
}

bool BPLib_NC_ValidParamSetMaxLifetime(BPLib_NC_MibPerNodeConfig_t *TblDataPtr)
{
    return TblDataPtr->Configs[PARAM_SET_MAX_LIFETIME] <= BPLIB_MAX_LIFETIME_ALLOWED;
}

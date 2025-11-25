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

#include "bpcat_nc.h"
#include "bplib.h"

static BPLib_PI_ChannelTable_t     ChanTbl;
static BPLib_CLA_ContactsTable_t ContactsTbl =
{
    .ContactSet = {
        {
            .DestEIDs               = {{
                                        .Scheme       = BPLIB_EID_SCHEME_IPN,
                                        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                                        .MaxAllocator = 0,
                                        .MinAllocator = 0,
                                        .MaxNode      = 200,
                                        .MinNode      = 200,
                                        .MaxService   = 64,
                                        .MinService   = 64},},
            .CLAType                = 1, /*CLA Type, uint32 */
            .ClaInAddr              = "0.0.0.0",
            .ClaOutAddr             = "127.0.0.1", /* CL ip address */
            .ClaInPort              = 4501, /* Port Number, int32 */
            .ClaOutPort             = 4551,
            .RetransmitTimeout      = 102, /*bundle reforwarding timeout in seconds, uint32*/
            .CSTimeTrigger          = 103, /*Custody Signal time trigger in seconds, uint32*/
            .CSSizeTrigger          = 10 /*Custody signal size trigger in bytes, size_t*/
        },
    }
};
static BPLib_ARP_CRSTable_t        CrsTbl;
static BPLib_PDB_CustodianTable_t  CustodianTbl;
static BPLib_PDB_CustodyTable_t    CustodyTbl;
static BPLib_NC_MibPerNodeConfig_t MibPnTbl = {
    .InstanceEID = {
        .Scheme       = BPLIB_LOCAL_EID_SCHEME,
        .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
        .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
        .Node         = BPLIB_LOCAL_EID_NODE_NUM,
        .Service      = BPLIB_LOCAL_EID_SERVICE_NUM
    },

    .Configs = {
        /* PARAM_BUNDLE_SIZE_NO_FRAGMENT      */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_MAX_SEQUENCE NUM         */ 100000,   
        /* PARAM_SET_MAX_PAYLOAD_LENGTH       */ BPLIB_MAX_PAYLOAD_SIZE,
        /* PARAM_SET_MAX_BUNDLE_LENGTH        */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_NODE_DTN_TIME            */ 0,
        /* PARAM_SET_BEHAVIOR_EVENT_REPORTING */ 10,
        /* PARAM_SET_MAX_LIFETIME             */ BPLIB_MAX_LIFETIME_ALLOWED
    }
};

static BPLib_NC_MIBConfigPSTable_t MibPsTbl;
static BPLib_PDB_ReportToTable_t   ReportTbl;
static BPLib_PDB_SrcAuthTable_t    AuthTbl;
static BPLib_PDB_SrcLatencyTable_t LatencyTbl;
static BPLib_STOR_StorageTable_t   StorTbl;

BPCat_Status_t BPCat_NC_Init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    BPLib_Status_t NCStatus;

    if (ConfigPtrs == NULL)
    {
        return BPCAT_NULL_PTR_ERR;
    }

    ConfigPtrs->ChanConfigPtr      = &ChanTbl;
    ConfigPtrs->ContactsConfigPtr  = &ContactsTbl;
    ConfigPtrs->CrsConfigPtr       = &CrsTbl;
    ConfigPtrs->CustodianConfigPtr = &CustodianTbl;
    ConfigPtrs->CustodyConfigPtr   = &CustodyTbl;
    ConfigPtrs->MibPnConfigPtr     = &MibPnTbl;
    ConfigPtrs->MibPsConfigPtr     = &MibPsTbl;
    ConfigPtrs->ReportConfigPtr    = &ReportTbl;
    ConfigPtrs->AuthConfigPtr      = &AuthTbl;
    ConfigPtrs->LatConfigPtr       = &LatencyTbl;
    ConfigPtrs->StorConfigPtr      = &StorTbl;    

    NCStatus = BPLib_NC_Init(ConfigPtrs);
    if (NCStatus != BPLIB_SUCCESS)
    {
        fprintf(stderr, "Failed to Initialize Node Config, RC=%d\n", NCStatus);
        
        return BPCAT_NC_INIT_ERR;
    }

    return BPCAT_SUCCESS;
}

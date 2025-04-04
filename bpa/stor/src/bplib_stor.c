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

/*
** Include
*/

#include "bplib_stor.h"
#include "bplib_qm.h"
#include "bplib_em.h"
#include "bplib_eventids.h"
#include "bplib_fwp.h"
#include "bplib_nc.h"
#include "bplib_eid.h"
#include "bplib_as.h"
#include "bplib_stor_cache.h"

#include <stdio.h>

/* 
** Globals
*/

BPLib_StorageHkTlm_Payload_t BPLib_STOR_StoragePayload;

/*
** Function Definitions
*/

int BPLib_STOR_Init(void) {
    return BPLIB_SUCCESS;
}

/* Validate Storage table data */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    return ReturnCode;
}

BPLib_Status_t BPLib_STOR_ScanCache(BPLib_Instance_t* Inst, uint32_t MaxBundlesToScan)
{
    BPLib_Status_t      Status           = BPLIB_SUCCESS;
    uint32_t            BundlesScanned   = 0;
    uint16_t            i, j;
    size_t NumEgressed;
    BPLib_EID_Pattern_t LocalEIDPattern;

    if (Inst == NULL)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_INVALID_ARG_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_STOR_ScanCache called with null instance pointer.");
        return BPLIB_NULL_PTR_ERROR;
    }

    /*
    ** Get all currently available channels/contacts to avoid repeatedly checking the
    ** destination EIDs of unavailable channels/contacts
    */
    for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
    {
        if (BPLib_NC_GetAppState(i) == BPLIB_NC_APP_STATE_STARTED)
        {
            LocalEIDPattern.MaxNode = BPLIB_EID_INSTANCE.Node;
            LocalEIDPattern.MinNode = BPLIB_EID_INSTANCE.Node;
            LocalEIDPattern.MaxService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[i].LocalServiceNumber;
            LocalEIDPattern.MinService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[i].LocalServiceNumber;
            BPLib_STOR_EgressForDestEID(Inst, i, true,
                &LocalEIDPattern,
                BPLIB_STOR_LOADBATCHSIZE,
                &NumEgressed);
            BundlesScanned += NumEgressed;
        }
    }

    /* Egress For Contacts */
    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        for (j = 0; j < BPLIB_MAX_CONTACT_DEST_EIDS; j++)
        {
            /* Code not available: BPLib_NC_GetContactState(i) to check if contact active */
            BPLib_STOR_EgressForDestEID(Inst, i, false,
                &BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[i].DestEIDs[j],
                BPLIB_STOR_LOADBATCHSIZE,
                &NumEgressed);
            BundlesScanned += NumEgressed;
        }
    }

    return Status;
}

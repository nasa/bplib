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

/* Initial, simplified (reduced feature set, using fifo) Bundle Cache scan */
BPLib_Status_t BPLib_STOR_ScanCache(BPLib_Instance_t* Inst, uint32_t MaxBundlesToScan)
{
    BPLib_Status_t      Status           = BPLIB_SUCCESS;
    BPLib_QM_JobState_t NextJobState     = NO_NEXT_STATE;
    uint32_t            BundlesScheduled = 0;
    uint16_t            NumChans         = 0;
    uint16_t            NumConts         = 0;
    uint16_t            EgressId         = BPLIB_UNKNOWN_ROUTE_ID;
    BPLib_Bundle_t     *QueuedBundle;
    uint16_t            AvailChans[BPLIB_MAX_NUM_CHANNELS];
    uint16_t            AvailConts[BPLIB_MAX_NUM_CONTACTS];
    uint16_t            i, j;
    
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
            AvailChans[NumChans] = i;
            NumChans++;
        }
    }

    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        /*
        if (Contact state is started) TODO fix me once contact directives are implemented
        {
        */
            AvailConts[NumConts] = i;
            NumConts++;

        /*
        }
        */
    }    

    /* Pull bundles from cache queue and process them */
    while (BundlesScheduled < MaxBundlesToScan && Status == BPLIB_SUCCESS)
    {
        QueuedBundle = NULL;
        if (BPLib_QM_WaitQueueTryPull(&(Inst->BundleCacheList), &QueuedBundle, 1))
        {
            if (QueuedBundle != NULL)
            {
                /* We may want to completely remove this event in the future, but for now it'll probably be helpful */
                BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_GOT_GOOD_BUNDLE_INF_EID, BPLib_EM_EventType_INFORMATION,
                    "BPLib_QM_ScanCache found bundle with Dest EID: \"ipn:%lu.%lu\".",
                    QueuedBundle->blocks.PrimaryBlock.DestEID.Node,
                    QueuedBundle->blocks.PrimaryBlock.DestEID.Service);

                /* If destination EID matches this node, look for an available channel */
                if (BPLib_EID_NodeIsMatch(QueuedBundle->blocks.PrimaryBlock.DestEID, 
                                            BPLIB_EID_INSTANCE))
                {
                    for (i = 0; i < NumChans; i++)
                    {
                        /* 
                        ** See if this available channel has the same service number as
                        ** this bundle's destination EID
                        */
                        if (QueuedBundle->blocks.PrimaryBlock.DestEID.Service ==
                            BPLib_FWP_ConfigPtrs.ChanTblPtr->Configs[AvailChans[i]].LocalServiceNumber)
                        {
                            EgressId = i;
                            NextJobState = CHANNEL_OUT_STOR_TO_CT;

                            break;
                        }
                    }
                }

                /* 
                ** If no local delivery options were found, look for a contact to send
                ** bundles out on
                */
                else
                {
                    i = 0;
                    while (EgressId != BPLIB_UNKNOWN_ROUTE_ID && i < NumConts)
                    {
                        for (j = 0; j < BPLIB_MAX_CONTACT_DEST_EIDS; j++)
                        {    
                            if (BPLib_EID_PatternIsMatch(QueuedBundle->blocks.PrimaryBlock.DestEID, 
                                BPLib_FWP_ConfigPtrs.ContactsTblPtr->ContactSet[AvailConts[i]].DestEIDs[j]))
                            {
                                EgressId = i;
                                NextJobState = CONTACT_OUT_STOR_TO_CT;

                                break;
                            }
                        }
    
                        i++;
                    }
    
                }

                /* Egress bundle if a route exists */
                if (EgressId != BPLIB_UNKNOWN_ROUTE_ID)
                {
                    Status = BPLib_QM_AddUnsortedJob(Inst, QueuedBundle, NextJobState,
                                                    QM_PRI_NORMAL, EgressId, QM_WAIT_FOREVER);
                    if (Status != BPLIB_SUCCESS)
                    {
                        BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_ADD_JOB_ERR_EID, BPLib_EM_EventType_ERROR,
                            "BPLib_STOR_ScanCache call to BPLib_QM_AddUnsortedJob returned error %d.",
                            Status);
                    }

                    /*
                    ** TODO cache bundle even if it's set to egress, can't delete it until
                    ** egress can be verified or custody processing has been done
                    */
                }
                /* Cache a bundle if it cannot currently be routed */
                else
                {
                    BPLib_STOR_CacheBundle(Inst, QueuedBundle);
                }
            }
            else
            {
                /* we may want to remove this `else` case entirely in the future, but currently kept for debugging */
                BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_GOT_NULL_BUNDLE_WARN_EID, BPLib_EM_EventType_WARNING,
                    "BPLib_QM_ScanCache found null bundle in BundleCacheList.");
            }
        }
        else
        {
            /* No bundles found in queue, end processing */
            break;
        }

        BundlesScheduled++;
    }

    return Status;
}

/* Put a bundle in Cache */
BPLib_Status_t BPLib_STOR_CacheBundle(BPLib_Instance_t *Inst, BPLib_Bundle_t *Bundle)
{
    /* For now, just put the bundle back in the queue, will replace with real Cache */
    BPLib_QM_WaitQueueTryPush(&(Inst->BundleCacheList), &Bundle, QM_WAIT_FOREVER);

    return BPLIB_SUCCESS;
}

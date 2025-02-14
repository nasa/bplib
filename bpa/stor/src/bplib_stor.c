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
BPLib_Status_t BPLib_STOR_ScanCache(BPLib_Instance_t* inst, uint32_t max_num_bundles_to_scan)
{
    BPLib_Status_t ReturnStatus = BPLIB_SUCCESS;
    uint32_t BundlesScheduled = 0;
    BPLib_Bundle_t *QueuedBundle;
    BPLib_QM_JobState_t NextJobState = NO_NEXT_STATE;
    BPLib_Status_t PushUnsortedJobStatus;

    if (inst == NULL)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_INVALID_ARG_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_STOR_ScanCache called with null instance pointer.");
        return BPLIB_NULL_PTR_ERROR;
    }

    while (BundlesScheduled < max_num_bundles_to_scan)
    {
        QueuedBundle = NULL;
        if (BPLib_QM_WaitQueueTryPull(&(inst->BundleCacheList), &QueuedBundle, 1))
        {
            if (QueuedBundle != NULL)
            {
                /* We may want to completely remove this event in the future, but for now it'll probably be helpful */
                BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_GOT_GOOD_BUNDLE_INF_EID, BPLib_EM_EventType_INFORMATION,
                    "BPLib_QM_ScanCache found bundle with Dest EID: \"ipn:%lu.%lu\".",
                    QueuedBundle->blocks.pri_blk.dest_eid.node_number,
                    QueuedBundle->blocks.pri_blk.dest_eid.service_number);

                /* Another hacky attempt at routing just to prove concept, FIX ME */
                if (QueuedBundle->blocks.pri_blk.dest_eid.node_number == 200)
                {
                    NextJobState = CONTACT_OUT_STOR_TO_CT;
                }
                else
                {
                    NextJobState = CHANNEL_OUT_STOR_TO_CT;
                }

                PushUnsortedJobStatus = BPLib_QM_AddUnsortedJob(inst, QueuedBundle, NextJobState,
                                                                QM_PRI_NORMAL, QM_WAIT_FOREVER);
                if (PushUnsortedJobStatus != BPLIB_SUCCESS)
                {
                    BPLib_EM_SendEvent(BPLIB_STOR_SCAN_CACHE_ADD_JOB_ERR_EID, BPLib_EM_EventType_ERROR,
                        "BPLib_STOR_ScanCache call to BPLib_QM_AddUnsortedJob returned error %d.",
                        PushUnsortedJobStatus);
                    /* If we can't add jobs, we shouldn't continue */
                    ReturnStatus = BPLIB_ERROR;
                    break;
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
            break;
        }
        BundlesScheduled++;
    }

    return ReturnStatus;
}

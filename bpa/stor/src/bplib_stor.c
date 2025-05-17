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
#include "bplib_stor_sql.h"

#include <stdio.h>

/* 
** Globals
*/

BPLib_StorageHkTlm_Payload_t BPLib_STOR_StoragePayload;


/*******************************************************************************
* Definitions and types
*/
/* We conditionally allow this to be defined by a compile time variable
** so that the unit tests can pass in :memory: here and avoid using the disk
*/
#ifndef BPLIB_STOR_DBNAME
#define BPLIB_STOR_DBNAME       "bplib-storage.db"
#endif

/*******************************************************************************
* Static Functions
*/
static BPLib_Status_t BPLib_STOR_FlushPendingUnlocked(BPLib_Instance_t* Inst)
{
    BPLib_Status_t Status;
    BPLib_BundleCache_t* CacheInst;
    int i;

    CacheInst = &Inst->BundleStorage;

    Status = BPLib_SQL_Store(Inst);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_SQL_Store failed to store bundle. RC=%d", Status);
    }
    else 
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_STORED, CacheInst->InsertBatchSize);
    }

    /* Free the bundles, as they're now persistent
    ** Note: even if the storage fails, we free everything to avoid a leak.
    */
    for (i = 0; i < CacheInst->InsertBatchSize; i++)
    {
        BPLib_MEM_BundleFree(&Inst->pool, CacheInst->InsertBatch[i]);
    }

    CacheInst->InsertBatchSize = 0;

    return Status;
}

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* Inst)
{
    BPLib_Status_t Status;
    int i;

    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    memset(&Inst->BundleStorage, 0, sizeof(BPLib_BundleCache_t));
    pthread_mutex_init(&Inst->BundleStorage.lock, NULL);
    for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
    {
        Status = BPLib_STOR_LoadBatch_Init(&Inst->BundleStorage.ChannelLoadBatches[i]);
        if (Status != BPLIB_SUCCESS)
        {
            return Status;
        }
    }
    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        Status = BPLib_STOR_LoadBatch_Init(&Inst->BundleStorage.ContactLoadBatches[i]);
        if (Status != BPLIB_SUCCESS)
        {
            return Status;
        }
    }

    Status = BPLib_SQL_Init(Inst, (const char *)BPLIB_STOR_DBNAME);
    return Status;
}

void BPLib_STOR_Destroy(BPLib_Instance_t* Inst)
{
    if (Inst == NULL)
    {
        return;
    }

    pthread_mutex_destroy(&Inst->BundleStorage.lock);
}

BPLib_Status_t BPLib_STOR_FlushPending(BPLib_Instance_t* Inst)
{
    BPLib_Status_t Status;
    BPLib_BundleCache_t* CacheInst;

    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    CacheInst = &Inst->BundleStorage;

    pthread_mutex_lock(&CacheInst->lock);
    if (CacheInst->InsertBatchSize > 0)
    {
        Status = BPLib_STOR_FlushPendingUnlocked(Inst);
    }
    else
    {
        /* Don't go further if there's nothing to store */
        Status = BPLIB_SUCCESS;
    }
    pthread_mutex_unlock(&CacheInst->lock);

    return Status;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_BundleCache_t* CacheInst;

    if ((Inst == NULL) || (Bundle == NULL) || (Bundle->blob == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    CacheInst = &Inst->BundleStorage;
    pthread_mutex_lock(&CacheInst->lock);

    /* Add to the next batch */
    CacheInst->InsertBatch[CacheInst->InsertBatchSize++] = Bundle;
    if (CacheInst->InsertBatchSize == BPLIB_STOR_INSERTBATCHSIZE)
    {
        Status = BPLib_STOR_FlushPendingUnlocked(Inst);
    }
    pthread_mutex_unlock(&CacheInst->lock);

    return Status;
}

BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t* Inst, uint32_t EgressID, bool LocalDelivery,
    size_t* NumEgressed)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_BundleCache_t* CacheInst;
    BPLib_STOR_LoadBatch_t* LoadBatch;
    BPLib_Bundle_t* CurrBundle;
    BPLib_EID_Pattern_t LocalEID;
    BPLib_EID_Pattern_t* DestEIDs;
    BPLib_QM_WaitQueue_t* EgressQueue;
    size_t EgressCnt = 0;
    int64_t CurrBundleID;
    size_t NumEIDs;

    if ((Inst == NULL) || (NumEgressed == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (BPLib_QM_IsIngressIdle(Inst) == false)
    {
        /* Avoid searching the DB if the unsorted jobs queue (which is the ingress queue) isn't empty.
        ** Note: this is a pretty critical performance optimization that allows bplib
        ** to use all of its CPU resources for ingress.
        */
        *NumEgressed = 0;
        return BPLIB_SUCCESS;
    }

    /* Determine which channel or contact's batch we're examining */
    CacheInst = &Inst->BundleStorage;
    if (LocalDelivery)
    {
        LoadBatch = &(CacheInst->ChannelLoadBatches[EgressID]);
        LocalEID.MaxNode = BPLIB_EID_INSTANCE.Node;
        LocalEID.MinNode = BPLIB_EID_INSTANCE.Node;
        LocalEID.MaxService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber;
        LocalEID.MinService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber;
        DestEIDs = &LocalEID;
        NumEIDs = 1;
        EgressQueue = &(Inst->ChannelEgressJobs[EgressID]);
    }
    else
    {
        LoadBatch = &(CacheInst->ContactLoadBatches[EgressID]);
        DestEIDs = BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[EgressID].DestEIDs;
        NumEIDs = BPLIB_MAX_CONTACT_DEST_EIDS;
        EgressQueue = &(Inst->ContactEgressJobs[EgressID]);
    }

    /* If the load batch is empty, try to read more from storage */
    if (BPLib_STOR_LoadBatch_IsEmpty(LoadBatch))
    {
        pthread_mutex_lock(&CacheInst->lock);

        /* Ask SQL to load egressable bundles from the specified Destination EID */
        Status = BPLib_SQL_FindForEIDs(Inst, LoadBatch, DestEIDs, NumEIDs);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_STOR_SQL_LOAD_ERR_EID, BPLib_EM_EventType_ERROR,
                "BPLib_SQL_EgressForDestEID failed to load bundle. RC=%d", Status);
        }

        pthread_mutex_unlock(&CacheInst->lock);
    }

    /* All of the bundles for this batch have been egressed */
    else if (BPLib_STOR_LoadBatch_IsConsumed(LoadBatch))
    {
        /* Mark the batch as egressed */
        pthread_mutex_lock(&CacheInst->lock);
        Status = BPLib_SQL_MarkBatchEgressed(Inst, LoadBatch);
        pthread_mutex_unlock(&CacheInst->lock);

        /* Clear the batch */
        (void) BPLib_STOR_LoadBatch_Reset(LoadBatch);
    }

    /* There are bundles in the current batch that need to be egressed */
    else
    {
        while (BPLib_STOR_LoadBatch_PeekNextID(LoadBatch, &CurrBundleID) == BPLIB_SUCCESS)
        {
            /* Set the metadata EID */
            Status = BPLib_SQL_LoadBundle(Inst, CurrBundleID, &CurrBundle);
            if (Status == BPLIB_SUCCESS)
            {
                CurrBundle->Meta.EgressID = EgressID;
                if (BPLib_QM_WaitQueueTryPush(EgressQueue, &CurrBundle, QM_NO_WAIT) == false)
                {
                    /* If QM couldn't accept the bundle, free it. It will be reloaded 
                    ** next time.
                    */
                    BPLib_MEM_BundleFree(&Inst->pool, CurrBundle);
                    break;
                }
            }
            else
            {
                /* If LoadBundle Failed, don't keep trying. */
                break;
            }

            /* After the bundle made it into the destination queue, mark it consumed */
            (void) BPLib_STOR_LoadBatch_AdvanceReader(LoadBatch);
            EgressCnt++;
        }
    }

    *NumEgressed = EgressCnt;
    return Status;
}

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst)
{
    BPLib_Status_t Status;
    BPLib_BundleCache_t* CacheInst;
    size_t NumDiscarded = 0;

    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    if (BPLib_QM_IsIngressIdle(Inst) == false)
    {
        /* Avoid searching the DB if the unsorted jobs queue (which is the ingress queue) isn't empty
        ** Note: this is a pretty critical performance optimization that allows bplib
        ** to use all of its CPU resources for ingress.
        */
        return BPLIB_SUCCESS;
    }

    CacheInst = &Inst->BundleStorage;
    pthread_mutex_lock(&CacheInst->lock);

    Status = BPLib_SQL_DiscardExpired(Inst, &NumDiscarded);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_GC_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_SQL_DiscardExpired failed. RC=%d", Status);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_EXPIRED, NumDiscarded);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, NumDiscarded);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, NumDiscarded);
        BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BUNDLE_COUNT_STORED, NumDiscarded);
    }

    Status = BPLib_SQL_DiscardEgressed(Inst, &NumDiscarded);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_GC_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_SQL_DiscardEgressed failed. RC=%d", Status);
    }
    else
    {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, NumDiscarded);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, NumDiscarded);
        BPLib_AS_Decrement(BPLIB_EID_INSTANCE, BUNDLE_COUNT_STORED, NumDiscarded);
    }

    pthread_mutex_unlock(&CacheInst->lock);

    return Status;
}

/* Validate Storage table data */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData)
{
    BPLib_Status_t           ReturnCode = BPLIB_SUCCESS;

    return ReturnCode;
}

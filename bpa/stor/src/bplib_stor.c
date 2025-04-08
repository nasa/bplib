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
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* Inst)
{
    BPLib_Status_t Status;

    if (Inst == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_init(&Inst->BundleStorage.lock, NULL);

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

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_BundleCache_t* CacheInst;
    int i;

    if ((Inst == NULL) || (Bundle == NULL) || (Bundle->blob == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    CacheInst = &Inst->BundleStorage;
    pthread_mutex_lock(&Inst->BundleStorage.lock);

    /* Add to the next batch */
    CacheInst->InsertBatch[CacheInst->InsertBatchSize++] = Bundle;
    if (CacheInst->InsertBatchSize == BPLIB_STOR_INSERTBATCHSIZE)
    {
        Status = BPLib_SQL_Store(Inst);
        if (Status != BPLIB_SUCCESS)
        {
            BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_ERROR,
                "BPLib_SQL_Store failed to store bundle. RC=%d", Status);
        }

        /* Free the bundles, as they're now persistent
        ** Note: even if the storage fails, we free everything to avoid a leak.
        */
        for (i = 0; i < CacheInst->InsertBatchSize; i++)
        {
            BPLib_MEM_BundleFree(&Inst->pool, CacheInst->InsertBatch[i]);
        }

        CacheInst->InsertBatchSize = 0;
    }
    pthread_mutex_unlock(&CacheInst->lock);

    return Status;
}

BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t* Inst, uint16_t EgressID, bool LocalDelivery,
    BPLib_EID_Pattern_t* DestEID, size_t MaxBundles, size_t* NumEgressed)
{
    BPLib_Status_t Status = BPLIB_SUCCESS;
    BPLib_BundleCache_t* CacheInst;
    int i, j;
    size_t EgressCnt = 0;
    BPLib_Bundle_t* CurrBundle;

    if ((Inst == NULL) || (NumEgressed == NULL) || (DestEID == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if ((MaxBundles == 0) || (MaxBundles > BPLIB_STOR_LOADBATCHSIZE))
    {
        return BPLIB_STOR_PARAM_ERR;
    }

    CacheInst = &Inst->BundleStorage;
    pthread_mutex_lock(&CacheInst->lock);

    /* Ask SQL to load egressable bundles from the specified Destination EID */
    Status = BPLib_SQL_EgressForDestEID(Inst, DestEID, MaxBundles);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_LOAD_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_SQL_Store failed to store bundle. RC=%d", Status);
    }

    /* SQL_EgressForDestEID Updates the LoadBatchSize. We can choose to egress whatever
    ** was loaded here
    */
    if (Status == BPLIB_SUCCESS)
    {
        for (i = 0; i < CacheInst->LoadBatchSize; i++)
        {
            /* Set the metadata EID */
            CurrBundle = CacheInst->LoadBatch[i];
            CurrBundle->Meta.EgressID = EgressID;
            if (LocalDelivery)
            {
                Status = BPLib_QM_AddUnsortedJob(Inst, CurrBundle, CHANNEL_OUT_STOR_TO_CT,
                    QM_PRI_NORMAL, QM_NO_WAIT);
            }
            else
            {
                Status = BPLib_QM_AddUnsortedJob(Inst, CurrBundle, CONTACT_OUT_STOR_TO_CT,
                    QM_PRI_NORMAL, QM_NO_WAIT);
            }
            if (Status != BPLIB_SUCCESS)
            {
                /* In this case, bundles were loaded, but we couldn't create jobs 
                ** Free only what we couldn't egress.
                */
                for (j = i; i < CacheInst->LoadBatchSize; i++)
                {
                    BPLib_MEM_BundleFree(&Inst->pool, CacheInst->LoadBatch[j]);
                }

                /* Note by breaking here we've chosen to lose the bundles that couldn't be
                ** egressed. We could alternatively keep trying through this loop if we find we drop large
                ** batches of bundles. The assumption made here is that by freeing all of the memory, we're giving
                ** the system a better chance of recovering.
                */
                break;
            }
            EgressCnt++;
        }
    }
    else
    {
        /* In this case, there was a storage error, free whatever was loaded before the error. */
        for (i = 0; i < CacheInst->LoadBatchSize; i++)
        {
            BPLib_MEM_BundleFree(&Inst->pool, CacheInst->LoadBatch[i]);
        }
    }

    pthread_mutex_unlock(&CacheInst->lock);

    *NumEgressed = EgressCnt;
    return Status;
}

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst, size_t* NumDiscarded)
{
    BPLib_Status_t Status;
    BPLib_BundleCache_t* CacheInst;

    if ((Inst == NULL) || (NumDiscarded == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    *NumDiscarded = 0;
    CacheInst = &Inst->BundleStorage;
    pthread_mutex_lock(&CacheInst->lock);

    Status = BPLib_SQL_GarbageCollect(Inst, NumDiscarded);
    if (Status != BPLIB_SUCCESS)
    {
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_GC_ERR_EID, BPLib_EM_EventType_ERROR,
            "BPLib_SQL_Store failed to run garbage collection. RC=%d", Status);
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
    BPLib_NC_ReaderLock();
    for (i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++)
    {
        if (BPLib_NC_GetAppState(i) == BPLIB_NC_APP_STATE_STARTED)
        {
            LocalEIDPattern.MaxNode = BPLIB_EID_INSTANCE.Node;
            LocalEIDPattern.MinNode = BPLIB_EID_INSTANCE.Node;
            LocalEIDPattern.MaxService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[i].LocalServiceNumber;
            LocalEIDPattern.MinService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[i].LocalServiceNumber;
            Status = BPLib_STOR_EgressForDestEID(Inst, i, true,
                &LocalEIDPattern,
                BPLIB_STOR_LOADBATCHSIZE,
                &NumEgressed);
            BundlesScanned += NumEgressed;
            if (Status != BPLIB_SUCCESS)
            {
                BPLib_EM_SendEvent(BPLIB_STOR_SQL_LOAD_ERR_EID, BPLib_EM_EventType_ERROR,
                    "BPLib_SQL_Store failed to egress bundle for local channel %d, RC=%d", i, Status);
            }
        }
    }

    /* Egress For Contacts */
    for (i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++)
    {
        for (j = 0; j < BPLIB_MAX_CONTACT_DEST_EIDS; j++)
        {
            /* Code not available: BPLib_NC_GetContactState(i) to check if contact active */
            Status = BPLib_STOR_EgressForDestEID(Inst, i, false,
                &BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[i].DestEIDs[j],
                BPLIB_STOR_LOADBATCHSIZE,
                &NumEgressed);
            BundlesScanned += NumEgressed;
            if (Status != BPLIB_SUCCESS)
            {
                BPLib_EM_SendEvent(BPLIB_STOR_SQL_LOAD_ERR_EID, BPLib_EM_EventType_ERROR,
                    "BPLib_SQL_Store failed to egress bundle for contact %d, RC=%d", i, Status);
            }
        }
    }

    BPLib_NC_ReaderUnlock();
    return Status;
}

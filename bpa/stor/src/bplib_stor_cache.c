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
#include "bplib_stor_cache.h"
#include "bplib_stor_sql.h"
#include "bplib_qm.h"

#include <stdio.h>

/*******************************************************************************
* Definitions and types
*/
#define BPLIB_STOR_DBNAME       "bplib-storage.db"

/*******************************************************************************
* Exported Functions
*/
BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst)
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
            fprintf(stderr, "StoreBundle Failed ERR=%d\n", Status);
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
    BPLib_EID_Pattern_t* DestEID, size_t MaxBundles, 
    size_t* NumEgressed)
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
            Status = BPLib_QM_AddUnsortedJob(Inst, CurrBundle, CONTACT_OUT_STOR_TO_CT,
                QM_PRI_NORMAL, QM_NO_WAIT);
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
        fprintf(stderr, "Failed to Discard Expired Bundles ERR=%d\n", Status);
    }

    pthread_mutex_unlock(&CacheInst->lock);

    return Status;
}

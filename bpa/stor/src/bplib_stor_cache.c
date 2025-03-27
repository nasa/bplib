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

#include <stdio.h>

/*******************************************************************************
* Definitions and types
*/
#define BPLIB_STOR_DBNAME       "bplib-storage.db"
/* Use this define to run SQLite3 entirely in RAM
** #define BPLIB_STOR_DBNAME       ":memory:"
*/

/* I heavily suspect we need to batch based on bytes, not on number of bundles 
** For now, we will store and load one bundle at a time
*/
#define BPLIB_STOR_STOREBATCHSIZE 1
#define BPLIB_STOR_LOADBATCHSIZE 1

typedef struct BPLib_BundleCache
{
    sqlite3* db;
    BPLib_Bundle_t* InsertBatch[BPLIB_STOR_STOREBATCHSIZE];
    size_t InsertBatchSize;
    pthread_mutex_t lock;
} BPLib_BundleCache_t;

/*******************************************************************************
* Module State 
*/
static BPLib_BundleCache_t CacheInst; // Move to BPLib_Inst_t

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

    pthread_mutex_init(&CacheInst.lock, NULL);

    Status = BPLib_SQL_Init(&CacheInst.db, (const char *)BPLIB_STOR_DBNAME);
    return Status;
}

BPLib_Status_t BPLib_STOR_Destroy()
{
    pthread_mutex_destroy(&CacheInst.lock);
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_Status_t Status;

    if ((Inst == NULL) || (Bundle == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    pthread_mutex_lock(&CacheInst.lock);

    /* Add to the next batch */
    CacheInst.InsertBatch[CacheInst.InsertBatchSize++] = Bundle;
    if (CacheInst.InsertBatchSize == BPLIB_STOR_STOREBATCHSIZE)
    {
        Status = BPLib_SQL_StoreBatch(CacheInst.db, CacheInst.InsertBatch, CacheInst.InsertBatchSize);
        if (Status != BPLIB_SUCCESS)
        {
            fprintf(stderr, "STORBatch Failed\n");
        }
        CacheInst.InsertBatchSize = 0;
    }

    pthread_mutex_unlock(&CacheInst.lock);

    printf("Batch\n");
    return Status;
}

BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t* Inst, BPLib_EID_t* DestEID,
    size_t MaxBundles, size_t* NumEgressed)
{
    BPLib_Status_t Status;

    if ((Inst == NULL) || (NumEgressed == NULL) || (DestEID == NULL))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (MaxBundles == 0)
    {
        return BPLIB_STOR_PARAM_ERR;
    }

    pthread_mutex_lock(&CacheInst.lock);

    Status = BPLib_SQL_EgressForDestEID(CacheInst.db, DestEID, BPLIB_STOR_LOADBATCHSIZE,
        NumEgressed);

    pthread_mutex_unlock(&CacheInst.lock);

    return Status;
}

BPLib_Status_t BPLib_STOR_DiscardExpired(BPLib_Instance_t* Inst, size_t* NumDiscarded)
{
    BPLib_Status_t Status;

    if ((Inst == NULL) || (NumDiscarded))
    {
        return BPLIB_NULL_PTR_ERROR;
    }
    *NumDiscarded = 0;

    pthread_mutex_lock(&CacheInst.lock);

    Status = BPLib_SQL_DiscardExpired(CacheInst.db, NumDiscarded);
    if (Status != BPLIB_SUCCESS)
    {
        // Event Message ?
    }

    pthread_mutex_unlock(&CacheInst.lock);

    return Status;
}

#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

#include "bplib_mem.h"
#include "bplib_eid.h"

#include <sqlite3.h>

/* To raise these from 1 (and vastly increase throughput, we should implement a "flush" timer) */
#define BPLIB_STOR_INSERTBATCHSIZE 1
#define BPLIB_STOR_LOADBATCHSIZE 1

typedef struct BPLib_Instance BPLib_Instance_t;

typedef struct BPLib_BundleCache
{
    pthread_mutex_t lock;
    sqlite3* db;
    BPLib_Bundle_t* InsertBatch[BPLIB_STOR_INSERTBATCHSIZE];
    size_t InsertBatchSize;
    BPLib_Bundle_t* LoadBatch[BPLIB_STOR_LOADBATCHSIZE];
    size_t LoadBatchSize;
} BPLib_BundleCache_t;

BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst); // Rename

void BPLib_STOR_Destroy(BPLib_Instance_t* Inst);

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle);

BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t* Inst, BPLib_EID_t* DestEID,
    size_t MaxBundles, size_t* NumEgressed);

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst, size_t* NumDiscarded);

#endif /* BPLIB_STOR_CACHE_H */

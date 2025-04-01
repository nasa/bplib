#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

#include "bplib_mem.h"
#include "bplib_eid.h"

#include <sqlite3.h>

#define BPLIB_STOR_INSERTBATCHSIZE 1 /* To raise from 1, we need a "flush timer" */
#define BPLIB_STOR_LOADBATCHSIZE 100

struct BPLib_BundleCache
{
    pthread_mutex_t lock;
    sqlite3* db;
    BPLib_Bundle_t* InsertBatch[BPLIB_STOR_INSERTBATCHSIZE];
    size_t InsertBatchSize;
    BPLib_Bundle_t* LoadBatch[BPLIB_STOR_LOADBATCHSIZE];
    size_t LoadBatchSize;
};

BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst);

void BPLib_STOR_Destroy(BPLib_Instance_t* Inst);

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle);

BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t* Inst, uint16_t EgressID, BPLib_EID_Pattern_t* DestEID,
    size_t MaxBundles, size_t* NumEgressed);

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst, size_t* NumDiscarded);

#endif /* BPLIB_STOR_CACHE_H */

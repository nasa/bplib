#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

#include "bplib_mem.h"
#include "bplib_qm.h"
#include "bplib_eid.h"

BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst); // Rename

BPLib_Status_t BPLib_STOR_Destroy();

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle);

BPLib_Status_t BPLib_STOR_EgressForDestEID(BPLib_Instance_t* Inst, BPLib_EID_t* DestEID,
    size_t MaxBundles, size_t* NumEgressed);

BPLib_Status_t BPLib_STOR_DiscardExpired(BPLib_Instance_t* Inst, size_t* NumDiscarded);

// BPLib_Status_t BPLib_STOR_MarkDeleted; // Don't need this until CT, Build 7.1

#endif /* BPLIB_STOR_CACHE_H */

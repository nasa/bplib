#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

#include "bplib_mem.h"
#include "bplib_qm.h"

BPLib_Status_t BPLib_STOR_CacheInit(BPLib_Instance_t* Inst);

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle);

#endif /* BPLIB_STOR_CACHE_H */

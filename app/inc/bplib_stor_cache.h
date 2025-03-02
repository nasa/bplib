#ifndef BPLIB_STOR_CACHE_H
#define BPLIB_STOR_CACHE_H

//#include "bplib_bundle.h"
#include "bplib.h" // Don't seem to have a way to include parts of BPLib when outside


BPLib_Status_t BPLib_STOR_Init();

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Bundle_t* bundle);

#endif /* BPLIB_STOR_CACHE_H */

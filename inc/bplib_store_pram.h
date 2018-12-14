/******************************************************************************
 * Filename     : bplib_store_pram.h
 * Purpose      : Bundle Protocol Library POSIX RAM Storage Service (header)
 * Design Notes :
 ******************************************************************************/

#ifndef __BPLIB_STORE_PRAM__
#define __BPLIB_STORE_PRAM__

#ifdef __cplusplus
extern "C" {
#endif 

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bp/bplib.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bplib_store_pram_create     (void);
int bplib_store_pram_destroy    (int handle);
int bplib_store_pram_enqueue    (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int bplib_store_pram_dequeue    (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
int bplib_store_pram_retrieve   (int handle, void** data, int* size, bp_sid_t sid, int timeout);
int bplib_store_pram_refresh    (int handle, void* data, int size, int offset, bp_sid_t sid, int timeout);
int bplib_store_pram_relinquish (int handle, bp_sid_t sid);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif

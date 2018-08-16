/******************************************************************************
 * Filename     : bplib_store_pfile.h
 * Purpose      : Bundle Protocol Library 
 *                POSIX-Compliant File System 
 *                Storage Service
 * Design Notes :
 ******************************************************************************/

#ifndef __BPLIB_STORE_PFILE__
#define __BPLIB_STORE_PFILE__

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

int bp_store_pfile_create       (void);
int bp_store_pfile_destroy      (int handle);
int bp_store_pfile_enqueue      (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int bp_store_pfile_dequeue      (int handle, void** data, int* size, bp_sid_t* sid, int timeout);
int bp_store_pfile_retrieve     (int handle, void** data, int* size, bp_sid_t sid, int timeout);
int bp_store_pfile_refresh      (int handle, void* data, int size, int offset, bp_sid_t sid, int timeout);
int bp_store_pfile_relinquish   (int handle, bp_sid_t sid);

#endif

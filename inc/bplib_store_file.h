/************************************************************************
 * File: bplib_store_file.h
 *
 *  Copyright 2019 United States Government as represented by the
 *  Administrator of the National Aeronautics and Space Administration.
 *  All Other Rights Reserved.
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be
 *  used, distributed and modified only pursuant to the terms of that
 *  agreement.
 *
 * Maintainer(s):
 *  Joe-Paul Swinski, Code 582 NASA GSFC
 *
 *************************************************************************/

#ifndef _bplib_store_file_h_
#define _bplib_store_file_h_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 INCLUDES
 ******************************************************************************/

#include "bplib.h"

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef struct {
    const char* root_path;          /* local directory used to store bundles as files */
    int         cache_size;         /* number of bundles to store in cache (data_cache_t) */
    bool        flush_on_write;     /* true: write-through cache */
} bp_file_attr_t;

typedef struct {
    FILE*   (*open)     (const char* filename, const char* modes);
    int     (*close)    (FILE* stream);
    size_t  (*read)     (void* dst, size_t size, size_t n, FILE* stream);
    size_t  (*write)    (const void* src, size_t size, size_t n, FILE* stream);
    int     (*seek)     (FILE* stream, long int offset, int whence);
    int     (*flush)    (FILE* stream);
} bp_file_driver_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
void    bplib_store_file_init          (bp_file_driver_t* driver);

/* Service API */
int     bplib_store_file_create        (int type, bp_ipn_t node, bp_ipn_t service, bool recover, void* parm);
int     bplib_store_file_destroy       (int handle);
int     bplib_store_file_enqueue       (int handle, const void* data1, size_t data1_size, const void* data2, size_t data2_size, int timeout);
int     bplib_store_file_dequeue       (int handle, bp_object_t** object, int timeout);
int     bplib_store_file_retrieve      (int handle, bp_sid_t sid, bp_object_t** object, int timeout);
int     bplib_store_file_release       (int handle, bp_sid_t sid);
int     bplib_store_file_relinquish    (int handle, bp_sid_t sid);
int     bplib_store_file_getcount      (int handle);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _bplib_store_file_h_ */

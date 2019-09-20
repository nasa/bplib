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

#ifndef __BPLIB_STORE_FILE__
#define __BPLIB_STORE_FILE__

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
    const char* root_path;
    int         cache_size;
} bp_file_attr_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
void    bplib_store_file_init          (void);

/* Service API */
int     bplib_store_file_create        (void* parm);
int     bplib_store_file_destroy       (int handle);
int     bplib_store_file_enqueue       (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int     bplib_store_file_dequeue       (int handle, bp_object_t** object, int timeout);
int     bplib_store_file_retrieve      (int handle, bp_sid_t sid, bp_object_t** object, int timeout);
int     bplib_store_file_release       (int handle, bp_sid_t sid);
int     bplib_store_file_relinquish    (int handle, bp_sid_t sid);
int     bplib_store_file_getcount      (int handle);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* __BPLIB_STORE_FILE__ */

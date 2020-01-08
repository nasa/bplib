/************************************************************************
 * File: flash_store.h
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

#ifndef _flash_store_h_
#define _flash_store_h_

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

#define BP_FLASH_DATA_TYPE          0
#define BP_FLASH_SPARE_TYPE         1
#define BP_FLASH_INVALID_INDEX      UINT32_MAX

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef uint32_t bp_flash_index_t;

typedef struct {
    bp_flash_index_t    block;
    bp_flash_index_t    page;
    int                 type;
} bp_flash_addr_t;

typedef int (*bp_flash_page_read_t)     (bp_flash_addr_t addr, void** data, int* size);
typedef int (*bp_flash_page_write_t)    (bp_flash_addr_t addr, void* data, int size);
typedef int (*bp_flash_block_erase_t)   (bp_flash_index_t block);

typedef struct {
    int                     num_blocks;
    int                     pages_per_block;
    int                     data_size; /* bytes */
    int                     spare_size; /* bytes */
    bp_flash_page_read_t    read;
    bp_flash_page_write_t   write;
    bp_flash_block_erase_t  erase;
} bp_flash_driver_t;

typedef struct {
    int cache_size; /* number of data items to cache */
    int max_data_size; /* max size of data being cached */ 
} bp_flash_attr_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
int     bplib_store_flash_init          (bp_flash_driver_t driver);

/* Service API */
int     bplib_store_flash_create        (void* parm);
int     bplib_store_flash_destroy       (int handle);
int     bplib_store_flash_enqueue       (int handle, void* data1, int data1_size, void* data2, int data2_size, int timeout);
int     bplib_store_flash_dequeue       (int handle, bp_object_t** object, int timeout);
int     bplib_store_flash_retrieve      (int handle, bp_sid_t sid, bp_object_t** object, int timeout);
int     bplib_store_flash_release       (int handle, bp_sid_t sid);
int     bplib_store_flash_relinquish    (int handle, bp_sid_t sid);
int     bplib_store_flash_getcount      (int handle);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* _flash_store_h_ */

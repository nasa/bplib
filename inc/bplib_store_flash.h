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

#ifndef _bplib_store_flash_h_
#define _bplib_store_flash_h_

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

#define BP_FLASH_INIT_FORMAT                0
#define BP_FLASH_INIT_RECOVER               1

#ifndef BP_FLASH_INVALID_INDEX
#define BP_FLASH_INVALID_INDEX              UINT16_MAX
#endif

#ifndef BP_FLASH_INDEX_TYPE
#define BP_FLASH_INDEX_TYPE                 uint16_t
#endif

#ifndef FLASH_MAX_STORES
#define FLASH_MAX_STORES                    24
#endif

#ifndef FLASH_MAX_PAGES_PER_BLOCK
#define FLASH_MAX_PAGES_PER_BLOCK           128
#endif

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef BP_FLASH_INDEX_TYPE bp_flash_index_t;

typedef struct {
    bp_flash_index_t    block;
    bp_flash_index_t    page;
} bp_flash_addr_t;

typedef int (*bp_flash_page_read_t)         (bp_flash_addr_t addr, void* page_data);
typedef int (*bp_flash_page_write_t)        (bp_flash_addr_t addr, void* page_data);
typedef int (*bp_flash_block_erase_t)       (bp_flash_index_t block);
typedef int (*bp_flash_block_is_bad_t)      (bp_flash_index_t block);
typedef int (*bp_flash_physical_block_t)    (bp_flash_index_t logblk);

typedef struct {
    bp_flash_index_t            num_blocks;
    bp_flash_index_t            pages_per_block;
    int                         page_size; /* bytes */
    bp_flash_page_read_t        read;
    bp_flash_page_write_t       write;
    bp_flash_block_erase_t      erase;
    bp_flash_block_is_bad_t     isbad;
    bp_flash_physical_block_t   phyblk;
} bp_flash_driver_t;

typedef struct {
    int num_free_blocks;
    int num_used_blocks;
    int num_bad_blocks;
    int error_count;
} bp_flash_stats_t;

typedef struct {
    int max_data_size; /* max size of data being cached */
} bp_flash_attr_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
int     bplib_store_flash_init          (bp_flash_driver_t driver, int init_mode, bool sw_edac);
void    bplib_store_flash_uninit        (void);
void    bplib_store_flash_stats         (bp_flash_stats_t* stats, bool log_stats, bool reset_stats);

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

#endif /* _bplib_store_flash_h_ */

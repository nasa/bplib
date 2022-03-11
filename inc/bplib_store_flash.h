/*
 * NASA Docket No. GSC-18,587-1 and identified as “The Bundle Protocol Core Flight
 * System Application (BP) v6.5”
 *
 * Copyright © 2020 United States Government as represented by the Administrator of
 * the National Aeronautics and Space Administration. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef BPLIB_STORE_FLASH_H
#define BPLIB_STORE_FLASH_H

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

/*
 * Value to use to indicate an invalid flash block and/or page
 */
#ifndef BP_FLASH_INVALID_INDEX
#define BP_FLASH_INVALID_INDEX UINT16_MAX
#endif

/*
 * Type used for holding indices into flash blocks and pages;
 * for example, uint8_t indicates that only 255 blocks or pages
 * are accessible.
 */
#ifndef BP_FLASH_INDEX_TYPE
#define BP_FLASH_INDEX_TYPE uint16_t
#endif

/*
 * The number of flash based storage service control structures to
 * statically allocate
 */
#ifndef FLASH_MAX_STORES
#define FLASH_MAX_STORES 24
#endif

/******************************************************************************
 TYPEDEFS
 ******************************************************************************/

typedef BP_FLASH_INDEX_TYPE bp_flash_index_t;

typedef struct
{
    bp_flash_index_t block;
    bp_flash_index_t page;
} bp_flash_addr_t;

typedef int (*bp_flash_page_read_t)(bp_flash_addr_t addr, void *page_data);
typedef int (*bp_flash_page_write_t)(bp_flash_addr_t addr, void *page_data);
typedef int (*bp_flash_block_erase_t)(bp_flash_index_t block);
typedef int (*bp_flash_block_is_bad_t)(bp_flash_index_t block);
typedef int (*bp_flash_physical_block_t)(bp_flash_index_t logblk);

typedef struct
{
    bp_flash_index_t          num_blocks;      /* number of blocks available in flash device */
    bp_flash_index_t          pages_per_block; /* number of pages per block available in flash device */
    int                       page_size;       /* size of page in bytes */
    bp_flash_page_read_t      read;            /* function pointer to read page */
    bp_flash_page_write_t     write;           /* function pointer to write page */
    bp_flash_block_erase_t    erase;           /* function pointer to erase block */
    bp_flash_block_is_bad_t   isbad;           /* functino pointer to check if block bad */
    bp_flash_physical_block_t phyblk; /* function pointer to convert between logical and physical block addresses */
} bp_flash_driver_t;

typedef struct
{
    int num_free_blocks; /* number of free blocks available to driver to store bundles in */
    int num_used_blocks; /* number of blocks currently used by the driver */
    int num_fail_blocks; /* number of blocks that have been removed from the free list due to errors */
    int error_count;     /* number of flash operations that have returned an error */
} bp_flash_stats_t;

typedef struct
{
    int max_data_size; /* max size of data stored, must exceed page size */
} bp_flash_attr_t;

/******************************************************************************
 PROTOTYPES
 ******************************************************************************/

/* Application API */
int  bplib_store_flash_init(bp_flash_driver_t driver, bool sw_edac);
void bplib_store_flash_uninit(void);
void bplib_store_flash_reclaim_used_blocks(bp_ipn_t node, bp_ipn_t service);
void bplib_store_flash_restore_failed_blocks(void);
void bplib_store_flash_stats(bp_flash_stats_t *stats, bool log_stats, bool reset_stats);

/* Service API */
bp_handle_t bplib_store_flash_create(int type, bp_ipn_t node, bp_ipn_t service, bool recover, void *parm);
int         bplib_store_flash_destroy(bp_handle_t h);
int bplib_store_flash_enqueue(bp_handle_t h, const void *data1, size_t data1_size, const void *data2, size_t data2_size,
                              int timeout);
int bplib_store_flash_dequeue(bp_handle_t h, bp_object_t **object, int timeout);
int bplib_store_flash_retrieve(bp_handle_t h, bp_sid_t sid, bp_object_t **object, int timeout);
int bplib_store_flash_release(bp_handle_t h, bp_sid_t sid);
int bplib_store_flash_relinquish(bp_handle_t h, bp_sid_t sid);
int bplib_store_flash_getcount(bp_handle_t h);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BPLIB_STORE_FLASH_H */
